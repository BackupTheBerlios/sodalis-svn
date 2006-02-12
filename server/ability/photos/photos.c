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
	
	int f,i,t;
	struct stat st;
	
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
			abil_num(tid,1,p,"PHOTO");
			
			if ( db_query(vstr("SELECT owner, caption, about, album FROM photos WHERE id='%d'",tid))!=1 )
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
			
			if ( !strcmp(p_arg[2],"S") )
			{
				p=vstr("%s%s/%d_sm.jpg",o_photo_dir,dbres[0],tid);
			}	else
			if ( !strcmp(p_arg[2],"L") )
			{
				p=vstr("%s%s/%d.jpg",o_photo_dir,dbres[0],tid);
			}	else
			{
				plog(gettext("Invalid photo type (uid=%d)\n"),usr->id);
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			
			//	отправление заголовка
			if ( stat(p,&st)==-1 )
			{
				plog(gettext("Failed to get stats of the file: %s\n"),strerror(errno));
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			SENDU(usr,vstr("PHOTO \"%s\" \"%s\" %s %d",dbres[1],dbres[2],dbres[3],st.st_size));
			
			//	чтение файла
			f=open(p,O_RDONLY);
			if ( f==-1 )
			{
				plog(gettext("Failed to open a file: %s\n"),strerror(errno));
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			usr->outdata=dmalloc(st.st_size);
			if ( usr->outdata==NULL )
			{
				plog(errtext(E_MEMORY));
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			i=0;
			while ( i<st.st_size )
			{
				t=read(f,usr->outdata,st.st_size-i);
				if ( i==-1 )
				{
					plog(gettext("Failed to read from file: %s\n"),strerror(errno));
					dfree(usr->outdata);
					SENDU(usr,"FAILED PHOTO");
					return 0;
				}
				i+=t;
			}
			usr->dataflags|=UF_DATA_OUT;
			usr->outdata_cur=0;
			usr->outdata_sz=st.st_size;
			pdebug("%d\n",usr->outdata_sz);
			if ( usr_write(usr,NULL)!=E_NONE )
			{
				dfree(usr->outdata);
				usr->dataflags&=~UF_DATA_OUT;
				return 1;
			}
			
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
			usr->indata_sz=size+strlen(p_arg[1])+strlen(p_arg[2])+2+sizeof(int);
			usr->indata_cur=usr->indata_sz-size;
			usr->indata=dmalloc(usr->indata_sz);
			if ( usr->indata==NULL )
			{
				SENDU(usr,"FAILED PHOTO");
				return 0;
			}
			*(int*)usr->indata=album;
			strcpy(usr->indata+sizeof(int),p_arg[1]);
			strcpy(usr->indata+sizeof(int)+strlen(p_arg[1])+1,p_arg[2]);
			if ( usr_write(usr,"PHALLOW")!=E_NONE )
			{
				dfree(usr->indata);
				return 1;
			}
			usr->dataflags|=UF_DATAPHOTOIN|UF_DATA_IN;
			
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
	album=*(int*)(usr->indata);
	name=usr->indata+sizeof(int);
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
			dfree(usr->indata);
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
		dfree(usr->indata);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	
	i=0;
	while ( i<usr->indata_sz-(data-usr->indata) )
	{
		int t;
		t=write(fd,data+i,usr->indata_sz-(data-usr->indata)-i);
		if ( t<=0 )
		{
			plog(gettext("Cannot write to file '%stemp.photo`: %s\n"),photodir,strerror(errno));
			dfree(usr->indata);
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
		dfree(usr->indata);
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
		dfree(usr->indata);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	if ( image==NULL )
	{
		plog(gettext("Image was not loaded\n"));
		DestroyExceptionInfo(&exception);
		DestroyMagick();
		dfree(usr->indata);
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
		dfree(usr->indata);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	if ( image_thumb==NULL )
	{
		plog(gettext("Image was not resized\n"));
		DestroyImage(image);
		DestroyExceptionInfo(&exception);
		DestroyMagick();
		dfree(usr->indata);
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
		dfree(usr->indata);
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
		dfree(usr->indata);
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
		dfree(usr->indata);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	
	if ( db_nr_query(vstr("UPDATE photo_albums SET photos=photos+1 WHERE id='%d'",album))!=E_NONE )
	{
		REQ_FAIL("PHOTO");
		dfree(usr->indata);
		SENDU(usr,"FAILED PHOTO");
		return 0;
	}
	
	dfree(usr->indata);
	SENDU(usr,"PHOK");
	
	pstop();
	return 0;
}

int abil_chalb( usr_record *usr )
{
	int photo, album;
	int photos, maxph, alb_old;
	char *p, **dbres;
	pstart();
	
	CHECK_ARGC(3,"CHALB");
	abil_num(photo,1,p,"CHALB");
	abil_num(album,2,p,"CHALB");
	
	//	проверка альбома
	if ( db_query(vstr("SELECT photos, maxphotos FROM photo_albums " \
			"WHERE id='%d' AND owner='%d'",album,usr->id))!=1 )
	{
		REQ_FAIL("CHALB");
		SENDU(usr,"FAILED CHALB");
		return 0;
	}
	if ( (dbres=db_row())==NULL )
	{
		REQ_FAIL("CHALB");
		SENDU(usr,"FAILED CHALB");
		return 0;
	}
	abil_str2num(photos,dbres[0],p,"CHALB");
	abil_str2num(maxph,dbres[1],p,"CHALB");
	
	//	проверка фотографии
	if ( db_query(vstr("SELECT album FROM photos WHERE id='%d' AND owner='%d'",photo,usr->id))!=1 )
	{
		REQ_FAIL("CHALB");
		SENDU(usr,"FAILED CHALB");
		return 0;
	}
	if ( (dbres=db_row())==NULL )
	{
		REQ_FAIL("CHALB");
		SENDU(usr,"FAILED CHALB");
		return 0;
	}
	abil_str2num(alb_old,dbres[0],p,"CHALB");
	
	//	можно ли переместить?
	if ( album==alb_old )
	{
		plog(gettext("Destanation album matches with the source (uid=%d on %s)\n"),usr->id,"CHALB");
		SENDU(usr,"FAILED CHALB");
		return 0;
	}
	if ( photos>=maxph )
	{
		plog(gettext("User cannot create more photos in album %d (uid=%d on %s)\n"),album,usr->id,"CHALB");
		SENDU(usr,"FAILED CHALB");
		return 0;
	}
	
	//	перемещение
	if ( db_nr_query(vstr("UPDATE photos AS ph, photo_albums AS al1, photo_albums AS al2 " \
			"SET ph.album='%d', al1.photos=al1.photos-1, al2.photos=al2.photos+1 " \
			"WHERE ph.id='%d' AND al1.id='%d' AND al2.id='%d'", \
			album,photo,alb_old,album))!=E_NONE )
	{
		REQ_FAIL("CHALB");
		SENDU(usr,"FAILED CHALB");
		return 0;
	}
	
	pstop();
	return 0;
}

int abil_chmph( usr_record *usr )
{
	int photo;
	char *p;
	pstart();
	
	CHECK_ARGC(2,"CHMPH");
	abil_num(photo,1,p,"CHMPH");
	
	//	проверка фотографии
	if ( db_query(vstr("SELECT id FROM photos WHERE id='%d' AND owner='%d'",photo,usr->id))!=1 )
	{
		REQ_FAIL("CHMPH");
		SENDU(usr,"FAILED CHMPH");
		return 0;
	}
	
	//	установка нового фото
	if ( db_nr_query(vstr("UPDATE logins SET main_photo='%d' WHERE id='%d'",photo,usr->id))!=E_NONE )
	{
		REQ_FAIL("CHMPH");
		SENDU(usr,"FAILED CHMPH");
		return 0;
	}
	
	pstop();
	return 0;
}

int abil_album( usr_record *usr )
{
	int albid;
	char *p, **dbres;
	int i,j;
	pstart();
	
	switch ( p_argc )
	{
		case 2: // удаление альбома
			abil_num(albid,1,p,"ALBUM");
			if ( db_query(vstr("SELECT owner FROM photo_albums WHERE id='%d' AND owner='%d'",albid,usr->id))!=1 )
			{
				REQ_FAIL("ALBUM");
				SENDU(usr,"FAILED ALBUM");
				return 0;
			}
			
			//	удаление фото из этого альбома
			if ( (i=db_query(vstr("SELECT id FROM photos WHERE album='%d'",albid)))<0 )
			{
				REQ_FAIL("ALBUM");
				SENDU(usr,"FAILED ALBUM");
				return 0;
			}
			for ( j=0; j<i; j++ )
			{
				if ( (dbres=db_row())==NULL )
				{
					REQ_FAIL("ALBUM");
					SENDU(usr,"FAILED ALBUM");
					return 0;
				}
				if ( unlink(vstr("%s%d/%s.jpg",o_photo_dir,usr->id,dbres[0]))==-1 )
				{
					plog(gettext("Failed to delete a file: %s\n"),strerror(errno));
				}
				if ( unlink(vstr("%s%d/%s_sm.jpg",o_photo_dir,usr->id,dbres[0]))==-1 )
				{
					plog(gettext("Failed to delete a file: %s\n"),strerror(errno));
				}
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
			plog(gettext("Invalid count of parameters (uid=%d on %s)\n"),usr->id,"ALBUM");
			usr_write(usr,"DISCON CMD");
			return 1;
	}
	
	pstop();
	return 0;
}

int abil_lsph( usr_record *usr )
{
	struct
	{
		int album;
		int uid;
	}	lsph={-1, -1};
	int i, tmp;
	char *p, *query, **dbres;
	pstart();
	
	//	сбор параметров
	for ( i=1; i<p_argc; i++ )
	{
		if ( !strcmp(p_arg[i],"MAIN") && (lsph.album==-1) )
		{
			lsph.album=-2;
		}	else
		if ( !strcmp(p_arg[i],"OF") && (lsph.uid==-1) && (i+1<p_argc) )
		{
			i++;
			abil_num(tmp,i,p,"LSPH");
			if ( tmp<0 )
			{
				plog(gettext("Invalid ID (uid=%d on %s)\n"),usr->id,"LSPH");
				usr_write(usr,"DISCON CMD");
				return 1;
			}
			lsph.uid=tmp;
		}	else
		if ( !strcmp(p_arg[i],"FROM") && (lsph.album==-1) && (i+1<p_argc) )
		{
			i++;
			abil_num(tmp,i,p,"LSPH");
			if ( tmp<0 )
			{
				plog(gettext("Invalid ID (uid=%d on %s)\n"),usr->id,"LSPH");
				usr_write(usr,"DISCON CMD");
				return 1;
			}
			lsph.album=tmp;
		}	else
		{
			plog(gettext("Invalid incoming parametre (uid=%d on %s)\n"),usr->id,"LSPH");
			usr_write(usr,"DISCON CMD");
			return 1;
		}
	}
	
	//	составление запроса
	if ( lsph.album==-2 )
	{
		//	main photo
		query=vstr("SELECT ph.id, ph.owner, ph.caption, ph.about, ph.album FROM photos ph, logins log " \
				"WHERE log.id='%d' AND ph.id=log.main_photo",lsph.uid==-1?usr->id:lsph.uid);
	}	else
	{
		query=vstr("SELECT id, owner, caption, about, album FROM photos " \
				"WHERE owner='%d'",lsph.uid==-1?usr->id:lsph.uid);;
		if ( lsph.album!=-1 )
		{
			query=vstr("%s AND album='%d",query,lsph.album);
		}
	}
	
	//	выполнение запроса
	if ( (tmp=db_query(query))<0 )
	{
		REQ_FAIL("LSPH");
		return 0;
	}
	for ( i=0; i<tmp; i++ )
	{
		if ( (dbres=db_row())==NULL )
		{
			REQ_FAIL("LSPH");
			return 0;
		}
		SENDU(usr,vstr("PHOTO %s %s \"%s\" \"%s\" %s",dbres[0],dbres[1],dbres[2],dbres[3],dbres[4]));
	}
	
	pstop();
	return 0;
}

int abil_lspa( usr_record *usr )
{
	int of=-1;
	char *p, **dbres;
	int tmp, i;
	pstart();
	
	//	проверка параметров
	if ( p_argc==3 )
	{
		if ( !strcmp(p_arg[1],"OF") )
		{
			abil_num(of,2,p,"LSPA");
		}
	}
	if ( (p_argc!=1) && (of==-1) )
	{
		plog(gettext("Invalid incoming parameter (uid=%d on %s)\n"),usr->id,"LSPA");
		usr_write(usr,"DISCON CMD");
		return 1;
	}
	
	//	выполнение запроса
	if ( of==-1 )
	{
		if ( (tmp=db_query(vstr("SELECT id, caption, about, photos, maxphotos FROM photo_albums " \
				"WHERE owner='%d'",usr->id)))<0 )
		{
			REQ_FAIL("LSPA");
			return 0;
		}
		for ( i=0; i<tmp; i++ )
		{
			if ( (dbres=db_row())==NULL )
			{
				REQ_FAIL("LSPA");
				return 0;
			}
			SENDU(usr,vstr("ALBUM %s \"%s\" \"%s\" %s %s",dbres[0],dbres[1],dbres[2],dbres[3],dbres[4]));
		}
	}	else
	{
		if ( (tmp=db_query(vstr("SELECT id, caption, about FROM photo_albums " \
				"WHERE owner='%d'",of)))<0 )
		{
			REQ_FAIL("LSPA");
			return 0;
		}
		for ( i=0; i<tmp; i++ )
		{
			if ( (dbres=db_row())==NULL )
			{
				REQ_FAIL("LSPA");
				return 0;
			}
			SENDU(usr,vstr("ALBUM %s \"%s\" \"%s\"",dbres[0],dbres[1],dbres[2]));
		}
	}
	
	pstop();
	return 0;
}
