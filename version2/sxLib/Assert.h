/********************************************************************
	created:	2013/01/18
	filename: 	Assert.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain macros and functions for assertion
*********************************************************************/
#ifndef GUARD_Assert_HEADER_FILE
#define GUARD_Assert_HEADER_FILE

#include "Def.h"

#if ( defined(_DEBUG) || SEGAN_ASSERT ) //! assertion function will stop application and report call stack
SEGAN_LIB_API sint lib_assert(const char* expression, const char* file, const sint line);
#endif

#endif	//	GUARD_Assert_HEADER_FILE
