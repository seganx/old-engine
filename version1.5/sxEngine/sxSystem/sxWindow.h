/********************************************************************
	created:	2010/08/10
	filename: 	sxWindow.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of window
*********************************************************************/
#ifndef GUARD_sxWindow_HEADER_FILE
#define GUARD_sxWindow_HEADER_FILE


#include "sxSystem_def.h"

//! define style of border of window
#define WBT_
enum WindowBorderType{
	WBT_NONE = 0,
	WBT_ORDINARY,
	WBT_ORDINARY_RESIZABLE,
	WBT_WINTOOL,
	WBT_WINTOOL_RESIZABLE,
	WBT_DIALOG,
	WBT_DIALOG_RESIZABLE,
};

//! contain information of window position and size
typedef struct WindowRect
{
	int Left;
	int Top;
	int Width;
	int Height;

	WindowRect():Left(0), Top(0), Width(0), Height(0){}
	WindowRect(int L, int T, int W, int H):	Left(L), Top(T), Width(W), Height(H){}
} *PWindowRect;

/*
Application call back is main loop function of application
*/
//typedef void (*SysCallBack_MainLoop)(MSG& msg, float elpsTime);
namespace sx { namespace sys
{
class SEGAN_API Window;
} }

typedef UINT (*SysCallback_MsgProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef void (*SysCallback_MainLoop)(float elpsTime);
typedef bool (*SysCallback_WindowEvent)(sx::sys::Window* Sender);
typedef void (*SysCallback_WindowRect)(sx::sys::Window* Sender, WindowRect& newRect);

namespace sx { namespace sys
{

/*
Window class is a simple class to create and modify a window
*/
class SEGAN_API Window
{
	SEGAN_STERILE_CLASS(Window);

public:
	Window(const WCHAR* _name = NULL);
	virtual ~Window(void);

	const WCHAR* GetName(void);

	void SetIcon(HICON hIcon);
	HICON GetIcon(void);

	void SetCursor(HCURSOR hCursor);
	HCURSOR GetCursor(void);

	void SetTitle(const WCHAR* _caption);
	const WCHAR* GetTitle(void);

	void SetBorder(WBT_ WindowBorderType wb);
	WindowBorderType GetBorder(void);

	void SetRect(int _Left, int _Top, int _Width, int _Height);
	void SetRect(WindowRect& _rect);
	void GetRect(WindowRect& OUT _rect);

	void SetTopMostEnable(bool _enable);
	bool GetTopMostEnable(void);

	void SetVisible(bool _visible);
	bool GetEnable(void);

	void SetParent(HWND hWnd);
	HWND GetParent(void);

	void SetBackground(HBRUSH _brush);
	HBRUSH GetBackground(void);

	const HWND GetHandle(void);

	void SetOnCreate(SysCallback_WindowEvent _OnCreate);
	void SetOnClose(SysCallback_WindowEvent _OnClose);
	void SetOnDestroy(SysCallback_WindowEvent _OnDestroy);
	void SetOnRect(SysCallback_WindowRect _OnRect);

	SysCallback_WindowEvent GetOnCreate(void);
	SysCallback_WindowEvent GetOnClose(void);
	SysCallback_WindowEvent GetOnDestroy(void);
	SysCallback_WindowRect	GetOnRect(void);

private:
	void Update(void);

	friend class Application;
	String			m_Name;		// name of the window
	String			m_Title;	// title of window
	WindowRect			m_Rect;		// rectangle of window
	WindowBorderType	m_Border;	// type of border
	WNDCLASSEX			m_Wc;		// class of window
	HWND				m_hWnd;		// handle of this window
	bool				m_Visible;	// visibility of this window
	bool				m_Topmost;	// position window to the always on top

	//	call backs to manage window events
	SysCallback_WindowEvent m_OnCreate;		// this function will call after the window has been created.
	SysCallback_WindowEvent	m_OnClose;		// this function will call before closing. return false to prevent closing.
	SysCallback_WindowEvent	m_OnDestroy;	// this function will call before destroying window.
	SysCallback_WindowRect	m_OnRect;		// this function will call when window rectangle is going to change.
};
typedef Window	*PWindow;


/*
Application is the main class that no need to create. this class run and control the project
*/
class SEGAN_API Application
{
public:
	//! return instance of application
	static HINSTANCE Get_Instance(void);

	//! create a new window from window structure
	static void Create_Window(PWindow pWin);

	//! destroy a window
	static void Destroy_Window(PWindow pWin);

	//! return a pointer to the window by specified handle
	static bool Get_Window(HWND hWnd, PWindow& pWin);

	//! run the application and call main loop
	static void Run(SysCallback_MainLoop MainLoop, SysCallback_MsgProc MsgProc = NULL);

	//! call all close window function and terminate the application
	static void Terminate(void);

private:
	static LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
};

} } // namespace sx { namespace sys

#endif // GUARD_sxWindow_HEADER_FILE
