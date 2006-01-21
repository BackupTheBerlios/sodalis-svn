/***************************************************************************
 *            photos.h
 *
 *  Wed Jan 18 12:41:31 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef ABIL__PHOTOS_H__
#define ABIL__PHOTOS_H__

#include "user/user.h"

//	Протокол 5.4.1 (+a)
int abil_photo( usr_record *usr );
int abil_photo_data( usr_record *usr, int status );

//	Протокол 5.4.2
int abil_chalb( usr_record *usr );

//	Протокол 5.4.3
int abil_chmph( usr_record *usr );

//	Протокол 5.4.4
int abil_album( usr_record *usr );

#endif
