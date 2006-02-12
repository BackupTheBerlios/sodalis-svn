/***************************************************************************
 *            tools.c
 *
 *  Sun Feb 12 17:48:59 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include "tools.h"
#include "ecode.h"
#include "errors/debug.h"
#include "database/db.h"
#include "dialogue/dialogue.h"

int toolkey=0;

ecode_t tool_blankdb( void )
{
	ecode_t ec;
	pstart();
	
	if ( (ec=db_init())!=E_NONE )
		return ec;
	
	dlgue_stream(stdin,stdout);
	dlgue_ask("test question!",NULL,DLGUE_BOOL);
	
	if ( (ec=db_halt())!=E_NONE )
		return ec;
	
	pstop();
	return E_NONE;
}
