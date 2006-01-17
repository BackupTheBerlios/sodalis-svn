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
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "cfgreader/cfg.h"
#include "errors/ecode.h"
#include "errors/debug.h"
#include "tree_cfg.h"

FILE *cfgf=NULL;
abtree_t_cfg *qtree;
int cfg_line;

kucode_t cfg_open( char *file )
{
	pstart();
	
	if ( cfgf!=NULL ) return KE_DOUBLE;
	
	cfgf=fopen(file,"r");
	if ( cfgf==NULL ) return KE_IO;
	
	qtree=abtree_create_cfg();
	if ( qtree==NULL )
	{
		fclose(cfgf);
		return kucode;
	}
	
	pstop();
	return KE_NONE;
}

kucode_t cfg_close( void )
{
	abtree_node_cfg *a=qtree->ptr, *b;
	pstart();
	
	if ( cfgf==NULL ) return KE_EMPTY;
	fclose(cfgf);
	
	//	очищение дерева
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
				dfree(a->data);
				dfree(a);
			}
		}
		a=b;
	}
	dfree(qtree);
	
	pstop();
	return KE_NONE;
}

kucode_t cfg_query( char *id, char *fmt, ... )
{
	cfg_query_t *q;
	int i;
	va_list va;
	pstart();
	
	q=dmalloc(sizeof(cfg_query_t)+
			sizeof(char)*(strlen(id)+strlen(fmt)+2)+
			sizeof(void**)*strlen(fmt));
	if ( q==NULL ) return KE_MEMORY;
	
	q->id=((char*)q)+sizeof(cfg_query_t);
	q->fmt=((char*)q->id)+sizeof(char)*(strlen(id)+1);
	q->ptr=(void**)(((char*)q->fmt)+sizeof(char)*(strlen(fmt)+1));
	
	strcpy(q->id,id);
	strcpy(q->fmt,fmt);
	
	va_start(va,fmt);
	for ( i=0; i<strlen(fmt); i++ )
		q->ptr[i]=va_arg(va,void*);
	va_end(va);
	
	if ( (kucode=abtree_ins_cfg(qtree,q))!=KE_NONE )
	{
		dfree(q);
		return kucode;
	}
	
	pstop();
	return KE_NONE;
}

void cfg_sksp( char **p )
{
	while ( isspace(**p) && (**p!=0) ) (*p)++;
}

char *cfg_readnext( char **p )
{
	static char buf[CFG_BUFFER];
	char *c=buf;
	cfg_sksp(p);
	if ( **p=='"' )
	{
		(*p)++;
		while ( (**p!='"') && (**p!=0) ) *(c++)=*((*p)++);
		if ( *((*p)++)==0 ) return NULL;
	}	else
	{
		while ( isalnum(**p) || (**p=='_') ) *(c++)=*((*p)++);
	}
	
	cfg_sksp(p);
	*c=0;
	return buf;
}

kucode_t cfg_process( void )
{
	char buf[CFG_BUFFER];
	cfg_query_t sq, *q;
	int quota=0;
	pstart();
	
	cfg_line=0;
	
	while ( fgets(buf,CFG_BUFFER,cfgf)!=NULL )
	{
		char *c=buf, *cur;
		int i, wassign;
		
		cfg_line++;
		
		cfg_sksp(&c);
		if ( *c==0 ) continue;
		if ( *c=='#' )
		{
			if ( c[1]=='#' ) quota=1-quota;
			continue;
		}
		
		if ( quota ) continue;
		
		cur=cfg_readnext(&c);
		if ( cur==NULL ) return KE_SYNTAX;
		
		sq.id=cur;
		q=abtree_search_cfg(qtree,&sq);
		if ( q==NULL ) return kucode;
		
		if ( *c!='=' ) return KE_SYNTAX;
		wassign=1;
		
		for ( i=0; i<strlen(q->fmt); i++ )
		{
			char *p;
			
			if ( *c==0 ) return KE_SYNTAX;
			if ( (wassign || (*c==',')) && !(wassign && (*c==',')) )
			{
				wassign=0;
				c++;
			}	else return KE_SYNTAX;
			cur=cfg_readnext(&c);
			if ( cur==NULL ) return KE_SYNTAX;
			
			switch ( q->fmt[i] )
			{
				case 'i':
					*((int*)q->ptr[i])=strtol(cur,&p,0);
					if ( *p!=0 ) return KE_SYNTAX;
					break;
				case 'f':
					*((double*)q->ptr[i])=strtod(cur,&p);
					if ( *p!=0 ) return KE_SYNTAX;
					break;
				case 's':
					strcpy((char*)q->ptr[i],cur);
					break;
				case 'b':
					if ( (strcmp(cur,"yes")&&strcmp(cur,"true"))==0 )
						*((int*)q->ptr[i])=1; else
							if ( (strcmp(cur,"no")&&strcmp(cur,"false"))==0 )
								*((int*)q->ptr[i])=0; else
									return KE_SYNTAX;
					break;
				default:
					return KE_SYNTAX;
			}
		}
		if ( *c!=0 ) return KE_SYNTAX;
	}
	
	if ( quota ) return KE_SYNTAX;
	
	pstop();
	return KE_NONE;
}
