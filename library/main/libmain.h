/***************************************************************************
 *            libmain.h
 *
 *  Fri Apr  7 17:42:17 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef SOD__LIBMAIN_H__
#define SOD__LIBMAIN_H__
#include "errors/open_code.h"

#include "ecode.h"

/*
	Статус сессии
*/
typedef
enum
{
	SOD_NOT_CONNECTED,		// не соеденён
	SOD_CONNECTED			// соеденён
}	sod_status_t;

/*
	Заголовок сессии
*/
typedef
struct
{
	sod_status_t status;	// статус соединения
	secode_t ecode;		// код последней ошибки
	char *errortext;		// текст последней ошибки
	int socket;				// сокет соединения
}	sod_session;

/*
	1.	Инициализация библиотеки, создание сессии
	2.	---
	3.	Указатель на сессию, или NULL, если не хватило памяти
	4.	---
*/
sod_session *sod_init( void );

/*
	1.	Закрытие сессии
	2.	session - сессия
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	ntni
*/
int sod_halt( sod_session *session );

/*
	1.	Цикл итерации сессии, обмен сообщениями с сервером и
		вызов обработчиков событий
	2.	session - сессия
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	ntni
*/
int sod_iterate( sod_session *session );

#include "errors/close_code.h"
#endif
