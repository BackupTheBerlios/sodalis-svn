/***************************************************************************
 *            photos.c
 *
 *  Wed Jan 18 12:41:40 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include "ability.h"
#include "common.h"
#include "main/options.h"

int abil_photo( usr_record *usr )
{
	int size, album, tid, photos, maxph;
	char *p, **dbres;
	pstart();
	
	switch ( p_argc )
	{
		case 2:	// удаление фото
			break;
		case 3:	// запрос
			break;
		case 5: // создание
			abil_num(album,3,p,"PHOTO");
			abil_num(size,4,p,"PHOTO");
			
			//	проверка размера фото
			if ( size>o_photo_size )
			{
				plog(gettext("Photo size is to big (uid=%d on %s)\n"),usr->id,"PHOTO");
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			
			//	проверка фотоальбома
			if ( db_query(vstr("SELECT owner, photos, maxphotos FROM photo_albums WHERE id='%d'",album))!=1 )
			{
				REQ_FAIL("PHOTO");
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			if ( (dbres=db_row())==NULL )
			{
				REQ_FAIL("PHOTO");
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			abil_str2num(tid,dbres[0],p,"PHOTO");
			abil_str2num(photos,dbres[1],p,"PHOTO");
			abil_str2num(maxph,dbres[2],p,"PHOTO");
			if ( tid!=usr->id )
			{
				plog(gettext("Cannot add photo to non-user album (uid=%d on %s)\n"),usr->id,"PHOTO");
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			if ( photos>=maxph )
			{
				plog(gettext("User cannot create more photos in album %d (uid=%d on %s)\n"),album,usr->id,"PHOTO");
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			
			//	заполнение данных
			usr->data_sz=size+strlen(p_arg[1])+strlen(p_arg[2])+2+sizeof(int);
			usr->data_cur=usr->data_sz-size;
			usr->data=dmalloc(usr->data_sz);
			if ( usr->data==NULL )
			{
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			*(int*)usr->data=album;
			strcpy(usr->data+sizeof(int),p_arg[1]);
			strcpy(usr->data+sizeof(int)+strlen(p_arg[1])+1,p_arg[2]);
			if ( usr_write(usr,"PHALLOW")!=E_NONE )
			{
				dfree(usr->data);
				return 1;
			}
			usr->dataflags|=UF_DATAPHOTOIN;
			
			break;
			
		default:
			plog(gettext("Invalid count of parametres (uid=%d on %s)\n"),usr->id,"PHOTO");
			usr_write(usr,"DISCON CMD");
			return 1;
	}
	
	pstop();
	return 0;
}

int abil_photo_data( usr_record *usr, int status )
{
	pstart();
	
	usr->dataflags&=!UF_DATAPHOTOIN;
	if ( status==ABIL_ST_FAILURE )
	{
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
	}	else
	{
		char *name, *about;
		void *data;
		int album;
		
		album=*(int*)usr->data;
		name=usr->data+sizeof(int);
		for ( about=name; *about; about++ );
		about++;
		for ( data=about; *(char*)data; data++ );
		data++;
		pdebug("===== here added photo '%s'; '%s' to album %d\n",name,about,album);
		dfree(usr->data);
		SENDU(usr,"PHOK");
	}
	
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
