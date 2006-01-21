/***************************************************************************
 *            config.h
 *
 *  Sat Oct 22 11:57:13 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define VERSION 1
#define PROTOCOL sodalisnpv2

#define CFG_FILE "sodalis.cfg"

#define CFG_PORT 1986
#define CFG_PORTLOG 10
#define CFG_LOGFILE "sodalis.log"

#define CFG_SQLHOST "localhost"
#define CFG_SQLNAME "sodalis"
#define CFG_SQLPSWD ""

#define CFG_STOPLEV 1000
#define CFG_BREAKLEV 2

#define CFG_DEFMSGTYPE 1

#define CFG_PHOTODIR "/var/sodalis/ph"
#define CFG_PHOTOSIZE 512

extern int server_is_on;

#endif
