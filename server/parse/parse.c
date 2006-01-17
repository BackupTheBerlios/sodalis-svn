/***************************************************************************
 *            parse.c
 *
 *  Mon Nov 14 17:54:30 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <libintl.h>
#include <string.h>
#include <stdlib.h>

#include "parse/parse.h"
#include "main/ecode.h"
#include "user/user.h"
#include "errors/debug.h"
#include "log/log.h"
#include "ability/ability.h"

char *p_arg[PARSER_ARG_CNT];
int p_argc;
char p_buf[BUFFER_SIZE_USER];

int parse_msg( usr_record *usr )
{
	pstart();
	
	if ( !strcmp(p_arg[0],"LOGOUT") )
	{
		usr_write(usr,"DISCON REQ");
		return 1;
	}	else
	
	if ( !strcmp(p_arg[0],"MSGU") )
	{
		if ( abil_msgu(usr) ) return 1;
	}	else
	if ( !strcmp(p_arg[0],"LSM") )
	{
		if ( abil_lsm(usr) ) return 1;
	}	else
	
	if ( !strcmp(p_arg[0],"INVITE") )
	{
		if ( abil_invite(usr) ) return 1;
	}	else
	if ( !strcmp(p_arg[0],"INVACC") )
	{
		if ( abil_invacc(usr) ) return 1;
	}	else
	if ( !strcmp(p_arg[0],"INVREJ") )
	{
		if ( abil_invrej(usr) ) return 1;
	}	else
	if ( !strcmp(p_arg[0],"LEAVE") )
	{
		if ( abil_leave(usr) ) return 1;
	}	else
	if ( !strcmp(p_arg[0],"CHGRP") )
	{
		if ( abil_chgrp(usr) ) return 1;
	}	else
	if ( !strcmp(p_arg[0],"GROUP") )
	{
		if ( abil_group(usr) ) return 1;
	}	else
	if ( !strcmp(p_arg[0],"LSF") )
	{
		if ( abil_lsf(usr) ) return 1;
	}	else
	
	if ( !strcmp(p_arg[0],"STOP") )
	{
		if ( abil_stop(usr) ) return 1;
	}	else
	if ( !strcmp(p_arg[0],"BREAK") )
	{
		if ( abil_break(usr) ) return 1;
	}	else
	
	{
		pdebug("recv %s\n",p_arg[0]);
		usr_write(usr,"DISCON CMD");
		return 1;
	}
	
	pstop();
	return 0;
}

int parse( char *msg )
{
	int i=1, qu=0, tmp;
	char *src=msg;
	char *dst=p_buf;
	char *p;
	pstart();
	
	p_arg[0]=dst;
	while ( *src!=0 )
	{
		switch ( *src )
		{
			case ' ':
				if ( !qu )
				{
					*(dst++)=0;
					if ( i==PARSER_ARG_CNT )
					{
						plog(gettext("Cannot parse: too many parametres\n"));
						return 0;
					}	else
						p_arg[i++]=dst;
					src++;
				}	else
				{
					*(dst++)=*(src++);
				}
				break;
			case '/':
				tmp=strtol(++src,&p,0);
				if ( p==src )
				{
					plog(gettext("Cannot parse: after '/' must be a number\n"));
					return 0;
				}
				if ( (tmp<0) || (tmp>255) )
				{
					plog(gettext("Cannot parse: symbol code is invalid\n"));
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
					plog(gettext("Cannot parse: '@` at the end\n"));
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
		plog(gettext("Cannot parse: quotation mark is needed\n"));
		return 0;
	}
	
	p_argc=i;
	pstop();
	return i;
}
