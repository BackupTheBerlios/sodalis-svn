/***************************************************************************
 *            parser.h
 *
 *  Sat Apr  8 17:10:31 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef SOD__PARSER_H__
#define SOD__PARSER_H__
#include "errors/open_code.h"

int sod_get_message( sod_session *session, int *datacnt, char **data );
int sod_put_message( sod_session *session, char *data );

#include "errors/close_code.h"
#endif
