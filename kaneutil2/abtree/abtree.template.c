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

#define max(a,b) ((a>b)?(a):(b))

static void ab_rotate_left( ABTREE *tree, NODE *node )
{
	NODE *t=node->right;
	pstart();
	
	if ( t!=NULL )
	{
		if ( (t->right!=NULL) && (t->rcnt>=t->lcnt) )
		{
			//	ACE стиль
			t->parent=node->parent;
			
			node->right=t->left;
			node->rcnt=t->lcnt;
			if ( t->left!=NULL ) t->left->parent=node;
			
			t->left=node;
			t->lcnt=max(node->lcnt,node->rcnt)+1;
			node->parent=t;
		}	else
		{
			//	ACD стиль
			NODE *d=t->left;
			d->parent=node->parent;
			t->parent=d;
			node->parent=d;
			
			node->right=d->left;
			node->rcnt=d->lcnt;
			if ( node->right!=NULL ) node->right->parent=node;
			
			t->left=d->right;
			t->lcnt=d->rcnt;
			if ( t->left!=NULL ) t->left->parent=t;
			
			d->left=node;
			d->lcnt=max(node->lcnt,node->rcnt)+1;
			d->right=t;
			d->rcnt=max(t->lcnt,t->rcnt)+1;
			
			t=d;
		}
		
		if ( t->parent!=NULL )
		{
			if ( t->parent->left==node )
			{
				t->parent->left=t;
				t->parent->lcnt=max(t->lcnt,t->rcnt)+1;
			}	else
			{
				t->parent->right=t;
				t->parent->rcnt=max(t->lcnt,t->rcnt)+1;
			}
		}	else
		{
			tree->ptr=t;
		}
	}
	
	pstop();
}

static void ab_rotate_right( ABTREE *tree, NODE *node )
{
	NODE *t=node->left;
	pstart();
	
	if ( t!=NULL )
	{
		if ( (t->left!=NULL) && (t->lcnt>=t->rcnt) )
		{
			//	ACE стиль
			t->parent=node->parent;
			
			node->left=t->right;
			node->lcnt=t->rcnt;
			if ( t->right!=NULL ) t->right->parent=node;
			
			t->right=node;
			t->rcnt=max(node->lcnt,node->rcnt)+1;
			node->parent=t;
		}	else
		{
			//	ACD стиль
			NODE *d=t->right;
			d->parent=node->parent;
			t->parent=d;
			node->parent=d;
			
			node->left=d->right;
			node->lcnt=d->rcnt;
			if ( node->left!=NULL ) node->left->parent=node;
			
			t->right=d->left;
			t->rcnt=d->lcnt;
			if ( t->right!=NULL ) t->right->parent=t;
			
			d->right=node;
			d->rcnt=max(node->lcnt,node->rcnt)+1;
			d->left=t;
			d->lcnt=max(t->lcnt,t->rcnt)+1;
			
			t=d;
		}
		
		if ( t->parent!=NULL )
		{
			if ( t->parent->left==node )
			{
				t->parent->left=t;
				t->parent->lcnt=max(t->lcnt,t->rcnt)+1;
			}	else
			{
				t->parent->right=t;
				t->parent->rcnt=max(t->lcnt,t->rcnt)+1;
			}
		}	else
		{
			tree->ptr=t;
		}
	}
	
	pstop();
}

ABTREE *NAME(abtree_create)( void )
{
	ABTREE *tree;
	pstart();
	
	tree=(ABTREE*)dmalloc(sizeof(ABTREE));
	if ( tree==NULL )
	{
		kucode=KE_MEMORY;
		return NULL;
	}
	
	tree->ptr=NULL;
	
	pstop();
	return tree;
}

kucode_t NAME(abtree_free)( ABTREE *tree )
{
	pstart();
	
	NAME(abtree_empty)(tree);
	dfree(tree);
	
	pstop();
	return KE_NONE;
}

kucode_t NAME(abtree_ins)( ABTREE *tree, TYPE data )
{
	NODE *cur=tree->ptr;
	NODE *p=NULL, *newnode;
	pstart();
	
	//	создание элемента
	newnode=(NODE*)dmalloc(sizeof(NODE));
	if ( newnode==NULL ) return KE_MEMORY;
	newnode->left=NULL;
	newnode->right=NULL;
	newnode->data=data;
	newnode->lcnt=0;
	newnode->rcnt=0;
	
	//	поиск места
	while ( cur!=NULL )
	{
		p=cur;
		if ( ABTREE_MORE(data,cur->data) )
		{
			cur=cur->right;
		}	else
		if ( !ABTREE_EQ(data,cur->data) )
		{
			cur=cur->left;
		}	else
		{
			dfree(newnode);
			return KE_DOUBLE;
		}
	}
	
	//	вставка
	if ( p==NULL )
	{
		//	первый элемент в списке
		tree->ptr=newnode;
		newnode->parent=NULL;
	}	else
	{
		newnode->parent=p;
		if ( ABTREE_MORE(data,p->data) )
		{
			p->right=newnode;
		}	else
		{
			p->left=newnode;
		}
	}
	
	//	балансировка
	while ( p!=NULL )
	{
		if ( p->left==newnode )
			p->lcnt=max(newnode->lcnt,newnode->rcnt)+1;
		else
			p->rcnt=max(newnode->lcnt,newnode->rcnt)+1;
		
		if ( p->lcnt-p->rcnt==2 )
		{
			ab_rotate_right(tree,p);
			p=p->parent;
		}	else
		if ( p->rcnt-p->lcnt==2 )
		{
			ab_rotate_left(tree,p);
			p=p->parent;
		}
		
		newnode=p;
		p=p->parent;
	}
	
	pstop();
	return KE_NONE;
}

kucode_t NAME(abtree_rem)( ABTREE *tree, TYPE data )
{
	NODE *cur=tree->ptr;
	NODE *p;
	pstart();
	
	//	поиск элемента
	while ( cur!=NULL )
	{
		if ( ABTREE_MORE(data,cur->data) )
		{
			cur=cur->right;
		}	else
		if ( !ABTREE_EQ(data,cur->data) )
		{
			cur=cur->left;
		}	else break;
	}
	if ( cur==NULL ) return KE_NOTFOUND;
	
	//	замена его минимальным большим элементом
	p=cur->right;
	if ( p==NULL )
	{
		p=cur->parent;
		if ( p!=NULL )
		{
			if ( p->left==cur )
			{
				p->left=cur->left;
				if ( cur->left!=NULL )
				{
					p->lcnt=max(cur->left->lcnt,cur->left->rcnt)+1;
					cur->left->parent=p;
				}	else
					p->lcnt=0;
			}	else
			{
				p->right=cur->left;
				if ( cur->left!=NULL )
				{
					p->rcnt=max(cur->left->lcnt,cur->left->rcnt)+1;
					cur->left->parent=p;
				}	else
					p->rcnt=0;
			}
		}	else
		{
			tree->ptr=cur->left;
			if ( cur->left!=NULL )
				cur->left->parent=NULL;
		}
		dfree(cur);
	}	else
	{
		while ( p->lcnt>0 ) p=p->left;
		cur->data=p->data;
		cur=p;
		p=cur->parent;
		if ( p->left==cur )
		{
			p->left=cur->right;
			if ( cur->right!=NULL )
			{
				p->lcnt=max(cur->right->lcnt,cur->right->rcnt)+1;
				cur->right->parent=p;
			}	else p->lcnt=0;
		}	else
		{
			p->right=cur->right;
			if ( cur->right!=NULL )
			{
				p->rcnt=max(cur->right->lcnt,cur->right->rcnt)+1;
				cur->right->parent=p;
			}	else p->rcnt=0;
		}
		dfree(cur);
	}
	
	//	балансировка
	if ( p!=NULL )
	{
		if ( p->lcnt-p->rcnt==2 )
		{
			ab_rotate_right(tree,p);
			p=p->parent;
		}	else
		if ( p->rcnt-p->lcnt==2 )
		{
			ab_rotate_left(tree,p);
			p=p->parent;
		}
		cur=p;
		p=p->parent;
	}
	while ( p!=NULL )
	{
		if ( p->left==cur )
			p->lcnt=max(cur->lcnt,cur->rcnt)+1;
		else
			p->rcnt=max(cur->lcnt,cur->rcnt)+1;
		
		if ( p->lcnt-p->rcnt==2 )
		{
			ab_rotate_right(tree,p);
			p=p->parent;
		}	else
		if ( p->rcnt-p->lcnt==2 )
		{
			ab_rotate_left(tree,p);
			p=p->parent;
		}
		
		cur=p;
		p=p->parent;
	}
	
	pstop();
	return KE_NONE;
}

TYPE NAME(abtree_search)( ABTREE *tree, TYPE data )
{
	NODE *cur=tree->ptr;
	pstart();
	
	while ( cur!=NULL )
	{
		if ( ABTREE_MORE(data,cur->data) )
		{
			cur=cur->right;
		}	else
		if ( !ABTREE_EQ(data,cur->data) )
		{
			cur=cur->left;
		}	else
		{
			return cur->data;
		}
	}
	kucode=KE_NOTFOUND;
	
	pstop();
	return ABTREE_ERRVAL;
}

kucode_t NAME(abtree_empty)( ABTREE *tree )
{
	NODE *a=tree->ptr, *b;
	pstart();
	
	while ( a!=NULL )
	{
		b=a->left;
		if ( b==NULL )
		{
			b=a->right;
			if ( b==NULL )
			{
				b=a->parent;
				if ( b!=NULL )
				{
					if ( b->left==a )
						b->left=NULL; else b->right=NULL;
				}
				dfree(a);
			}
		}
		a=b;
	}
	
	tree->ptr=NULL;
	
	pstop();
	return KE_NONE;
}
