/***************************************************************************
 *            friends.c
 *
 *  Thu Jan 12 18:29:18 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include "friends.h"
#include "common.h"

int abil_invite( usr_record *usr )
{
	int uid, aid, bid;
	char *p, **dbres;
	usr_record *dst;
	pstart();
	
	switch ( p_argc )
	{
		case 2:	// запрос на получение или отмену приглашения
			abil_num(uid,1,p,"INVITE");
			if ( db_query(vstr("SELECT whose, who_invited, invite_text, date FROM invites WHERE id='%d'",uid))!=1 )
			{
				REQ_FAIL("INVITE");
				SENDU(usr,"FAILED INVITE");
				return 0;
			}
			if ( (dbres=db_row())==NULL )
			{
				REQ_FAIL("INVITE");
				SENDU(usr,"FAILED INVITE");
				return 0;
			}
			
			//	запрос или отмена?
			abil_str2num(aid,dbres[0],p,"INVITE");
			abil_str2num(bid,dbres[1],p,"INVITE");
			if ( usr->id==aid )	// запрос
			{
				if ( dbres[2]==NULL )
				{
					SENDU(usr,vstr("INVITE %d %d %s",uid,bid,dbres[3]));
				}	else
				{
					SENDU(usr,vstr("INVITE %d %d %s %s",uid,bid,dbres[3],dbres[2]));
				}
				if ( db_nr_query(vstr("UPDATE invites SET shown='+' WHERE id='%d'",uid))!=E_NONE )
				{
					REQ_FAIL("INVITE");
					SENDU(usr,"FAILED INVITE");
					return 0;
				}
			}	else
			if ( usr->id==bid ) // отмена
			{
				if ( db_nr_query(vstr("DELETE FROM invites WHERE id='%d'",uid))!=E_NONE )
				{
					REQ_FAIL("INVITE");
					SENDU(usr,"FAILED INVITE");
					return 0;
				}
			}	else
			{	// запрос не возможен
				plog(gettext("The invitation %d cannot be requested (uid=%d on %s)\n"),uid,usr->id,"INVITE");
				SENDU(usr,"FAILED INVITE");
				return 0;
			}
			
			break;
		
		case 3:	// создание нового
			abil_num(uid,1,p,"INVITE");
			if ( uid==usr->id )	// нельзя пригласить самого себя
			{
				plog(gettext("User cannot invite himself (uid=%d on %s)\n"),usr->id,"INVITE");
				SENDU(usr,"FAILED INVITE");
				return 0;
			}
			
			//	было ли уже отправлено приглашение?
			if ( db_query(vstr("SELECT id FROM invites WHERE whose='%d' AND who_invited='%d'",uid,usr->id))!=0 )
			{
				plog(gettext("User cannot be invited twice (uid=%d on %s)\n"),usr->id,"INVITE");
				SENDU(usr,"FAILED INVITE");
				return 0;
			}
			
			//	отправка
			if ( !strcmp(p_arg[2],"NULL") )
			{
				if ( db_nr_query(vstr("INSERT INTO invites VALUES (DEFAULT, %d, %d, NULL, %d, DEFAULT)", \
					uid,usr->id,time(NULL)))!=E_NONE )
				{
					REQ_FAIL("INVITE");
					SENDU(usr,"FAILED INVITE");
					return 0;
				}
			}	else
			{
				if ( db_nr_query(vstr("INSERT INTO invites VALUES (DEFAULT, %d, %d, '%s', %d, DEFAULT)", \
					uid,usr->id,p_arg[2],time(NULL)))!=E_NONE )
				{
					REQ_FAIL("INVITE");
					SENDU(usr,"FAILED INVITE");
					return 0;
				}
			}
			
			//	оповещение
			dst=usr_online(uid);
			if ( dst!=NULL )
			{
				usr_write(dst,vstr("INVITE %d %d",db_id(),usr->id));
			}
			
			break;
		
		default:
			plog(gettext("Invalid count of parametres (uid=%d on %s)\n"),usr->id,"INVITE");
			usr_write(usr,"DISCON CMD");
			return 1;
	}
	
	pstop();
	return 0;
}

int abil_invacc( usr_record *usr )
{
	int invid, aid, bid, id1;
	usr_record *dst;
	char *p, **dbres;
	pstart();
	
	CHECK_ARGC(2,"INVACC");
	
	//	отправлено ли данное приглошение отправителю запроса
	abil_num(invid,1,p,"INVACC");
	if ( db_query(vstr("SELECT whose, who_invited FROM invites WHERE id='%d'",invid))!=1 )
	{
		REQ_FAIL("INVACC");
		SENDU(usr,"FAILED INVACC");
		return 0;
	}
	if ( (dbres=db_row())==NULL )
	{
		REQ_FAIL("INVACC");
		SENDU(usr,"FAILED INVACC");
		return 0;
	}
	abil_str2num(aid,dbres[0],p,"INVACC");
	abil_str2num(bid,dbres[1],p,"INVACC");
	if ( usr->id!=aid )
	{
		plog(gettext("The invitation %d cannot be requested (uid=%d on %s)\n"),invid,usr->id,"INVACC");
		SENDU(usr,"FAILED INVACC");
		return 0;
	}
	
	//	принятие приглошения
	if ( db_nr_query(vstr("INSERT INTO friends VALUES (DEFAULT, %d, %d, DEFAULT, %d, DEFAULT)",aid,bid,time(NULL)))!=E_NONE )
	{
		REQ_FAIL("INVACC");
		SENDU(usr,"FAILED INVACC");
		return 0;
	}
	id1=db_id();
	if ( db_nr_query(vstr("INSERT INTO friends VALUES (DEFAULT, %d, %d, DEFAULT, %d, DEFAULT)",bid,aid,time(NULL)))!=E_NONE )
	{
		db_nr_query(vstr("DELETE FROM friends WHERE id='%d'",id1));
		REQ_FAIL("INVACC");
		SENDU(usr,"FAILED INVACC");
		return 0;
	}
	if ( db_nr_query(vstr("DELETE FROM invites WHERE id='%d'",invid))!=E_NONE )
	{
		db_nr_query(vstr("DELETE FROM friends WHERE id='%d' OR id='%d'",db_id(),id1));
		REQ_FAIL("INVACC");
		SENDU(usr,"FAILED INVACC");
		return 0;
	}
	
	//	оповещение
	dst=usr_online(bid);
	if ( dst!=NULL )
	{
		usr_write(dst,vstr("INVACC %d",aid));
	}
	
	pstop();
	return 0;
}

int abil_invrej( usr_record *usr )
{
	int invid, aid, bid;
	usr_record *dst;
	char *p, **dbres;
	pstart();
	
	CHECK_ARGC(2,"INVREJ");
	
	//	отправлено ли данное приглошение отправителю запроса
	abil_num(invid,1,p,"INVACC");
	if ( db_query(vstr("SELECT whose, who_invited FROM invites WHERE id='%d'",invid))!=1 )
	{
		REQ_FAIL("INVACC");
		SENDU(usr,"FAILED INVREJ");
		return 0;
	}
	if ( (dbres=db_row())==NULL )
	{
		REQ_FAIL("INVACC");
		SENDU(usr,"FAILED INVREJ");
		return 0;
	}
	abil_str2num(aid,dbres[0],p,"INVREJ");
	abil_str2num(bid,dbres[1],p,"INVACC");
	if ( usr->id!=aid )
	{
		plog(gettext("The invitation %d cannot be requested (uid=%d on %s)\n"),invid,usr->id,"INVREJ");
		SENDU(usr,"FAILED INVREJ");
		return 0;
	}
	
	//	отклонение приглошения
	if ( db_nr_query(vstr("DELETE FROM invites WHERE id='%d'",invid))!=E_NONE )
	{
		REQ_FAIL("INVREJ");
		SENDU(usr,"FAILED INVREJ");
		return 0;
	}
	
	//	оповещение
	dst=usr_online(bid);
	if ( dst!=NULL )
	{
		usr_write(dst,vstr("INVREJ %d",aid));
	}
	
	pstop();
	return 0;
}

int abil_leave( usr_record *usr )
{
	char *p, **dbres;
	int uid;
	usr_record *dst;
	pstart();
	
	CHECK_ARGC(2,"LEAVE");
	
	//	является ли он другом?
	abil_num(uid,1,p,"LEAVE");
	if ( db_query(vstr("SELECT id FROM friends WHERE whose='%d' AND who='%d'",usr->id,uid))!=1 )
	{
		REQ_FAIL("LEAVE");
		SENDU(usr,"FAILED LEAVE");
		return 0;
	}
	if ( (dbres=db_row())==NULL )
	{
		REQ_FAIL("LEAVE");
		SENDU(usr,"FAILED LEAVE");
		return 0;
	}
	
	//	удаление
	if ( db_nr_query(vstr("DELETE FROM friends WHERE (id='%s') OR (whose='%d' AND who='%d')",dbres[0],uid,usr->id))!=E_NONE )
	{
		REQ_FAIL("LEAVE");
		SENDU(usr,"FAILED LEAVE");
		return 0;
	}
	
	//	оповещение
	dst=usr_online(uid);
	if ( dst!=NULL )
	{
		usr_write(dst,vstr("LEAVE %d",usr->id));
	}
	
	pstop();
	return 0;
}

int abil_chgrp( usr_record *usr )
{
	int uid, gid;
	char *p;
	pstart();
	
	CHECK_ARGC(3,"CHGRP");
	
	abil_num(uid,1,p,"CHGRP");
	abil_num(gid,2,p,"CHGRP");
	
	//	является ли uid другом отправителя?
	if ( db_query(vstr("SELECT id FROM friends WHERE whose='%d' AND who='%d'",usr->id,uid))!=1 )
	{
		plog(gettext("Cannot change group to a nonfriend %d (uid=%d on %s)\n"),uid,usr->id,"CHGRP");
		SENDU(usr,"FAILED CHGRP");
		return 0;
	}
	
	//	принадлежит ли группа отправилелю?
	if ( db_query(vstr("SELECT id FROM groups WHERE owner='%d' AND id='%d'",usr->id,gid))!=1 )
	{
		plog(gettext("Cannot make a friend a member of nonsender group %d (uid=%d on %s)\n"),gid,usr->id,"CHGRP");
		SENDU(usr,"FAILED CHGRP");
		return 0;
	}
	
	//	смена группы
	if ( db_nr_query(vstr("UPDATE friends SET undergroup='%d' WHERE whose='%d' AND who='%d'",gid,usr->id,uid))!=E_NONE )
	{
		REQ_FAIL("CHGRP");
		SENDU(usr,"FAILED CHGRP");
		return 0;
	}
	
	pstop();
	return 0;
}

int abil_group( usr_record *usr )
{
	int cnt, aid;
	char *p, **dbres;
	pstart();
	
	switch ( p_argc )
	{
		case 2:	// удаление группы
			abil_num(cnt,1,p,"GROUP");
			if ( (db_query(vstr("SELECT owner FROM groups WHERE id='%d'",cnt))!=1) || \
					((dbres=db_row())==NULL) )
			{
				REQ_FAIL("GROUP");
				SENDU(usr,"FAILED GROUP");
				return 0;
			}
			abil_str2num(aid,dbres[0],p,"GROUP");
			if ( aid!=usr->id )
			{
				plog(gettext("User cannot delete the group %d (uid=%d on %s)\n"),cnt,aid,"GROUP");
				SENDU(usr,"FAILED GROUP");
				return 0;
			}
			if ( (db_nr_query(vstr("UPDATE friends SET undergroup=DEFAULT WHERE undergroup='%d'",cnt))!=E_NONE) || \
					(db_nr_query(vstr("DELETE FROM groups WHERE id='%d'",cnt))!=E_NONE) )
			{
				REQ_FAIL("GROUP");
				SENDU(usr,"FAILED GROUP");
			}
			break;
			
		case 3:	// создание группы
			cnt=db_query(vstr("SELECT id FROM groups WHERE owner='%d'",usr->id));
			if ( (cnt<0) || (cnt>=usr->max_gr) )
			{
				plog(gettext("User cannot create more groups (uid=%d on %s)\n"),usr->id,"GROUP");
				SENDU(usr,"FAILED GROUP");
				return 0;
			}
			if ( db_nr_query(vstr("INSERT INTO groups VALUES (DEFAULT, %d, '%s', '%s')",usr->id,p_arg[1],p_arg[2]))!=E_NONE )
			{
				REQ_FAIL("GROUP");
				SENDU(usr,"FAILED GROUP");
				return 0;
			}
			
			break;
			
		default:
			plog(gettext("Invalid count of parametres (uid=%d on %s)\n"),usr->id,"GROUP");
			usr_write(usr,"DISCON CMD");
			return 1;
	}
	
	pstop();
	return 0;
}

int abil_lsf( usr_record *usr )
{
	struct
	{
		enum
		{
			lsf_NONE_o,
			lsf_ON,
			lsf_OFF
		}	online;
		enum
		{
			lsf_NONE_ug,
			lsf_GRP,
			lsf_OF
		}	usgr;
		int ugid;
		int new;
	}	lsf_p={lsf_NONE_o, lsf_NONE_ug, -1, 0};
	int i, tmp, friend_cnt;
	char *query, *p;
	char **dbres;
	pstart();
	
	//	сбор параметров
	for ( i=1; i<p_argc; i++ )
	{
		if ( !strcmp(p_arg[i],"ON") && (lsf_p.online==lsf_NONE_o) )
		{
			lsf_p.online=lsf_ON;
		}	else
		if ( !strcmp(p_arg[i],"OFF") && (lsf_p.online==lsf_NONE_o) )
		{
			lsf_p.online=lsf_OFF;
		}	else
		if ( !strcmp(p_arg[i],"GRP") && (lsf_p.usgr==lsf_NONE_ug) )
		{
			i++;
			abil_num(tmp,i,p,"LSF");
			if ( tmp<=0 )
			{
				plog(gettext("Invalid ID (uid=%d on %s)\n"),usr->id,"LSM");
				usr_write(usr,"DISCON CMD");
				return 1;
			}
			lsf_p.ugid=tmp;
			lsf_p.usgr=lsf_GRP;
		}	else
		if ( !strcmp(p_arg[i],"OF") && (lsf_p.usgr==lsf_NONE_ug) )
		{
			i++;
			abil_num(tmp,i,p,"LSF");
			if ( tmp<=0 )
			{
				plog(gettext("Invalid ID (uid=%d on %s)\n"),usr->id,"LSM");
				usr_write(usr,"DISCON CMD");
				return 1;
			}
			lsf_p.ugid=tmp;
			lsf_p.usgr=lsf_OF;
		}	else
		if ( !strcmp(p_arg[i],"NEW") && (lsf_p.online==lsf_NONE_o) && (lsf_p.usgr==lsf_NONE_ug) )
		{
			lsf_p.new=1;
			lsf_p.online=lsf_ON;
			lsf_p.usgr=lsf_OF;
		}	else
		{
			plog(gettext("Invalid incoming parametre (uid=%d on %s)\n"),usr->id,"LSF");
			usr_write(usr,"DISCON CMD");
			return 1;
		}
	}
	
	if ( lsf_p.new==0 )
	{
		//	обработка запроса
		switch ( lsf_p.usgr )
		{
			case lsf_NONE_ug:
				query=vstr("SELECT who FROM friends WHERE whose='%d'",usr->id);
				break;
			case lsf_GRP:
				query=vstr("SELECT who FROM friends WHERE whose='%d' AND undergroup='%d'",usr->id,lsf_p.ugid);
				break;
			case lsf_OF:
				query=vstr("SELECT who FROM friends WHERE whose='%d'",lsf_p.ugid);
		}
		
		//	поиск всех друзей
		tmp=db_query(query);
		if ( friend_cnt<0 )
		{
			REQ_FAIL("LSF");
			return 0;
		}
		if ( tmp==0 ) return 0;
		if ( (dbres=db_row())==NULL )
		{
			REQ_FAIL("LSF");
			return 0;
		}
		query=dbres[0];
		for ( i=1; i<tmp; i++ )
		{
			if ( (dbres=db_row())==NULL )
			{
				REQ_FAIL("LSF");
				return 0;
			}
			query=vstr("%s, %s",query,dbres[0]);
		}
		
		//	обработка запроса
		switch ( lsf_p.online )
		{
			case lsf_NONE_o:
				query=vstr("SELECT log.id, log.name, log.online, " \
						"log.main_photo, fr.undergroup FROM logins log, friends fr " \
						"WHERE log.id IN (%s) AND fr.whose='%d' AND fr.who=log.id", \
						query,lsf_p.usgr==lsf_OF?lsf_p.ugid:usr->id);
				break;
			case lsf_ON:
				query=vstr("SELECT log.id, log.name, log.online, " \
						"log.main_photo, fr.undergroup FROM logins log, friends fr " \
						"WHERE log.id IN (%s) AND log.online=1 AND fr.whose='%d' AND fr.who=log.id", \
						query,lsf_p.usgr==lsf_OF?lsf_p.ugid:usr->id);
				break;
			case lsf_OFF:
				query=vstr("SELECT log.id, log.name, log.online, " \
						"log.main_photo, fr.undergroup FROM logins log, friends fr " \
						"WHERE log.id IN (%s) AND log.online=0 AND fr.whose='%d' AND fr.who=log.id", \
						query,lsf_p.usgr==lsf_OF?lsf_p.ugid:usr->id);
		}
	}	else
	{
		//	список новых друзей
		query=vstr("SELECT log.id, log.name, log.online, log.main_photo, fr.undergroup, fr.id " \
				"FROM logins log, friends fr " \
				"WHERE fr.whose='%d' AND fr.knew='-' AND log.id=fr.who",usr->id);
	}
	
	//	поиск информации о друзьях
	tmp=db_query(query);
	if ( tmp<0 )
	{
		REQ_FAIL("LSF");
		return 0;
	}
	
	for ( i=0; i<tmp; i++ )
	{
		dbres=db_row();
		if ( dbres==NULL )
		{
			REQ_FAIL("LSF");
			return 0;
		}
		SENDU(usr,vstr("USR %s %s %s %s %s",dbres[0],dbres[1]?dbres[1]:"NULL",\
				*(dbres[2])=='1'?"T":"F",dbres[4],dbres[3]));
		if ( lsf_p.new==1 )
		{
			if ( db_nr_query(vstr("UPDATE friends SET knew='+' WHERE id='%s'",dbres[5]))!=E_NONE )
			{
				REQ_FAIL("LSF");
				return 0;
			}
		}
	}
	
	pstop();
	return 0;
}

int abil_lsg( usr_record *usr )
{
	int i, tmp;
	char **dbres;
	pstart();
	
	CHECK_ARGC(1,"LSG");
	
	if ( (tmp=db_query(vstr("SELECT id, caption, about FROM groups WHERE owner='%d'",usr->id)))<0 )
	{
		REQ_FAIL("LSG");
		return 0;
	}
	
	for ( i=0; i<tmp; i++ )
	{
		if ( (dbres=db_row())==NULL )
		{
			REQ_FAIL("LSG");
			return 0;
		}
		SENDU(usr,vstr("GROUP %s \"%s\" \"%s\"",dbres[0],dbres[1],dbres[2]));
	}
	
	
	pstop();
	return 0;
}

int abil_lsinv( usr_record *usr )
{
	char *query;
	int makeshown=0;
	int i, tmp;
	char **dbres;
	pstart();
	
	//	сбор параметров
	switch ( p_argc )
	{
		case 1:	// без параметров
			query=vstr("SELECT id, who_invited, date, invite_text " \
					"FROM invites WHERE whose='%d'",usr->id);
			
			break;
			
		case 2: // с одним параметром
			if ( !strcmp(p_arg[1],"NEW") )
			{
				query=vstr("SELECT id, who_invited, date, invite_text " \
						"FROM invites WHERE whose='%d' AND shown='-'",usr->id);
				makeshown=1;
			}	else
			if ( !strcmp(p_arg[1],"OLD") )
			{
				query=vstr("SELECT id, who_invited, date, invite_text " \
						"FROM invites WHERE whose='%d' AND shown='+'",usr->id);
			}	else
			if ( !strcmp(p_arg[1],"SENT") )
			{
				query=vstr("SELECT id, whose, date, invite_text FROM invites " \
						"WHERE who_invited='%d'",usr->id);
			}	else
			{
				plog(gettext("Invalid incoming parametre (uid=%d on %s)\n"),usr->id,"LSF");
				usr_write(usr,"DISCON CMD");
				return 1;
			}
			
			break;
			
		default:
			plog(gettext("Invalid count of parametres (uid=%d on %s)\n"),usr->id,"LSINV");
			usr_write(usr,"DISCON CMD");
			return 1;
	}
	
	//	выполнение запроса
	if ( (tmp=db_query(query))<0 )
	{
		REQ_FAIL("LSINV");
		return 0;
	}
	for ( i=0; i<tmp; i++ )
	{
		if ( (dbres=db_row())==NULL )
		{
			REQ_FAIL("LSINV");
			return 0;
		}
		if ( !strcmp(dbres[3],"NULL") )
		{
			SENDU(usr,vstr("INVITE %s %s %s",dbres[0],dbres[1],dbres[2]));
		}	else
		{
			SENDU(usr,vstr("INVITE %s %s %s %s",dbres[0],dbres[1],dbres[2],dbres[3]));
		}
		if ( makeshown )
		{
			if ( db_nr_query(vstr("UPDATE invites SET shown='+' WHERE id='%s'",dbres[0]))!=E_NONE )
			{
				REQ_FAIL("LSINV");
				return 0;
			}
		}
	}
	
	pstop();
	return 0;
}
