/***************************************************************************
 *            other.c
 *
 *  Sun Oct 23 10:02:17 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <libintl.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include "other.h"
#include "log/log.h"

char *qstr( char *s1, char *s2 )
{
	static char _qstr_[QSTR_STRCNT][QSTR_STRSIZE];
	static int i=-1;
	if ( (++i)>=QSTR_STRCNT ) i=0;
	sprintf(_qstr_[i],"%s%s",s1,s2);
	#ifdef DEBUG
	if ( strlen(_qstr_[i])>=QSTR_STRSIZE )
	{
		plog(gettext("WARNING(qstr): buffer overflow detected!"));
	}
	#endif
	return _qstr_[i];
}

char *vstr( char *fmt, ... )
{
	va_list ap;
	static char _vstr_[QSTR_STRCNT][QSTR_STRSIZE];
	static int i=-1;
	va_start(ap,fmt);
	if ( (++i)>=QSTR_STRCNT ) i=0;
	vsprintf(_vstr_[i],fmt,ap);
	va_end(ap);
	#ifdef DEBUG
	if ( strlen(_vstr_[i])>=QSTR_STRSIZE )
	{
		plog(gettext("WARNING(vstr): buffer overflow detected!"));
	}
	#endif
	return _vstr_[i];
}

void qdir( char *path )
{
	char *c=path;
	while ( *c ) c++;
	if ( c[-1]!='/' )
	{
		c[0]='/';
		c[1]=0;
	}
}

int qdir2( char *path )
{
	DIR *dir;
	qdir(path);
	dir=opendir(path);
	if ( dir==NULL )
	{
		plog(gettext("Failed to open a directory '%s`: %s\n"),path,strerror(errno));
		return 1;
	}	else
	{
		if ( closedir(dir) ) return 1;
		return 0;
	}
}
