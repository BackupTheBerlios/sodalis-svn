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
	Размер IO буффера
*/
#ifndef SOD_BUFFER_SIZE
#define SOD_BUFFER_SIZE 2048
#endif

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
	char *errorfunc;		// функция, где произошла ошибка
	int socket;				// сокет соединения
	
							// сетевые буфферы
	char inbuf[SOD_BUFFER_SIZE];
	char outbuf[SOD_BUFFER_SIZE];
	int inpos, instart;		// текущие (последние) и начальные позиции
	int outpos, outstart;	// в буфферах
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
