/***************************************************************************
 *            events.h
 *
 *  Sat Apr 15 22:17:30 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef SOD__EVENTS_H__
#define SOD__EVENTS_H__
#include "errors/open_code.h"

#include "libmain.h"

/*
	Типы событий
*/
typedef
enum
{
	SOD_EV_ERROR,
	SOD_EV_DISCON,
	SOD_EV_AUTH_OK
}	sod_event_t;

/*
	Обработчик событий
*/
typedef
void (*sod_callback_f)( sod_session *session, sod_event_t event, ... );

/*
	Обработчик событий по умолчаний
*/
void sod_ev_default( sod_session *session, sod_event_t event, ... );

/*
	1.	Привязать обработчик к событию
	2.	session - сессия
		event - привязываемое событие
		callback - привязываемый обработчик
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	SE_EVENT - неверный код события
*/
int sod_assign_event( sod_session *session, sod_event_t event, sod_callback_f callback );

/*
	1.	Освободить событие от обработчика
	2.	session - сессия
		event - событие
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	SE_EVENT - неверный код события
*/
int sod_drop_event( sod_session *session, sod_event_t event );

/*
	1.	Вернуть обработчик события
	2.	session - сессия
		event - событие
	3.	Обработчик события, или NULL, если произошла ошибка
	4.	SE_EVENT - неверный код события
*/
sod_callback_f sod_get_event( sod_session *session, sod_event_t event );

/*
	События:
*/
extern sod_callback_f sod_ev_error;
extern sod_callback_f sod_ev_disconnected;
extern sod_callback_f sod_ev_auth_ok;

#include "errors/close_code.h"
#endif
