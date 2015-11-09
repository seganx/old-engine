#if defined(_WIN32)

#include "../Window.h"
#include "../OS.h"
#include "Win6.h"

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


LRESULT WINAPI DefaultMsgProc(HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam);
LARGE_INTEGER GetWinStyles(WindowBorderType wbtype, bool topmost);


//////////////////////////////////////////////////////////////////////////
//	window_win32
//////////////////////////////////////////////////////////////////////////
Window::Window( void ): m_border(WBT_ORDINARY_RESIZABLE), m_option(0), m_callback(0) {}
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

		m_rect.left		= sx_os_get_monitor()->workingWidth / 2 - 512;
		m_rect.top		= sx_os_get_monitor()->workingHeight / 2 - 300;
		m_rect.width	= 1024;
		m_rect.height	= 600;
	}

	virtual ~Window_win32( void )
	{
		if ( !m_handle ) return;

		// destroy window and class
		DestroyWindow( m_handle );
		UnregisterClass( m_windowClass.lpszClassName, m_windowClass.hInstance );
	}

	virtual const struct HWND__* get_handle( void ) const
	{
		return m_handle;
	}

	virtual void set_title( const wchar* caption )
	{
		if ( m_title == caption ) return;

		m_title = caption;
		if ( m_handle )
			SetWindowText( m_handle, *m_title );
	}

	virtual void set_cursor( const WindowCursorType cursorType )
	{
		m_windowClass.hCursor = cursorHandle[cursorType];
		SetClassLong( m_handle, GCL_HCURSOR, (LONG)((uint64)m_windowClass.hCursor) );
		Update();
	}

	virtual void set_rect( const sint left, const sint top, const sint width, const sint height )
	{
		m_rect.left		= left;
		m_rect.top		= top;
		m_rect.width	= width;
		m_rect.height	= height;
		Update();
	}

	virtual void set_border( const WindowBorderType border )
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

	virtual void set_topmost( const bool enable )
	{
		if ( enable )
			sx_set_add( m_option, WINDOW_TOPMOST );
		else
			sx_set_rem( m_option, WINDOW_TOPMOST );
		Update();
	}

	virtual void set_visible( const bool visible )
	{
		if ( visible )
			sx_set_add( m_option, WINDOW_VISIBLE );
		else
			sx_set_rem( m_option, WINDOW_VISIBLE	);
		Update();
	}

	virtual void SetFullScreen( const bool fullscreen )
	{
		if ( fullscreen )
			sx_set_add( m_option, WINDOW_FULLSCREEN );
		else
			sx_set_rem( m_option, WINDOW_FULLSCREEN );
		Update();
	}

	void Update(void)
	{
		if ( !m_handle ) return;
		sx_callstack();

		HWND winpos = ( m_option & WINDOW_TOPMOST ) ? HWND_TOPMOST : HWND_TOP;
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

		SetWindowPos( m_handle, winpos, m_rect.left, m_rect.top, m_rect.width + W, m_rect.height + H, SWP_NOOWNERZORDER );

		if ( m_option & WINDOW_VISIBLE )
			ShowWindow( m_handle, SW_SHOWDEFAULT );
		else
			ShowWindow( m_handle, SW_HIDE );

		UpdateWindow( m_handle );
	}

public:

	struct HWND__*			m_handle;			//! handle of this window
	WNDCLASSEX				m_windowClass;		//! class of window

};





//////////////////////////////////////////////////////////////////////////
//	helper functions
//////////////////////////////////////////////////////////////////////////
Map<uint64, class Window_win32*>*	s_windows = null;

Window* sx_create_window( const wchar* name, const CB_Window callback, const bool background /*= true*/, const bool visible /*= true */ )
{
	sx_callstack_param(sx_create_window(name=%s), name);

	if ( s_windows == null )
		s_windows = sx_new Map<uint64, class Window_win32*>;

	Window_win32* win = sx_new( Window_win32 );

	win->m_name = name ? name : L"SeganX Window";
	win->m_callback = callback;
	win->m_border = WBT_ORDINARY_RESIZABLE;

	// add this new window to the map with zero handle
	s_windows->insert_multi( 0, win );

	// Register the window class
	win->m_windowClass.lpfnWndProc = DefaultMsgProc;
	win->m_windowClass.lpszClassName = win->m_name;
	win->m_windowClass.hbrBackground = background ? (HBRUSH)COLOR_WINDOW : null;
	win->m_windowClass.hCursor = LoadCursor(null, IDC_ARROW);
	win->m_windowClass.hIcon = LoadIcon( null, IDI_APPLICATION );
	RegisterClassEx( &win->m_windowClass );

	//  compute win styles
	LARGE_INTEGER winStyles = GetWinStyles( win->m_border, ( win->m_option & Window::WINDOW_TOPMOST ) != 0 );

	// Create the window
	win->m_handle = CreateWindowEx(
		winStyles.LowPart,
		win->m_windowClass.lpszClassName,
		( win->m_title.text() ? win->m_title.text() : win->m_name.text() ),
		winStyles.HighPart | WS_TABSTOP | WS_GROUP,
		win->m_rect.left,
		win->m_rect.top,
		win->m_rect.width,
		win->m_rect.height,
		null,
		null,
		win->m_windowClass.hInstance,
		null );
	win->set_title( win->m_name );

	//  update the window
	win->set_visible( visible );

	return win;
}

void sx_destroy_window( Window*& pwindow )
{
	if ( !pwindow ) return;
	sx_callstack();

	Window_win32* win = (Window_win32*)pwindow;
	if ( win->m_handle )
	{
		// remove this window from the map list
		s_windows->remove( reinterpret_cast<uint64>(win->m_handle) );

		if ( s_windows->m_count < 1 )
			sx_safe_delete_and_null( s_windows );
	}

	sx_safe_delete_and_null( pwindow );
}

#if 0
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
#endif

LRESULT WINAPI DefaultMsgProc( HWND hWnd, uint msg, WPARAM wParam, LPARAM lParam )
{
	Window_win32* pwin = null;

	//	search in registered windows
	if ( s_windows->find( reinterpret_cast<uint64>(hWnd), pwin ) == false )
	{
		//	if no window has been fined search for new created window
		if ( hWnd && s_windows->find( 0, pwin ) )
		{
			s_windows->remove( 0 );
			s_windows->insert( reinterpret_cast<uint64>(hWnd), pwin );
		}
	}

	//	verify that any window has been found
	if ( pwin )
	{
		WindowEvent wevent = { msg, wParam, lParam, hWnd };
		if ( pwin->m_callback && pwin->m_callback( pwin, &wevent ) == 0 )
			return 0;
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
