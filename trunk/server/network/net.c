/***************************************************************************
 *            net.c
 *
 *  Sun Oct 23 10:23:36 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libintl.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "network/net.h"
#include "main/ecode.h"
#include "errors/debug.h"
#include "network/nstack.h"
#include "stack/stack_end.h"
#include "network/nlist.h"
#include "list/list_end.h"
#include "log/log.h"

//	слушающий сокет
int listen_sock;

//	список соединённых сокетов
kulist_t_net *sock_list=NULL;
//	сокеты, доступные для чтения
stack_t_net *sock_wait=NULL;

//	сокеты, которые нужно проверить на запись
kulist_t_net *sock_wlist=NULL;
//	сокеты, доступные для записи
stack_t_net *sock_wwait=NULL;

inline ecode_t alloc_storage( void )
{
	if ( ((sock_list=kulist_create_net())==NULL) ||
			((sock_wlist=kulist_create_net())==NULL) )
	{
		plog(gettext("Failed to create a list (kucode=%d)\n"),kucode);
		return E_KU2;
	}
	if ( ((sock_wait=stack_create_net())==NULL) ||
			((sock_wwait=stack_create_net())==NULL) )
	{
		plog(gettext("Failed to create a stack (kucode=%d)\n"),kucode);
		return E_KU2;
	}
	return E_NONE;
}

inline ecode_t free_storage( void )
{
	kucode=KE_NONE;
	if ( (sock_list!=NULL) && ((kucode=kulist_free_net(sock_list))!=KE_NONE) )
		plog(gettext("Failed to free a list (kucode=%d)\n"),kucode);
	if ( (sock_wait!=NULL) && ((kucode=stack_free_net(sock_wait))!=KE_NONE) )
		plog(gettext("Failed to free a stack (kucode=%d)\n"),kucode);
	if ( (sock_wlist!=NULL) && ((kucode=kulist_free_net(sock_wlist))!=KE_NONE) )
		plog(gettext("Failed to free a list (kucode=%d)\n"),kucode);
	if ( (sock_wwait!=NULL) && ((kucode=stack_free_net(sock_wwait))!=KE_NONE) )
		plog(gettext("Failed to free a stack (kucode=%d)\n"),kucode);
	return kucode==KE_NONE?E_NONE:E_KU2;
}

ecode_t net_open( uint16_t port, int backlog )
{
	struct sockaddr_in addr;
	
	pstart();
	
	if ( alloc_storage()!=E_NONE )
		free_storage();
	
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=INADDR_ANY;
	addr.sin_port=htons(port);
	
	if ( (listen_sock=socket(PF_INET,SOCK_STREAM,0))==-1 )
	{
		plog(gettext("Failed to create a socket: %s\n"),strerror(errno));
		free_storage();
		return E_SOCKET;
	}
	
	if ( bind(listen_sock,(void*)&addr,sizeof(addr))!=0 )
	{
		plog(gettext("Failed to bind a listening socket: %s\n"),strerror(errno));
		free_storage();
		if ( close(listen_sock)!=0 )
			plog(gettext("Failed to close the socket (fd=%d): %s\n"),listen_sock,strerror(errno));
		return E_BIND;
	}
	
	if ( listen(listen_sock,backlog)!=0 )
	{
		plog(gettext("Failed to begin listening with the socket: %s\n"),strerror(errno));
		free_storage();
		if ( close(listen_sock)!=0 )
			plog(gettext("Failed to close the socket (fd=%d): %s\n"),listen_sock,strerror(errno));
		return E_LISTEN;
	}
	
	plog(gettext("Started network server on port %d with backlog %d (fd=%d)\n"),port,backlog,listen_sock);
	
	pstop();
	return E_NONE;
}

ecode_t net_close( void )
{
	int i;
	net_id *id;
	
	pstart();
	
	plog(gettext("Networing is being stopped!\n"));
	
	for ( i=0; i<sock_list->size; i++ )
	{
		if ( (id=kulist_next_net(sock_list))==NULL )
			plog(gettext("Failed to get next element of the list (kucode=%d)\n"),kucode);
		if ( shutdown(id->sock,2)!=0 )
			plog(gettext("Failed to shut down the socket (fd=%d): %s\n"),id->sock,strerror(errno));
		if ( close(id->sock)!=0 )
			plog(gettext("Failed to close the socket (fd=%d): %s\n"),id->sock,strerror(errno));
		dfree(id);
	}
	free_storage();
	if ( close(listen_sock)!=0 )
			plog(gettext("Failed to close the socket (fd=%d): %s\n"),listen_sock,strerror(errno));
	
	pstop();
	return E_NONE;
}

ecode_t net_wait( int *connected, stack_t_net **socks, stack_t_net **wsocks )
{
	fd_set rset, wset;
	int maxfd=listen_sock;
	int i;
	net_id *id;
	
	pstart();
	
	//	добавление сокетов в fd_set и поиск максимального
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_SET(listen_sock,&rset);
	
	for ( i=0; i<sock_list->size; i++ )
	{
		if ( (id=kulist_next_net(sock_list))==NULL )
		{
			plog(gettext("Failed to get next element of the list (kucode=%d)\n"),kucode);
			return E_KU2;
		}
		FD_SET(id->sock,&rset);
		if ( id->sock>maxfd ) maxfd=id->sock;
	}
	for ( i=0; i<sock_wlist->size; i++ )
	{
		if ( (id=kulist_next_net(sock_wlist))==NULL )
		{
			plog(gettext("Failed to get next element of the list (kucode=%d)\n"),kucode);
			return E_KU2;
		}
		FD_SET(id->sock,&wset);
	}
	
	//	поиск изменившихся сокетов
	if ( (i=select(maxfd+1,&rset,&wset,NULL,NULL))==-1 )
	{
		plog(gettext("Select function failed: %s\n"),strerror(errno));
		return E_SELECT;
	}
	
	//	добавление их в список
	for ( i=0; i<sock_list->size; i++ )
	{
		if ( (id=kulist_next_net(sock_list))==NULL )
		{
			plog(gettext("Failed to get next element of the list (kucode=%d)\n"),kucode);
			return E_KU2;
		}
		if ( FD_ISSET(id->sock,&rset) )
		{
			if ( (kucode=stack_push_net(sock_wait,id))!=KE_NONE )
			{
				plog(gettext("Failed to insert an element into the stack (kucode=%d)\n"),kucode);
				return E_KU2;
			}
		}
	}
	for ( i=0; i<sock_wlist->size; i++ )
	{
		if ( (id=kulist_next_net(sock_wlist))==NULL )
		{
			plog(gettext("Failed to get next element of the list (kucode=%d)\n"),kucode);
			return E_KU2;
		}
		if ( FD_ISSET(id->sock,&wset) )
		{
			if ( (kucode=stack_push_net(sock_wwait,id))!=KE_NONE )
			{
				plog(gettext("Failed to insert an element into the stack (kucode=%d)\n"),kucode);
				return E_KU2;
			}
			if ( (kucode=kulist_rem_net(sock_wlist))!=KE_NONE )
			{
				plog(gettext("Failed to remove the element from list (kucode=%d)\n"),kucode);
				return E_KU2;
			}
		}
	}
	
	*socks=sock_wait;
	*wsocks=sock_wwait;
	if ( FD_ISSET(listen_sock,&rset) ) *connected=1; else *connected=0;
	
	pstop();
	return E_NONE;
}

net_id *net_accept( void )
{
	struct sockaddr_in addr;
	unsigned int sz=sizeof(addr);
	int s, fl;
	net_id *id;
	
	pstart();
	
	id=dmalloc(sizeof(net_id));
	if ( id==NULL )
	{
		ecode=E_MEMORY;
		return NULL;
	}
	
	if ( (s=accept(listen_sock,(void*)&addr,&sz))==-1 )
	{
		plog(gettext("Cannot accept a connection: %s\n"),strerror(errno));
		ecode=E_ACCEPT;
		dfree(id);
		return NULL;
	}	else
	{
		if ( ((fl=fcntl(s,F_GETFL))==-1) ||
				(fcntl(s,F_SETFL,fl|O_NONBLOCK)==-1) )
		{
			plog(gettext("Failed to change flags of the accepted socket: %s\n"),strerror(errno));
			ecode=E_FCNTL;
			if ( shutdown(s,2)!=0 )
				plog(gettext("Failed to shut down the socket (fd=%d): %s\n"),s,strerror(errno));
			if ( close(s)!=0 )
				plog(gettext("Failed to close the socket (fd=%d): %s\n"),s,strerror(errno));
			dfree(id);
			return NULL;
		}
		
		id->sock=s;
		strcpy(id->ip,inet_ntoa(addr.sin_addr));
		id->data=NULL;
		
		if ( (kucode=kulist_ins_net(sock_list,id))!=KE_NONE )
		{
			plog(gettext("Failed to insert an element into the list (kucode=%d)\n"),kucode);
			ecode=E_KU2;
			if ( shutdown(s,2)!=0 )
				plog(gettext("Failed to shut down the socket (fd=%d): %s\n"),s,strerror(errno));
			if ( close(s)!=0 )
				plog(gettext("Failed to close the socket (fd=%d): %s\n"),s,strerror(errno));
			dfree(id);
			return NULL;
		}	else
		{
			plog(gettext("A new connection accepted from IP %s (fd=%d)\n"),id->ip,s);
		}
	}
	
	pstop();
	return id;
}

ecode_t net_discon( net_id *id )
{
	pstart();
	
	if ( (kucode=kulist_search_net(sock_list,id))!=KE_NONE )
	{
		plog(gettext("Failed to find an element in the list (kucode=%d)\n"),kucode);
		return E_KU2;
	}
	
	if ( (kucode=kulist_rem_net(sock_list))!=KE_NONE )
	{
		plog(gettext("Failed to remove an element from the list (kucode=%d)\n"),kucode);
		return E_KU2;
	}
	
	if ( shutdown(id->sock,2)!=0 )
		plog(gettext("Failed to shut down the socket (fd=%d): %s\n"),id->sock,strerror(errno));
	if ( close(id->sock)!=0 )
		plog(gettext("Failed to close the socket (fd=%d): %s\n"),id->sock,strerror(errno));
	
	plog(gettext("A connection closed (fd=%d)\n"),id->sock);
	
	dfree(id);
	
	pstop();
	return E_NONE;
}
