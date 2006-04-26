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
	Параметры для передачи в поток
*/
typedef
struct
{
	sod_session *session;
	char *login;
	char *password;
} sod_thread_arg_t;

/*
	1.	Открыть соединение и авторизироваться на сервере
		При этом создаётся отдельный поток, который обрабатывает сообщения
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
	1.	Закрыть соединение + завершить поток
	2.	session - сессия
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	ntni
*/
int sod_disconnect( sod_session *session );

/*
	1.	Обменяться данными и вызвать обработчики (потоковая функция)
	2.	arg - аргументы
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	ntni
*/
int sod_thread( sod_thread_arg_t *arg );

#include "errors/close_code.h"
#endif
