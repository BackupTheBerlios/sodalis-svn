/***************************************************************************
 *            net.h
 *
 *  Sun Oct 23 10:10:30 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __NET_H__
#define __NET_H__

#include <stdint.h>

#include "main/ecode.h"

#include "net_id.h"

#include "network/nstack.h"
#include "stack/stack_end.h"

#include "network/nlist.h"
#include "list/list_end.h"

extern kulist_t_net *sock_wlist;
extern kulist_t_net *sock_list;

/*
	1.	Создать слушающий сокет
	2.	port - номер порта
		backlog - длина очереди ожиданий на соединение
	3.	Код ошибки
	4.	E_NONE - нет ошибок
		E_KU2 - ошибка kaneutil2
		E_SOCKET - не создался сокет
		E_BIND - не удалось "привязать" сокет
		E_LISTEN - не удалось начать слушать соединения
*/
ecode_t net_open( uint16_t port, int backlog );

/*
	1.	Закрыть все соединения
	2.	---
	3.	Код ошибки
	4.	Всегда E_NONE
*/
ecode_t net_close( void );

/*
	1.	Ждать данных из сети
	2.	connected - 0 если новых соединений нет, иначе 1
		socks - стек сокетов, с которых пришли новые данные
		wsocks - стек сокетов, на которые можно писать
			(только среди сокетов из списка sock_wlist)
	3.	Код ошибки
	4.	E_NONE - нет ошибок
		E_KU2 - ошибка kaneutil2
		E_SELECT - ошибка при вызове функции select()
*/
ecode_t net_wait( int *connected, stack_t_net **socks, stack_t_net **wsocks );

/*
	1.	Принять соединение
	2.	---
	3.	Указатель на идентификатор, NULL при ошибке (ecode)
	4.	E_ACCEPT - не удалось принять соединение
		E_KU2 - ошибка kaneutil2
*/
net_id *net_accept( void );

/*
	1.	Прервать соединение
	2.	id - указатель на идентификатор
	3.	Код ошибки
	4.	E_NONE - нет ошибки
		E_KU2 - ошибка kaneutil2
*/
ecode_t net_discon( net_id *id );

#endif
