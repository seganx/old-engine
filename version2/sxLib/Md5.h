/********************************************************************
	created:	2016/4/19
	filename: 	Md5.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contains basic interface functions of MD5
*********************************************************************/
#ifndef DEFINED_Md5
#define DEFINED_Md5

#include "Def.h"


/*! MD5 hash given strings. Buffer 'buffer' must be 33 bytes long
args is a NULL terminated list of ASCIIz strings like sx_md5(buffer, "a", "b", null) */
SEGAN_LIB_API char* sx_md5( char buf[33], ... );

#endif // DEFINED_Md5

