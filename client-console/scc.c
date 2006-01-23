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

#include "errors/debug.h"

#define VERSION 1
#define PROTOCOL sodalisnpv2

int main( int argc, char *argv[] )
{
	int i;
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
	for ( i=1; i<argc; i++ )
	{
		//	помощь
		if ( strcmp(argv[i],"--help")==0 )
		{
			printf(gettext("Usage: %s [parametre]\nParametres:\n"),argv[0]);
			printf(gettext("\t'--help`: show out this help;\n"));
			printf(gettext("\t'--about`: show the information about the server;\n"));
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
		//	неверный ключ
		{
			printf(gettext("Invalid parametre '%s`, see '%s help`\n"),argv[i],argv[0]);
			return EXIT_FAILURE;
		}
	}
	
	pstop();
	return EXIT_SUCCESS;
}
