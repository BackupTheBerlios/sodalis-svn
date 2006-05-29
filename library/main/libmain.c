/***************************************************************************
 *            libmain.c
 *
 *  Fri Apr  7 17:43:05 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdlib.h>

#ifdef USE_GETTEXT
#include <libintl.h>
#endif

#include "libmain.h"
#include "ecode.h"
#include "errors/debug.h"
#include "other/other.h"
#include "network.h"

int main()
{
}


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
	session->errorfunc="";
	session->socket=-1;
	
	session->inpos=0;
	session->instart=0;
	session->outpos=0;
	session->outstart=0;
	
	pstop();
	return session;
}

int sod_halt( sod_session *session )
{
	pstart();
	
	if ( session->ecode!=SE_NONE )
	{
		#ifdef USE_GETTEXT
		printf(gettext(vstr("WARNING: unmanaged error:\n\tfunction: %s\n\terror: %s\n\tcode: %d\n", \
			session->errorfunc,session->errortext,session->ecode)));
		#else
		printf(vstr("WARNING: unmanaged error:\n\tfunction: %s\n\terror: %s\n\tcode: %d\n", \
			session->errorfunc,session->errortext,session->ecode));
		#endif
	}
	
	if ( session->status==SOD_CONNECTED )
	{
		sod_disconnect(session);
	}
	
	dfree(session);
	
	pstop();
	return SOD_OK;
}
