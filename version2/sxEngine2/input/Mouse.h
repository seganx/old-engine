/********************************************************************
	created:	2013/03/11
	filename: 	Mouse.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain an input device for mouse on window
*********************************************************************/
#ifndef GUARD_Mouse_HEADER_FILE
#define GUARD_Mouse_HEADER_FILE



#include "Input.h"


//! this is a mouse device 
class SEGAN_ENG_API Mouse : public InputDevice
{
	SEGAN_STERILE_CLASS(Mouse);
public:

	Mouse( uint playerID );
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

	float					m_speed;
};

#endif	//	GUARD_Mouse_HEADER_FILE

