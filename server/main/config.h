/***************************************************************************
 *            config.h
 *
 *  Sat Oct 22 11:57:13 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "other/other.h"

#define VERSION 1
#define PROTOCOL sodalisnpv2

#ifndef PREFIX
#define __PREFIX "."
#else
#define __PREFIX STR(PREFIX)
#endif

#define CFG_FILE __PREFIX"/etc/sodalis.cfg"

#define CFG_PORT 1986
#define CFG_PORTLOG 10
#define CFG_LOGFILE __PREFIX"/var/sodalis/sodalis.log"

#define CFG_SQLHOST "localhost"
#define CFG_SQLNAME "sodalis"
#define CFG_SQLPSWD ""
#define CFG_SQLDBASE "sodalis"

#define CFG_STOPLEV 1000
#define CFG_BREAKLEV 2

#define CFG_DEFMSGTYPE 1

#define CFG_PHOTODIR __PREFIX"/var/sodalis/ph"
#define CFG_PHOTOSIZE 512
#define CFG_PHOTOAREA 120000

extern int server_is_on;

#endif
