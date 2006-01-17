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

#include <stdio.h>
#include <stdlib.h>
#include "errors/debug.h"
#include "errors/ecode.h"

LIST *NAME(kulist_create)( void )
{
	LIST *ls=NULL;
	
	pstart();
	
	ls=(LIST*)dmalloc(sizeof(LIST));
	if ( ls==NULL )
	{
		kucode=KE_MEMORY;
		return NULL;
	}
	
	ls->size=0;
	ls->ptr=NULL;
	
	pstop();
	return ls;
}

kucode_t NAME(kulist_free)( LIST *list )
{
	pstart();
	kucode=NAME(kulist_delete)(list,list->size);
	if ( kucode!=KE_NONE ) return kucode;
	dfree(list);
	pstop();
	return KE_NONE;
}

kucode_t NAME(kulist_ins)( LIST *list, TYPE data )
{
	NODE *nd=NULL;
	
	pstart();
	
	nd=(NODE*)dmalloc(sizeof(NODE));
	if ( nd==NULL ) return KE_MEMORY;
	
	nd->data=data;
	
	if ( list->ptr )
	{
		nd->prev=list->ptr;
		nd->next=list->ptr->next;
		nd->prev->next=nd;
		nd->next->prev=nd;
	}	else
	{
		nd->next=nd;
		nd->prev=nd;
	}
	
	list->ptr=nd;
	list->size++;
	
	pstop();
	return KE_NONE;
}

kucode_t NAME(kulist_rem)( LIST *list )
{
	NODE *nd;
	
	pstart();
	
	nd=list->ptr;
	if ( nd==NULL ) return KE_EMPTY;
	
	if ( (list->ptr=nd->prev)==nd ) list->ptr=NULL;
	
	nd->prev->next=nd->next;
	nd->next->prev=nd->prev;
	dfree(nd);
	
	list->size--;
	
	pstop();
	return KE_NONE;
}

kucode_t NAME(kulist_search)( LIST *list, TYPE data )
{
	NODE *cur=list->ptr;
	NODE *first=cur;
	pstart();
	
	if ( cur==NULL ) return KE_NOTFOUND;
	
	while ( !LIST_EQ(cur->data,data) )
	{
		cur=cur->next;
		if ( cur==first ) return KE_NOTFOUND;
	}
	list->ptr=cur;
	
	pstop();
	return KE_NONE;
}

TYPE NAME(kulist_cur)( LIST *list )
{
	pstart();
	if ( list->ptr==NULL )
	{
		kucode=KE_EMPTY;
		return LIST_ERRVAL;
	}
	pstop();
	return list->ptr->data;
}

TYPE NAME(kulist_next)( LIST *list )
{
	pstart();
	if ( list->ptr==NULL )
	{
		kucode=KE_EMPTY;
		return LIST_ERRVAL;
	}
	list->ptr=list->ptr->next;
	pstop();
	return list->ptr->data;
}

TYPE NAME(kulist_prev)( LIST *list )
{
	pstart();
	if ( list->ptr==NULL )
	{
		kucode=KE_EMPTY;
		return LIST_ERRVAL;
	}
	list->ptr=list->ptr->prev;
	pstop();
	return list->ptr->data;
}

kucode_t NAME(kulist_unite)( LIST *dst, LIST *src )
{
	return KE_NONE;
}

kucode_t NAME(kulist_delete)( LIST *list, kucounter cnt )
{
	//NODE *node=list->ptr;
	int i;
	pstart();
	for ( i=0; i<cnt; i++ )
		if ( (kucode=NAME(kulist_rem)(list))!=KE_NONE )
			return kucode;
	pstop();
	return KE_NONE;
}

LIST *NAME(kulist_extract)( LIST *list, kucounter cnt )
{
	return KE_NONE;
}

LIST *NAME(kulist_copy)( LIST *list )
{
	return KE_NONE;
}
