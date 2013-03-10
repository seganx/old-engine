/********************************************************************
	created:	2010/11/17
	filename: 	sxMouse_editor.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a mouse object for input device that
				use default windows mouse cursor to synchronize engine's
				cursor with editor's cursor in editor mode.
*********************************************************************/
#ifndef GUARD_sxMouse_editor_HEADER_FILE
#define GUARD_sxMouse_editor_HEADER_FILE

#include "sxInputClass.h"

//! contain structures and an static class for input devices.
namespace sx { namespace io {

	//! this is an editor mouse device used in editor mode
	class SEGAN_API Mouse_editor : public InputDeviceBase
	{
		SEGAN_STERILE_CLASS(Mouse_editor);
	public:
		Mouse_editor(UINT playerID);
		~Mouse_editor();

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
		POINT	m_lastPoint;	//  used to calculate relative values
	};

}} // namespace sx { namespace io {

#endif	//	GUARD_sxMouse_editor_HEADER_FILE