/***************************************************************************
 *            db.c
 *
 *  Tue Nov  8 16:59:14 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <mysql/mysql.h>
#include <libintl.h>

#include "main/ecode.h"
#include "database/db.h"
#include "log/log.h"
#include "errors/debug.h"
#include "main/options.h"

MYSQL sql;
MYSQL_RES *res;

ecode_t db_init( void )
{
	pstart();
	
	mysql_init(&sql);
	if ( mysql_real_connect(&sql,o_sqlhost,o_sqlname,o_sqlpswd,o_sqldbase,0,NULL,0)==NULL )
	{
		plog(gettext("Failed to connect to SQL dataserver: %s\n"),mysql_error(&sql));
		return E_SQL_CONNECT;
	}
	
	pstop();
	return E_NONE;
}

ecode_t db_halt( void )
{
	pstart();
	
	mysql_close(&sql);
	
	pstop();
	return E_NONE;
}

int db_query( char *query )
{
	pstart();
	
	#ifdef LOG_DB
	plog("*** QUERY *** \"%s\"\n",query);
	#endif
	
	if ( mysql_query(&sql,query)!=0 )
	{
		plog(gettext("Failed to send an SQL query: %s\n"),mysql_error(&sql));
		ecode=E_SQL_QUERY;
		return -1;
	}
	
	res=mysql_store_result(&sql);
	if ( res==NULL )
	{
		plog(gettext("Failed to store a result of the query: %s\n"),mysql_error(&sql));
		ecode=E_SQL_RESULT;
		return -1;
	}
	
	pstop();
	return mysql_num_rows(res);
}

ecode_t db_nr_query( char *query )
{
	pstart();
	
	#ifdef LOG_DB
	plog("*** QUERY *** \"%s\"\n",query);
	#endif
	
	if ( mysql_query(&sql,query)!=0 )
	{
		plog(gettext("Failed to send an SQL query: %s\n"),mysql_error(&sql));
		return E_SQL_QUERY;
	}
	
	pstop();
	return E_NONE;
}

char **db_row( void )
{
	MYSQL_ROW row;
	pstart();
	
	row=mysql_fetch_row(res);
	if ( row==NULL )
	{
		plog(gettext("Cannot fetch the row: %s\n"),mysql_error(&sql));
		ecode=E_SQL_FETCH;
		return NULL;
	}
	
	pstop();
	return (char**)row;
}

int db_id( void )
{
	return mysql_insert_id(&sql);
}
