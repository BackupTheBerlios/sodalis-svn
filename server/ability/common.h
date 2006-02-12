/***************************************************************************
 *            common.h
 *
 *  Tue Jan 10 11:51:30 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef ABIL__COMMON_H__
#define ABIL__COMMON_H__

//	общие заголовки для возможностей
#include <libintl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "user/user.h"
#include "database/db.h"
#include "errors/debug.h"
#include "parse/parse.h"
#include "log/log.h"
#include "other/other.h"

//	перевод строки в число
#define abil_num( dst, n, p, abil_id ) \
dst=strtol(p_arg[n],&p,10); \
if ( *p!=0 ) \
{ \
	usr_write(usr,"DISCON CMD"); \
	plog(gettext("Invalid %d parameter (uid=%d on %s)\n"),n,usr->id,abil_id); \
	return 1; \
}

#define abil_str2num( dst, str, p, abil_id ) \
dst=strtol(str,&p,0); \
if ( *p!=0 ) \
{ \
	plog(gettext("Invalid number (uid=%d on %s)\n"),usr->id,abil_id); \
	SENDU(usr,"FAILED "abil_id); \
	return 0; \
}

//	отправка сообщения пользователю  с прверкой ошибок
#define SENDU( usr, msg ) \
if ( usr_write(usr,msg)!=E_NONE ) return 1

//	провера количестка параметров
#define CHECK_ARGC( c, abil_id ) \
if ( p_argc!=c ) \
{ \
	usr_write(usr,"DISCON CMD"); \
	plog(gettext("Invalid count of parameters (uid=%d on %s)\n"),usr->id,abil_id); \
	return 1; \
}

//	проверка уровня доступа
#define CHECK_LEVEL( lvl, abil_id ) \
if ( usr->level<lvl ) \
{ \
	pdebug("%d %d\n",usr->level,lvl); \
	plog(gettext("Invalid level (uid=%d on %s)\n"),usr->id,abil_id); \
	usr_write(usr,"DISCON CMD"); \
	return 1; \
}

//	сообщение об ошибке
#define REQ_FAIL( abil_id ) \
plog(gettext("Request failed (uid=%d on %s)\n"),usr->id,abil_id)

#endif
