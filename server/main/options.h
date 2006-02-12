/***************************************************************************
 *            options.h
 *
 *  Sat Dec 17 11:27:23 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __OPTIONS_H__
#define __OPTIONS_H__
#include "cfgreader/cfg.h"

//	путь запуска
extern char *exec_path;

//	сеть
extern int o_port;
extern int o_portlog;

//	лог
extern char o_logfile[CFG_BUFFER];

//	SQL база данных
extern char o_sqlhost[CFG_BUFFER];
extern char o_sqlname[CFG_BUFFER];
extern char o_sqlpswd[CFG_BUFFER];
extern char o_sqldbase[CFG_BUFFER];

//	уровни доступа
extern int o_stoplev;
extern int o_breaklev;

//	тип сообщений (см. протокол, 1=s, 0=i)
extern int o_defmsgtype;

//	фотографии
extern char o_photo_dir[CFG_BUFFER];
extern int o_photo_size;
extern int o_photo_area;

#endif
