/***************************************************************************
 *            msg.c
 *
 *  Thu Jan 26 17:29:42 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>
#include <libintl.h>
#include <stdlib.h>
#include <string.h>

#include "ability.h"
#include "parse.h"
#include "msg.h"
#include "errors/debug.h"
#include "other/other.h"
#include "network.h"

//	msg/message {group gid}/{uid} [save/{not save}] "message text"
int abil_msg( void )
{
	int ugid=-1, gr=0, save=0;
	int i;
	char *cmd;
	pstart();
	
	for ( i=1; i<p_argc-1; i++ )
	{
		uppertext(p_arg[i]);
		pdebug("%d: '%s'\n",i,p_arg[i]);
		if ( !strcmp(p_arg[i],"GROUP") && (gr==0) )
		{
			gr=1;
			i++;
			if ( !str2num(p_arg[i],&ugid) && (i>=p_argc-1) )
			{
				inv_syn();
				return 0;
			}
		}	else
		if ( !strcmp(p_arg[i],"SAVE") && (save==0) )
		{
			save=1;
		}	else
		if ( !strcmp(p_arg[i],"NOT") && (save==0) )
		{
			i++;
			uppertext(p_arg[i]);
			if ( strcmp(p_arg[i+1],"SAVE") && (i>=p_argc-1) )
			{
				inv_syn();
				return 0;
			}
			save=-1;
		}	else
		if ( ugid==-1 )
		{
			if ( !str2num(p_arg[i],&ugid) )
			{
				inv_syn();
				return 0;
			}
		}	else
		{
			inv_syn();
			return 0;
		}
	}
	pdebug("%d: '%s'\n",i,p_arg[i]);
	if ( gr==1 )
	{
		cmd="MSGG";
	}	else
	{
		cmd="MSGU";
	}
	switch ( save )
	{
		case -1:
			cmd=vstr("%s %d i \"%s\"",cmd,ugid,p_arg[i]);
			break;
		case 0:
			cmd=vstr("%s %d \"%s\"",cmd,ugid,p_arg[i]);
			break;
		case 1:
			cmd=vstr("%s %d s \"%s\"",cmd,ugid,p_arg[i]);
			break;
	}
	
	if ( net_send(cmd) ) return -1;
	
	pstop();
	return 0;
}

int abil_msg_serv( void )
{
	int uid, msgid, time;
	pstart();
	if ( p_argc==3 )
	{	// оповещение
		if ( !str2num(p_arg[1],&msgid) || !str2num(p_arg[2],&uid) )
		{
			inv_serv();
			return -1;
		}
		//printf(gettext("You have a new message, msgid %d, from user %d.\n"),msgid,uid);
		if ( net_send(vstr("MSGU %d",msgid)) ) return -1;
	}	else
	if ( p_argc==5 )
	{	// полное сообщение
		if ( !str2num(p_arg[1],&msgid) || \
				!str2num(p_arg[2],&uid) || \
				!str2num(p_arg[3],&time) )
		{
			inv_serv();
			return -1;
		}
		printf(gettext("Message %d from user %d at %d: \"%s\".\n"),msgid,uid,time,p_arg[4]);
	}	else
	{
		inv_serv();
		return -1;
	}
	pstop();
	return 0;
}
