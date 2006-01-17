/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef KU__CFG_H__
#define KU__CFG_H__
#include "errors/open_code.h"

#include "errors/ecode.h"

#define CFG_BUFFER 2048

/*
	Один запрос
*/
typedef
struct
{
	char *id;
	char *fmt;
	void **ptr;
}	cfg_query_t;

/*
	секущая строчка
*/
extern int cfg_line;

/*
	Открыть файл
*/
kucode_t cfg_open( char *file );

/*
	Закрыть файл
*/
kucode_t cfg_close( void );

/*
	Добавить запрос
	Формат:
		i: int
		f: double
		s: char* (string)
		b: int (boolean 1/0)
	... указываются указатели на переменные, этот
	модуль переменные не создаёт, а работает с существующими
*/
kucode_t cfg_query( char *id, char *fmt, ... );

/*
	Выполнить запросы
	ВНИМАНИЕ!!
		при чтении запроса, в котором есть строки,
		НЕ проверяется размер строки назначения, но известно,
		что эта строка не может быть больше CFG_BUFFER символов
*/
kucode_t cfg_process( void );

#include "errors/close_code.h"
#endif
