/***************************************************************************
 *            user.h
 *
 *  Fri Oct 28 12:32:21 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __USER_H__
#define __USER_H__

//	размер буффера приёма и отправки сообщений
#define BUFFER_SIZE_USER 1024
//	минимальный размер буффера, который должен быть свободен
#define BUFFER_MINIMUM_FREE 128

/*
	флаги datamode
*/
#define UF_DATA_HERE 1
#define UF_DATA_IN 2
#define UF_DATA_OUT 4
#define UF_DATAPHOTOIN 8

#include "network/net_id.h"

//	запись пользователя
typedef
struct
{	
	//	информация о пользователе
	int id;
	int level;	// уровень доступа
	int max_gr, max_ph;	// максимальное кол-во групп и фотоальбомов
	
	//	datamode
	unsigned int dataflags;
	void *indata, *outdata;
	int indata_sz, indata_cur, outdata_sz, outdata_cur;
	
	//	сеть
	int sock;
	char in[BUFFER_SIZE_USER];
	int inpos, instart, msg;
	char out[BUFFER_SIZE_USER];
	int outpos, outstart;
	net_id *netid;
}	usr_record;

#include "main/ecode.h"

/*
	1.	Инициализация подсистемы
	2.	---
	3.	Код ошибки
	4.	любая ошибка db_init()
		E_KU2 - ошибка kaneutil2
		E_NONE - нет ошибок
		
*/
ecode_t usr_init( void );

/*
	1.	Деинициализация подсистемы
	2.	---
	3.	Код ошибки
	4.	любая ошибка db_halt()
		E_KU2 - ошибка kaneutil2
		E_NONE - нет ошибок
*/
ecode_t usr_halt( void );

/*
	1.	Добавление нового соединения
	2.	sock - дескриптор сокета
	3.	Код ошибки
	4.	любая ошибка usr_write()
		E_MEMORY - ошибка памяти
		E_KU2 - ошибка kaneutil2
		E_NONE - нет ошибок
*/
ecode_t usr_add( net_id *id );

/*
	1.	Отсоединение/выход пользователя
	2.	usr - запись пользователя
	3.	Код ошибки
	4.	E_KU2 - ошибка kaneutil2
		E_NONE - нет ошибок
*/
ecode_t usr_rem( usr_record *usr );

/*
	1.	Авторизация пользователя
	2.	usr - запись пользователя
	3.	Код ошибки
	4.	любая ошибка usr_write()
		E_ARG - неверная команда авторизации
		E_AUTH - данные в БД не совпали с запрошенными
		E_KU2 - ошибка kaneutil2
		E_NONE - нет ошибок
*/
ecode_t usr_auth( usr_record *usr );

/*
	1.	Поиск пользователя по ID в online
	2.	id - идентификатор
	3.	Запись пользователя
	4.	любая ошибка abtree_search
*/
usr_record *usr_online( int id );

/*
	1.	Приём, обработка и отправка сообщений пользователю
	2.	sock - дескриптор сокета
	3.	Код ошибки
	4.	любая ошибка usr_read(), usr_getmsg() (кроме E_AGAIN) и usr_auth()
		E_KU2 - ошибка kaneutil2
		E_DISCONNECT - обработка команды пользователя не удалась
*/
ecode_t usr_manage( usr_record *usr );

/*
	1.	Чтение из сокета
	2.	usr - запись пользователя
	3.	Код ошибки
	4.	E_OBUFFER - буффер переполнен
		E_FILE - ошибка при чтении сокета
		E_DISCONNECT - другая сторона закрыла соединение
		E_NONE - нет ошибок
*/
ecode_t usr_read( usr_record *usr );

/*
	1.	Получение одного целого сообщения
	2.	usr - запись пользователя
	3.	Код ошибки
	4.	E_ARG - пустое сообщение или ошибка его анализа
		E_AGAIN - доступно целое сообщение ( поле msg )
		E_NONE - целого сообщения нет
*/
ecode_t usr_getmsg( usr_record *usr );

/*
	1.	Запись в сокет
	2.	usr - запись пользователя
		msg - сообщение для отправки
	3.	Код ошибки
	4.	любая ошибка usr_writel()
		E_FILE - ошибка при записи в сокет
		E_DISCONNECT - другая сторона закрыла соединение
		E_KU2 - ошибка kaneutil2
		E_NONE - нет ошибок
*/
ecode_t usr_write( usr_record *usr, char *msg );

/*
	1.	Запись в сокет из буффера
	2.	usr - запись пользователя
	3.	Код ошибки
	4.	E_FILE - ошибка при записи в сокет
		E_DISCONNECT - другая сторона закрыла соединение
		E_KU2 - ошибка kaneutil2
		E_NONE - нет ошибок
*/
ecode_t usr_writel( usr_record *usr );

#endif
