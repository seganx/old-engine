/********************************************************************
	created:	2013/01/21
	filename: 	Assert.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple assertion function
*********************************************************************/
#ifndef GUARD_Assert_HEADER_FILE
#define GUARD_Assert_HEADER_FILE

// assertion
#if ( defined(_DEBUG) || SEGAN_LIB_ASSERT )

#define sx_assert(expression)	((!!(expression)) || lib_assert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), __LINE__))

//! assertion function will stop application and report call stack
SEGAN_LIB_API sint lib_assert( const wchar* expression, const wchar* file, const sint line );

#else

#define sx_assert(expression)

#endif

#endif	//	GUARD_Assert_HEADER_FILE