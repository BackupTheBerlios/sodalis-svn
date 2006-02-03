/***************************************************************************
 *            photos.c
 *
 *  Wed Jan 18 12:41:40 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <magick/api.h>

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
			abil_num(tid,1,p,"PHOTO");
			
			//	проверка прав на фото
			if ( db_query(vstr("SELECT album FROM photos WHERE id='%d' AND owner='%d'",tid,usr->id))!=1 )
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
			
			//	удаление записи о фото
			if ( (db_nr_query(vstr("DELETE FROM photos WHERE id='%d'",tid))!=E_NONE) || \
					(db_nr_query(vstr("UPDATE photo_albums SET photos=photos-1 WHERE id='%s'",dbres[0]))!=E_NONE) )
			{
				REQ_FAIL("PHOTO");
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			
			//	удаление файлов
			if ( unlink(vstr("%s%d/%d.jpg",o_photo_dir,usr->id,tid))==-1 )
			{
				plog(gettext("Failed to delete a file: %s\n"),strerror(errno));
			}
			if ( unlink(vstr("%s%d/%d_sm.jpg",o_photo_dir,usr->id,tid))==-1 )
			{
				plog(gettext("Failed to delete a file: %s\n"),strerror(errno));
			}
			
			break;
			
		case 3:	// запрос
			break;
		case 5: // создание
			abil_num(album,3,p,"PHOTO");
			abil_num(size,4,p,"PHOTO");
			
			//	проверка размера фото
			if ( size>o_photo_size*1024 )
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

int abil_photo_data( usr_record *usr )
{
	char *name, *about, *photodir;
	void *data;
	int album;
	int fd, i;
	
	ExceptionInfo exception;
    Image *image, *image_thumb;
	ImageInfo *image_info;
	int imw, imh, imtw, imth;
	double ims;
	int photo_id;
	
	pstart();
	
	usr->dataflags&=~UF_DATAPHOTOIN;
	
	//	получение данных
	album=*(int*)(usr->data);
	name=usr->data+sizeof(int);
	for ( about=name; *about; about++ );
	about++;
	for ( data=about; *(char*)data; data++ );
	data++;
	
	pdebug("name: %s;\n",name);
	pdebug("about: %s;\n",about);
	
	//	создание папки
	photodir=vstr("%s%d",o_photo_dir,usr->id);
	if ( qdir2(photodir) )
	{
		plog(gettext("Photo directory does not exist, creating (uid=%d)\n"),usr->id);
		if ( mkdir(photodir,-1)==-1 )
		{
			plog(gettext("Failed to create a directory: %s\n"),strerror(errno));
			dfree(usr->data);
			SENDU(usr,"FAILED PHOTO");
			return 0;
		}
	}
	
	//	запись файла
	fd=open(vstr("%stemp.photo",photodir),O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
	if ( fd==-1 )
	{
		plog(gettext("Failed to open a file '%stemp.photo` for writing: %s\n"), \
				photodir,strerror(errno));
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	
	i=0;
	while ( i<usr->data_sz-(data-usr->data) )
	{
		int t;
		t=write(fd,data+i,usr->data_sz-(data-usr->data)-i);
		if ( t<=0 )
		{
			plog(gettext("Cannot write to file '%stemp.photo`: %s\n"),photodir,strerror(errno));
			dfree(usr->data);
			SENDU(usr,"FAILED PHOTO");
			return 0;
		}
		i+=t;
	}
	
	close(fd);
	
	/*
		Обработка изображения
	*/
	//	загрузка
	InitializeMagick(exec_path);
	GetExceptionInfo(&exception);
	pdebug("name: %s;\n",name);
	pdebug("name: %p;\n",name);
	image_info=CloneImageInfo(NULL);
	pdebug("name: %s;\n",name);
	pdebug("name: %p;\n",name);
	if ( image_info==NULL )
	{
		plog(gettext("Failed to create an image info structure\n"));
		DestroyExceptionInfo(&exception);
		DestroyMagick();
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	strcpy(image_info->filename,vstr("%stemp.photo",photodir));
	image=ReadImage(image_info,&exception);
	if (exception.severity != UndefinedException)
	{
		plog(gettext("Got an ImageMagick exception no. %d (%s)\n"), \
				exception.error_number,GetLocaleExceptionMessage(exception.severity,exception.reason));
		DestroyExceptionInfo(&exception);
		DestroyMagick();
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	if ( image==NULL )
	{
		plog(gettext("Image was not loaded\n"));
		DestroyExceptionInfo(&exception);
		DestroyMagick();
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	
	pdebug("name: %s;\n",name);
	pdebug("about: %s;\n",about);
	
	//	преобразование размеров
	imw=image->columns;
	imh=image->rows;
	ims=sqrt((double)o_photo_area/(double)(imw*imh));
	imtw=(int)(imw*ims);
	imth=(int)(imh*ims);
	pdebug("scaled to %dx%d\n",imw,imh);
	image_thumb=ResizeImage(image,imtw,imth,SincFilter,1,&exception);
	pdebug("filename: %s\n",image_thumb->filename);
	if (exception.severity != UndefinedException)
	{
		plog(gettext("Got an ImageMagick exception no. %d (%s)\n"), \
				exception.error_number,GetLocaleExceptionMessage(exception.severity,exception.reason));
		DestroyImage(image);
		DestroyExceptionInfo(&exception);
		DestroyMagick();
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	if ( image_thumb==NULL )
	{
		plog(gettext("Image was not resized\n"));
		DestroyImage(image);
		DestroyExceptionInfo(&exception);
		DestroyMagick();
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	
	pdebug("name: %s;\n",name);
	pdebug("about: %s;\n",about);
	
	//	запись
	strcpy(image_thumb->filename,vstr("%stemp.photo_thumb",photodir));
	if ( WriteImage(image_info,image_thumb)==MagickFalse )
	{
		plog(gettext("Failed to write an image\n"));
		DestroyImage(image);
		DestroyImage(image_thumb);
		DestroyExceptionInfo(&exception);
		DestroyMagick();
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	
	image_info=DestroyImageInfo(image_info);
	DestroyImage(image);
	DestroyImage(image_thumb);
	DestroyExceptionInfo(&exception);
	DestroyMagick();
	
	pdebug("name: %s;\n",name);
	pdebug("about: %s;\n",about);
	
	/*
		Добавление в базу данных
	*/
	if ( db_nr_query(vstr("INSERT photos VALUES (DEFAULT, %d, %d, '%s', '%s', %d, %d, DEFAULT)", \
			usr->id,album,name,about,imw,imh))!=E_NONE )
	{
		REQ_FAIL("PHOTO");
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	photo_id=db_id();
	
	/*
		Переименование файлов
	*/
	if ( (rename(vstr("%stemp.photo",photodir),vstr("%s%d.jpg",photodir,photo_id))==-1) || \
		(rename(vstr("%stemp.photo_thumb",photodir),vstr("%s%d_sm.jpg",photodir,photo_id))==-1) )
	{
		plog(gettext("Failed to rename photo files: %s\n"),strerror(errno));
		db_nr_query(vstr("DELETE FROM photos WHERE id='%d'",photo_id));
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	
	if ( db_nr_query(vstr("UPDATE photo_albums SET photos=photos+1 WHERE id='%d'",album))!=E_NONE )
	{
		REQ_FAIL("PHOTO");
		dfree(usr->data);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	
	dfree(usr->data);
	SENDU(usr,"PHOK");
	
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
