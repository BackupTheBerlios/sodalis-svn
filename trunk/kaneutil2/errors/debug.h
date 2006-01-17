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

#ifndef KU__DEBUG_H__
#define KU__DEBUG_H__
#include "errors/open_code.h"

#define STR( m ) _STR_(m)
#define _STR_( m ) #m

#define CONCAT( m, n ) m ## n

#ifdef DEBUG

#include "errors/errors.h"
void printf_debug( char *file, const char *func, int line, char *fmt, ... );
void *malloc_debug( int size );
void free_debug( void *__ptr );

#define pdebug( m, ... ) \
printf_debug(__FILE__,__FUNCTION__,__LINE__,m,##__VA_ARGS__)

#define dmalloc( size ) \
malloc_debug(size)

#define dfree( ptr ) \
free_debug(ptr)

#else	//	DEBUG

#define pdebug( m, ... ) ;
#define dmalloc( size ) malloc(size)
#define dfree( ptr ) free(ptr)

#endif	//	DEBUG

#ifdef DEBUG_FUNC

#include <stdio.h>

#define pstart() printf("### ==> %s >>>\n",__FUNCTION__);
#define pstop() printf("### <== %s <<<\n",__FUNCTION__);

#else	//	DEBUG_FUNC

#define pstart() ;
#define pstop() ;

#endif	// DEBUG_FUNC

#define _ pdebug("CHECK POINT\n");

#include "errors/close_code.h"
#endif
