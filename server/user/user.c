/***************************************************************************
 *            user.c
 *
 *  Fri Oct 28 12:40:29 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <libintl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "user/user.h"
#include "main/ecode.h"
#include "errors/ecode.h"
#include "errors/debug.h"
#include "log/log.h"
#include "database/db.h"
#include "other/other.h"
#include "parse/parse.h"
#include "main/config.h"
#include "network/net.h"
#include "time.h"
#include "utree.h"

abtree_t_usr *utree;

ecode_t usr_init( void )
{
	pstart();
	
	if ( (utree=abtree_create_usr())==NULL )
	{
		plog(gettext("Failed to create a tree (kucode=%d)\n"),kucode);
		return E_KU2;
	}
	
	if ( (ecode=db_init())!=E_NONE )
	{
		plog(gettext("Failed to initialize the data souce\n"));
		abtree_free_usr(utree);
		return ecode;
	}
	
	plog(gettext("Loaded module 'user`!\n"));
	
	pstop();
	return E_NONE;
}

ecode_t usr_halt( void )
{
	ecode_t ec;
	int i;
	net_id *id;
	pstart();
	
	plog(gettext("Module 'user` is beging stopped!\n"));
	
	//	отсоединение пользователей
	for ( i=0; i<sock_list->size; i++ )
	{
		if ( (id=kulist_next_net(sock_list))==NULL )
		{
			plog(gettext("Failed to get next element of the list (kucode=%d)\n"),kucode);
			ecode=E_KU2;
		}
		if ( ((usr_record*)id->data)->dataflags!=0 )
		{
			dfree(((usr_record*)id->data)->data);
		}
		usr_rem((usr_record*)id->data);
	}
	
	//	закрытие базы данных
	if ( (ec=db_halt())!=E_NONE )
	{
		ecode=ec;
		plog(gettext("Failed to halt the data source\n"));
	}
	
	//	удаление дерева
	if ( abtree_free_usr(utree)!=KE_NONE )
	{
		plog(gettext("Failed to free the tree (kucode=%d)\n"),kucode);
		ecode=E_KU2;
	}
	
	if ( ecode!=E_NONE ) return ecode;
	
	pstop();
	return E_NONE;
}

ecode_t usr_add( net_id *id )
{
	usr_record *usr;
	pstart();
	
	if ( (usr=dmalloc(sizeof(usr_record)))==NULL )
	{
		plog(gettext("Memory allocation failed!\n"));
		return E_MEMORY;
	}
	
	usr->sock=id->sock;
	usr->id=-1;
	usr->inpos=0;
	usr->instart=0;
	usr->msg=0;
	usr->outpos=0;
	usr->outstart=0;
	
	usr->dataflags=0;
	
	usr->netid=id;
	id->data=(void*)usr;
	
	if ( (ecode=usr_write(usr,"SERVER "STR(PROTOCOL)" "STR(BUFFER_SIZE_USER)))!=E_NONE )
		return ecode;
	
	pstop();
	return E_NONE;
}

ecode_t usr_rem( usr_record *usr )
{
	pstart();
	
	if ( usr->id>-1 )
	{
		db_nr_query(vstr("UPDATE logins SET online=0 WHERE id=%d",usr->id));
		abtree_rem_usr(utree,usr);
	}
	usr->netid->data=NULL;
	dfree(usr);
	
	pstop();
	return E_NONE;
}

ecode_t usr_auth( usr_record *usr )
{
	int res;
	char **dbres;
	char *ch;
	pstart();
	
	//	проверка параметров и команды
	if ( p_argc!=3 )
	{
		plog(gettext("Authorization failed due to invalid command (sock=%d)\n"),usr->sock);
		return E_ARG;
	}
	if ( strcmp(p_arg[0],"AUTH")!=0 )
	{
		plog(gettext("Authorization failed due to invalid command (sock=%d)\n"),usr->sock);
		return E_ARG;
	}
	
	//	проверка пароля и логина + запрос данных
	res=db_query(vstr("SELECT id, access, max_photoalbums, max_groups "\
			"FROM logins WHERE login='%s' AND password='%s'",p_arg[1],p_arg[2]));
	if ( res!=1 )
	{
		plog(gettext("Authorization failed due to invalid login (sock=%d)\n"),usr->sock);
		return E_AUTH;
	}
	
	//	обработка результатов запроса
	dbres=db_row();
	if ( dbres==NULL ) return E_AUTH;
	res=strtol(dbres[0],&ch,10);
	if ( *ch!=0 )
	{
		plog(gettext("Invalid id, possible database error\n"));
		return E_AUTH;
	}
	usr->id=res;
	usr->level=strtol(dbres[1],&ch,10);
	if ( *ch!=0 )
	{
		plog(gettext("Invalid access level, possible database error\n"));
		return E_AUTH;
	}
	usr->max_ph=strtol(dbres[2],&ch,10);
	if ( *ch!=0 )
	{
		plog(gettext("Invalid photoalbum maximum, possible database error\n"));
		return E_AUTH;
	}
	usr->max_gr=strtol(dbres[3],&ch,10);
	if ( *ch!=0 )
	{
		plog(gettext("Invalid group maximum, possible database error\n"));
		return E_AUTH;
	}
	
	usr->data=NULL;
	usr->data_sz=0;
	usr->data_cur=0;
	
	if ( (kucode=abtree_ins_usr(utree,usr))!=KE_NONE )
	{
		plog(gettext("Failed to insert data to the tree (kucode=%d)\n"),kucode);
		return E_KU2;
	}
	
	if ( (ecode=usr_write(usr,"ACCEPTED"))!=E_NONE )
		return ecode;
	
	db_nr_query(vstr("UPDATE logins SET last_ip='%s', last_date=%d, online=1 WHERE id=%d",usr->netid->ip,time(NULL),usr->id));
	
	plog(gettext("User %s at level %d with id %d has authorizated (sock=%d)\n"),p_arg[1],res,usr->id,usr->sock);
	
	pstop();
	return E_NONE;
}

usr_record *usr_online( int id )
{
	usr_record *usr, susr;
	pstart();
	
	susr.id=id;
	usr=abtree_search_usr(utree,&susr);
	
	pstop();
	return usr;
}

ecode_t usr_manage( usr_record *usr )
{
	pstart();
	
	if ( (ecode=usr_read(usr))!=E_NONE )
	{
		if ( ecode==E_FILE )
		{
			plog(gettext("Cannot read from the socket, disconnecting (sock=%d)\n"),usr->sock);
		}	else
		if ( ecode==E_OBUFFER )
		{
			usr_write(usr,"DISCON SIZE");
		}
		return ecode;
	}
	
	while ( (ecode=usr_getmsg(usr))==E_AGAIN )
	{
		//	проверка на авторизацию
		if ( usr->id==-1 )
		{
			if ( (ecode=usr_auth(usr))!=E_NONE )
			{
				usr_write(usr,"DISCON AUTH");
				return ecode;
			}
		}	else
		{
			//	обновление состояния
			db_nr_query(vstr("UPDATE logins SET last_ip='%s', last_date=%d, online=1 "\
					"WHERE id=%d",usr->netid->ip,time(NULL),usr->id));
			//	здесь обработка сообщений
			if ( parse_msg(usr) )
			{
				return E_DISCONNECT;
			}
		}
	}
	//	была ли ошибка?
	if ( ecode!=E_NONE )
	{
		usr_write(usr,"DISCON CMD");
		return ecode;
	}
	
	pstop();
	return E_NONE;
}

ecode_t usr_read( usr_record *usr )
{
	int rcnt;
	pstart();
	
	if ( usr->dataflags )
	{
		//	чтение бинарных данных
		rcnt=read(usr->sock,usr->data+usr->data_cur,usr->data_sz-usr->data_cur);
		if ( (rcnt==-1) && (errno!=EAGAIN) )
		{
			plog(gettext("Reading failed (fd=%d): %s\n"),usr->sock,strerror(errno));
			return E_FILE;
		}	else
		if ( rcnt==0 )
		{
			return E_DISCONNECT;
		}	else
		if ( rcnt>0 )
		{
			usr->data_cur+=rcnt;
		}
		if ( usr->data_cur==usr->data_sz )
		{
			usr->dataflags|=UF_DATA_HERE;
		}
	}	else
	{
		if ( usr->inpos+BUFFER_MINIMUM_FREE>=BUFFER_SIZE_USER )
		{
			//	буффер переполнен, сдвигаем
			if ( usr->instart==0 )
			{
				//	нет места, ошибка
				plog(gettext("Reading buffer is full, cannot read any more (fd=%d)\n"),usr->sock);
				return E_OBUFFER;
			}	else
			{
				memmove(usr->in,usr->in+usr->inpos,usr->inpos-usr->instart);
				usr->inpos-=usr->instart;
				usr->instart=0;
			}
		}
		
		//	чтение в буффер
		rcnt=read(usr->sock,usr->in+usr->inpos,BUFFER_SIZE_USER-usr->inpos);
		if ( (rcnt==-1) && (errno!=EAGAIN) )
		{
			plog(gettext("Reading failed (fd=%d): %s\n"),usr->sock,strerror(errno));
			return E_FILE;
		}	else
		if ( rcnt==0 )
		{
			return E_DISCONNECT;
		}	else
		if ( rcnt>0 )
		{
			usr->inpos+=rcnt;
		}
	}
	
	pstop();
	return E_NONE;
}

ecode_t usr_getmsg( usr_record *usr )
{
	char *cur;
	pstart();
	
	if ( usr->dataflags )
	{
		pstop();
		if ( (usr->dataflags&UF_DATA_HERE)==UF_DATA_HERE )
		{
			pdebug("fl: %d\n",usr->dataflags);
			usr->dataflags&=~UF_DATA_HERE;
			pdebug("fl: %d\n",usr->dataflags);
			return E_AGAIN;
		}	else
		{
			return E_NONE;
		}
	}
	
	cur=usr->in+usr->instart;
	while ( (*cur!=0) && (cur-usr->in<usr->inpos) )
		cur++;
	
	if ( cur-usr->in>=usr->inpos )
	{
		pstop();
		return E_NONE;
	}
	if ( *cur==0 )
	{
		usr->msg=usr->instart;
		usr->instart=cur-usr->in+1;
		#ifdef LOG_NET
		plog("*** RECEIVED (fd=%d, uid=%d, size=%d) *** \"%s\"\n",usr->sock,usr->id,strlen(usr->in+usr->msg),usr->in+usr->msg);
		#endif
		
		if ( parse(usr->in+usr->msg)<1 )
		{
			plog(gettext("Parsing failed or nothing to parse (sock=%d)\n"),usr->sock);
			return E_ARG;
		}
	}
	
	pstop();
	return *cur==0?E_AGAIN:E_NONE;
}

ecode_t usr_write( usr_record *usr, char *msg )
{
	int scnt;
	pstart();
	
	#ifdef LOG_NET
	plog("*** SENDING (fd=%d, uid=%d, size=%d) *** \"%s\"\n",usr->sock,usr->id,strlen(msg),msg);
	#endif
	
	if ( usr->outstart!=usr->outpos )
	{
		//	в бефере чтото есть
		if ( (ecode=usr_writel(usr))!=E_NONE )
			return ecode;
	}
	
	if ( usr->outstart==usr->outpos )
	{
		scnt=write(usr->sock,msg,strlen(msg)+1);
		if ( (scnt==-1) && (errno!=EAGAIN) )
		{
			plog(gettext("Writing failed (fd=%d): %s\n"),usr->sock,strerror(errno));
			return E_FILE;
		}	else
		if ( scnt==0 )
		{
			return E_DISCONNECT;
		}
	}	else
		scnt=0;
	
	if ( scnt<strlen(msg)+1 )
	{
		//	отослалось не всё
		msg+=scnt;
		if ( usr->outpos+BUFFER_MINIMUM_FREE>=BUFFER_SIZE_USER )
		{
			//	буффер полный, сдвигаем
			if ( usr->outstart==0 )
			{
				//	нет места, ошибка
				plog(gettext("Writing buffer is full, cannot write any more (fd=%d)\n"),usr->sock);
				return E_OBUFFER;
			}	else
			{
				memmove(usr->out,usr->out+usr->outstart,usr->outpos-usr->outstart);
				usr->outpos-=usr->outstart;
				usr->outstart=0;
			}
		}
		if ( strlen(msg)+1>BUFFER_SIZE_USER-usr->outpos )
		{
			//	нет места, ошибка
			plog(gettext("Writing buffer is full, cannot write any more (fd=%d)\n"),usr->sock);
			return E_OBUFFER;
		}
		memcpy(usr->out+usr->outpos,msg,strlen(msg)+1);
		usr->outpos+=strlen(msg)+1;
		if ( scnt!=0 )
		{
			if ( (kucode=kulist_ins_net(sock_wlist,usr->netid))!=KE_NONE )
			{
				plog(gettext("Failed to insert an element into the list (kucode=%d)\n"),kucode);
				return E_KU2;
			}
		}
	}
	
	pstop();
	return E_NONE;
}

ecode_t usr_writel( usr_record *usr )
{
	int scnt;
	pstart();
	
	scnt=write(usr->sock,usr->out+usr->outstart,usr->outpos-usr->outstart);
	if ( (scnt==-1) && (errno!=EAGAIN) )
	{
		plog(gettext("Writing failed (fd=%d): %s\n"),usr->sock,strerror(errno));
		return E_FILE;
	}	else
	if ( scnt==0 )
	{
		return E_DISCONNECT;
	}	else
	{
		if ( scnt!=-1 )
			usr->outstart+=scnt;
		if ( usr->outstart!=usr->outpos )
		{
			//	записалось не всё, повторить
			if ( (kucode=kulist_ins_net(sock_wlist,usr->netid))!=KE_NONE )
			{
				plog(gettext("Failed to insert an element into the list (kucode=%d)\n"),kucode);
				return E_KU2;
			}
		}
	}
	
	pstop();
	return E_NONE;
}
