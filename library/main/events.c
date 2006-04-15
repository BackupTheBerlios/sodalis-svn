/***************************************************************************
 *            events.c
 *
 *  Sat Apr 15 22:17:41 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdlib.h>

#include "events.h"
#include "errors/debug.h"
#include "ecode.h"

int sod_assign_event( sod_session *session, sod_event_t event, sod_callback_f *callback )
{
	pstart();
	pstop();
	return SOD_OK;
}

int sod_drop_event( sod_session *session, sod_event_t event )
{
	pstart();
	pstop();
	return SOD_OK;
}

sod_callback_f *sod_get_event( sod_session *session, sod_event_t event )
{
	pstart();
	pstop();
	return NULL;
}
