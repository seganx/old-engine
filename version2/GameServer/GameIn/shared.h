/********************************************************************
	created:	2016/4/10
	filename: 	shared.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		
*********************************************************************/
#ifndef DEFINED_shared
#define DEFINED_shared

#include "../../sxLib/Lib.h"

struct RequestObject
{
	struct mg_connection*	conn;
	void*					user;
};

#endif // DEFINED_shared


