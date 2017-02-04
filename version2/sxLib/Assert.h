/********************************************************************
	created:	2013/01/18
	filename: 	Assert.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain macros and functions for assertion
*********************************************************************/
#ifndef HEADER_ASSERT
#define HEADER_ASSERT

#include "Def.h"

//////////////////////////////////////////////////////////////////////////
// assertion
#if ( defined(_DEBUG) || SEGAN_ASSERT )
#define sx_assert(expression)	((!!(expression)) || lib_assert(#expression, __FILE__, __LINE__))

//! assertion function will stop application and report call stack
SEGAN_LIB_API int lib_assert(const char* expression, const char* file, const int line);

#else

#define sx_assert(expression)

#endif


#endif	//	HEADER_ASSERT
