/***************************************************************************
 *            nlist.h
 *
 *  Thu Nov 24 20:15:10 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __NLIST_H__
#define __NLIST_H__

#define LIST_TYPE net_id*
#define LIST_EQ(a, b) ((a->sock)==(b->sock))
#define LIST_ERRVAL NULL
#define LIST_ID net
#include "list/list_core.h"

#endif
