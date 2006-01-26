/***************************************************************************
 *            other.h
 *
 *  Sun Oct 23 10:00:40 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef KU__OTHER_H__
#define KU__OTHER_H__
#include "errors/open_code.h"

//	длина строки
#define QSTR_STRSIZE 2048
//	количество строк
#define QSTR_STRCNT 16

/*
	склеивание двух строк
*/
char *qstr( char *s1, char *s2 );

/*
	создание строк
*/
char *vstr( char *fmt, ... );

/*
	преобразование строки в путь до директории /a/b/
*/
void qdir( char *path );

/*
	qdir + проверка существования директории
*/
int qdir2( char *path );

#include "errors/close_code.h"
#endif
