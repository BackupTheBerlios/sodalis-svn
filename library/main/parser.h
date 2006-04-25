/***************************************************************************
 *            parser.h
 *
 *  Sat Apr  8 17:10:31 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef SOD__PARSER_H__
#define SOD__PARSER_H__
#include "errors/open_code.h"

#include "libmain.h"

/*
	1.	Получить текст сообщения из очереди
	2.	session - сессия
		datacnt - количество слов в сообщении (см. протокол)
		data - слова (см. протокол)
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	ntni
*/
int sod_get_message( sod_session *session, int *datacnt, char **data );

/*
	1.	Добавить текст в очередь сообщений
	2.	session - сессия
		data - сообщение
	3.	В случае ошибки - SOD_ERROR, иначе - SOD_OK
	4.	ntni
*/
int sod_put_message( sod_session *session, char *data );

#include "errors/close_code.h"
#endif
