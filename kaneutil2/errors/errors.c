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

#include "errors/ecode.h"
#include "errors/types.h"
#include "errors/debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

kucode_t kucode;
kucounter mallocs=0;
kucounter sizes=0;

void printf_debug( char *file, const char *func, int line, char *fmt, ... )
{
	va_list ap;
	va_start(ap,fmt);
	printf(">> %s:%d [%s] >> ",file,line,func);
	vprintf(fmt,ap);
	va_end(ap);
}

void *malloc_debug( int size )
{
	void *ptr=malloc(size);
	if ( ptr!=NULL )
	{
		mallocs++;
		sizes+=size;
	}
	#ifdef DEBUG_MEMORY
	printf("MALLOC: %p sized %d (total %d)\n",ptr,size,mallocs);
	#endif
	return ptr;
}

void free_debug( void *__ptr )
{
	free(__ptr);
	mallocs--;
	#ifdef DEBUG_MEMORY
	printf("FREE  : %p (total left %d)\n",__ptr,mallocs);
	#endif
}
