/***************************************************************************
 *            ecode.h
 *
 *  Fri Apr  7 17:44:42 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __ECODE_H__
#define __ECODE_H__

/*
	Коды возврата функций
*/
#define SOD_OK 0
#define SOD_ERROR -1

/*
	Коды ошибок
*/
typedef
enum
{
	E_NONE,
	E_MEMORY,
	E_KU2
}	ecode_t;

#endif
