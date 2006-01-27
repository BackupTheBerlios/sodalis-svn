/***************************************************************************
 *            scc.h
 *
 *  Thu Jan 26 12:39:45 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __SCC_H__
#define __SCC_H__

#define VERSION 1
#define PROTOCOL sodalisnpv2
#define PORT 1986

#define DATA_NORMAL 0
#define DATA_RAW 1

extern char *login;
extern char *password;
extern int go_on;
extern int is_auth;
extern int data_mode;

#endif
