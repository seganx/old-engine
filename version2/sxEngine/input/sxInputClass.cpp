#include "sxInputClass.h"
#include "../sxSystem/sxLog.h"

namespace sx { namespace io {

	typedef struct sxInputState
	{
		Input_State_Cursor		m_Cursor_RL[SX_INPUT_PLAYER_MAXID];		//  relative values of cursor
		Input_State_Cursor		m_Cursor_ABS[SX_INPUT_PLAYER_MAXID];	//  absolute values of cursor
		Input_State_Analog		m_Analog_1[SX_INPUT_PLAYER_MAXID];		//	values of analog 1 in joystick
		Input_State_Analog		m_Analog_2[SX_INPUT_PLAYER_MAXID];		//  values of analog 2 in joystick
		Input_State_Buttons		m_Buttons[SX_INPUT_PLAYER_MAXID];		//  states of buttons
	} 
	*PsxInputState;

	//  init static members
	static Array<PInputDeviceBase>	g_aInputDevices;
	static sxInputState						g_InputState;

	//	some static shared variables between all devices
	extern HWND		g_input_win_hwnd = NULL;		//  this hold the handle of window which device attached
	extern POINT	g_input_win_size = {0, 0};		//  this hold the size of input rectangle


	//////////////////////////////////////////////////////////////////////////
	//	INPUT DEVICE BASE
	//////////////////////////////////////////////////////////////////////////
	InputDeviceBase::InputDeviceBase( UINT playerID )
	{
		m_playerID = playerID;
	}

	InputDeviceBase::~InputDeviceBase()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	//	INPUT DEVICE MANAGER
	//////////////////////////////////////////////////////////////////////////
	void Input::Attach( PInputDeviceBase inputDevice )
	{
		g_aInputDevices.PushBack(inputDevice);
	}

	int Input::GetDeviceCount( void )
	{
		return g_aInputDevices.Count();
	}

	sx::io::PInputDeviceBase Input::GetDevice( int index )
	{
		if (index<0 || index>=g_aInputDevices.Count()) return NULL;

		return g_aInputDevices[index];
	}

	int Input::GetDevice( const WCHAR* pdeviceName, OUT PInputDeviceBase& pinputDevice )
	{
		String dname = pdeviceName;
		for (int i=0; i<g_aInputDevices.Count(); i++)
		{
			if (dname == g_aInputDevices[i]->GetName())
			{
				pinputDevice = g_aInputDevices[i];
				return i;
			}
		}
		return -1;
	}

	void Input::Initialize( HWND windowHandle )
	{
		ZeroMemory(&g_InputState, sizeof(sxInputState));

		WINDOWINFO winfo;
		GetWindowInfo(windowHandle, &winfo);
		g_input_win_size.x = winfo.rcClient.right - winfo.rcClient.left;
		g_input_win_size.y = winfo.rcClient.bottom  - winfo.rcClient.top;

		for (int i=0; i<g_aInputDevices.Count(); i++)
		{
			g_aInputDevices[i]->Initialize(windowHandle);
		}

		g_input_win_hwnd = windowHandle;
	}

	void Input::Finalize( void )
	{
		sxLog::Log(L"Finalizing [ %d ] input device(s).\r\n", (int)g_aInputDevices.Count());
		for (int i=0; i<g_aInputDevices.Count(); i++)
		{
			g_aInputDevices[i]->Finalize();
		}
		for (int i=0; i<g_aInputDevices.Count(); i++)
		{
			sx_delete(g_aInputDevices[i]);
		}
		g_aInputDevices.Clear();
	}

	void Input::SendSignal( int playerID, InputSignalType idSignal, void* data )
	{
		sx_callstack();

		switch (idSignal)
		{
		case IST_SET_SIZE:
			{
				POINT* sz = (POINT*)data;
				g_input_win_size.x = sz->x;
				g_input_win_size.y = sz->y;
			}
			break;
		}

		for (int i=0; i<g_aInputDevices.Count(); i++)
			g_aInputDevices[i]->OnSignal(playerID, idSignal, data);
	}

	void Input::Update( float elpsTime )
	{
		sx_callstack();

		if (GetForegroundWindow() != g_input_win_hwnd)
			return;

		for (int i=0; i<g_aInputDevices.Count(); i++)
			g_aInputDevices[i]->Update(elpsTime);

		//  guard the cursor sizes
		for (int i=0; i<SX_INPUT_PLAYER_MAXID; i++)
		{
			SEGAN_CLAMP(g_InputState.m_Cursor_ABS[i].x, 0, (float)g_input_win_size.x);
			SEGAN_CLAMP(g_InputState.m_Cursor_ABS[i].y, 0, (float)g_input_win_size.y);
		}
	}

	FORCEINLINE Input_State_Cursor* Input::GetCursor_RL( int playerID )
	{
		return &g_InputState.m_Cursor_RL[playerID];
	}

	FORCEINLINE Input_State_Cursor* Input::GetCursor_ABS( int playerID )
	{
		return &g_InputState.m_Cursor_ABS[playerID];
	}

	FORCEINLINE Input_State_Analog* Input::GetAnalog_1( int playerID )
	{
		return &g_InputState.m_Analog_1[playerID];
	}

	FORCEINLINE Input_State_Analog* Input::GetAnalog_2( int playerID )
	{
		return &g_InputState.m_Analog_2[playerID];
	}

	FORCEINLINE Input_State_Buttons* Input::GetButtons( int playerID )
	{
		return &g_InputState.m_Buttons[playerID];
	}

	FORCEINLINE const char* Input::GetKeys( int playerID )
	{
		return g_InputState.m_Buttons[playerID].Keys;
	}
}}  //  namespace sx { namespace io {