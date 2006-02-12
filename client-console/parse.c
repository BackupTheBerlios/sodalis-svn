/***************************************************************************
 *            parse.c
 *
 *  Tue Jan 24 23:25:10 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <unistd.h>

#include "parse.h"
#include "network.h"
#include "ability.h"
#include "errors/debug.h"
#include "scc.h"

char *p_arg[ARG_CNT];
int p_argc, p_size[ARG_CNT];
char p_buf[BUFFER_SIZE];

int parse_out( void )
{
	char *msg;
	pstart();
	
	if ( data_mode==DATA_RAW )
	{
		
	}	else
	{
		while ( !net_msg(&msg) )
		{
			if ( parse(msg,PATYPE_ONESPACE)<=0 )
				return -1;
			if ( abil_serv() ) return -1;
		}
	}
	
	pstop();
	return 0;
}

int parse_in( void )
{
	static char buf[BUFFER_SIZE];
	static int cur=0, pos=0;
	char *msg, *p;
	int i;
	pstart();
	
	/*
		чтение из консоли в буффер
	*/
	if ( BUFFER_SIZE-pos<MINIMUM_BUFSIZE )
	{
		if ( cur==0 )
		{
			printf(gettext("Console message buffer is overflowed\n"));
			return -1;
		}
		memmove(buf,buf+cur,pos-cur);
		pos-=cur;
		cur=0;
	}
	i=read(0,buf+pos,BUFFER_SIZE-pos);
	pos+=i;
	
	/*
		разделение команд
	*/
	for (;;)
	{
		for ( p=buf+cur; (p<buf+pos) && (*p!=10); p++ );
		if ( (*p==10) && (p<buf+pos) )
		{
			*p=0;
			msg=buf+cur;
			cur=p-buf+1;
		}	else
		{
			break;
		}
		
		if ( parse(msg,PATYPE_MANYSPACES)<=0 )
		{
			printf(gettext("Please check the command syntax\n"));
			break;
		}
		
		if ( abil_usr() ) return -1;
	}
	
	pstop();
	return 0;
}

int parse( char *msg, int type )
{
	int i=1, qu=0, tmp;
	char *src=msg;
	char *dst=p_buf;
	char *p;
	pstart();
	
	p_arg[0]=dst;
	if ( *src==0 ) return 0;
	while ( *src!=0 )
	{
		switch ( *src )
		{
			case ' ':
				if ( !qu )
				{
					if ( type==PATYPE_MANYSPACES )
					{
						while ( src[1]==' ' ) src++;
						if ( src[1]==0 ) continue;
					}
						
					*(dst++)=0;
					if ( i==ARG_CNT )
					{
						printf(gettext("Cannot parse: too many parameters\n"));
						return 0;
					}	else
					{
						p_size[i-1]=dst-p_arg[i-1]-1;
						p_arg[i++]=dst;
						src++;
					}
					
				}	else
				{
					*(dst++)=*(src++);
				}
				break;
			case '/':
				tmp=strtol(++src,&p,0);
				if ( p==src )
				{
					printf(gettext("Cannot parse: after '/' must be a number\n"));
					return 0;
				}
				if ( (tmp<0) || (tmp>255) )
				{
					printf(gettext("Cannot parse: symbol code is invalid\n"));
					return 0;
				}
				*(dst++)=(char)tmp;
				src=p;
				break;
			case '@':
				if ( *(++src)!=0 )
					*(dst++)=*(src++);
				else
				{
					printf(gettext("Cannot parse: '@` at the end\n"));
					return 0;
				}
				break;
			case '"':
				qu=1-qu;
				src++;
				break;
			default:
				*(dst++)=*(src++);
		}
	}
	*dst=0;
	if ( qu )
	{
		printf(gettext("Cannot parse: quotation mark is needed\n"));
		return 0;
	}
	
	p_size[i-1]=dst-p_arg[i-1];
	p_argc=i;
	pstop();
	return i;
}
