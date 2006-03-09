/***************************************************************************
 *            tools.h
 *
 *  Sun Feb 12 17:48:54 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __TOOLS_H__
#define __TOOLS_H__
#include "ecode.h"

#define TOOL_ADMINDB 1

extern int toolkey;

ecode_t tool_admindb( void );
ecode_t tool_adduser( void );
ecode_t tool_lsuser( void );
ecode_t tool_rmuser( void );
ecode_t tool_passwuser( void );
ecode_t tool_blankdb( void );

#endif
