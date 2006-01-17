/***************************************************************************
 *            friends.h
 *
 *  Thu Jan 12 18:28:55 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef ABIL__FRIENDS_H__
#define ABIL__FRIENDS_H__

#include "user/user.h"

//	Протокол 5.3.1 (+a,b)
int abil_invite( usr_record *usr );
int abil_invacc( usr_record *usr );
int abil_invrej( usr_record *usr );

//	Протокол 5.3.2
int abil_leave( usr_record *usr );

//	Протокол 5.3.3
int abil_chgrp( usr_record *usr );

//	Протокол 5.3.5
int abil_group( usr_record *usr );

//	Протокол 4.2
int abil_lsf( usr_record *usr );

//	Протокол 4.3
int abil_lsg( usr_record *usr );

//	Протокол 4.4
int abil_lsinv( usr_record *usr );

#endif
