/***************************************************************************
 *            res.h
 *
 *  Wed May 24 19:20:00 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef KU__RES_H__
#define KU__RES_H__
#include "errors/open_code.h"

#include "errors/ecode.h"

typedef
enum
{
	RES_LOAD,
	RES_ULOAD
}	rescontrol_t;

typedef
enum
{
	RES_DEFAULT
}	resmode_t;

//zagruz4ik resursa
typedef
void *(*rescontrol_f)( const char *path, rescontrol_t mode, void *data );

//tip resursa
typedef
struct
{
	int type;
	rescontrol_f control;
	resmode_t mode;
}	restype_t;

// element resursa
typedef
struct
{
	const char *name, *path;
	void *data;
	unsigned int loadcnt;
	resmode_t mode;
	int type;
}	res_t;

kucode_t res_init( const char *fname );
kucode_t res_halt( void );
kucode_t res_assign( int type, resmode_t mode, rescontrol_f control );
kucode_t res_add( const char *path, const char *name, int type, resmode_t mode );
res_t *res_access( const char *name );
kucode_t res_release( res_t *res );

#include "errors/close_code.h"
#endif
