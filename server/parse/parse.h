/***************************************************************************
 *            parse.h
 *
 *  Mon Nov 14 17:54:26 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include "main/ecode.h"
#include "user/user.h"

#define PARSER_ARG_CNT 16
extern char *p_arg[PARSER_ARG_CNT];
extern int p_argc;

/*
	1.	Обработка принатого сообщения
	2.	usr - запись пользователя
	3.	Нужно ли отсоединять пользователя
	4.	1 - нужно
		0 - не нежно
*/
int parse_msg( usr_record *usr );

/*
	1.	Разбиение сообщения на параметры
	2.	msg - сообщение
	3.	Количество прочитаных параметров,
		0, если ошибка,
		p_arg - список полученых параметров
		p_argc - их количество
	4.	---
*/
int parse( char *msg );
