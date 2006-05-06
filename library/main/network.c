/***************************************************************************
 *            network.c
 *
 *  Sat Apr  8 12:06:07 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "network.h"
#include "errors/debug.h"
#include "ecode.h"
#include "events.h"
#include "parser.h"
#include "other/other.h"

int sod_connect( sod_session *session, char *host, u_int16_t port, char *login, char *password )
{
	__label__ _gethost, _pthread;
	struct sockaddr_in addr;
	struct hostent *p_he;
	static sod_thread_arg_t sod_thread_arg;
	int iret;
	pstart();
	
	session->socket=socket(PF_INET,SOCK_STREAM,0);
	if ( session->socket==-1 )
		sod_throw_error_no_gettext(SE_SOCKET,strerror(errno));
	
	_gethost:
	p_he=gethostbyname(host);
	if ( p_he==NULL )
	{
		if ( h_errno==TRY_AGAIN ) goto _gethost; else
		{
			close(session->socket);
			sod_throw_error_no_gettext(SE_GETHOST,(char*)hstrerror(h_errno));
		}
	}
	
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	addr.sin_addr.s_addr=*((int*)p_he->h_addr_list[0]);
	
	if ( connect(session->socket,(void*)&addr,sizeof(addr))==-1 )
	{
		close(session->socket);
		sod_throw_error_no_gettext(SE_CONNECT,strerror(errno));
	}
	
	session->status=SOD_CONNECTED;
	
	sod_thread_arg.session=session;
	sod_thread_arg.login=login;
	sod_thread_arg.password=password;
	_pthread:
	iret=pthread_create(&(session->thread),NULL,(void*)sod_thread,(void*)&sod_thread_arg);
	if ( iret!=0 )
	{
		if ( iret==EAGAIN ) goto _pthread; else
		{
			sod_disconnect(session);
			sod_throw_error(SE_THREAD,"Failed to start the thread");
		}
	}
	
	pstop();
	return SOD_OK;
}

int sod_disconnect( sod_session *session )
{
	pstart();
	
	if ( session->status==SOD_NOT_CONNECTED )
		sod_throw_error(SE_NOT_CONNECTED,"The current session is not connected");
	
	if ( shutdown(session->socket,2)==-1 )
		sod_place_error_no_gettext(SE_SHUTDOWN,strerror(errno));
	
	if ( close(session->socket)==-1 )
		sod_place_error_no_gettext(SE_CLOSE,strerror(errno));
	
	session->status=SOD_NOT_CONNECTED;
	pstop();
	return SOD_OK;
}

int sod_logout( sod_session *session )
{
	pstart();
	
	if ( session->status!=SOD_AUTHORIZED )
		sod_throw_error(SE_NOT_CONNECTED,"The current session is not authorized");
	
	if ( sod_put_message(session,"LOGOUT")==SOD_ERROR )
		return SOD_ERROR;
	
	pstop();
	return SOD_OK;
}

int sod_thread( sod_thread_arg_t *arg )
{
	__label__ _write, _exit;
	int readc, writec, iret;
	sod_session *const session=arg->session;
	int datacnt;
	char *data[SOD_ARG_CNT];
	pstart();
	
	//	инициализация
	if ( (iret=pthread_mutex_init(&session->msg_mutex,NULL))!=0 )
	{
		sod_place_error(SE_MUTEX,vstr("Failed to initialize a mutex (code=%d)",iret));
		sod_ev_error(session,SOD_EV_ERROR);
		return SOD_ERROR;
	}
	
	for (;;)
	{
		_
		//	запись в сокет
		if ( session->outpos!=session->outstart )
		{
			_
			//	записаны не все данные
			_write:
			writec=write(session->socket,session->outbuf+session->outstart,session->outpos-session->outstart);
			if ( writec==-1 )
			{
				sod_place_error_no_gettext(SE_WRITE,strerror(errno));
				sod_ev_error(session,SOD_EV_ERROR);
				if ( session->ecode!=SE_NONE ) break;
			}	else
			if ( writec==0 )
			{
				//	соединение закрыто
				sod_disconnect(session);
				sod_ev_disconnected(session,SOD_EV_DISCON);
				break;
			}
			session->outstart+=writec;
			if ( session->outpos!=session->outstart ) goto _write;
		}
		
		//	прверка свободного места в буффере
		if ( session->inpos>SOD_BUFFER_SIZE-SOD_BUFFER_FREE )
		{
			if ( session->instart>0 )
			{
				memmove(session->inbuf,session->inbuf+session->instart,session->inpos-session->instart);
				session->inpos-=session->instart;
				session->instart=0;
			}	else
			{
				if ( session->inpos==SOD_BUFFER_SIZE )
				{
					sod_place_error(SE_PARSE,"Message size is too big (buffer is full)");
					sod_ev_error(session,SOD_EV_ERROR);
					if ( session->ecode!=SE_NONE ) break;
				}
			}
		}
		
		//	чтение из сокета
		readc=read(session->socket,session->inbuf+session->inpos,SOD_BUFFER_SIZE-session->inpos);
		if ( readc==-1 )
		{
			sod_place_error_no_gettext(SE_READ,strerror(errno));
			sod_ev_error(session,SOD_EV_ERROR);
			if ( session->ecode!=SE_NONE ) break;
		}	else
		if ( readc==0 )
		{
			sod_disconnect(session);
			sod_ev_disconnected(session,SOD_EV_DISCON);
			break;
		}
		session->inpos+=readc;
		
		//	обработка
		do
		{
			iret=sod_get_message(session,&datacnt,data);
			if ( iret==SOD_ERROR )
			{
				sod_ev_error(session,SOD_EV_ERROR);
				if ( session->ecode!=SE_NONE ) goto _exit;
			}
			if ( iret!=SOD_AGAIN ) break;
			
			/*
				здесь - принятие решений по командам
			*/
			if ( (session->status==SOD_CONNECTED) && (!strcmp(data[0],"SERVER")) )
			{
				//	авторизация
				if ( (datacnt!=3) || strcmp(data[1],STR(SOD_PROTOCOL)) )
				{
					sod_place_error(SE_PROTOCOL,"Invalid protocol version");
					sod_ev_error(session,SOD_EV_ERROR);
					sod_disconnect(session);
					sod_ev_disconnected(session,SOD_EV_DISCON);
					goto _exit;
				}	else
				{
					char *p;
					session->cmdsize=strtol(data[2],&p,0);
					if ( *p!=0 )
					{
						sod_place_error(SE_PARSE,"Invalid integer");
						sod_ev_error(session,SOD_EV_ERROR);
						if ( session->ecode!=SE_NONE ) goto _exit;
					}	else
					if ( sod_put_message(session,vstr("AUTH \"%s\" \"%s\"",arg->login,arg->password))==SOD_ERROR )
					{
						sod_ev_error(session,SOD_EV_ERROR);
						if ( session->ecode!=SE_NONE ) goto _exit;
					}	else
					{
						session->status=SOD_ON_AUTHORIZING;
					}
				}
			}	else
			if ( (session->status==SOD_ON_AUTHORIZING) && (!strcmp(data[0],"ACCEPTED")) )
			{
				session->status=SOD_AUTHORIZED;
				sod_ev_auth_ok(session,SOD_EV_AUTH_OK);
			}	else
			{
				sod_place_error(SE_PROTOCOL,"Unknown command");
				sod_ev_error(session,SOD_EV_ERROR);
				if ( session->ecode!=SE_NONE ) goto _exit;
			}
		}	while ( iret==SOD_AGAIN );
	}
	_exit:
	
	//	деинициализация
	if ( (iret=pthread_mutex_destroy(&session->msg_mutex))!=0 )
	{
		sod_place_error(SE_MUTEX,vstr("Failed to destroy a mutex (code=%d)",iret));
		sod_ev_error(session,SOD_EV_ERROR);
		return SOD_ERROR;
	}
	
	pstop();
	return SOD_OK;
}
