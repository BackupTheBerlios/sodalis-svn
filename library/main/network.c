/***************************************************************************
 *            network.c
 *
 *  Sat Apr  8 12:06:07 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

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
