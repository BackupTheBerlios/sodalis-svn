/***************************************************************************
 *            libmain.c
 *
 *  Fri Apr  7 17:43:05 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdlib.h>

#include "libmain.h"
#include "ecode.h"
#include "errors/debug.h"
#include "network.h"

sod_session *sod_init( void )
{
	sod_session *session;
	pstart();
	
	session=dmalloc(sizeof(sod_session));
	if ( session==NULL )
		return NULL;
	
	session->status=SOD_NOT_CONNECTED;
	session->ecode=SE_NONE;
	session->errortext="";
	session->socket=-1;
	
	pstop();
	return session;
}

int sod_halt( sod_session *session )
{
	pstart();
	
	if ( session->status==SOD_CONNECTED )
	{
		sod_disconnect(session);
	}
	
	dfree(session);
	
	pstop();
	return SOD_OK;
}
