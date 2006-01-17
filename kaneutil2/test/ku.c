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

#include <stdio.h>
#include "cfgreader/cfg.h"
#include "errors/debug.h"

#define p(a) printf("%d\n",a);

int z=26;

int main()
{
	int a,b,c=23,d;
	char q[256];
	
	printf("%d\n",cfg_open("config"));
	
	//p(cfg_query("odin","ii",&a,&b));
	p(cfg_query("dva","i",&z));
	//p(cfg_query("f","bi",&d, &a));
	//p(cfg_query("q","si",q,&a));
	
	p(cfg_process());
	
	printf("lines: %d; values: '%s', %d\n",cfg_line,"noval",z);
	
	printf("%d\n",cfg_close());
	
	return 0;
}
