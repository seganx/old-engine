/********************************************************************
	created:	2013/01/18
	filename: 	Callstack.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some macro and functions for 
				call stack system
*********************************************************************/
#ifndef GUARD_Callstack_HEADER_FILE
#define GUARD_Callstack_HEADER_FILE


#include "Def.h"

#if SEGAN_CALLSTACK

SEGAN_LIB_API void callstack_report_to_file( const wchar* name, const wchar* title = L" " );

#endif

#endif	//	GUARD_Callstack_HEADER_FILE
