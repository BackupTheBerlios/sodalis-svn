/***************************************************************************
 *            ecode.h
 *
 *  Sat Oct 22 12:23:56 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __ECODE_H__
#define __ECODE_H__

/*
	коды ошибок
*/
typedef
enum
{
	E_NONE,
	E_ANY,
	E_MEMORY,
	E_FILE,
	E_KU2,
	E_SOCKET,
	E_SOCKOPT,
	E_FCNTL,
	E_BIND,
	E_LISTEN,
	E_SELECT,
	E_ACCEPT,
	E_AUTH,
	E_SQL_CONNECT,
	E_SQL_QUERY,
	E_SQL_RESULT,
	E_SQL_FETCH,
	E_DISCONNECT,
	E_AGAIN,
	E_ARG,
	E_OBUFFER,
	E_UBUFFER
}	ecode_t;

extern ecode_t ecode;

/*
	перевод кода ошибки в текстовый вид
*/
char *errtext( ecode_t c );

#endif
