#include "sxMouse.h"
#include "../sxSystem/sxLog.h"

namespace sx { namespace io {

	//	some static shared variables between all devices
	extern HWND		g_input_win_hwnd;		//  this hold the handle of window which device attached
	extern POINT	g_input_win_size;		//  this hold the size of window which device attached


	Mouse::Mouse( UINT playerID ): InputDeviceBase(playerID), m_pMouseInput(NULL), m_speed(1.0f)
	{

	}

	Mouse::~Mouse()
	{

	}

	void Mouse::Initialize( HWND windowHandle )
	{
		//  release last device
		Finalize();

		LPDIRECTINPUT8 l_pInput8 = NULL;

		if (FAILED(DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&l_pInput8, NULL)))
			goto e_exit;

		if (FAILED(l_pInput8->CreateDevice(GUID_SysMouse, &m_pMouseInput, NULL)))
			goto e_exit;

		if (FAILED(m_pMouseInput->SetDataFormat(&c_dfDIMouse)))
			Finalize();

		if (FAILED(m_pMouseInput->SetCooperativeLevel(windowHandle, DISCL_NONEXCLUSIVE|DISCL_BACKGROUND)))
			Finalize();

		if (m_pMouseInput)
		{
			HRESULT res = E_FAIL;
			while ((res != DI_OK && res != S_FALSE) || res == DIERR_OTHERAPPHASPRIO)
			{
				res = m_pMouseInput->Acquire();
			}

			sxLog::Log(L"Input device 'Mouse' initialized successfully.");
		}

e_exit:
		SEGAN_RELEASE_AND_NULL(l_pInput8);
	}

	void Mouse::Finalize( void )
	{
		if (m_pMouseInput)
		{
			m_pMouseInput->Unacquire();
			SEGAN_RELEASE_AND_NULL(m_pMouseInput);
		}
	}

	bool Mouse::Connected( void )
	{
		if (!m_pMouseInput) return false;

		static DIDEVCAPS caps;
		caps.dwSize = sizeof(DIDEVCAPS);
		m_pMouseInput->GetCapabilities(&caps);
		return (caps.dwFlags & DIDC_ATTACHED);
	}

	const WCHAR* Mouse::GetName( void )
	{
		return L"Direct Input Mouse";
	}

	const WCHAR* Mouse::GetDesc( void )
	{
		return L"Direct Input Mouse which most computers have";
	}

	const DWORD Mouse::GetCapabilities( void )
	{
		return SX_INPUT_CAPS_CURSOR;
	}

	void Mouse::Update( float elapsTime )
	{
		if (!m_pMouseInput) return;
		
		//  get device state
		DIMOUSESTATE mouseState;
		HRESULT res = m_pMouseInput->GetDeviceState(sizeof(mouseState), (LPVOID)&mouseState);

		if (res == DIERR_INPUTLOST || res == DIERR_NOTACQUIRED)
		{
			sxLog::Log(L"Input device 'Mouse' has been lost! Now I'm trying to reinitialize it.");
			Initialize(g_input_win_hwnd);
			if (m_pMouseInput) m_pMouseInput->GetDeviceState(sizeof(mouseState), (LPVOID)&mouseState);
		}

		//  update cursor relative data
		Input::GetCursor_RL(m_playerID)->x = m_speed * (float)mouseState.lX;
		Input::GetCursor_RL(m_playerID)->y = m_speed * (float)mouseState.lY;

		//	update cursor absolute data
		Input::GetCursor_ABS(m_playerID)->x += m_speed * (float)mouseState.lX;
		Input::GetCursor_ABS(m_playerID)->y += m_speed * (float)mouseState.lY;

		//  update input keys
		char* iKeys = Input::GetButtons(m_playerID)->Keys;

		//  update mouse wheel data
		if (!mouseState.lZ)
			iKeys[SX_INPUT_KEY_MOUSE_WHEEL] = 0;
		else if (mouseState.lZ>0)
			iKeys[SX_INPUT_KEY_MOUSE_WHEEL] = 1;
		else
			iKeys[SX_INPUT_KEY_MOUSE_WHEEL] = -1;

		//  update mouse buttons
		int keyIndex;
		for (int i=0; i<3; i++)
		{
			keyIndex = i + SX_INPUT_KEY_MOUSE_LEFT;
			if (mouseState.rgbButtons[i])
			{
				if (iKeys[keyIndex] == SX_INPUT_STATE_NORMAL)
					iKeys[keyIndex] = SX_INPUT_STATE_DOWN;
				else
					iKeys[keyIndex] = SX_INPUT_STATE_HOLD;
			}
			else
			{
				if (iKeys[keyIndex] == SX_INPUT_STATE_DOWN || iKeys[keyIndex] == SX_INPUT_STATE_HOLD)
					iKeys[keyIndex] = SX_INPUT_STATE_UP;
				else
					iKeys[keyIndex] = SX_INPUT_STATE_NORMAL;
			}
		}
	}

	void Mouse::OnSignal( int playerID, InputSignalType idSignal, void* data )
	{
		if (playerID != (int)m_playerID) return;

		switch ( idSignal )
		{
		case IST_SET_CURSOR:
			if ( data )
			{
				Input_Signal_Cursor* curpos = static_cast<Input_Signal_Cursor*>(data);

				Input::GetCursor_ABS(m_playerID)->x = curpos->x;
				Input::GetCursor_ABS(m_playerID)->y = curpos->y;
			}
			return;

		case IST_SET_SPEED:
			if ( data )
			{
				m_speed = *((float*)data);
			}
			return;
		}
	}


}} // namespace sx { namespace io {