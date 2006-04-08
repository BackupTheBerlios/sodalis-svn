/***************************************************************************
 *            scc.c
 *
 *  Fri Apr  7 18:04:51 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <libintl.h>
#include <locale.h>
#include <stdlib.h>

#include "scc.h"
#include "errors/debug.h"

#include "main/libmain.h"
#include "main/network.h"

sod_session *session;

//	scc [parameters] [host] [login] [password]
int main( int argc, char *argv[] )
{
	pstart();
	
	/*
		поддержка языков
	*/
	setlocale(LC_ALL,"");
	bindtextdomain("sodaliscc","gettext");
	textdomain("sodaliscc");
	
	session=sod_init();
	sod_connect(session,NULL,0,NULL,NULL);
	sod_halt(session);
	
	pstop();
	return EXIT_SUCCESS;
}
