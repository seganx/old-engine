/********************************************************************
	created:	2013/11/3
	filename: 	Window.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple window class for game/editor
*********************************************************************/
#ifndef GUARD_Window_HEADER_FILE
#define GUARD_Window_HEADER_FILE

#include "../Engine_def.h"

//! define style of border of window
enum WindowBorderType {
	WBT_NONE = 0,
	WBT_ORDINARY,
	WBT_ORDINARY_RESIZABLE,
	WBT_WINTOOL,
	WBT_WINTOOL_RESIZABLE,
	WBT_DIALOG,
	WBT_DIALOG_RESIZABLE,
};
#define WBT_

//! enumeration cursor types
enum WindowCursorType{
	WCT_ARROW = 0, 
	WCT_IBEAM,
	WCT_WAIT,
	WCT_CROSS,
	WCT_UPARROW,
	WCT_SIZE,
	WCT_ICON,
	WCT_SIZENWSE,
	WCT_SIZENESW,
	WCT_SIZEWE,
	WCT_SIZENS,
	WCT_SIZEALL,
	WCT_NO,
	WCT_HAND,
	WCT_HELP,
	WCT_Count
};
#define WCT_

//! contain information of window position and size
struct WindowRect
{
	int left;
	int top;
	int width;
	int height;

	WindowRect():left(0), top(0), width(0), height(0){}
	WindowRect(int L, int T, int W, int H):	left(L), top(T), width(W), height(H){}
};

#if defined(_WIN32)
struct WindowEvent 
{
	uint			msg;
	uint64			wparam;
	uint64			lparam;
	struct HWND__*	windowHandle;		//	this can be null
};
#endif

//! window event call back. return 0 if event handled
typedef int (*CB_Window)( class Window* sender, const WindowEvent* data );

/*
Window class is a simple abstract class to create and modify a window.
NOTE : to create a window use Application::CreateWindowForm
*/
class SEGAN_ENG_API Window
{
public:
	Window( void );
	virtual ~Window( void );

#if defined(_WIN32)
	virtual const struct HWND__* get_handle( void ) const = 0;
#endif
	virtual void set_title( const wchar* caption ) = 0;
	virtual void set_cursor( const WindowCursorType cursorType ) = 0;
	virtual void set_rect( const int left, const int top, const int width, const int height ) = 0;
	virtual void set_border( const WindowBorderType WBT_ border ) = 0;
	virtual void set_topmost( const bool enable ) = 0;
	virtual void set_visible( const bool visible ) = 0;

public:
	enum Options
	{
		WINDOW_VISIBLE		= 0x00000001,
		WINDOW_TOPMOST		= 0x00000002,
		WINDOW_FULLSCREEN	= 0x00000004,
	};

	dword				m_option;		//! options of window
	String				m_name;			//!	name of the window
	String				m_title;		//!	title of window
	WindowRect			m_rect;			//!	window rectangle
	WindowBorderType	m_border;		//! type of border
	CB_Window			m_callback;		//! callback function

};

//! create a new window
SEGAN_ENG_API Window* sx_create_window( const wchar* name, const CB_Window callback, const bool background = true, const bool visible = true );

//! destroy a window
SEGAN_ENG_API void sx_destroy_window( Window* pwindow );


#endif	//	GUARD_Window_HEADER_FILE
