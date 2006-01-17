/***************************************************************************
 *            admin.c
 *
 *  Tue Jan 10 12:04:47 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include "admin.h"
#include "../common.h"
#include "main/config.h"
#include "main/options.h"
#include "network/net.h"

int abil_stop( usr_record *usr )
{
	pstart();
	
	CHECK_ARGC(1,"STOP");
	CHECK_LEVEL(o_stoplev,"STOP");
	
	server_is_on=0;
	
	pstop();
	return 0;
}

int abil_break( usr_record *usr )
{
	char *p;
	int dstid;
	usr_record *dst;
	pstart();
	
	CHECK_ARGC(2,"BREAK");
	CHECK_LEVEL(o_breaklev,"BREAK");
	
	abil_num(dstid,1,p,"BREAK");
	dst=usr_online(dstid);
	if ( dst!=NULL )
	{
		net_id *netid=dst->netid;
		usr_write(dst,"DISCON ADM");
		usr_rem(dst);
		net_discon(netid);
	}
	
	pstop();
	return 0;
}
