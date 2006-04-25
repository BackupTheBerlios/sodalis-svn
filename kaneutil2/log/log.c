/***************************************************************************
 *            log.c
 *
 *  Sat Oct 22 12:19:20 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libintl.h>
#include <time.h>
#include <stdarg.h>

#include "log.h"
#include "errors/ecode.h"
#include "errors/debug.h"

FILE *logstream;

kucode_t openlog( char *file )
{
	FILE *f;
	pstart();
	
	f=fopen(file,"a");
	if ( f==NULL )
	{
		plog(gettext("Failed to open a log file: %s\n"),strerror(errno));
		return KE_IO;
	}
	
	logstream=f;
	fprintf(logstream,"========\n");
	plog(gettext("Logging has been started\n"));
	
	pstop();
	return KE_NONE;
}

kucode_t closelog( void )
{
	pstart();
	
	plog(gettext("Logging is being stopped\n"));
	fprintf(logstream,"========\n\n");
	
	if ( fclose(logstream)!=0 )
	{
		plog(gettext("Failed to close a log file: %s\n"),strerror(errno));
		return KE_IO;
	}
	
	logstream=stdout;
	
	pstop();
	return KE_NONE;
}

void plog( char *fmt, ... )
{
	va_list ap;
	time_t t;
	struct tm *tm;
	time(&t);
	tm=localtime(&t);
	
	va_start(ap,fmt);
	fprintf(logstream,"[%.2d.%.2d.%.4d %.2d:%.2d:%.2d] ",
		tm->tm_mday,tm->tm_mon+1,tm->tm_year+1900,
		tm->tm_hour,tm->tm_min,tm->tm_sec);
	vfprintf(logstream,fmt,ap);
	#ifdef DEBUG_LOG
	printf("=== LOG === [%.2d.%.2d.%.4d %.2d:%.2d:%.2d] ",
		tm->tm_mday,tm->tm_mon+1,tm->tm_year+1900,
		tm->tm_hour,tm->tm_min,tm->tm_sec);
	vprintf(fmt,ap);
	#endif
	va_end(ap);
}
