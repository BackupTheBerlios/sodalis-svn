/***************************************************************************
 *            msgu.h
 *
 *  Fri Dec  2 18:00:51 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef ABIL__MESSAGES_H__
#define ABIL__MESSAGES_H__

#include "user/user.h"

//	Протокол 5.2.1
int abil_msgu( usr_record *usr );

//	Протокол 4.1
int abil_lsm( usr_record *usr );

#endif
