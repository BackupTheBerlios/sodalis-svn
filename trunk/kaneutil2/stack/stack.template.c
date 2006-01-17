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

STACK *NAME(stack_create)( void )
{
	NODE *node;
	STACK *stack;
	pstart();
	
	stack=dmalloc(sizeof(STACK));
	if ( stack==NULL )
	{
		kucode=KE_MEMORY;
		return NULL;
	}
	
	node=dmalloc(sizeof(NODE));
	if ( node==NULL )
	{
		kucode=KE_MEMORY;
		dfree(stack);
		return NULL;
	}
	
	stack->pos=0;
	stack->ptr=node;
	node->prev=NULL;
	
	pstop();
	return stack;
}

kucode_t NAME(stack_free)( STACK *stack )
{
	NODE *node, *onode;
	pstart();
	
	onode=stack->ptr;
	while ( onode!=NULL )
	{
		node=onode->prev;
		dfree(onode);
		onode=node;
	}
	
	dfree(stack);
	
	pstop();
	return KE_NONE;
}

kucode_t NAME(stack_push)( STACK *stack, TYPE data )
{
	NODE *node;
	pstart();
	
	if ( stack->pos==STACK_INITSIZE )
	{
		node=dmalloc(sizeof(NODE));
		if ( node==NULL ) return KE_MEMORY;
		_
		node->prev=stack->ptr;
		stack->ptr->next=node;
		stack->ptr=node;
		stack->pos=1;
		node->data[0]=data;
	}	else
	{
		stack->ptr->data[stack->pos++]=data;
	}
	
	pstop();
	return KE_NONE;
}

kucode_t NAME(stack_jpop)( STACK *stack )
{
	NODE *node;
	pstart();
	
	if ( stack->pos<=1 )
	{
		node=stack->ptr->prev;
		if ( node==NULL )
		{
			if ( stack->pos==0 ) return KE_EMPTY;
			stack->pos=0;
		}	else
		{
			dfree(stack->ptr);
			stack->ptr=node;
			stack->pos=STACK_INITSIZE;
		}
	}	else
	{
		stack->pos--;
	}
	
	pstop();
	return KE_NONE;
}

TYPE NAME(stack_pop)( STACK *stack )
{
	NODE *node;
	TYPE ret;
	pstart();
	
	if ( stack->pos<=1 )
	{
		ret=stack->ptr->data[0];
		node=stack->ptr->prev;
		if ( node==NULL )
		{
			if ( stack->pos==0 )
			{
				kucode=KE_EMPTY;
				return STACK_ERRVAL;
			}
			stack->pos=0;
		}	else
		{
			dfree(stack->ptr);
			stack->ptr=node;
			stack->pos=STACK_INITSIZE;
		}
	}	else
	{
		ret=stack->ptr->data[--stack->pos];
	}
	
	pstop();
	return ret;
}
