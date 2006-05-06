/***************************************************************************
 *            parser.c
 *
 *  Tue Apr 25 22:04:51 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "errors/debug.h"
#include "ecode.h"
#include "other/other.h"

int sod_get_message( sod_session *session, int *datacnt, char **data )
{
	int i=1, qu=0, tmp;
	char *src=session->inbuf+session->instart;
	char *dst=session->buffer;
	char *p;
	pstart();
	
	#ifdef DEBUG
	while ( (*src!=0) && (src<session->inbuf+session->inpos) ) src++;
	if ( (*src==0) && (src<session->inbuf+session->inpos) )
		pdebug("*** RECEIVED (size=%d) *** \"%s\"\n",src-session->inbuf-session->instart,session->inbuf+session->instart);
	src=session->inbuf+session->instart;
	#endif
	
	data[0]=dst;
	if ( (*src==0) && (src<session->inbuf+session->inpos) )
		sod_throw_error(SE_PARSE,"There is no any data");
	
	while ( (*src!=0) && (src<session->inbuf+session->inpos) )
	{
		switch ( *src )
		{
			case ' ':
				if ( !qu )
				{
					*(dst++)=0;
					if ( i==SOD_ARG_CNT )
					{
						sod_throw_error(SE_PARSE,"Too mamy parameters");
					}	else
					{
						data[i++]=dst;
					}
					src++;
				}	else
				{
					*(dst++)=*(src++);
				}
				break;
			case '/':
				tmp=strtol(++src,&p,0);
				if ( p==src )
				{
					sod_throw_error(SE_PARSE,"After '/` must be a number");
				}
				if ( (tmp<0) || (tmp>255) )
				{
					sod_throw_error(SE_PARSE,"Symbol code is invalid");
				}
				*(dst++)=(char)tmp;
				src=p;
				break;
			case '@':
				if ( *(++src)!=0 )
					*(dst++)=*(src++);
				else
				{
					sod_throw_error(SE_PARSE,"'@` at the end");
				}
				break;
			case '"':
				qu=1-qu;
				src++;
				break;
			default:
				*(dst++)=*(src++);
		}
	}
	*dst=0;
	if ( src==session->inbuf+session->inpos )
		return SOD_OK;
	if ( qu )
	{
		sod_throw_error(SE_PARSE,"Quotation mark is needed");
	}
	
	session->instart=src-session->inbuf+1;
	*datacnt=i;
	
	pstop();
	return SOD_AGAIN;
}

int sod_put_message( sod_session *session, char *data )
{
	int iret;
	pstart();
	
	if ( (iret=pthread_mutex_lock(&session->msg_mutex))!=0 )
		sod_throw_error(SE_MUTEX,vstr("Failed to lock a mutex (code=%d)",iret));
	
	if ( SOD_BUFFER_SIZE-session->outpos<=strlen(data) )
	{
		//	сообщение длинное, сдвигаем буффер
		if ( SOD_BUFFER_SIZE-session->outpos+session->outstart<=strlen(data) )
			sod_throw_error(SE_PARSE,"Message size is too big (buffer is full)");
		memmove(session->outbuf,session->outbuf+session->outstart,session->outpos-session->outstart);
		session->outpos-=session->outstart;
		session->outstart=0;
	}
	
	strcpy(session->outbuf+session->outpos,data);
	session->outpos+=strlen(data)+1;
	
	pdebug("*** SENDING (size=%d) *** \"%s\"\n",strlen(data),data);
	
	if ( (iret=pthread_mutex_unlock(&session->msg_mutex))!=0 )
		sod_throw_error(SE_MUTEX,vstr("Failed to unlock a mutex (code=%d)",iret));
	
	pstop();
	return SOD_OK;
}
