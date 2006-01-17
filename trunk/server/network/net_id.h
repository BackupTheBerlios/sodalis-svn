/***************************************************************************
 *            net_id.h
 *
 *  Sun Nov 27 11:26:27 2005
 *  Copyright  2005  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __NET_ID__
#define __NET_ID__

/*
	Запись соединения
*/
typedef
struct
{
	int sock;
	char ip[16];
	void *data;
}	net_id;

#endif
