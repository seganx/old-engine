#include "sxWindow.h"
#include "sxTaskMan.h"
#include "sxSys.h"


//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES AND FUCTIONS
//////////////////////////////////////////////////////////////////////////
typedef Map<UINT64, sx::sys::Window*>		mapWindow;

static mapWindow			s_mapWindow;
static HINSTANCE			s_Instance = GetModuleHandle(NULL);
static HWND					s_MainWindow = NULL;
static SysCallback_MsgProc	s_MsgProc = NULL;

LARGE_INTEGER GetWinStyles(WindowBorderType wbtype, bool topmost)
{
	DWORD wStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX ;
	DWORD exStyle = BYTE(topmost) * WS_EX_TOPMOST;
	switch (wbtype)
	{
	case WBT_NONE:		
		wStyle = WS_POPUP;
		break;
	case WBT_ORDINARY:	
		break;
	case WBT_ORDINARY_RESIZABLE:
		wStyle |= WS_SIZEBOX;
		break;
	case WBT_WINTOOL:
		wStyle ^= (WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
		exStyle|= WS_EX_TOOLWINDOW;		
		break;
	case WBT_WINTOOL_RESIZABLE:
		wStyle ^= (WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
		wStyle |= WS_SIZEBOX;
		exStyle|= WS_EX_TOOLWINDOW;		
		break;
	case WBT_DIALOG:
		wStyle = WS_CAPTION;
		break;
	case WBT_DIALOG_RESIZABLE:
		wStyle = WS_CAPTION | WS_SIZEBOX;
		break;
	}
	LARGE_INTEGER result;
	result.HighPart = wStyle;
	result.LowPart	= exStyle;
	return result;
}

namespace sx { namespace sys
{
	
//////////////////////////////////////////////////////////////////////////
//		WINDOW OBJECT
//////////////////////////////////////////////////////////////////////////
Window::Window( const WCHAR* _name )
{
	s_Instance = GetModuleHandle(NULL);

	if (!_name)
		m_Name	= L"SeganX Window";
	else
		m_Name	= _name;

	m_hWnd		= NULL;
	m_Title		= m_Name;
	m_Rect		= WindowRect(sx::sys::GetDesktopWidth()/2-200, sx::sys::GetDesktopHeight()/2-150, 400, 300);
	m_Border	= WBT_ORDINARY_RESIZABLE;
	m_Visible	= false;
	m_Topmost	= false;

	m_Wc.cbSize			= sizeof(WNDCLASSEX);
	m_Wc.style 			= CS_CLASSDC;
	m_Wc.lpfnWndProc	= NULL;
	m_Wc.cbClsExtra		= 0;
	m_Wc.cbWndExtra		= 0;
	m_Wc.hInstance		= s_Instance;
	m_Wc.hIcon			= NULL;
	m_Wc.hCursor		= LoadCursor(NULL, IDC_ARROW); 
	m_Wc.hbrBackground	= (HBRUSH)COLOR_WINDOW; 
	m_Wc.lpszMenuName	= NULL;
	m_Wc.lpszClassName	= *m_Name;
	m_Wc.hIconSm		= NULL;

	m_OnCreate	= NULL;
	m_OnClose	= NULL;
	m_OnDestroy = NULL;
	m_OnRect	= NULL;
}

Window::~Window(void)
{

}

const WCHAR* Window::GetName( void )
{
	return m_Name.Text();
}

void Window::SetIcon( HICON hIcon )
{
	m_Wc.hIcon = hIcon;
	SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	Update();
}

HICON Window::GetIcon( void )
{
	return m_Wc.hIcon;
}

void Window::SetCursor( HCURSOR hCursor )
{
	m_Wc.hCursor = hCursor;
	SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)((UINT64)hCursor));
	Update();
}

HCURSOR Window::GetCursor( void )
{
	return m_Wc.hCursor;
}

void Window::SetTitle( const WCHAR* _caption )
{
	if (m_Title == _caption) return;

	m_Title = _caption;
	if (m_hWnd)
		SetWindowText(m_hWnd, *m_Title);
}

const WCHAR* Window::GetTitle( void )
{
	return *m_Title;
}

void Window::SetBorder( WBT_ WindowBorderType wb )
{
	if (m_Border != wb)
	{
		m_Border = wb;

		if (m_hWnd)
		{
			LARGE_INTEGER styles = GetWinStyles(wb, m_Topmost);
			SetWindowLong(m_hWnd, GWL_STYLE, styles.HighPart);
			SetWindowLong(m_hWnd, GWL_EXSTYLE, styles.LowPart);

			Update();
		}
	}
}

WindowBorderType Window::GetBorder( void )
{
	return m_Border;
}

void Window::SetRect( int _Left, int _Top, int _Width, int _Height )
{
	m_Rect.Left		= _Left;
	m_Rect.Top		= _Top;
	m_Rect.Width	= _Width;
	m_Rect.Height	= _Height;

	Update();
}

void Window::SetRect( WindowRect& _rect )
{
	m_Rect.Left		= _rect.Left;
	m_Rect.Top		= _rect.Top;
	m_Rect.Width	= _rect.Width;
	m_Rect.Height	= _rect.Height;

	Update();
}

void Window::GetRect( WindowRect& OUT _rect )
{
	_rect = m_Rect;
}

void Window::SetTopMostEnable( bool _enable )
{
	if (m_Topmost != _enable)
	{
		m_Topmost = _enable;

		Update();
	}
}

bool Window::GetTopMostEnable( void )
{
	return m_Topmost;
}

void Window::SetVisible( bool _visible )
{
	if (_visible != m_Visible)
	{
		m_Visible = _visible;

		Update();
	}
}

bool Window::GetEnable( void )
{
	return m_Visible;
}

void Window::SetParent( HWND hWnd )
{
	SetWindowLong(m_hWnd, GWL_HWNDPARENT, (LONG)(UINT64)hWnd);
}

HWND Window::GetParent( void )
{
	return (HWND)(UINT64)GetWindowLong(m_hWnd, GWL_HWNDPARENT);
}

void Window::SetBackground( HBRUSH _brush )
{
	m_Wc.hbrBackground = _brush;
	SetClassLong(m_hWnd, GCL_HBRBACKGROUND, (LONG)(UINT64)_brush);
	Update();
}

HBRUSH Window::GetBackground( void )
{
	return m_Wc.hbrBackground;
}

const HWND Window::GetHandle( void )
{
	return m_hWnd;
}

void Window::Update( void )
{
	if (!m_hWnd) return;

	int W=0, H=0;
	if ( m_Border != WBT_NONE )
	{
		WINDOWINFO winfo;
		GetWindowInfo(m_hWnd, &winfo);
		W = (winfo.rcClient.left - winfo.rcWindow.left) + (winfo.rcWindow.right - winfo.rcClient.right);
		H = (winfo.rcClient.top  - winfo.rcWindow.top)  + (winfo.rcWindow.bottom - winfo.rcClient.bottom);
	}

	HWND winpos = m_Topmost ? HWND_TOPMOST : HWND_NOTOPMOST;

	SetWindowPos(m_hWnd,
		winpos,
		m_Rect.Left,
		m_Rect.Top,
		m_Rect.Width + W,
		m_Rect.Height + H,
		SWP_NOOWNERZORDER);

	if (m_Visible)
		ShowWindow( m_hWnd, SW_SHOWDEFAULT );
	else
		ShowWindow( m_hWnd, SW_HIDE );

	UpdateWindow(m_hWnd);
}

void Window::SetOnCreate( SysCallback_WindowEvent _OnCreate )
{
	m_OnCreate = _OnCreate;
}

void Window::SetOnClose( SysCallback_WindowEvent _OnClose )
{
	m_OnClose = _OnClose;
}

void Window::SetOnDestroy( SysCallback_WindowEvent _OnDestroy )
{
	m_OnDestroy = _OnDestroy;
}

void Window::SetOnRect( SysCallback_WindowRect _OnRect )
{
	m_OnRect = _OnRect;
}

SysCallback_WindowEvent Window::GetOnCreate( void )
{
	return m_OnCreate;
}

SysCallback_WindowEvent Window::GetOnClose( void )
{
	return m_OnClose;
}

SysCallback_WindowEvent Window::GetOnDestroy( void )
{
	return m_OnDestroy;
}

SysCallback_WindowRect	Window::GetOnRect( void )
{
	return m_OnRect;
}



//////////////////////////////////////////////////////////////////////////
//	APPLICATION
//////////////////////////////////////////////////////////////////////////

HINSTANCE Application::Get_Instance( void )
{
	s_Instance = GetModuleHandle(NULL);
	return s_Instance;
}

void Application::Create_Window( PWindow pWin )
{
	if (!pWin) return;
	
	//  verify that new window is not exist in created list
	PWindow isExist;
	if (s_mapWindow.Find((UINT64)pWin->m_hWnd, isExist))
		return;

	// Register the window class
	pWin->m_Wc.lpfnWndProc = MsgProc;
	RegisterClassEx( &pWin->m_Wc );

	//  calculate win styles
	LARGE_INTEGER winStyles = GetWinStyles(pWin->m_Border, pWin->m_Topmost);

	// Create the window
	pWin->m_hWnd = CreateWindowEx(
		winStyles.LowPart,
		pWin->m_Wc.lpszClassName,
		pWin->m_Title.Text(),
		winStyles.HighPart,
		pWin->m_Rect.Left,
		pWin->m_Rect.Top,
		pWin->m_Rect.Width,
		pWin->m_Rect.Height,
		s_MainWindow,
		NULL,
		s_Instance,
		NULL );

	if (!s_MainWindow)
		 s_MainWindow = pWin->m_hWnd;

	// add this new window to the map list
	s_mapWindow.Insert((UINT64)pWin->m_hWnd, pWin);

	// new window has been created. call OnCreate function
	if (pWin->m_OnCreate)
		pWin->m_OnCreate(pWin);

	pWin->Update();
}

void Application::Destroy_Window( PWindow pWin )
{
	if (pWin && pWin->m_hWnd)
	{
		// remove this window from the map list
		s_mapWindow.Remove((UINT64)pWin->m_hWnd);

		// destroy window and class
		DestroyWindow(pWin->m_hWnd);
		UnregisterClass( pWin->m_Wc.lpszClassName, s_Instance );
	}
}

bool Application::Get_Window( HWND hWnd, PWindow& pWin )
{
	return s_mapWindow.Find((UINT64)hWnd, pWin);
}

void Application::Run( SysCallback_MainLoop MainLoop, SysCallback_MsgProc MsgProc /*= NULL*/ )
{
	s_MsgProc = MsgProc;

	static float blendedElapesTime = 0;
	float initTime = sys::GetSysTime();
	float elpsTime = 0;
	
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			msg.hwnd = NULL;	
			msg.message = 0;
		}

		// calculate elapsed time
		elpsTime = sys::GetSysTime() - initTime;
		initTime = sys::GetSysTime();

		//  avoid update when system timer has been reseted after about 47 days
		if ( 0.0f < elpsTime && elpsTime < 2000.0f )
		{
			// do tasks in task man
			TaskManager::Update(elpsTime);

			// cal the main loop function
			blendedElapesTime += (elpsTime - blendedElapesTime) * 0.1f;
			if (MainLoop) MainLoop(blendedElapesTime);
		}
	}

	//  clear all tasks before clearing application and finalize task manager to close threads and events
	TaskManager::Clear();
	TaskManager::Finalize();

	// step 1: call OnClose function for each window
	for (mapWindow::Iterator it = s_mapWindow.First(); !it.IsLast(); it++)
	{
		if ((*it)->m_OnDestroy)
			(*it)->m_OnDestroy(*it);
	}

	// step 2: destroy windows and unregister classes
	for (mapWindow::Iterator it = s_mapWindow.First(); !it.IsLast(); it++)
	{
		// destroy window and unregister classes
		DestroyWindow((*it)->m_hWnd);
		UnregisterClass((*it)->m_Wc.lpszClassName, s_Instance );
	}
}

void Application::Terminate( void )
{
	PostQuitMessage(0);
}

LRESULT WINAPI Application::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// at first send message to the client application and verify the result
	if (s_MsgProc && !s_MsgProc(hWnd, msg, wParam, lParam))
		return 0;

	switch (msg)
	{
	case WM_SIZE:
		{
			sx::sys::PWindow pWin;
			if (s_mapWindow.Find((UINT64)hWnd, pWin))
			{
				WindowRect curRect;

				if (wParam == SIZE_MINIMIZED)
				{
					curRect.Left	= 0;
					curRect.Top		= 0;
					curRect.Width	= 0;
					curRect.Height	= 0;
				}
				else
				{
					WINDOWINFO winfo;
					GetWindowInfo(hWnd, &winfo);
					int iW = (winfo.rcClient.left - winfo.rcWindow.left) + (winfo.rcWindow.right - winfo.rcClient.right);
					int iH = (winfo.rcClient.top  - winfo.rcWindow.top)  + (winfo.rcWindow.bottom - winfo.rcClient.bottom);

					RECT prc;
					GetWindowRect(hWnd, &prc);
					curRect.Left	= prc.left;
					curRect.Top		= prc.top;
					curRect.Width	= prc.right - prc.left - iW;
					curRect.Height	= prc.bottom - prc.top - iH;
				}

				if (pWin->m_OnRect)
					pWin->m_OnRect(pWin, curRect);

				WINDOWINFO winfo;
				GetWindowInfo(hWnd, &winfo);
				pWin->m_Rect.Left	= winfo.rcClient.left;
				pWin->m_Rect.Top	= winfo.rcClient.top;
				pWin->m_Rect.Width	= winfo.rcClient.right - winfo.rcClient.left;
				pWin->m_Rect.Height	= winfo.rcClient.bottom - winfo.rcClient.top;

				return 0;
			}
		}
		break;

	case WM_SIZING:
	case WM_MOVING:
		{
			sx::sys::PWindow pWin;
			if (s_mapWindow.Find((UINT64)hWnd, pWin))
			{
				
				WINDOWINFO winfo;
				GetWindowInfo(hWnd, &winfo);
				int iW = (winfo.rcClient.left - winfo.rcWindow.left) + (winfo.rcWindow.right - winfo.rcClient.right);
				int iH = (winfo.rcClient.top  - winfo.rcWindow.top)  + (winfo.rcWindow.bottom - winfo.rcClient.bottom);

				WindowRect curRect;
				PRECT prc		= (PRECT)lParam;

				curRect.Left	= prc->left;
				curRect.Top		= prc->top;
				curRect.Width	= prc->right - prc->left - iW;
				curRect.Height	= prc->bottom - prc->top - iH;
				
				if (pWin->m_OnRect)
					pWin->m_OnRect(pWin, curRect);

				prc->left	= curRect.Left;
				prc->top	= curRect.Top;
				prc->right	= curRect.Width + prc->left + iW;
				prc->bottom	= curRect.Height + prc->top + iH;

				return TRUE;
			}
		}
		break;

	case WM_CLOSE:
		{
			sx::sys::PWindow pWin;
			if (s_mapWindow.Find((UINT64)hWnd, pWin))
			{
				if (pWin->m_OnClose && !pWin->m_OnClose(pWin))
					return 0;

				if (hWnd != s_MainWindow)
				{
					pWin->SetVisible(false);
					return 0;
				}
			}
		}
		break;

	case WM_DESTROY:
		{
			if (hWnd == s_MainWindow)
			{
				PostQuitMessage( 0 );
				return 0;
			}
		}
		break;

	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
			return 0;
		}
		break;
		
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


} } // namespace sx { namespace sys