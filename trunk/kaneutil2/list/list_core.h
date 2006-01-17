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

#ifndef LIST_TYPE
#error List type is not declared!
#endif

#ifndef LIST_EQ
#error List comparison function is not declared!
#endif

#ifndef LIST_ERRVAL
#error List error value is not declared!
#endif

#ifndef LIST_ID
#error List identificator is not declared!
#endif

#include "errors/types.h"
#include "errors/ecode.h"
#include "errors/debug.h"

#define _NAME2_( name, m ) name ## m
#define _NAME1_( name, m ) _NAME2_(name,m)
#define NAME( name ) _NAME1_(name ## _,LIST_ID)
#define LIST NAME(kulist_t)
#define NODE NAME(kulist_node)
#define TYPE NAME(kulist_data)
#define STRUCT_NODE NAME(KUL_NODE)

typedef LIST_TYPE TYPE;

typedef
struct STRUCT_NODE
{
	TYPE data;
	struct STRUCT_NODE *next, *prev;
}	NODE;

typedef
struct
{
	kucounter size;
	NODE *ptr;
}	LIST;

LIST *NAME(kulist_create)( void );
kucode_t NAME(kulist_free)( LIST *list );
kucode_t NAME(kulist_ins)( LIST *list, TYPE data );
kucode_t NAME(kulist_rem)( LIST *list );
kucode_t NAME(kulist_search)( LIST *list, TYPE data );

TYPE NAME(kulist_cur)( LIST *list );
TYPE NAME(kulist_next)( LIST *list );
TYPE NAME(kulist_prev)( LIST *list );

kucode_t NAME(kulist_unite)( LIST *dst, LIST *src );
kucode_t NAME(kulist_delete)( LIST *list, kucounter cnt );
LIST *NAME(kulist_extract)( LIST *list, kucounter cnt );
LIST *NAME(kulist_copy)( LIST *list );
