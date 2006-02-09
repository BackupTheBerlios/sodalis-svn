/***************************************************************************
 *            msgu.c
 *
 *  Fri Dec  2 18:00:48 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include "messages.h"
#include "common.h"
#include "main/options.h"

int abil_msgu( usr_record *usr )
{
	int uid, tid;
	char **dbres;
	usr_record *dst;
	char *p;
	pstart();
	
	switch ( p_argc )
	{
		case 2:	// запрос на получение нового
			abil_num(uid,1,p,"MSGU");
			if ( db_query(vstr("SELECT whose, who_sent, date, message, type FROM messages WHERE id='%d'",uid))!=1 )
			{
				REQ_FAIL("MSGU");
				SENDU(usr,"FAILED MSGU");
				return 0;
			}
			if ( (dbres=db_row())==NULL )
			{
				REQ_FAIL("MSGU");
				SENDU(usr,"FAILED MSGU");
				return 0;
			}
			
			//	разрешено ли получить это сообщение?
			if ( !strcmp(dbres[4],"i") )
			{
				abil_str2num(uid,dbres[0],p,"MSGU");
				tid=1;
			}	else
			if ( !strcmp(dbres[4],"s") )
			{
				abil_str2num(uid,dbres[1],p,"MSGU");
				tid=0;
			}	else
			{
				plog(gettext("Invalid requested message type (uid=%d on %s)\n"),usr->id,"MSGU");
				SENDU(usr,"FAILED MSGU");
				return 0;
			}
			if ( uid!=usr->id )
			{
				plog(gettext("Message %d cannot be requested by the user (uid=%d)\n"),usr->id);
				SENDU(usr,"FAILED MSGU");
				return 0;
			}
			
			//	отправка сообщения
			SENDU(usr,vstr("MSGU %s %s %s \"%s\"",p_arg[1],dbres[tid],dbres[2],dbres[3]));
			db_nr_query(vstr("UPDATE messages SET know='+' WHERE id='%s'",p_arg[1]));
			
			break;
			
		case 3:	// отправление сообщения с типом по умолчанию
			if ( o_defmsgtype ) tid=1; else tid=0;
			
			break;
			
		case 4: // отправление сообщения
			if ( !strcmp(p_arg[2],"i") )
			{
				tid=0;
			}	else
			if ( !strcmp(p_arg[2],"s") )
			{
				tid=1;
			}	else
			{
				plog("Invalid message type (uid=%d on %s)\n",usr->id,"MSGU");
				usr_write(usr,"DISCON CMD");
				return 1;
			}
			
			break;
			
		default:	// неверное кол-во параметров
			plog(gettext("Invalid count of parametres (uid=%d on %s)\n"),usr->id,"MSGU");
			usr_write(usr,"DISCON CMD");
			return 1;
	}
	
	if ( p_argc>=3 )
	{
		abil_num(uid,1,p,"MSGU");
		//	обновление базы двнных
		if ( tid )
		if ( db_nr_query(vstr("INSERT INTO messages VALUES (DEFAULT, %d, %d, %d, '%s', DEFAULT, DEFAULT, 's')", \
			uid,usr->id,time(NULL),p_arg[p_argc==3?2:3]))!=E_NONE )
		{
			REQ_FAIL("MSGU");
			SENDU(usr,"FAILED MSGU");
			return 0;
		}
		if ( db_nr_query(vstr("INSERT INTO messages VALUES (DEFAULT, %d, %d, %d, '%s', DEFAULT, DEFAULT, 'i')", \
			uid,usr->id,time(NULL),p_arg[p_argc==3?2:3]))!=E_NONE )
		{
			REQ_FAIL("MSGU");
			SENDU(usr,"FAILED MSGU");
			return 0;
		}
		
		//	если пользователь онлайн, то отправить оповещение
		dst=usr_online(uid);
		if ( dst!=NULL )
		{
			usr_write(dst,vstr("MSGU %d %d",db_id(),usr->id));
		}
	}
	
	pstop();
	return 0;
}

int abil_msgg( usr_record *usr )
{
	int tip, gid, uid, i;
	char *p, **dbres;
	usr_record *dst;
	pstart();
	
	switch ( p_argc )
	{
		case 3:	// тип по умолчанию
			if ( o_defmsgtype ) tip=1; else tip=0;
			
			break;
			
		case 4: // сообщение с указанным типом
			if ( !strcmp(p_arg[2],"i") )
			{
				tip=0;
			}	else
			if ( !strcmp(p_arg[2],"s") )
			{
				tip=1;
			}	else
			{
				plog("Invalid message type (uid=%d on %s)\n",usr->id,"MSGG");
				usr_write(usr,"DISCON CMD");
				return 1;
			}
			
			break;
			
		default:	// неверное кол-во параметров
			plog(gettext("Invalid count of parametres (uid=%d on %s)\n"),usr->id,"MSGG");
			usr_write(usr,"DISCON CMD");
			return 1;
	}
	
	//	принадлежит лт группа пользователю?
	abil_num(gid,1,p,"MSGG");
	if ( db_query(vstr("SELECT owner FROM groups WHERE id='%d'",gid))!=1 )
	{
		REQ_FAIL("MSGG");
		SENDU(usr,"FAILED MSGG");
		return 0;
	}
	if ( (dbres=db_row())==NULL )
	{
		REQ_FAIL("MSGG");
		SENDU(usr,"FAILED MSGG");
		return 0;
	}
	abil_str2num(uid,dbres[0],p,"MSGG");
	if ( uid!=usr->id )
	{
		plog(gettext("Group %d does not belong to user (uid=%d on %s)\n"),gid,uid,"MSGG");
		SENDU(usr,"FAILED MSGG");
		return 0;
	}
	
	//	поиск друзей из этой группы
	if ( (uid=db_query(vstr("SELECT who FROM friends WHERE whose='%d' AND undergroup='%d'",usr->id,gid)))<0 )
	{
		REQ_FAIL("MSGG");
		SENDU(usr,"FAILED MSGG");
		return 0;
	}
	
	//	отправка им сообщений
	for ( i=0; i<uid; i++ )
	{
		if ( (dbres=db_row())==NULL )
		{
			REQ_FAIL("MSGG");
			SENDU(usr,"FAILED MSGG");
			return 0;
		}
		//	обновление базы двнных
		if ( tip )
		if ( db_nr_query(vstr("INSERT INTO messages VALUES (DEFAULT, %s, %d, %d, '%s', DEFAULT, DEFAULT, 's')", \
			dbres[0],usr->id,time(NULL),p_arg[p_argc==3?2:3]))!=E_NONE )
		{
			REQ_FAIL("MSGG");
			SENDU(usr,"FAILED MSGG");
			return 0;
		}
		if ( db_nr_query(vstr("INSERT INTO messages VALUES (DEFAULT, %s, %d, %d, '%s', DEFAULT, DEFAULT, 'i')", \
			dbres[0],usr->id,time(NULL),p_arg[p_argc==3?2:3]))!=E_NONE )
		{
			REQ_FAIL("MSGG");
			SENDU(usr,"FAILED MSGG");
			return 0;
		}
		
		//	если пользователь онлайн, то отправить оповещение
		dst=usr_online(uid);
		if ( dst!=NULL )
		{
			usr_write(dst,vstr("MSGG %d %d",db_id(),usr->id));
		}
	}
	
	pstop();
	return 0;
}

int abil_msgd( usr_record *usr )
{
	int mid, uid;
	char *p, **dbres;
	pstart();
	
	CHECK_ARGC(2,"MSGD");
	abil_num(mid,1,p,"MSGD");
	
	if ( db_query(vstr("SELECT whose, who_sent, type FROM messages WHERE id='%d'",mid))!=1 )
	{
		REQ_FAIL("MSGD");
		SENDU(usr,"FAILED MSGD");
		return 0;
	}
	if ( (dbres=db_row())==NULL )
	{
		REQ_FAIL("MSGD");
		SENDU(usr,"FAILED MSGD");
		return 0;
	}
	
	//	прверка типа и пользователя
	if ( !strcmp(dbres[2],"i") )
	{
		abil_str2num(uid,dbres[0],p,"MSGD");
	}	else
	if ( !strcmp(dbres[2],"s") )
	{
		abil_str2num(uid,dbres[1],p,"MSGD");
	}	else
	{
		plog(gettext("Invalid requested message type (uid=%d on %s)\n"),usr->id,"MSGD");
		SENDU(usr,"FAILED MSGD");
		return 0;
	}
	if ( uid!=usr->id )
	{
		plog(gettext("User cannot delete message which does not belong to him (uid=%d on %s)\n"),usr->id,"MSGD");
		SENDU(usr,"FAILED MSGD");
		return 0;
	}
	
	//	удаление
	if ( db_nr_query(vstr("DELETE FROM messages WHERE id='%d'",mid))!=E_NONE )
	{
		REQ_FAIL("MSGD");
		SENDU(usr,"FAILED MSGD");
		return 0;
	}
	
	pstop();
	return 0;
}

int abil_lsm( usr_record *usr )
{
	struct
	{
		enum
		{
			lsm_NONE,
			lsm_NEW,
			lsm_OLD,
			lsm_SENT
		}	type;
		int tfor;
		int after;
		int before;
	}	lsm_p={lsm_NONE, -1, -1, -1};
	int i, tmp;
	char *query, *p;
	char **dbres;
	pstart();
	
	//	сбор параметров
	for ( i=1; i<p_argc; i++ )
	{
		if ( !strcmp(p_arg[i],"NEW") && (lsm_p.type==lsm_NONE) )
		{
			lsm_p.type=lsm_NEW;
		}	else
		if ( !strcmp(p_arg[i],"OLD") && (lsm_p.type==lsm_NONE) )
		{
			lsm_p.type=lsm_OLD;
		}	else
		if ( !strcmp(p_arg[i],"SENT") && (lsm_p.type==lsm_NONE) )
		{
			lsm_p.type=lsm_SENT;
		}	else
		if ( !strcmp(p_arg[i],"FOR") && (lsm_p.tfor==-1) && (i+1<p_argc) )
		{
			i++;
			abil_num(tmp,i,p,"LSM");
			if ( tmp<=0 )
			{
				plog(gettext("Invalid ID (uid=%d on %s)\n"),usr->id,"LSM");
				usr_write(usr,"DISCON CMD");
				return 1;
			}
			lsm_p.tfor=tmp;
		}	else
		if ( !strcmp(p_arg[i],"AFTER") && (lsm_p.after==-1) && (i+1<p_argc) )
		{
			i++;
			abil_num(tmp,i,p,"LSM");
			if ( tmp<=0 )
			{
				plog(gettext("Invalid time (uid=%d on %s)\n"),usr->id,"LSM");
				usr_write(usr,"DISCON CMD");
				return 1;
			}
			lsm_p.after=tmp;
		}	else
		if ( !strcmp(p_arg[i],"BEFORE") && (lsm_p.before==-1) && (i+1<p_argc) )
		{
			i++;
			abil_num(tmp,i,p,"LSM");
			if ( tmp<=0 )
			{
				plog(gettext("Invalid time (uid=%d on %s)\n"),usr->id,"LSM");
				usr_write(usr,"DISCON CMD");
				return 1;
			}
			lsm_p.before=tmp;
		}	else
		{
			plog(gettext("Invalid incoming parametre (uid=%d on %s)\n"),usr->id,"LSM");
			usr_write(usr,"DISCON CMD");
			return 1;
		}
	}
	
	//	обработка запроса
	switch ( lsm_p.type )
	{
		case lsm_NONE:
			query=vstr("SELECT id, who_sent FROM messages WHERE whose='%d' AND type='i'",usr->id);
			break;
		case lsm_NEW:
			query=vstr("SELECT id, who_sent FROM messages WHERE whose='%d' AND type='i' AND know='-'",usr->id);
			break;
		case lsm_OLD:
			query=vstr("SELECT id, who_sent FROM messages WHERE whose='%d' AND type='i' AND know='+'",usr->id);
			break;
		case lsm_SENT:
			query=vstr("SELECT id, whose FROM messages WHERE who_sent='%d' AND type='s'",usr->id);
	}
	if ( lsm_p.tfor!=-1 )
	{
		if ( lsm_p.type==lsm_SENT )
			query=qstr(query,vstr(" AND whose='%d'",lsm_p.tfor)); else
			query=qstr(query,vstr(" AND who_sent='%d'",lsm_p.tfor));
	}
	if ( lsm_p.after!=-1 )
	{
		query=qstr(query,vstr(" AND date>'%d'",lsm_p.after));
	}
	if ( lsm_p.before!=-1 )
	{
		query=qstr(query,vstr(" AND date<'%d'",lsm_p.before));
	}
	
	//	выплнение запроса
	tmp=db_query(query);
	if ( tmp<0 )
	{
		REQ_FAIL("LSM");
		return 0;
	}
	for ( i=0; i<tmp; i++ )
	{
		dbres=db_row();
		if ( dbres==NULL )
		{
			REQ_FAIL("LSM");
			return 0;
		}
		SENDU(usr,vstr("MSGU %s %s",dbres[0],dbres[1]));
	}
	
	pstop();
	return 0;
}
