/********************************************************************
	created:	2010/11/17
	filename: 	sxInputClass.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a main static class of Input system
				for the SeganX engine. any other input devices will
				only control data of this class. this class will convert
				data for use in engine.
*********************************************************************/
#ifndef GUARD_sxInputClass_HEADER_FILE
#define GUARD_sxInputClass_HEADER_FILE

#include "sxInput_def.h"
#include "sxTypesInput.h"

#define SEGAN_KEYDOWN(playerID, key)	(sx::io::Input::GetKeys(playerID)[key]==SX_INPUT_STATE_DOWN)
#define SEGAN_KEYHOLD(playerID, key)	(sx::io::Input::GetKeys(playerID)[key]==SX_INPUT_STATE_HOLD)
#define SEGAN_KEYUP(playerID, key)		(sx::io::Input::GetKeys(playerID)[key]==SX_INPUT_STATE_UP)
#define SEGAN_MOUSE_RLX(playerID)		(static_cast<float>(sx::io::Input::GetCursor_RL(playerID)->x))
#define SEGAN_MOUSE_RLY(playerID)		(static_cast<float>(sx::io::Input::GetCursor_RL(playerID)->y))
#define SEGAN_MOUSE_ABSX(playerID)		(static_cast<float>(sx::io::Input::GetCursor_ABS(playerID)->x))
#define SEGAN_MOUSE_ABSY(playerID)		(static_cast<float>(sx::io::Input::GetCursor_ABS(playerID)->y))

//! contain structures and an static class for input devices.
namespace sx { namespace io {

/*!
this is the base of other input devices. other input device classes should inherit from this class.
to handle signals for more that one device in same type (2 joystick) the application responsible to attach more
Input Devices to Input class with different playerID. ( each player need his separate devices )
*/
typedef class InputDeviceBase
{

public:
	InputDeviceBase(UINT playerID);
	virtual ~InputDeviceBase();

	//! initialize this device
	virtual void Initialize(HWND windowHandle) = 0;

	//! finalize this device
	virtual void Finalize(void) = 0;
	
	//! verify that input device is initialized and connected
	virtual bool Connected(void) = 0;

	//! get the name of the input device
	virtual const WCHAR* GetName(void) = 0;

	//! get description of the input device
	virtual const WCHAR* GetDesc(void) = 0;

	//! return device's capabilities. cursor, shock, LCD, LED, etc
	virtual const DWORD GetCapabilities(void) = 0;

	//! update input device
	virtual void Update(float elapsTime) = 0;

	/*!
	this may call by engine in game to send signal to shock, vibration, etc. 
	use playerID to specify which player should handle this signal.
	*/
	virtual void OnSignal(int playerID, IST_ InputSignalType idSignal, void* data) = 0;

protected:
	UINT	m_playerID;		//  the player ID of this device
}
*PInputDeviceBase;


//////////////////////////////////////////////////////////////////////////
//! use this class to catch the user input and also send signal to other devices ( shock, vibration, LED, LCD, etc )
class SEGAN_API Input
{
	SEGAN_STERILE_CLASS(Input);
public:
	//! attach a device to the input class
	static void Attach(PInputDeviceBase inputDevice);

	//! get number of devices currently attached to engine
	static int GetDeviceCount(void);

	//! get device by index. return null if the index was out of range
	static PInputDeviceBase GetDevice(int index);

	//! return the index and pointer of input device by name. return -1 of no match name
	static int GetDevice(const WCHAR* pdeviceName, OUT PInputDeviceBase& pinputDevice);

	//! initialize devices
	static void Initialize(HWND windowHandle);

	//! finalize devices
	static void Finalize(void);

	/*!
	this may call by engine in game to send signal to shock, vibration, etc to all other devices
	use playerID to specify which player should handle this signal.
	*/
	static void SendSignal(int playerID, IST_ InputSignalType idSignal, void* data);

	//! update input devices
	static void Update(float elpsTime);

	//! get relative values of cursor structure
	static Input_State_Cursor*	GetCursor_RL(int playerID);

	//! get absolute values of cursor structure
	static Input_State_Cursor*	GetCursor_ABS(int playerID);

	//! get analog 1 structure
	static Input_State_Analog*	GetAnalog_1(int playerID);

	//! get analog 2 structure
	static Input_State_Analog*	GetAnalog_2(int playerID);

	//! get buttons structure
	static Input_State_Buttons*	GetButtons(int playerID);

	//! return array of keys
	static const char*			GetKeys(int playerID);
};


}} // namespace sx { namespace io

#endif	//	GUARD_sxInputClass_HEADER_FILE