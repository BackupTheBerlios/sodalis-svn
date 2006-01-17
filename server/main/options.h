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

extern int o_port;
extern int o_portlog;
extern char o_logfile[CFG_BUFFER];

extern char o_sqlhost[CFG_BUFFER];
extern char o_sqlname[CFG_BUFFER];
extern char o_sqlpswd[CFG_BUFFER];

extern int o_stoplev;
extern int o_breaklev;

extern int o_defmsgtype;

#endif
