/***************************************************************************
 *            network.c
 *
 *  Sat Apr  8 12:06:07 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifdef USE_GETTEXT
#include <libintl.h>
#endif

#include "network.h"
#include "errors/debug.h"
#include "ecode.h"

int sod_connect( sod_session *session, char *host, u_int16_t port, char *login, char *password )
{
	pstart();
	session->status=SOD_CONNECTED;
	pstop();
	return SOD_OK;
}

int sod_disconnect( sod_session *session )
{
	pstart();
	session->status=SOD_NOT_CONNECTED;
	pstop();
	return SOD_OK;
}

int sod_exchange_data( sod_session *session )
{
	pstart();
	
	if ( session->status!=SOD_CONNECTED )
	{
		sod_throw_error(SE_NOT_CONNECTED,"The current session is not connected");
	}
	
	pstop();
	return SOD_OK;
}
