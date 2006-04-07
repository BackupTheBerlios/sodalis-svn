/***************************************************************************
 *            libmain.c
 *
 *  Fri Apr  7 17:43:05 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdlib.h>

#include "libmain.h"
#include "ecode.h"
#include "errors/debug.h"

sod_session *sod_init( void )
{
	pstart();
	pstop();
	return NULL;
}

int sod_halt( sod_session *session )
{
	pstart();
	pstop();
	return SOD_OK;
}
