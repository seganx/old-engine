#include "sxKeyboard.h"
#include "../sxSystem/sxLog.h"

namespace sx { namespace io {


	//	some static shared variables between all devices
	extern HWND		g_input_win_hwnd;		//  this hold the handle of window which device attached
	extern POINT	g_input_win_size;		//  this hold the size of window which device attached


	Keyboard::Keyboard( UINT playerID ): InputDeviceBase(playerID), m_pKeyInput(NULL)
	{
		ZeroMemory( m_pKeys, sizeof(m_pKeys) );
	}

	Keyboard::~Keyboard()
	{

	}

	void Keyboard::Initialize( HWND windowHandle )
	{
		//  release last device
		Finalize();

		LPDIRECTINPUT8 l_pInput8 = NULL;
		
		if (FAILED(DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&l_pInput8, NULL)))
			goto e_exit;

		if (FAILED(l_pInput8->CreateDevice(GUID_SysKeyboard, &m_pKeyInput, NULL)))
			goto e_exit;

		if (FAILED(m_pKeyInput->SetDataFormat(&c_dfDIKeyboard)))
			Finalize();

		if (FAILED(m_pKeyInput->SetCooperativeLevel(windowHandle, DISCL_NONEXCLUSIVE|DISCL_BACKGROUND)))
			Finalize();

		if (m_pKeyInput)
		{
			HRESULT res = E_FAIL;
			while ((res != DI_OK && res != S_FALSE) || res == DIERR_OTHERAPPHASPRIO)
			{
				res = m_pKeyInput->Acquire();
			}

			sxLog::Log( L"Input device 'Keyboard' initialized successfully." );
		}

e_exit:
		SEGAN_RELEASE_AND_NULL(l_pInput8);
	}

	void Keyboard::Finalize( void )
	{
		if (m_pKeyInput)
		{
			m_pKeyInput->Unacquire();
			SEGAN_RELEASE_AND_NULL(m_pKeyInput);
		}
	}

	bool Keyboard::Connected( void )
	{
		if (!m_pKeyInput) return false;

		static DIDEVCAPS caps;
		m_pKeyInput->GetCapabilities(&caps);
		return !(caps.dwFlags & DIDC_ATTACHED);
	}

	const WCHAR* Keyboard::GetName( void )
	{
		return L"Direct Input Keyboard";
	}

	const WCHAR* Keyboard::GetDesc( void )
	{
		return L"Direct Input Keyboard which most computers have";
	}

	const DWORD Keyboard::GetCapabilities( void )
	{
		return 0;
	}

	void Keyboard::Update( float elapsTime )
	{
		if (!m_pKeyInput) return;

		//  get device state
		HRESULT res = m_pKeyInput->GetDeviceState(sizeof(m_pKeys), (LPVOID)m_pKeys);

		if (res == DIERR_INPUTLOST || res == DIERR_NOTACQUIRED)
		{
			sxLog::Log(L"Input device 'keyboard' has been lost! Now I'm trying to reinitialize it.");
			Initialize(g_input_win_hwnd);
			if (m_pKeyInput) m_pKeyInput->GetDeviceState(sizeof(m_pKeys), (LPVOID)m_pKeys);
		}

		//  compare the changes
		char* iKeys = Input::GetButtons(m_playerID)->Keys;
		for (int i=1; i<0xED; i++)
		{
			if (m_pKeys[i])
			{
				if (iKeys[i] == SX_INPUT_STATE_NORMAL)
					iKeys[i] = SX_INPUT_STATE_DOWN;
				else
					iKeys[i] = SX_INPUT_STATE_HOLD;
			}
			else
			{
				if (iKeys[i] == SX_INPUT_STATE_DOWN || iKeys[i] == SX_INPUT_STATE_HOLD)
					iKeys[i] = SX_INPUT_STATE_UP;
				else
					iKeys[i] = SX_INPUT_STATE_NORMAL;
			}
		}
	}

	void Keyboard::OnSignal( int playerID, InputSignalType idSignal, void* data )
	{
		return;
	}


}} // namespace sx { namespace io {