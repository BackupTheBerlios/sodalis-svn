/***************************************************************************
 *            network.h
 *
 *  Tue Jan 24 18:15:04 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __NETWORK_H__
#define __NETWORK_H__

#define BUFFER_SIZE 2048
#define MINIMUM_BUFSIZE 64

extern int sock;

/*
	1.	Соединение
	2.	host - адрес соединения
		port - порт соединения
	3.	Не 0, если ошибка
	4.	---
*/
int net_connect( char *host, u_int16_t port );

/*
	1.	Закрытие соединения
	2.	---
	3.	Не 0, если ошибка
	4.	---
*/
int net_disconnect( void );

/*
	1.	Отправка сообщения
	2.	msg - сообщение
	3.	Не 0, если ошибка
	4.	---
*/
int net_send( char *msg );

/*
	1.	Принять новые данные
	2.	---
	3.	Не 0, если ошибка
	4.	---
*/
int net_recv( void );

/*
	1.	Получение одного нового сообщения
	2.	msg - полученное сообщение
	3.	Не 0, если нет сообщений
	4.	---
*/
int net_msg( char **msg );

int net_start_data( void );

int net_stop_data( void );

int net_get_data( void );

#endif
