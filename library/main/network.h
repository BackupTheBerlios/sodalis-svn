/***************************************************************************
 *            network.h
 *
 *  Sat Apr  8 12:06:01 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef SOD__NETWORK_H__
#define SOD__NETWORK_H__
#include "errors/open_code.h"

#include <sys/types.h>
#include "libmain.h"

/*
	1.	Открыть соединение и авторизироваться на сервере
	2.	session - сессия
		host - адрес сервера
		port - порт соединения
		login - имя соединения
		password - пароль соединения
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	ntni
*/
int sod_connect( sod_session *session, char *host, u_int16_t port, char *login, char *password );

/*
	1.	Закрыть соединение
	2.	session - сессия
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	ntni
*/
int sod_disconnect( sod_session *session );

/*
	1.	Обменяться данными
	2.	session - сессия
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	ntni
*/
int sod_exchange_data( sod_session *session );

#include "errors/close_code.h"
#endif
