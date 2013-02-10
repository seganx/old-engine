#if defined(_WIN32)

#include "../Application.h"
#include "../Log.h"
#include "../OS.h"
#include "Win6.h"
#include "resource.h"

typedef Map<u64, class Window_win32*> MapWindow;

extern const HCURSOR cursorHandle[WCT_Count] = 
{
	LoadCursor( null, IDC_ARROW   ),
	LoadCursor( null, IDC_IBEAM   ),
	LoadCursor( null, IDC_WAIT    ),
	LoadCursor( null, IDC_CROSS   ),
	LoadCursor( null, IDC_UPARROW ),
	LoadCursor( null, IDC_SIZE    ),
	LoadCursor( null, IDC_ICON    ),
	LoadCursor( null, IDC_SIZENWSE),
	LoadCursor( null, IDC_SIZENESW),
	LoadCursor( null, IDC_SIZEWE  ),
	LoadCursor( null, IDC_SIZENS  ),
	LoadCursor( null, IDC_SIZEALL ),
	LoadCursor( null, IDC_NO      ),
	LoadCursor( null, IDC_HAND    ),
	LoadCursor( null, IDC_HELP    )
};

// internal class
struct app_internal
{
	MapWindow				windows;
	Window_win32*			newOne;
	HWND					mainWindow;
	WindowRect				fullscreenRect;
	WindowEventCallback		callbackEvents;

	app_internal(): mainWindow(0), newOne(0), callbackEvents(0) {}
};
static app_internal * s_application = null;


LRESULT WINAPI DefaultMsgProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam);
LARGE_INTEGER GetWinStyles(WindowBorderType wbtype, bool topmost);


//////////////////////////////////////////////////////////////////////////
//	window_win32
//////////////////////////////////////////////////////////////////////////
Window::Window( void ): m_border(WBT_ORDINARY_RESIZABLE), m_option(0) {}
Window::~Window( void ) {}

// ! driven class of window for windows operation system
class Window_win32 : public Window
{

public:

	Window_win32( void ): Window(), m_handle(0)
	{
		ZeroMemory(&m_windowClass, sizeof(m_windowClass));

		m_windowClass.cbSize		= sizeof(WNDCLASSEX);
		m_windowClass.style 		= CS_CLASSDC;
		m_windowClass.hInstance		= GetModuleHandle( null );

		m_rect.left		= sx_os_get_monitor()->workingWidth / 2 - 400;
		m_rect.top		= sx_os_get_monitor()->workingHeight / 2 - 300;
		m_rect.width	= 800;
		m_rect.height	= 600;

		s_application->fullscreenRect.left		= 0;
		s_application->fullscreenRect.top		= 0;
		s_application->fullscreenRect.width		= sx_os_get_monitor()->areaWidth;
		s_application->fullscreenRect.height	= sx_os_get_monitor()->areaHeight;
	}

	virtual ~Window_win32( void )
	{
		if ( !m_handle ) return;

		// destroy window and class
		DestroyWindow( m_handle );
		UnregisterClass( m_windowClass.lpszClassName, m_windowClass.hInstance );
	}

	virtual const handle GetHandle( void ) const
	{
		return (handle)&m_handle;
	}

	virtual void SetTitle( const wchar* caption )
	{
		if ( m_title == caption ) return;

		m_title = caption;
		if ( m_handle )
			SetWindowText( m_handle, *m_title );
	}

	virtual void SetCursor( const WindowCursorType cursorType )
	{
		m_windowClass.hCursor = cursorHandle[cursorType];
		SetClassLong( m_handle, GCL_HCURSOR, (LONG)((uint64)m_windowClass.hCursor) );
		Update();
	}

	virtual void SetRect( const sint left, const sint top, const sint width, const sint height )
	{
		m_rect.left		= left;
		m_rect.top		= top;
		m_rect.width	= width;
		m_rect.height	= height;
		Update();
	}

	virtual void SetRect( const WindowRect& rect )
	{
		m_rect = rect;
		Update();
	}

	virtual void SetBorder( const WindowBorderType border )
	{
		if ( m_border != border )
		{
			m_border = border;

			if ( m_handle )
			{
				LARGE_INTEGER styles = GetWinStyles( m_border, (m_option & WINDOW_TOPMOST)!=0 );
				SetWindowLong( m_handle, GWL_STYLE, styles.HighPart );
				SetWindowLong( m_handle, GWL_EXSTYLE, styles.LowPart );
				Update();
			}
		}
	}

	virtual void SetTopMostEnable( const bool enable )
	{
		if ( enable )
			SEGAN_SET_ADD( m_option, WINDOW_TOPMOST );
		else
			SEGAN_SET_REM( m_option, WINDOW_TOPMOST );
		Update();
	}

	virtual void SetVisible( const bool visible )
	{
		if ( visible )
			SEGAN_SET_ADD( m_option, WINDOW_VISIBLE );
		else
			SEGAN_SET_REM( m_option, WINDOW_VISIBLE	);
		Update();
	}

	virtual void SetFullScreen( const bool fullscreen )
	{
		if ( fullscreen )
			SEGAN_SET_ADD( m_option, WINDOW_FULLSCREEN );
		else
			SEGAN_SET_REM( m_option, WINDOW_FULLSCREEN );
		Update();
	}

	void Update(void)
	{
		if ( !m_handle ) return;
		sx_callstack_push(Window_win32::Update());

		HWND winpos = ( m_option & WINDOW_TOPMOST ) ? HWND_TOPMOST : HWND_TOP;
		WindowRect* rect = ( m_option & WINDOW_FULLSCREEN ) ? &s_application->fullscreenRect : &m_rect;
		WindowBorderType border = ( m_option & WINDOW_FULLSCREEN ) ? WBT_NONE : m_border;

		//  compute offsets
		sint W = 0, H = 0;
		if ( border != WBT_NONE )
		{
			WINDOWINFO winfo;
			GetWindowInfo(m_handle, &winfo);
			W = (winfo.rcClient.left - winfo.rcWindow.left) + (winfo.rcWindow.right - winfo.rcClient.right);
			H = (winfo.rcClient.top  - winfo.rcWindow.top)  + (winfo.rcWindow.bottom - winfo.rcClient.bottom);
		}

		SetWindowPos( m_handle, winpos, rect->left, rect->top, rect->width + W, rect->height + H, SWP_NOOWNERZORDER );

		if ( m_option & WINDOW_VISIBLE )
			ShowWindow( m_handle, SW_SHOWDEFAULT );
		else
			ShowWindow( m_handle, SW_HIDE );

		UpdateWindow( m_handle );
	}

public:

	HWND					m_handle;			//! handle of this window
	WNDCLASSEX				m_windowClass;		//! class of window
};





//////////////////////////////////////////////////////////////////////////
//	application
//////////////////////////////////////////////////////////////////////////

void sx_app_initialize( WindowEventCallback callbackEvents )
{
	if ( s_application )
	{
		g_logger->Log(L"Warning! calling Application::Initialize failed. application has been initialized !");
		return;
	}
	s_application = sx_new( app_internal );
	s_application->callbackEvents = callbackEvents;
}

void sx_app_finalize( void )
{
	if ( !s_application )
	{
		g_logger->Log(L"Warning! calling system::Finalize failed. system is not initialized !");
		return;
	}

	// destroy windows and unregister classes
	for ( MapWindow::Iterator it = s_application->windows.First(); !it.IsLast(); it++ )
	{
		sx_delete_and_null( *it );
	}

	sx_delete_and_null( s_application );
}

Window* sx_app_create_window( const wchar* name, WindowBorderType WBT_ borderType /*= WBT_ORDINARY_RESIZABLE*/, bool background /*= true */ )
{
	sx_callstack_push(sx_app_create_window(name=%s),name);

	Window_win32* win = sx_new( Window_win32 );
	s_application->newOne = win;

	win->m_name = name ? name : L"SeganX Window";
	win->m_border = borderType;

	// Register the window class
	win->m_windowClass.lpfnWndProc = DefaultMsgProc;
	win->m_windowClass.lpszClassName = win->m_name;
	win->m_windowClass.hbrBackground = background ? (HBRUSH)COLOR_WINDOW : null;
	win->m_windowClass.hCursor = LoadCursor(null, IDC_ARROW);
	win->m_windowClass.hIcon = LoadIcon(win->m_windowClass.hInstance, MAKEINTRESOURCE(IDI_SXENGINE));
	RegisterClassEx( &win->m_windowClass );

	//  calculate win styles
	LARGE_INTEGER winStyles = GetWinStyles( win->m_border, ( win->m_option & Window::WINDOW_TOPMOST ) != 0 );

	// Create the window
	win->m_handle = CreateWindowEx(
		winStyles.LowPart,
		win->m_windowClass.lpszClassName,
		win->m_title.Text(),
		winStyles.HighPart | WS_TABSTOP | WS_GROUP,
		win->m_rect.left,
		win->m_rect.top,
		win->m_rect.width,
		win->m_rect.height,
		s_application->mainWindow,
		null,
		win->m_windowClass.hInstance,
		null );
	win->SetTitle( win->m_name );

	//  store main window handle
	if ( !s_application->mainWindow )
		s_application->mainWindow = win->m_handle;

	// add this new window to the map list
	s_application->windows.Insert( reinterpret_cast<uint64>(win->m_handle), win );

	//  update the window
	win->Update();

	return win;
}

void sx_app_destroy_window( Window*& pwindow )
{
	if ( !pwindow ) return;
	sx_callstack_push(sx_app_destroy_window());

	Window_win32* win = (Window_win32*)pwindow;
	if ( win->m_handle )
	{
		// remove this window from the map list
		s_application->windows.Remove( reinterpret_cast<uint64>(win->m_handle) );
	}

	sx_delete_and_null( pwindow );
}

bool sx_app_get_window( handle whandle, Window*& pwindow )
{
	return s_application->windows.Find( reinterpret_cast<uint64>( *( (HWND*)whandle ) ), (Window_win32*&)pwindow );
}

void sx_app_run( ApplicationMainLoop mainLoop )
{
	static float blendedElapesTime = 0;
	float initTime = (float)sx_os_get_timer();
	float elpsTime = 0;
	
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while( msg.message != WM_QUIT )
	{
		if ( PeekMessage( &msg, null, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			msg.hwnd = null;	
			msg.message = 0;
		}

		// calculate elapsed time
		float curTime = (float)sx_os_get_timer();
		elpsTime = curTime - initTime;
		initTime = curTime;

		//  avoid update when system timer has been reseted after about 47 days
		if ( 0.0f < elpsTime && elpsTime < 2000.0f )
		{
			// call the main loop function
			blendedElapesTime += (elpsTime - blendedElapesTime) * 0.1f;
			if ( mainLoop ) mainLoop( blendedElapesTime );
		}
	}
}

void sx_app_terminate( void )
{
	PostQuitMessage(0);
}

LRESULT WINAPI DefaultMsgProc( HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam )
{
	switch (msg)
	{
	case WM_CLOSE:
		{
			Window_win32* win;
			if ( s_application->windows.Find(reinterpret_cast<uint64>(hWnd), win) )
			{
				if ( s_application->callbackEvents )
				{
					WindowEvent wevent = { msg, wParam, lParam, &hWnd };
					if ( s_application->callbackEvents( win, &wevent ) == 0 )
						return 0;
				}

				if ( hWnd != s_application->mainWindow )
				{
					win->SetVisible(false);
					return 0;
				}
			}
		}
		break;

	case WM_CREATE:
		if ( s_application->newOne )
		{
			s_application->newOne->m_handle = hWnd;
			WindowEvent wevent = { msg, wParam, lParam, &hWnd };
			if ( s_application->callbackEvents )
				s_application->callbackEvents( s_application->newOne, &wevent );
			s_application->newOne = null;
		}
		break;

	case WM_DESTROY:
		{
			if ( s_application->callbackEvents )
			{
				Window_win32* pWin = null;
				if (  s_application->windows.Find( reinterpret_cast<uint64>(hWnd), pWin ) )
				{
					WindowEvent wevent = { msg, wParam, lParam, &hWnd };
					if ( s_application->callbackEvents( pWin, &wevent ) == 0 )
						return 0;
				}
			}

			if ( hWnd == s_application->mainWindow )
			{
				PostQuitMessage( 0 );
				return 0;
			}
		}
		break;

	default:
		if ( s_application->callbackEvents )
		{
			Window_win32* pWin = null;
			s_application->windows.Find( reinterpret_cast<uint64>(hWnd), pWin );
			WindowEvent wevent = { msg, wParam, lParam, &hWnd };
			if ( s_application->callbackEvents( pWin, &wevent ) == 0 )
					return 0;
		}
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

SEGAN_INLINE LARGE_INTEGER GetWinStyles( WindowBorderType wbtype, bool topmost )
{
	dword wStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX ;
	dword exStyle = topmost ? WS_EX_TOPMOST : 0;
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

#endif
