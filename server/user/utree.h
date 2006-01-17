/***************************************************************************
 *            utree.h
 *
 *  Tue Dec 27 15:36:41 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __UTREE_H__
#define __UTREE_H__

#define ABTREE_TYPE usr_record*
#define ABTREE_EQ(a, b) (a->id==b->id)
#define ABTREE_MORE(a, b) (a->id>b->id)
#define ABTREE_ERRVAL NULL
#define ABTREE_ID usr
#include "abtree/abtree_core.h"

#endif
