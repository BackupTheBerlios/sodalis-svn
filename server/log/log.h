/***************************************************************************
 *            log.h
 *
 *  Sat Oct 22 12:18:58 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include "main/ecode.h"

extern FILE *logstream;

/*
	1.	Открытие файла лога
	2.	file - имя файла
	3.	Код ошибки
	4.	E_NONE - нет ошибки
		E_FILE - не удалось открыть файл
*/
ecode_t openlog( char *file );

/*
	1.	Закрытие файла лога
	2.	---
	3.	Код ошибки
	4.	E_NONE - нет ошибки
		E_FILE - не удалось закрыть файл
*/
ecode_t closelog( void );

/*
	печать лога
*/
void plog( char *fmt, ... );

#endif
