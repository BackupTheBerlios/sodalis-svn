/***************************************************************************
 *            ability.c
 *
 *  Wed Jan 25 11:20:33 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <libintl.h>
#include <stdlib.h>
#include <string.h>

#include "ability.h"
#include "errors/debug.h"
#include "parse.h"
#include "scc.h"
#include "network.h"

#include "other/other.h"

#include "commands/msg.h"
#include "commands/send.h"

int max_data_size;

int str2num( char *str, int *data )
{
	char *p;
	int res;
	res=strtol(str,&p,10);
	*data=res;
	if ( *p!=0 )
	{
		printf(gettext("Cannot convert string to an integer!\n"));
		pdebug("%s\n",str);
		return 0;
	}	else
	{
		return 1;
	}
}

void inv_serv( void )
{
	printf(gettext("Server seems to use unsupported network protocol!\n"));
}

void inv_syn( void )
{
	printf(gettext("Wrong command syntax!\n"));
}

void uppertext( char *text )
{
	char *p;
	for ( p=text; *p; p++ ) *p=toupper(*p);
}

int abil_serv( void )
{
	pstart();
	
	if ( !strcmp(p_arg[0],"SERVER") )
	{
		if ( p_argc!=3 )
		{
			inv_serv();
			return -1;
		}
		if ( strcmp(p_arg[1],STR(PROTOCOL))!=0 )
		{
			printf(gettext("Server`s protocol %s is not supported!\n"),p_arg[1]);
			return -1;
		}
		if ( !str2num(p_arg[2],&max_data_size) )
		{
			inv_serv();
			return -1;
		}
		if ( net_send(vstr("AUTH \"%s\" \"%s\"",login,password)) )
		{
			return -1;
		}
	}	else
	if ( !strcmp(p_arg[0],"DISCON") )
	{
		if ( p_argc!=2 )
		{
			inv_serv();
			return -1;
		}
		printf(gettext("Disconnection reason: "));
		if ( !strcmp(p_arg[1],"SIZE") )
		{
			printf(gettext("maximum message size is excessed.\n"));
		}	else
		if ( !strcmp(p_arg[1],"AUTH") )
		{
			printf(gettext("authorization failed.\n"));
		}	else
		if ( !strcmp(p_arg[1],"REQ") )
		{
			printf(gettext("by user request.\n"));
		}	else
		if ( !strcmp(p_arg[1],"CMD") )
		{
			printf(gettext("invalid command.\n"));
		}	else
		if ( !strcmp(p_arg[1],"ADM") )
		{
			printf(gettext("by the administrator request.\n"));
		}	else
		{
			inv_serv();
			return -1;
		}
		go_on=0;
	}	else
	if ( !strcmp(p_arg[0],"ACCEPTED") )
	{
		printf(gettext("Authorization is successful!\n"));
		//if ( net_send("LSM NEW") ) return -1;
		is_auth=1;
	}	else
	if ( !strcmp(p_arg[0],"MSGU") )
	{
		if ( abil_msg_serv() ) return -1;
	}	else
	if ( !strcmp(p_arg[0],"PHALLOW") )
	{
		if ( abil_send_start() ) return -1;
	}	else
	{
		inv_serv();
		//return -1;
		return 0;
	}
	
	pstop();
	return 0;
}


int abil_usr( void )
{
	pstart();
	
	uppertext(p_arg[0]);
	
	if ( !strcmp(p_arg[0],"RAW") )
	{
		if ( p_argc!=2 )
		{
			inv_syn();
		}	else
		{
			if ( net_send(p_arg[1]) ) return -1;
		}
	}	else
	if ( !strcmp(p_arg[0],"QUIT") )
	{
		if ( net_send("LOGOUT") ) return -1;
	}	else
	if ( !strcmp(p_arg[0],"MSG") || !strcmp(p_arg[0],"MESSAGE") )
	{
		if ( abil_msg() ) return -1;
	}	else
	if ( !strcmp(p_arg[0],"LS") || !strcmp(p_arg[0],"LIST") )
	{
		if ( abil_list() ) return -1;
	}	else
	if ( !strcmp(p_arg[0],"S") || !strcmp(p_arg[0],"SHOW") )
	{
		//if ( abil_show() ) return -1;
	}	else
	if ( !strcmp(p_arg[0],"RM") || !strcmp(p_arg[0],"DELETE") )
	{
		//if ( abil_delete() ) return -1;
	}	else
	if ( !strcmp(p_arg[0],"SEND") )
	{
		if ( abil_send() ) return -1;
	}	else
	{
		printf(gettext("Unknown command!\n"));
	}
	
	pstop();
	return 0;
}
