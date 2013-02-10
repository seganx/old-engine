#include "sxMouse_editor.h"

namespace sx { namespace io {

	//	some static shared variables between all devices
	extern HWND		g_input_win_hwnd;		//  this hold the handle of window which device attached
	extern POINT	g_input_win_size;		//  this hold the size of window which device attached

	Mouse_editor::Mouse_editor( UINT playerID ): InputDeviceBase(playerID)
	{
		GetCursorPos(&m_lastPoint);
	}

	Mouse_editor::~Mouse_editor()
	{

	}

	void Mouse_editor::Initialize( HWND windowHandle )
	{
		//g_WinHandle = windowHandle;
	}

	void Mouse_editor::Finalize( void )
	{

	}

	bool Mouse_editor::Connected( void )
	{
		return true;
	}

	const WCHAR* Mouse_editor::GetName( void )
	{
		return L"Windows Mouse";
	}

	const WCHAR* Mouse_editor::GetDesc( void )
	{
		return L"This device use windows generic mouse to return state of cursor and buttons of input";
	}

	const DWORD Mouse_editor::GetCapabilities( void )
	{
		return SX_INPUT_CAPS_CURSOR;
	}

	void Mouse_editor::Update( float elapsTime )
	{
		//  update cursor position
		POINT p;
		GetCursorPos(&p);

		//  get window rect
		if (g_input_win_hwnd)
		{
			WINDOWINFO winfo;
			GetWindowInfo(g_input_win_hwnd, &winfo);
			p.x -= winfo.rcClient.left;
			p.y -= winfo.rcClient.top;

			Input::GetCursor_RL(m_playerID)->x = (float)p.x - (float)m_lastPoint.x;
			Input::GetCursor_RL(m_playerID)->y = (float)p.y - (float)m_lastPoint.y;
		
			Input::GetCursor_ABS(m_playerID)->x = (float)p.x;
			Input::GetCursor_ABS(m_playerID)->y = (float)p.y;
		}
		
		m_lastPoint = p;
	}

	void Mouse_editor::OnSignal( int playerID, InputSignalType idSignal, void* data )
	{
		if (playerID != (int)m_playerID) return;

		if (idSignal == IST_SET_CURSOR && data)
		{
			Input_Signal_Cursor* curpos = (Input_Signal_Cursor*)data;			
			SetCursorPos((int)curpos->x, (int)curpos->y);
		}
	}

}} // namespace sx { namespace io {