/********************************************************************
	created:	2013/03/11
	filename: 	Mouse_editor.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a mouse object for input device that
				use default windows mouse cursor to synchronize engine's
				cursor with editor's cursor in editor mode.
*********************************************************************/
#ifndef GUARD_Mouse_editor_HEADER_FILE
#define GUARD_Mouse_editor_HEADER_FILE

#include "Input.h"

//! this is an editor mouse device used in editor mode
class SEGAN_ENG_API Mouse_editor : public InputDevice
{
	SEGAN_STERILE_CLASS(Mouse_editor);

public:

	Mouse_editor( uint playerID );
	virtual ~Mouse_editor( void );

	//! initialize this device
	virtual void Initialize( void );

	//! finalize this device
	virtual void Finalize( void );

	//! get the name of the input device
	virtual const wchar* GetName( void );

	//! get description of the input device
	virtual const wchar* GetDesc( void );

	//! return device's capabilities. cursor, shock, LCD, LED, etc
	virtual dword GetCapabilities( void );

	//! update input device
	virtual void Update( float elapsTime );

	/*!
	this may call by engine in game to send signal to shock, vibration, etc. 
	use playerID to specify which player should handle this signal.
	*/
	virtual void OnSignal( const InputSignalType idSignal, void* data, const uint playerID = 0 );

private:

	int4		m_rect;			//	used to define bounds
	int2		m_last;			//  used to calculate relative values
	InputState	m_button[2];	//	state of mouse buttons
};

#endif	//	GUARD_Mouse_editor_HEADER_FILE

