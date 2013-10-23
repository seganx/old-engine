/********************************************************************
	created:	2012/04/14
	filename: 	Application.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain window and application class to 
				manage windows of game/editor application
*********************************************************************/
#ifndef GUARD_Application_HEADER_FILE
#define GUARD_Application_HEADER_FILE

#include "System_def.h"

//! define style of border of window
#define WBT_
enum WindowBorderType {
	WBT_NONE = 0,
	WBT_ORDINARY,
	WBT_ORDINARY_RESIZABLE,
	WBT_WINTOOL,
	WBT_WINTOOL_RESIZABLE,
	WBT_DIALOG,
	WBT_DIALOG_RESIZABLE,
};

//! enumeration cursor types
#define WCT_
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
	uint	msg;
	uint64	wparam;
	uint64	lparam;
	handle	windowHandle;		//	this can be null
};
#endif

//! window event call back. return 0 if event handled
typedef int (*WindowEventCallback)( class Window* Sender, const WindowEvent* data );


//!	main loop of application.
typedef void (*ApplicationMainLoop)( float elpsTime );


/*
Window class is a simple abstract class to create and modify a window.
NOTE : to create a window use Application::CreateWindowForm
*/
class SEGAN_ENG_API Window
{
public:
	Window( void );
	virtual ~Window( void );

	virtual const handle GetHandle( void ) const = 0;
	virtual void SetTitle( const wchar* caption ) = 0;
	virtual void SetCursor( const WindowCursorType cursorType ) = 0;
	virtual void SetRect( const int left, const int top, const int width, const int height ) = 0;
	virtual void SetRect( const WindowRect& rect ) = 0;
	virtual void SetBorder( const WindowBorderType border ) = 0;
	virtual void SetTopMostEnable( const bool enable ) = 0;
	virtual void SetVisible( const bool visible ) = 0;
	virtual void SetFullScreen( const bool fullscreen ) = 0;

public:
	enum Options
	{
		WINDOW_VISIBLE		= 0x00000001,
		WINDOW_TOPMOST		= 0x00000002,
		WINDOW_FULLSCREEN	= 0x00000004,
	};

	String				m_name;		//	name of the window
	String				m_title;	//	title of window
	WindowRect			m_rect;		//	window rectangle
	WindowBorderType	m_border;	//! type of border
	dword				m_option;	//  options of window

};

//////////////////////////////////////////////////////////////////////////
//	application management
//////////////////////////////////////////////////////////////////////////

//! initialize the application
SEGAN_ENG_API void sx_app_initialize( WindowEventCallback callbackEvents );

//! finalize the application
SEGAN_ENG_API void sx_app_finalize( void );

//! create a new window
SEGAN_ENG_API Window* sx_app_create_window( const wchar* name, WindowBorderType WBT_ borderType = WBT_ORDINARY_RESIZABLE, bool background = true );

//! destroy a window
SEGAN_ENG_API void sx_app_destroy_window( Window*& pwindow );

//! return a pointer to the window by specified handle
SEGAN_ENG_API bool sx_app_get_window( handle whandle, Window*& pwindow );

//! run the application and call main loop
SEGAN_ENG_API void sx_app_run( ApplicationMainLoop mainLoop );

//! close all windows and terminate the application
SEGAN_ENG_API void sx_app_terminate(void);

#endif	//	GUARD_Application_HEADER_FILE
