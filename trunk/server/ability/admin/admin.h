/***************************************************************************
 *            admin.h
 *
 *  Tue Jan 10 12:00:28 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef ABIL__ADMIN_H__
#define ABIL__ADMIN_H__

#include "user/user.h"

//	Протокол 3.1
int abil_stop( usr_record *usr );

//	Протокол 3.2
int abil_break( usr_record *usr );

#endif
