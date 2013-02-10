/********************************************************************
	created:	2010/07/22
	filename: 	sxLog.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a singleton Logger class 
*********************************************************************/
#ifndef GUARD_sxLog_HEADER_FILE
#define GUARD_sxLog_HEADER_FILE

#include "sxSystem_def.h"

static const DWORD WM_SX_LOGGER = WM_USER + 100;

#define LM_
#define LM_CONSOLE	0x01
#define LM_WINDOW	0x02
#define LM_FILE		0x04

typedef void (*CallBack_Logger)(const WCHAR* Message);

class SEGAN_API sxLog
{
public:

	//! log with time stamp
	static void Log(const WCHAR* format, ...);

	//! log without time stamp
	static void Log_(const WCHAR* format, ...);

	//! can be one or more combination of LM_ series
	static void SetMode(UINT LM_ mode);

	static UINT GetMode(void) LM_ ;

	static void SetWindow(HWND window);

	static HWND GetWindow(void);

	static void SetFileName(const WCHAR* FileName, bool Rewrite = true);

	static const WCHAR* GetFileName(void);

	static void SetCallback(CallBack_Logger callback);
};

#endif // GUARD_sxLog_HEADER_FILE
