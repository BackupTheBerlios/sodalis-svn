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
#include "main/events.h"

sod_session *session;

void event_disconnect( sod_session *session, sod_event_t event, ... )
{
	pstart();
	printf("you are disconnected!\n");
	exit(0);
	pstop();
}

void event_error( sod_session *session, sod_event_t event, ... )
{
	pstart();
	printf("ERROR!!!\n");
	printf("%s: %s (%d)\n",session->errorfunc,session->errortext,session->ecode);
	session->ecode=SE_NONE;
	pstop();
}

//	scc [parameters] [host] [login] [password]
int main( int argc, char *argv[] )
{
	int i;
	int *a;
	pstart();
	a=dmalloc(sizeof(int));
	/*
		поддержка языков
	*/
	setlocale(LC_ALL,"");
	bindtextdomain("sodaliscc","gettext");
	textdomain("sodaliscc");
	
	session=sod_init();
	sod_assign_event(session,SOD_EV_DISCON,event_disconnect);
	sod_assign_event(session,SOD_EV_ERROR,event_error);
	sod_connect(session,"localhost",1986,"kane","passw");
	while ( session->status!=SOD_AUTHORIZED );
	sod_logout(session);
	scanf("%d",&i);
	sod_halt(session);
	
	pstop();
	return EXIT_SUCCESS;
}
