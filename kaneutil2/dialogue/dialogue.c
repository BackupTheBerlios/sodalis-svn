/***************************************************************************
 *            dialogue.c
 *
 *  Sun Feb 12 19:18:40 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>

#include "dialogue.h"
#include "errors/ecode.h"
#include "errors/debug.h"

FILE *fin=NULL, *fout=NULL;

void dlgue_stream( FILE *in, FILE *out )
{
	pstart();
	fin=in;
	fout=out;
	pstop();
}

kucode_t dlgue_ask( char *question, void *answer, dlgue_t type )
{
	pstart();
	fprintf(fout,question);
	switch ( type )
	{
		case DLGUE_BOOL:
			fprintf(fout," [y/n]: ");
			break;
		case DLGUE_INT:
			fprintf(fout," [integer]: ");
			break;
		case DLGUE_STRING:
			fprintf(fout," [text]: ");
			break;
		case DLGUE_FLOAT:
			fprintf(fout," [real]: ");
			break;
	}
	pstop();
	return KE_NONE;
}
