/***************************************************************************
 *            photos.c
 *
 *  Wed Jan 18 12:41:40 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include "photos.h"
#include "common.h"
#include "main/options.h"

int abil_photo( usr_record *usr )
{
	pstart();
	
	
	
	pstop();
	return 0;
}

int abil_photo_data( usr_record *usr, int status )
{
	pstart();
	pstop();
	return 0;
}

int abil_chalb( usr_record *usr )
{
	pstart();
	pstop();
	return 0;
}

int abil_chmph( usr_record *usr )
{
	pstart();
	pstop();
	return 0;
}

int abil_album( usr_record *usr )
{
	int albid, uid;
	char *p, **dbres;
	pstart();
	
	switch ( p_argc )
	{
		case 2: // удаление альбома
			abil_num(albid,1,p,"ALBUM");
			if ( db_query(vstr("SELECT owner FROM photo_albums WHERE id='%d'",albid))!=1 )
			{
				REQ_FAIL("ALBUM");
				SENDU(usr,"FAILED ALBUM");
				return 0;
			}
			if ( (dbres=db_row())==NULL )
			{
				REQ_FAIL("ALBUM");
				SENDU(usr,"FAILED ALBUM");
				return 0;
			}
			abil_str2num(uid,dbres[0],p,"ALBUM");
			if ( uid!=usr->id )
			{
				plog(gettext("User cannot delete non-his album %d (uid=%d od %s)\n"),albid,usr->id,"ALBUM");
				SENDU(usr,"FAILED ALBUM");
				return 0;
			}
			if ( db_nr_query(vstr("DELETE FROM photo_albums WHERE id='%d'",albid))!=E_NONE )
			{
				REQ_FAIL("ALBUM");
				SENDU(usr,"FAILED ALBUM");
				return 0;
			}
			
			break;
			
		case 3:	// создание альбома
			//	сколько уже есть альбомов?
			if ( (albid=db_query(vstr("SELECT id FROM photo_albums WHERE owner='%d'",usr->id)))<0 )
			{
				REQ_FAIL("ALBUM");
				SENDU(usr,"FAILED ALBUM");
				return 0;
			}
			if ( albid>=usr->max_ph )
			{
				plog(gettext("User cannot create more photo albums (uid=%d on %s)\n"),usr->id,"ALBUM");
				SENDU(usr,"FAILED ALBUM");
				return 0;
			}
			
			//	создание
			if ( db_nr_query(vstr("INSERT INTO photo_albums VALUES (DEFAULT, %d, DEFAULT, DEFAULT, '%s', '%s')",usr->id,p_arg[1],p_arg[2]))!=E_NONE )
			{
				REQ_FAIL("ALBUM");
				SENDU(usr,"FAILED ALBUM");
				return 0;
			}
			
			break;
			
		default:
			plog(gettext("Invalid count of parametres (uid=%d on %s)\n"),usr->id,"ALBUM");
			usr_write(usr,"DISCON CMD");
			return 1;
	}
	
	pstop();
	return 0;
}
