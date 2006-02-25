/***************************************************************************
 *            tools.c
 *
 *  Sun Feb 12 17:48:59 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>
#include <libintl.h>
#include <string.h>

#include "tools.h"
#include "ecode.h"
#include "errors/debug.h"
#include "database/db.h"
#include "dialogue/dialogue.h"
#include "other/other.h"
#include "options.h"

int toolkey=0;

ecode_t tool_admindb( void )
{
	int admin_goon=1;
	ecode_t ec;
	char *cmd;
	
	pstart();
	
	if ( (ec=db_init())!=E_NONE )
		return ec;
	
	dlgue_stream(stdin,stdout);
	dlgue_claim(gettext(vstr("Welcome to the Sodalis Database administration tool.\n" \
			"Current database: %s@%s/%s\n" \
			"Input 'help` for help information.",o_sqlname,o_sqlhost,o_sqldbase)));
	while ( admin_goon )
	{
		if ( dlgue_ask(">> ",&cmd,DLGUE_STRING)!=KE_NONE )
			return E_KU2;
		if ( !strcmp(cmd,"help") )
		{
			dlgue_claim(gettext("Possible commands:\n" \
					"\t'help`: show this help;\n" \
					"\t'quit`: finish the administration;\n" \
					"\t'add user`: add a new user account;\n" \
					"\t'blank`: blank the existing database or create a new one;"));
		}	else
		if ( !strcmp(cmd,"quit") )
		{
			admin_goon=0;
		}	else
		if ( !strcmp(cmd,"add user") )
		{
			if ( (ec=tool_adduser())!=E_NONE )
				return ec;
		}	else
		if ( !strcmp(cmd,"remove user") )
		{
			if ( (ec=tool_adduser())!=E_NONE )
				return ec;
		}	else
		if ( !strcmp(cmd,"user password") )
		{
			if ( (ec=tool_adduser())!=E_NONE )
				return ec;
		}	else
		if ( !strcmp(cmd,"blank") )
		{
			if ( (ec=tool_blankdb())!=E_NONE )
				return ec;
		}	else
		{
			dlgue_claim(gettext("Invalid command, see help for possible ones."));
		}
	}
	
	if ( (ec=db_halt())!=E_NONE )
		return ec;
	
	pstop();
	return E_NONE;
}

ecode_t tool_adduser( void )
{
	char *login, *password, *name;
	int access, grp, alb;
	int t;
	ecode_t ec;
	pstart();
	
	if ( dlgue_ask(gettext("Login"),&login,DLGUE_STRING)!=KE_NONE )
		return E_KU2;
	login=vstr(login);
	if ( dlgue_ask(gettext("Password"),&password,DLGUE_STRING)!=KE_NONE )
		return E_KU2;
	password=vstr(password);
	if ( dlgue_ask(gettext("Name"),&name,DLGUE_STRING)!=KE_NONE )
		return E_KU2;
	name=vstr(name);
	if ( dlgue_ask(gettext("Maximum photo albums"),&alb,DLGUE_INT)!=KE_NONE )
		return E_KU2;
	if ( dlgue_ask(gettext("Maximum groups"),&grp,DLGUE_INT)!=KE_NONE )
		return E_KU2;
	if ( dlgue_ask(gettext("Access level"),&access,DLGUE_INT)!=KE_NONE )
		return E_KU2;
	
	if ( dlgue_ask(vstr(gettext("You are going to create a user \"%s\" %s:%s " \
			"with access level %d (albums=%d, groups=%d).\n" \
			"Continue?"),name,login,password,access,alb,grp),&t,DLGUE_BOOL)!=KE_NONE )
		return E_KU2;
	
	if ( t )
	{
		if ( (ec=db_nr_query(vstr("INSERT logins VALUES (DEFAULT, '%s', '%s', '%s', " \
				"DEFAULT, DEFAULT, %d, %d, %d, DEFAULT, DEFAULT)",login,password,name, \
				access,alb,grp)))!=E_NONE )
			return ec;
		dlgue_claim(gettext("User was added successfully!"));
	}
	
	pstop();
	return E_NONE;
}

ecode_t tool_blankdb( void )
{
	kucode_t kec;
	int t;
	char *backuppath=NULL;
	pstart();
	
	kec=dlgue_ask(gettext("WARNING! You are going to crete a blank database, " \
			"this will drop all the existing tables! " \
			"Would You like to back up the current database if it exists?"), \
			&t,DLGUE_BOOL|DLGUE_CANCEL);
	
	if ( kec==KE_EMPTY )
	{
		//	отмена операции
		dlgue_claim(gettext("Operation canceled!"));
		return E_NONE;
	}
	if ( kec!=KE_NONE )
		return E_KU2;
	
	if ( t )
	{
		kec=dlgue_ask(gettext("Please select the direcotry where the back ups are to be stored"), \
				&backuppath,DLGUE_STRING|DLGUE_CANCEL);
		if ( kec==KE_EMPTY )
		{
			dlgue_claim(gettext("Operation canceled!"));
			return E_NONE;
		}
		if ( kec!=KE_NONE )
			return E_KU2;
	}
	
	if ( backuppath!=NULL )
	{
		//	backing up
		dlgue_claim("DEVELOPMENT VERSION, backing up is not implemented!");
	}
	
	if (
		(db_nr_query("DROP TABLE IF EXISTS `friends`")!=E_NONE) ||
		(db_nr_query("CREATE TABLE friends ( " \
			"`id` int(11) NOT NULL auto_increment, " \
			"`whose` int(11) NOT NULL default '0', " \
			"`who` int(11) NOT NULL default '0', " \
			"`undergroup` int(11) default '-1', " \
			"`date` int(11) default '0', " \
			"`knew` char(1) default '-', " \
			"PRIMARY KEY  (`id`) " \
			") TYPE=MyISAM")!=E_NONE) ||
		(db_nr_query("DROP TABLE IF EXISTS `groups`")!=E_NONE) ||
		(db_nr_query("CREATE TABLE `groups` ( " \
			"`id` int(11) NOT NULL auto_increment, " \
			"`owner` int(11) NOT NULL default '0', " \
			"`caption` char(50) NOT NULL default '', " \
			"`about` char(255) NOT NULL default '', " \
			"PRIMARY KEY  (`id`) " \
			") TYPE=MyISAM")!=E_NONE) ||
		(db_nr_query("DROP TABLE IF EXISTS `invites`")!=E_NONE) ||
		(db_nr_query("CREATE TABLE `invites` ( " \
			"`id` int(11) NOT NULL auto_increment, " \
			"`whose` int(11) NOT NULL default '0', " \
			"`who_invited` int(11) NOT NULL default '0', " \
			"`invite_text` char(255) default NULL, " \
			"`date` int(11) NOT NULL default '0', " \
			"`shown` char(1) default '-', " \
			"PRIMARY KEY  (`id`) " \
			") TYPE=MyISAM")!=E_NONE) ||
		(db_nr_query("DROP TABLE IF EXISTS `logins`")!=E_NONE) ||
		(db_nr_query("CREATE TABLE `logins` ( " \
			"`id` int(11) NOT NULL auto_increment, " \
			"`login` char(100) NOT NULL default '', " \
			"`password` char(100) default NULL, " \
			"`name` char(30) default NULL, " \
			"`last_ip` char(15) NOT NULL default 'none', " \
			"`last_date` int(11) NOT NULL default '0', " \
			"`access` int(11) NOT NULL default '1', " \
			"`max_photoalbums` int(1) default '1', " \
			"`max_groups` int(11) default '3', " \
			"`main_photo` int(11) NOT NULL default '-1', " \
			"`online` char(1) default '0', " \
			"PRIMARY KEY  (`id`), " \
			"UNIQUE KEY `login` (`login`) " \
			") TYPE=MyISAM")!=E_NONE) ||
		(db_nr_query("DROP TABLE IF EXISTS `messages`")!=E_NONE) ||
		(db_nr_query("CREATE TABLE `messages` ( " \
			"`id` int(11) NOT NULL auto_increment, " \
			"`whose` int(11) NOT NULL default '0', " \
			"`who_sent` int(11) NOT NULL default '0', " \
			"`date` int(11) default '0', " \
			"`message` text, " \
			"`know` char(1) default '-', " \
			"`icon` int(1) default '0', " \
			"`type` char(1) default 'i', " \
			"PRIMARY KEY  (`id`) " \
			") TYPE=MyISAM")!=E_NONE) ||
		(db_nr_query("DROP TABLE IF EXISTS `photo_albums`")!=E_NONE) ||
		(db_nr_query("CREATE TABLE `photo_albums` ( " \
			"`id` int(11) NOT NULL auto_increment, " \
			"`owner` int(11) NOT NULL default '0', " \
			"`photos` int(11) default '0', " \
			"`maxphotos` int(11) default '5', " \
			"`caption` char(50) default 'album', " \
			"`about` char(255) default '', " \
			"PRIMARY KEY  (`id`) " \
			") TYPE=MyISAM")!=E_NONE) ||
		(db_nr_query("DROP TABLE IF EXISTS `photos`")!=E_NONE) ||
		(db_nr_query("CREATE TABLE `photos` ( " \
			"`id` int(11) NOT NULL auto_increment, " \
			"`owner` int(11) default NULL, " \
			"`album` int(11) default NULL, " \
			"`caption` char(100) default '', " \
			"`about` char(255) default '', " \
			"`width` int(11) default NULL, " \
			"`height` int(11) default NULL, " \
			"`comment` int(11) NOT NULL default '-1', " \
			"PRIMARY KEY  (`id`) " \
			") TYPE=MyISAM")!=E_NONE)
	){
		dlgue_claim(gettext("Operation failed!"));
	}
	
	pstop();
	return E_NONE;
}
