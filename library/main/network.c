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

int sod_connect( sod_session *session, char *host, u_int16_t port, char *login, char *password )
{
	struct sockaddr_in addr;
	struct hostent *p_he;
	sod_thread_arg_t sod_thread_arg;
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
	pstart();
	pstop();
	return SOD_OK;
}
