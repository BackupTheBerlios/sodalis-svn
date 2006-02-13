/***************************************************************************
 *            dialogue.c
 *
 *  Sun Feb 12 19:18:40 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dialogue.h"
#include "errors/ecode.h"
#include "errors/debug.h"

#ifdef DLGUE_USE_GETTEXT
#include <libintl.h>
#endif

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
	static union
	{
		int _int;
		char _text[DLGUE_STRSIZE];
		double _real;
	}	ans;
	char *p;
	pstart();
	
	fprintf(fout,question);
	if ( (type&DLGUE_CANCEL)==DLGUE_CANCEL )
		#ifdef DLGUE_USE_GETTEXT
		fprintf(fout,gettext(" (ENTER cancels the operation)"));
		#else
		fprintf(fout," (ENTER cancels the operation)");
		#endif
	switch ( type&7 )
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
	
	ans._text[DLGUE_STRSIZE-1]=0;
	if ( fgets(ans._text,DLGUE_STRSIZE,fin)==NULL )
		return KE_IO;
	if ( ans._text[DLGUE_STRSIZE-1]!=0 )
		return KE_SYNTAX;
	
	if ( ((type&DLGUE_CANCEL)==DLGUE_CANCEL) && (ans._text[0]==10) )
	{
		return KE_EMPTY;
	}	else
	{
		switch ( type&7 )
		{
			case DLGUE_BOOL:
				if ( !strcmp(ans._text,"y") )
					*((int*)answer)=1;	else
				if ( !strcmp(ans._text,"n") )
					*((int*)answer)=0; else
						return KE_SYNTAX;
				break;
					
			case DLGUE_INT:
				ans._int=strtol(ans._text,&p,0);
				if ( *p!=10 ) return KE_SYNTAX;
				*((int*)answer)=ans._int;
				break;
				
			case DLGUE_STRING:
				*((char**)answer)=ans._text;
				break;
			
			case DLGUE_FLOAT:
				ans._real=strtod(ans._text,&p);
				if ( *p!=10 ) return KE_SYNTAX;
				*((double*)answer)=ans._real;
				break;
		}
	}
	
	
	pstop();
	return KE_NONE;
}

void dlgue_claim( char *text )
{
	pstart();
	fprintf(fout,"%s\n",text);
	pstop();
}
