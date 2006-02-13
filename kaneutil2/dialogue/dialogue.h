/***************************************************************************
 *            dialogue.h
 *
 *  Sun Feb 12 19:18:34 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef KU__DIALOGUE_H__
#define KU__DIALOGUE_H__
#include "errors/open_code.h"

#include "errors/ecode.h"
#include <stdio.h>

#define DLGUE_STRSIZE 256
#define DLGUE_USE_GETTEXT

typedef
enum
{
	DLGUE_BOOL=1,
	DLGUE_INT=2,
	DLGUE_STRING=3,
	DLGUE_FLOAT=4,
	DLGUE_CANCEL=8
}	dlgue_t;

/*
	1.	Перенаправление потока ввода/вывода,
		!по умолчанию NULL!
	2.	in, out - потоки ввода/вывода
	3.	---
	4.	---
*/
void dlgue_stream( FILE *in, FILE *out );

/*
	1.	Задавание вопроса, функция "засыпает" до получения ответа
	2.	question - вопрос
		answer - указатель на переменную, куда запишется ответ
		type - тип предпологаемого ответа
	3.	Код ошибки
	4.	ntni
*/
kucode_t dlgue_ask( char *question, void *answer, dlgue_t type );

/*
	1.	Вывод сообщения на экран
	2.	text - текст сообщения
	3.	---
	4.	---
*/
void dlgue_claim( char *text );

#include "errors/close_code.h"
#endif
