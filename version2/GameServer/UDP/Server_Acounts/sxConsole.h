/********************************************************************
	created:	2010/10/01
	filename: 	sxConsole.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the header file of editor of seganx
*********************************************************************/
#ifndef GUARD_sxConsole_HEADER_FILE
#define GUARD_sxConsole_HEADER_FILE

#include "../Net.h"

#include <windows.h>

HWND sx_create_window(const wchar* windowsName);

void sx_window_set_text(const wchar* str);

//////////////////////////////////////////////////////////////////////////
//  some global functions


#endif	//	GUARD_sxConsole_HEADER_FILE