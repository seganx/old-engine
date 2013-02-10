/********************************************************************
	created:	2010/11/18
	filename: 	sxMouse.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Mouse object for input device that
				use direct input to get state of the Mouse
*********************************************************************/
#ifndef GUARD_sxMouse_HEADER_FILE
#define GUARD_sxMouse_HEADER_FILE


#include "sxInputClass.h"

//! contain structures and an static class for input devices.
namespace sx { namespace io {

	//! this is a mouse device 
	class SEGAN_API Mouse : public InputDeviceBase
	{
		SEGAN_STERILE_CLASS(Mouse);
	public:
		Mouse(UINT playerID);
		~Mouse();

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
		LPDIRECTINPUTDEVICE8	m_pMouseInput;	// direct Input Device
		float					m_speed;
	};

}} // namespace sx { namespace io {

#endif	//	GUARD_sxMouse_HEADER_FILE