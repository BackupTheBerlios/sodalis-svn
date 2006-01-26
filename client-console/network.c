/***************************************************************************
 *            network.c
 *
 *  Tue Jan 24 18:26:29 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <libintl.h>

#include "network.h"
#include "errors/debug.h"
#include "scc.h"

int sock;
int out_cur=0, in_cur=0;
int out_pos=0, in_pos=0;
char out_buf[BUFFER_SIZE], in_buf[BUFFER_SIZE];

int net_connect( char *host, u_int16_t port )
{
	struct sockaddr_in addr;
	struct hostent *p_he;
	pstart();
	
	sock=socket(PF_INET,SOCK_STREAM,0);
	if ( sock==-1 )
	{
		printf(gettext("Cannot create the socket: %s\n"),strerror(errno));
		return -1;
	}
	
	p_he=gethostbyname(host);
	if ( p_he==NULL )
	{
		char *h_errtext;
		switch ( h_errno )
		{
			case HOST_NOT_FOUND:
				h_errtext=gettext("Host was not found");
				break;
			default:
				h_errtext=gettext("Error was detected");
		}
		printf(gettext("Cannot get the host: %s\n"),h_errtext);
		if ( close(sock)==-1 )
			printf(gettext("Cannot close the socket: %s\n"),strerror(errno));
		return -1;
	}
	
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=*((int*)p_he->h_addr_list[0]);
	addr.sin_port=htons(port);
	
	if ( connect(sock,(void*)&addr, sizeof(addr))==-1 )
	{
		printf(gettext("Cannot connect: %s\n"),strerror(errno));
		if ( close(sock)==-1 )
			printf(gettext("Cannot close the socket: %s\n"),strerror(errno));
		return -1;
	}
	
	printf(gettext("Connection was successful!\n"));
	
	pstop();
	return 0;
}

int net_disconnect( void )
{
	pstart();
	
	if ( shutdown(sock,2)==-1 )
		printf(gettext("Cannot shut down the socket: %s\n"),strerror(errno));
	if ( close(sock)==-1 )
		printf(gettext("Cannot close the socket: %s\n"),strerror(errno));
	
	pstop();
	return 0;
}

int net_send( char *msg )
{
	pstart();
	
	write(sock,msg,strlen(msg)+1);
	#ifdef DEBUG_NET
	printf("=== SEND: \"%s\"\n",msg);
	#endif
	
	pstop();
	return 0;
}

int net_recv( void )
{
	int t;
	pstart();
	
	if ( BUFFER_SIZE-in_pos<MINIMUM_BUFSIZE )
	{
		if ( in_cur==0 )
		{
			printf(gettext("Incoming message buffer is overflowed\n"));
			return -1;
		}
		memmove(in_buf,in_buf+in_cur,in_pos-in_cur);
		in_pos-=in_cur;
		in_cur=0;
	}
	t=read(sock,in_buf+in_pos,BUFFER_SIZE-in_pos);
	if ( t==-1 )
	{
		printf(gettext("Reading failed: %s\n"),strerror(errno));
		return -1;
	}
	if ( t==0 )
	{
		printf(gettext("Connection was closed by the server\n"));
		go_on=0;
	}
	in_pos+=t;
	
	pstop();
	return 0;
}

int net_msg( char **msg )
{
	char *p=in_buf+in_cur;
	pstart();
	
	while ( (p<in_buf+in_pos) && (*p) ) p++;
	if ( (*p==0) && (p<in_buf+in_pos) )
	{
		*msg=in_buf+in_cur;
		in_cur=p-in_buf+1;
		#ifdef DEBUG_NET
		printf("=== RECEIVE: \"%s\"\n",*msg);
		#endif
	}	else
	{
		return -1;
	}
	
	pstop();
	return 0;
}
