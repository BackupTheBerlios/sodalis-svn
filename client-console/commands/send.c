/***************************************************************************
 *            send.c
 *
 *  Fri Jan 27 08:28:34 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <libintl.h>
#include <errno.h>

#include "ability.h"
#include "errors/debug.h"
#include "send.h"
#include "parse.h"
#include "other/other.h"
#include "network.h"

int fd;
struct stat st;

int open_file( char *path )
{
	pstart();
	fd=open(path,O_RDONLY);
	if ( fd==-1 )
	{
		printf(gettext("Failed to open a file.\n"));
		return -1;
	}
	if ( fstat(fd,&st)==-1 )
	{
		printf(gettext("Failed to get stats of the file.\n"));
		close(fd);
		return -1;
	}
	pstop();
	return st.st_size;
}

//	send photo caption description path albumid
int abil_send( void )
{
	int sz;
	pstart();
	
	uppertext(p_arg[1]);
	if ( !strcmp(p_arg[1],"PHOTO") )
	{
		int albumid;
		if ( p_argc!=6 )
		{
			_
			inv_syn();
			return 0;
		}
		if ( !str2num(p_arg[5],&albumid) )
		{
			_
			inv_syn();
			return 0;
		}
		sz=open_file(p_arg[4]);
		if ( sz==-1 ) return 0;
		if ( net_send(vstr("PHOTO \"%s\" \"%s\" %d %d",p_arg[2],p_arg[3],albumid,sz)) )
		{
			_
			close(fd);
			return 0;
		}
	}	else
	{
		inv_syn();
		return 0;
	}
	
	pstop();
	return 0;
}

int abil_send_start( void )
{
	int i=0;
	char buf[st.st_size];
	pstart();
	
	while ( i<st.st_size )
	{
		int t=read(fd,buf+i,st.st_size-i);
		if ( t==-1 )
		{
			printf(gettext("Failed to read from file: %s\n"),strerror(errno));
			close(fd);
			return 0;
		}
		if ( write(sock,buf,t)!=t )
		{
			printf(gettext("Failed to send data to the socket: %s\n"),strerror(errno));
			return 0;
		}
		i+=t;
	}
	
	pstop();
	return 0;
}
