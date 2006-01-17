/***************************************************************************
 *            sodalis.c
 *
 *  Sat Oct 22 11:46:41 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <locale.h>
#include <signal.h>
#include <errno.h>

#include "main/config.h"
#include "main/ecode.h"
#include "errors/debug.h"
#include "log/log.h"
#include "network/net.h"
#include "user/user.h"

#include "list/list_int.h"
#include "list/list_end.h"

#include "stack/stack_int.h"
#include "stack/stack_end.h"

#include "cfgreader/cfg.h"
#include "main/options.h"

//	код ошибки
ecode_t ecode;

//	запущен ли сервер
int server_is_on;

//	список сокетов, доступных на чтение и запись
stack_t_net *sockl;
stack_t_net *sockw;

//	список опций
char *o_cfgfile=CFG_FILE;
int o_port=CFG_PORT;
int o_portlog=CFG_PORTLOG;
char o_logfile[CFG_BUFFER]=CFG_LOGFILE;
char o_sqlhost[CFG_BUFFER]=CFG_SQLHOST;
char o_sqlname[CFG_BUFFER]=CFG_SQLNAME;
char o_sqlpswd[CFG_BUFFER]=CFG_SQLPSWD;
int o_stoplev=CFG_STOPLEV;
int o_breaklev=CFG_BREAKLEV;
int o_defmsgtype=CFG_DEFMSGTYPE;

//	функции
char *errtext( ecode_t c )
{
	switch (c)
	{
		case E_NONE:
			return gettext("No error");
		case E_ANY:
			return gettext("Unknown error");
		case E_MEMORY:
			return gettext("Memory management error");
		case E_FILE:
			return gettext("File input/output error");
		case E_KU2:
			return gettext("An error in Kaneutil2");
		case E_SOCKET:
			return gettext("Socket error");
		case E_BIND:
			return gettext("Socket bind error");
		case E_LISTEN:
			return gettext("Socket listen error");
		case E_SELECT:
			return gettext("Socket select error");
		case E_ACCEPT:
			return gettext("Socket accept error");
		case E_AUTH:
			return gettext("Authorization error");
		case E_SQL_CONNECT:
			return gettext("SQL connection error");
		case E_SQL_QUERY:
			return gettext("SQL query error");
		case E_SQL_RESULT:
			return gettext("SQL result error");
		case E_SQL_FETCH:
			return gettext("SQL fetch error");
		case E_DISCONNECT:
			return gettext("User has disconnected");
		case E_AGAIN:
			return gettext("New data avaliable");
		case E_ARG:
			return gettext("Invalid arguments");
		case E_OBUFFER:
			return gettext("Buffer overflow");
		case E_UBUFFER:
			return gettext("Buffer underrun");
		default:
			return gettext("Invalid error code");
	}
}

void main_close( void )
{
	plog(gettext("Halting the server...\n"));
	
	//	деинициализация модуля 'user`
	if ( (ecode=usr_halt())!=E_NONE )
	{
		plog(gettext("Failed to halt the 'user` subsystem: %s\n"),errtext(ecode));
	}
	
	//	закрытие сети
	if ( (ecode=net_close())!=E_NONE )
	{
		plog("%s\n",errtext(ecode));
	}
	
	//	закрытие лога
	if ( (ecode=closelog())!=E_NONE )
	{
		plog("%s\n",errtext(ecode));
	}
}

void sig_handler( int sig )
{
	static int doubleerr=0;
	if ( !doubleerr )
	{
		plog(gettext("Signal %d recieved, exiting...\n"),sig);
		doubleerr=1;
		main_close();
	}	else
	{
		plog(gettext("Second signal (%d) recieved, exiting, it is bad, very bad!\n"),sig);
	}
	exit(EXIT_FAILURE);
}

int main( int argc, char *argv[] )
{
	int i;
	
	pstart();
	
	/*
		инициализация переменных
	*/
	logstream=stdout;
	server_is_on=1;
	
	/*
		установна обработчиков сигналов
	*/
	if ( (signal(SIGINT,sig_handler)==SIG_ERR) ||
		(signal(SIGSEGV,sig_handler)==SIG_ERR) )
	{
		plog(gettext("Failed to set up a signal handler: %s\n"),strerror(errno));
		return EXIT_FAILURE;
	}
	
	/*
		поддержка языков
	*/
	setlocale(LC_ALL,"");
	bindtextdomain("sodalis","gettext");
	textdomain("sodalis");
	
	/*
		проверка параметров командной строки
	*/
	for ( i=1; i<argc; i++ )
	{
		//	замена стандартного файла настроек
		if ( strcmp(argv[i],"-c")==0 )
		{
			if ( ++i==argc )
			{
				printf(gettext("Config file name is omitted.\n"));
				return EXIT_FAILURE;
			}
			o_cfgfile=argv[i];
		}	else
		//	помощь
		if ( strcmp(argv[i],"--help")==0 )
		{
			printf(gettext("Usage: %s [parametre]\nParametres:\n"),argv[0]);
			printf(gettext("\t'--help`: show out this help;\n"));
			printf(gettext("\t'--about`: show the information about the server;\n"));
			printf(gettext("\t'-c file`: use this file istead of '"CFG_FILE"`;\n"));
			return EXIT_SUCCESS;
		}	else
		//	информация о программе
		if ( strcmp(argv[i],"--about")==0 )
		{
			printf(gettext("SODALIS server by Jelkin Anton kane@mail.berlios.de\n"\
				"Version: "STR(VERSION)"\n"\
				"Protocol: "STR(PROTOCOL)"\n"\
				"Build on: %s %s\n"),__DATE__,__TIME__);
			return EXIT_SUCCESS;
		}	else
		//	неверный ключ
		{
			printf(gettext("Invalid parametre '%s`, see '%s help`\n"),argv[i],argv[0]);
			return EXIT_FAILURE;
		}
	}
	
	/*
		чтение файла настроек
	*/
	if ( (kucode=cfg_open(o_cfgfile))!=KE_NONE )
	{
		if ( kucode==KE_IO )
			plog(gettext("Failed to open a config file '%s`: %s\n"),o_cfgfile,strerror(errno));
		else
			plog(gettext("Failed to open a config file '%s` (kucode=%d)\n"),o_cfgfile,kucode);
		return EXIT_FAILURE;
	}	else
	{
		if ((cfg_query("logfile","s",o_logfile)!=KE_NONE) || \
			(cfg_query("port","i",&o_port)!=KE_NONE) || \
			(cfg_query("portlog","i",&o_portlog)!=KE_NONE) || \
			(cfg_query("mysql_host","s",o_sqlhost)!=KE_NONE) || \
			(cfg_query("mysql_name","s",o_sqlname)!=KE_NONE) || \
			(cfg_query("mysql_pswd","s",o_sqlpswd)!=KE_NONE) || \
			(cfg_query("save_msg","b",&o_defmsgtype)!=KE_NONE) || \
			(cfg_query("lev_stop","i",&o_stoplev)!=KE_NONE) || \
			(cfg_query("lev_break","i",&o_breaklev)!=KE_NONE))
		{
			plog(gettext("Failed to add values to a the config query\n"));
			cfg_close();
			return EXIT_FAILURE;
		}
		
		if ( (kucode=cfg_process())!=KE_NONE )
		{
			plog(gettext("Failed to parse config file '%s` on line %d (kucode=%d)\n"),o_cfgfile,cfg_line,kucode);
			cfg_close();
			return EXIT_FAILURE;
		}
		
		if ( (kucode=cfg_close())!=KE_NONE )
		{
			plog(gettext("Failed to close a config file '%s` (kucode=%d)\n"),o_cfgfile);
			return EXIT_FAILURE;
		}
	}
	
	//	открытие файла лога
	if ( (ecode=openlog(o_logfile))!=E_NONE )
	{
		plog("%s\n",errtext(ecode));
	}	else
	//	открытие слушаещего сокета
	if ( (ecode=net_open((u_int16_t)o_port,o_portlog))!=E_NONE )
	{
		plog("%s\n",errtext(ecode));
		closelog();
	}	else
	//	система работы с пользователем
	if ( (ecode=usr_init())!=E_NONE )
	{
		plog("%s\n",errtext(ecode));
		net_close();
		closelog();
	}	else
	{
		
		/*	!!!!!
			основной цикл
			!!!!!	*/
		while ( server_is_on )
		{
			int newconnection;
			net_id *id;
			
			//	получение списка новых сообщений
			if ( (ecode=net_wait(&newconnection,&sockl,&sockw))!=E_NONE )
			{
				plog(gettext("Waiting for a network event failed, skipping: %s\n"),errtext(ecode));
				continue;
			}
			
			//	если доступно новое соединение, то принимаем его
			if ( newconnection )
			{
				if ( (id=net_accept())==NULL )
				{
					plog(gettext("Failed to accept a new connection: %s\n"),errtext(ecode));
				}	else
				{
					if ( (ecode=usr_add(id))!=E_NONE )
					{
						plog(gettext("Failed to manage the accepted connetcion: %s\n"),errtext(ecode));
						if ( (ecode=net_discon(id))!=E_NONE )
						{
							plog(gettext("Failed to disconnect a socket (fd=%d): %s\n"),id->sock,errtext(ecode));
						}
					}
				}
			}
			
			//	дозаписываем в сокеты
			while ( (id=stack_pop_net(sockw))!=NULL )
			{
				if ( (ecode=usr_writel((usr_record*)id->data))!=E_NONE )
				{
					usr_rem((usr_record*)id->data);
					if ( (ecode=net_discon(id))!=E_NONE )
					{
						plog(gettext("Failed to disconnect a socket (fd=%d): %s\n"),id->sock,errtext(ecode));
					}
				}
			}
			
			//	обрабатываем все запросы из полученного списка
			while ( (id=stack_pop_net(sockl))!=NULL )
			{
				if ( (ecode=usr_manage((usr_record*)id->data))!=E_NONE )
				{
					plog(gettext("Failed to manage the incoming message (fd=%d): %s\n"),id->sock,errtext(ecode));
					usr_rem((usr_record*)id->data);
					if ( (ecode=net_discon(id))!=E_NONE )
					{
						plog(gettext("Failed to disconnect a socket (fd=%d): %s\n"),id->sock,errtext(ecode));
					}
				}
			}
		}
		
		//	закрытие подсистем
		main_close();
	}
	
	pstop();
	return EXIT_SUCCESS;
}
