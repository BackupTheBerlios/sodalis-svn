/***************************************************************************
 *            scc.c
 *
 *  Mon Jan 23 11:20:52 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <locale.h>
#include <errno.h>

#include "errors/debug.h"
#include "network.h"
#include "parse.h"
#include "ability.h"

#define VERSION 1
#define PROTOCOL sodalisnpv2
#define PORT 1986

//	scc [parameters] [host] [loing] [password]
int main( int argc, char *argv[] )
{
	int i, go_on=1;
	u_int16_t port=PORT;
	char *host;
	pstart();
	
	/*
		поддержка языков
	*/
	setlocale(LC_ALL,"");
	bindtextdomain("sodaliscc","gettext");
	textdomain("sodaliscc");
	
	/*
		проверка параметров командной строки
	*/
	if ( argc<2 )
	{
		printf(gettext("Invalid command line parameter count, see '%s --help`\n"),argv[0]);
		return EXIT_FAILURE;
	}
	for ( i=1; i<argc; i++ )
	{
		//	помощь
		if ( strcmp(argv[i],"--help")==0 )
		{
			printf(gettext("Usage: %s [parameters] [host] [login] [password]\nParameters:\n"),argv[0]);
			printf(gettext("\t'--help`: show out this help;\n"));
			printf(gettext("\t'--about`: show the information about the server;\n"));
			printf(gettext("\t'-p [port]`: define a non-standart port ("STR(PORT)");\n"));
			return EXIT_SUCCESS;
		}	else
		//	информация о программе
		if ( strcmp(argv[i],"--about")==0 )
		{
			printf(gettext("SODALIS console client by Jelkin Anton kane@mail.berlios.de\n"\
				"Version: "STR(VERSION)"\n"\
				"Protocol: "STR(PROTOCOL)"\n"\
				"Build on: %s %s\n"),__DATE__,__TIME__);
			return EXIT_SUCCESS;
		}	else
		if ( strcmp(argv[i],"-p")==0 )
		{
			char *p;
			int tmp;
			if ( i+1==argc )
			{
				printf(gettext("Invalid parameter '-p` usage, see '%s help`\n"),argv[0]);
				return EXIT_FAILURE;
			}
			tmp=strtol(argv[++i],&p,10);
			if ( (*p!=0) || (tmp<0) || (tmp>65545) )
			{
				printf(gettext("Invalid port\n"));
				return EXIT_FAILURE;
			}
			port=(u_int16_t)tmp;
		}	else
		//	параметры закончились
		{
			if ( i+3!=argc )
			{
				printf(gettext("Invalid command line parameter count, see '%s --help`\n"),argv[0]);
				return EXIT_FAILURE;
			}
			host=argv[i];
			login=argv[i+1];
			password=argv[i+2];
			break;
		}
	}
	
	/*
		соединение и главный цикл
	*/
	if ( net_connect(host,port) )
		return EXIT_FAILURE;
	while ( go_on )
	{
		fd_set rset;
		FD_ZERO(&rset);
		FD_SET(0,&rset);
		FD_SET(sock,&rset);
		if ( select(sock+1,&rset,NULL,NULL,NULL)==-1 )
		{
			printf(gettext("Select function failed: %s\n"),strerror(errno));
			net_disconnect();
			return EXIT_FAILURE;
		}
		
		if ( FD_ISSET(0,&rset) )
		{
			if ( parse_in() )
			{
				net_disconnect();
				return EXIT_FAILURE;
			}
		}
		
		if ( FD_ISSET(sock,&rset) )
		{
			if ( net_recv() || parse_out() )
			{
				net_disconnect();
				return EXIT_FAILURE;
			}
		}
	}
	if ( net_disconnect() )
		return EXIT_FAILURE;
	
	pstop();
	return EXIT_SUCCESS;
}
