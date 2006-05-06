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

sod_callback_f sod_ev_error=sod_ev_default;
sod_callback_f sod_ev_disconnected=sod_ev_default;
sod_callback_f sod_ev_auth_ok=sod_ev_default;

void sod_ev_default( sod_session *session, sod_event_t event, ... )
{
	pstart();
	pdebug("An event has accured: %d\n",event);
	pstop();
}

int sod_assign_event( sod_session *session, sod_event_t event, sod_callback_f callback )
{
	pstart();
	switch ( event )
	{
		case SOD_EV_ERROR:
			sod_ev_error=callback;
			break;
		case SOD_EV_DISCON:
			sod_ev_disconnected=callback;
			break;
		case SOD_EV_AUTH_OK:
			sod_ev_auth_ok=callback;
			break;
		default:
			sod_throw_error(SE_EVENT,"Wrong event type");
	}
	pstop();
	return SOD_OK;
}

int sod_drop_event( sod_session *session, sod_event_t event )
{
	pstart();
	switch ( event )
	{
		case SOD_EV_ERROR:
			sod_ev_error=sod_ev_default;
			break;
		case SOD_EV_DISCON:
			sod_ev_disconnected=sod_ev_default;
			break;
		case SOD_EV_AUTH_OK:
			sod_ev_auth_ok=sod_ev_default;
			break;
		default:
			sod_throw_error(SE_EVENT,"Wrong event type");
	}
	pstop();
	return SOD_OK;
}

sod_callback_f sod_get_event( sod_session *session, sod_event_t event )
{
	sod_callback_f callback;
	pstart();
	switch ( event )
	{
		case SOD_EV_ERROR:
			callback=sod_ev_error;
			break;
		case SOD_EV_DISCON:
			callback=sod_ev_disconnected;
			break;
		case SOD_EV_AUTH_OK:
			callback=sod_ev_auth_ok;
			break;
		default:
			sod_throw_value(SE_EVENT,"Wrong event type",NULL);
	}
	pstop();
	return callback;
}
