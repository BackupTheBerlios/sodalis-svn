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

#ifdef USE_GETTEXT
#include <libintl.h>
#endif

#include "network.h"
#include "errors/debug.h"
#include "ecode.h"
#include "events.h"
#include "parser.h"

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
	
	if ( session->status==SOD_AUTHORIZED )
	{
		/*
			отправление команды LOGOUT
			завершение потока
		*/
	}
	
	if ( shutdown(session->socket,2)==-1 )
		sod_place_error_no_gettext(SE_SHUTDOWN,strerror(errno));
	
	if ( close(session->socket)==-1 )
		sod_place_error_no_gettext(SE_CLOSE,strerror(errno));
	
	session->status=SOD_NOT_CONNECTED;
	pstop();
	return SOD_OK;
}

int sod_thread( sod_thread_arg_t *arg )
{
	__label__ _write;
	int readc, writec, iret;
	sod_session *const session=arg->session;
	int datacnt;
	char *data[SOD_ARG_CNT];
	pstart();
	
	for (;;)
	{
		//	запись в сокет
		if ( session->outpos!=session->outstart )
		{
			//	записаны не все данные
			_write:
			writec=write(session->socket,session->outbuf+session->outstart,session->outstart-session->outpos);
			if ( writec==-1 )
			{
				sod_place_error_no_gettext(SE_WRITE,strerror(errno));
				sod_ev_error(SOD_EV_ERROR);
				if ( session->ecode!=SE_NONE ) break;
			}	else
			if ( writec==0 )
			{
				//	соединение закрыто
				sod_disconnect(session);
				sod_ev_disconnected(SOD_EV_DISCON);
				break;
			}
			session->outstart+=writec;
			if ( session->outpos!=session->outstart ) goto _write;
		}
		
		//	чтение из сокета
		readc=read(session->socket,session->inbuf+session->outpos,SOD_BUFFER_SIZE-session->outpos);
		if ( readc==-1 )
		{
			sod_place_error_no_gettext(SE_READ,strerror(errno));
			sod_ev_error(SOD_EV_ERROR);
			if ( session->ecode!=SE_NONE ) break;
		}	else
		if ( readc==0 )
		{
			sod_disconnect(session);
			sod_ev_disconnected(SOD_EV_DISCON);
			break;
		}
		
		//	обработка
		do
		{
			iret=sod_get_message(session,&datacnt,data);
			if ( iret==SOD_ERROR )
			{
				sod_ev_error(SOD_EV_ERROR);
				if ( session->ecode!=SE_NONE ) break;
			}
			/*
				здесь - принятие решений по командам
			*/
		}	while ( iret==SOD_AGAIN );
		
	}
	
	pstop();
	return SOD_OK;
}
