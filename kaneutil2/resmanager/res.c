/***************************************************************************
 *            res.c
 *
 *  Wed May 24 19:20:07 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <string.h>

#include "resmanager/res.h"
#include "errors/debug.h"
#include "tree_id_res.h"
#include "abtree/abtree_end.h"
#include "tree_name_res.h"
#include "abtree/abtree_end.h"

static abtree_t_res *reses;
static abtree_t_restype *restypes;

kucode_t res_init( const char *fname )
{
	pstart();
	if ( (reses=abtree_create_res())==NULL ) return kucode;
	if ( (restypes=abtree_create_restype())==NULL )
	{
		abtree_free_res(reses);
		return kucode;
	}
	pstop();
	return KE_NONE;
}

kucode_t res_halt( void )
{
	pstart();
	if ( (kucode=abtree_free_res(reses))!=KE_NONE )
	{
		abtree_free_restype(restypes);
		return kucode;
	}
	if ( (kucode=abtree_free_restype(restypes))!=KE_NONE ) return kucode;
	pstop();
	return KE_NONE;
}

kucode_t res_assign( int type, resmode_t mode, rescontrol_f control )
{
	restype_t *rtype;
	pstart();
	
	rtype=dmalloc(sizeof(restype_t));
	if ( rtype==NULL ) return KE_MEMORY;
	rtype->type=type;
	rtype->mode=mode;
	rtype->control=control;
	
	kucode=abtree_ins_restype(restypes,rtype);
	if ( kucode!=KE_NONE )
	{
		dfree(rtype);
		return kucode;
	}
	
	pstop();
	return KE_NONE;
}

kucode_t res_add( const char *path, const char *name, int type, resmode_t mode )
{
	res_t *res;
	pstart();
	
	res=dmalloc(sizeof(res_t)+strlen(name)+strlen(path)+2);
	if ( res==NULL ) return KE_MEMORY;
	res->name=(char*)res+sizeof(res_t);
	strcpy((char*)res->name,name);
	res->path=res->name+strlen(name)+1;
	strcpy((char*)res->path,path);
	res->type=type;
	res->mode=mode;
	res->loadcnt=0;
	
	kucode=abtree_ins_res(reses,res);
	if ( kucode!=KE_NONE )
	{
		dfree(res);
		return kucode;
	}
	
	pstop();
	return KE_NONE;
}

res_t *res_access( const char *name )
{
	res_t searchres;
	res_t *res;
	restype_t searchtype;
	restype_t *rtype;
	pstart();
	
	searchres.name=name;
	res=abtree_search_res(reses,&searchres);
	if ( res==NULL ) return NULL;
	
	if ( res->loadcnt==0 )
	{
		//	загрузка ресурса
		searchtype.type=res->type;
		rtype=abtree_search_restype(restypes,&searchtype);
		if ( rtype==NULL ) return NULL;
		
		res->data=rtype->control(res->path,RES_LOAD,NULL);
		if ( res->data==NULL )
		{
			kucode=KE_EXTERNAL;
			return NULL;
		}
	}
	res->loadcnt++;
	
	pstop();
	return res;
}

kucode_t res_release( res_t *res )
{
	restype_t searchtype;
	restype_t *rtype;
	pstart();
	
	if ( res->loadcnt==1 )
	{
		//	выгрузка ресурса
		searchtype.type=res->type;
		rtype=abtree_search_restype(restypes,&searchtype);
		if ( rtype==NULL ) return kucode;
		
		res->data=rtype->control(res->path,RES_ULOAD,res->data);
		if ( res->data!=NULL ) return KE_EXTERNAL;
	}
	res->loadcnt--;
	
	pstop();
	return KE_NONE;
}
