/********************************************************************
	created:	2010/10/01
	filename: 	MainWindow.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain main window of editor structure and
				some functions to manage that.
*********************************************************************/
#ifndef SX_MainWindow_HEADER_FILE
#define SX_MainWindow_HEADER_FILE

#include "../sxEngine/sxEngine.h"

class MainWin
{
public:
	static void InitWindow(void);
	static void OnRect(sx::sys::Window* Sender, WindowRect& newRect);
	static bool OnClose(sx::sys::Window* Sender);
public:
	static str256				s_Title;
	static sx::sys::Window		s_Win;
};

#endif