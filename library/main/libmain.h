/***************************************************************************
 *            libmain.h
 *
 *  Fri Apr  7 17:42:17 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __LIBMAIN_H__
#define __LIBMAIN_H__

#include "ecode.h"

/*
	Статус сессии
*/
typedef
enum
{
	SOD_NOT_CONNECTED,
	SOD_CONNECTED
}	sod_status_t;

/*
	Заголовок сессии
*/
typedef
struct
{
	sod_status_t status;
	ecode_t ecode;
	char *errortext;
	int socket;
}	sod_session;

sod_session *sod_init( void );
int sod_halt( sod_session *session );

#endif
