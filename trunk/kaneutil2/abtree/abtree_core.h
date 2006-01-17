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

#ifndef ABTREE_TYPE
#error Tree type is not declared!
#endif

#ifndef ABTREE_EQ
#error Tree comparison function is not declared!
#endif

#ifndef ABTREE_MORE
#error Tree comparison function is not declared!
#endif

#ifndef ABTREE_ERRVAL
#error Tree error value is not declared!
#endif

#ifndef ABTREE_ID
#error Tree identificator is not declared!
#endif

#include "errors/types.h"
#include "errors/ecode.h"
#include "errors/debug.h"

#define _NAME2_( name, m ) name ## m
#define _NAME1_( name, m ) _NAME2_(name,m)
#define NAME( name ) _NAME1_(name ## _,ABTREE_ID)
#define ABTREE NAME(abtree_t)
#define NODE NAME(abtree_node)
#define TYPE NAME(abtree_data)
#define STRUCT_NODE NAME(ABT_NODE)

//	Тип данных, который хранится в дереве
typedef ABTREE_TYPE TYPE;

/*
	Узел дерева
*/
typedef
struct STRUCT_NODE
{
	TYPE data;
	struct STRUCT_NODE *left, *right, *parent;
	unsigned int lcnt, rcnt;
}	NODE;

/*
	Дерево
*/
typedef
struct
{
	NODE *ptr;
}	ABTREE;

/*
	Создать дерево
*/
ABTREE *NAME(abtree_create)( void );

/*
	Удалить дерево
*/
kucode_t NAME(abtree_free)( ABTREE *tree );

/*
	Вставить элемент
*/
kucode_t NAME(abtree_ins)( ABTREE *tree, TYPE data );

/*
	Удалить элемент
*/
kucode_t NAME(abtree_rem)( ABTREE *tree, TYPE data );

/*
	Поиск элемента
*/
TYPE NAME(abtree_search)( ABTREE *tree, TYPE data );

/*
	Очистить дерево (удалить все элементы)
*/
kucode_t NAME(abtree_empty)( ABTREE *tree );
