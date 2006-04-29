/***************************************************************************
 *            ecode.h
 *
 *  Fri Apr  7 17:44:42 2006
 *  Copyright  2006  J. Anton
 *  kane@mail.berlios.de
 ****************************************************************************/

#ifndef SOD__ECODE_H__
#define SOD__ECODE_H__

/*
	Коды возврата функций
*/
#define SOD_AGAIN 1
#define SOD_OK 0
#define SOD_ERROR -1

/*
	Коды ошибок
*/
typedef
enum
{
	SE_NONE,
	SE_MEMORY,
	SE_KU2,
	SE_NOT_CONNECTED,
	SE_SOCKET,
	SE_GETHOST,
	SE_CONNECT,
	SE_SHUTDOWN,
	SE_CLOSE,
	SE_THREAD,
	SE_WRITE,
	SE_READ
}	secode_t;

#define sod_place_error_no_gettext( _ec, _et ) \
{ \
	session->ecode=_ec; \
	session->errortext=_et; \
	session->errorfunc=(char*)__FUNCTION__; \
}

#define sod_throw_error_no_gettext( _ec, _et ) \
{ \
	session->ecode=_ec; \
	session->errortext=_et; \
	session->errorfunc=(char*)__FUNCTION__; \
	return SOD_ERROR; \
}

#ifdef USE_GETTEXT
#define sod_place_error( _ec, _et ) \
{ \
	session->ecode=_ec; \
	session->errortext=gettext(_et); \
	session->errorfunc=(char*)__FUNCTION__; \
}

#define sod_throw_error( _ec, _et ) \
{ \
	session->ecode=_ec; \
	session->errortext=gettext(_et); \
	session->errorfunc=(char*)__FUNCTION__; \
	return SOD_ERROR; \
}

#define sod_throw_value( _ec, _et, _value ) \
{ \
	session->ecode=_ec; \
	session->errortext=gettext(_et); \
	session->errorfunc=(char*)__FUNCTION__; \
	return _value; \
}
#else	// USE_GETTEXT
#define sod_place_error( _ec, _et ) \
{ \
	session->ecode=_ec; \
	session->errortext=_et; \
	session->errorfunc=(char*)__FUNCTION__; \
}

#define sod_throw_error( _ec, _et ) \
{ \
	session->ecode=_ec; \
	session->errortext=_et; \
	session->errorfunc=(char*)__FUNCTION__; \
	return SOD_ERROR; \
}

#define sod_throw_value( _ec, _et, _value ) \
{ \
	session->ecode=_ec; \
	session->errortext=_et; \
	session->errorfunc=(char*)__FUNCTION__; \
	return _value; \
}
#endif	// USE_GETTEXT

#endif
