/***************************************************************************
 *            ability.h
 *
 *  Wed Jan 25 11:18:29 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef __ABILITY_H__
#define __ABILITY_H__

extern int max_data_size;

int str2num( char *str, int *data );
void inv_serv( void );
void inv_syn( void );

/*

*/
int abil_serv( void );

/*

*/
int abil_usr( void );

#endif
