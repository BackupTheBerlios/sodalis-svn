/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef KU__ECODE_H__
#define KU__ECODE_H__
#include "errors/open_code.h"

typedef
enum
{
	KE_NONE,
	KE_ANY,
	KE_MEMORY,
	KE_EMPTY,
	KE_NOTFOUND,
	KE_DOUBLE,
	KE_IO,
	KE_SYNTAX
}	kucode_t;

extern kucode_t kucode;

#include "errors/close_code.h"
#endif
