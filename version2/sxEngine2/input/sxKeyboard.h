/********************************************************************
	created:	2010/11/18
	filename: 	sxKeyboard.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a keyboard object for input device that
				use direct input to get state of the Keyboard
*********************************************************************/
#ifndef GUARD_sxKeyboard_HEADER_FILE
#define GUARD_sxKeyboard_HEADER_FILE

#include "sxInputClass.h"

//! contain structures and an static class for input devices.
namespace sx { namespace io {

	//! this is a keyboard device 
	class SEGAN_API Keyboard : public InputDeviceBase
	{
		SEGAN_STERILE_CLASS(Keyboard);
	public:
		Keyboard(UINT playerID);
		~Keyboard();

		//! initialize this device
		void Initialize(HWND windowHandle);

		//! finalize this device
		void Finalize(void);

		//! verify that input device is initialized and connected
		bool Connected(void);

		//! get the name of the input device
		const WCHAR* GetName(void);

		//! get description of the input device
		const WCHAR* GetDesc(void);

		//! return device's capabilities. cursor, shock, LCD, LED, etc
		const DWORD GetCapabilities(void);

		//! update input device
		void Update(float elapsTime);

		/*!
		this may call by engine in game to send signal to shock, vibration, etc. 
		use playerID to specify which player should handle this signal.
		*/
		virtual void OnSignal(int playerID, InputSignalType idSignal, void* data);

	private:
		LPDIRECTINPUTDEVICE8	m_pKeyInput;	// direct Input Device
		BYTE					m_pKeys[256];	// device state
	};

}} // namespace sx { namespace io {

#endif	//	GUARD_sxKeyboard_HEADER_FILE