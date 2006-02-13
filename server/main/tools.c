/***************************************************************************
 *            tools.c
 *
 *  Sun Feb 12 17:48:59 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>
#include <libintl.h>

#include "tools.h"
#include "ecode.h"
#include "errors/debug.h"
#include "database/db.h"
#include "dialogue/dialogue.h"

int toolkey=0;

ecode_t tool_blankdb( void )
{
	ecode_t ec;
	kucode_t kec;
	int t;
	pstart();
	
	dlgue_stream(stdin,stdout);
	kec=dlgue_ask(gettext("WARNING! You are going to crete a blank database,\n" \
			"this will drop all the existing tables!\n" \
			"Would You like to back up the current database\nif it exists?"), \
			&t,DLGUE_BOOL|DLGUE_CANCEL);
	
	if ( kec==KE_EMPTY )
	{
		//	отмена операции
		dlgue_claim(gettext("Operation canceled!"));
		return E_NONE;
	}
	if ( kec!=KE_NONE )
		return E_KU2;
	
	if ( (ec=db_init())!=E_NONE )
		return ec;
	
	if ( (ec=db_halt())!=E_NONE )
		return ec;
	
	pstop();
	return E_NONE;
}
