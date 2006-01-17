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

#ifndef STACK_TYPE
#error Stack type is not declared!
#endif

#ifndef STACK_ERRVAL
#error Stack error value is not declared!
#endif

#ifndef STACK_ID
#error Stack identificator is not declared!
#endif

#ifndef STACK_INITSIZE
#define STACK_INITSIZE 64
#endif

#include "errors/types.h"
#include "errors/ecode.h"
#include "errors/debug.h"

#define _NAME2_( name, m ) name ## m
#define _NAME1_( name, m ) _NAME2_(name,m)
#define NAME( name ) _NAME1_(name ## _,STACK_ID)
#define STACK NAME(stack_t)
#define NODE NAME(stack_node)
#define TYPE NAME(stack_data)
#define STRUCT_NODE NAME(STACK_NODE)

//	Тип данных, который хранится в стеке
typedef STACK_TYPE TYPE;

//	Недилимая честь/группа стека
typedef
struct STRUCT_NODE
{
	struct STRUCT_NODE *prev, *next;
	TYPE data[STACK_INITSIZE];
}	NODE;

//	Стек
typedef
struct
{
	NODE *ptr;
	int pos;
}	STACK;

STACK *NAME(stack_create)( void );
kucode_t NAME(stack_free)( STACK *stack );

kucode_t NAME(stack_push)( STACK *stack, TYPE data );
kucode_t NAME(stack_jpop)( STACK *stack );
TYPE NAME(stack_pop)( STACK *stack );
