#include "Mouse_editor.h"

#include <Windows.h>

Mouse_editor::Mouse_editor( const uint playerID ): InputDevice(playerID), m_rect( 0, 0, 800, 600 )
{
	POINT p;
	GetCursorPos( &p );
	m_last.x = p.x;
	m_last.y = p.y;
	m_button[0] = m_button[1] = IS_NORMAL;
}

Mouse_editor::~Mouse_editor()
{

}

void Mouse_editor::Initialize( void )
{

}

void Mouse_editor::Finalize( void )
{

}

const wchar* Mouse_editor::GetName( void )
{
	return L"Windows Mouse For Editor";
}

const wchar* Mouse_editor::GetDesc( void )
{
	return L"This device uses windows generic mouse to return state of cursor and buttons of input";
}

dword Mouse_editor::GetCapabilities( void )
{
	return SX_IO_CAPS_CURSOR | SX_IO_CAPS_KEYS;
}

void Mouse_editor::Update( float elapsTime )
{
	//  update cursor position
	POINT p;
	GetCursorPos(&p);

	p.x -= m_rect.x;
	p.y -= m_rect.y;

	m_owner->m_values[m_playerID].rl_x = (float)( p.x - m_last.x );
	m_owner->m_values[m_playerID].rl_y = (float)( p.y - m_last.y );
	m_last.x = p.x;
	m_last.y = p.y;

	m_owner->m_values[m_playerID].abs_x = (float)sx_clamp_i( p.x, 0, m_rect.w );
	m_owner->m_values[m_playerID].abs_y = (float)sx_clamp_i( p.y, 0, m_rect.h );

	//	update state of mouse buttons
	InputState mousebutton[3];
	mousebutton[0] = ( GetKeyState(VK_LBUTTON) & 0x80 ) ? IS_DOWN : IS_NORMAL;
	mousebutton[1] = ( GetKeyState(VK_RBUTTON) & 0x80 ) ? IS_DOWN : IS_NORMAL;
	mousebutton[2] = ( GetKeyState(VK_MBUTTON) & 0x80 ) ? IS_DOWN : IS_NORMAL;

	//  update mouse buttons
	int keyIndex;
	InputState* ikeys = m_owner->m_keys[m_playerID].keys;
	for ( int i=0; i<3; ++i )
	{
		keyIndex = i + IK_MOUSE_LEFT;
		if ( mousebutton[i] == IS_DOWN )
		{
			if (ikeys[keyIndex] == IS_NORMAL)
				ikeys[keyIndex] = IS_DOWN;
			else
				ikeys[keyIndex] = IS_HOLD;
		}
		else
		{
			if (ikeys[keyIndex] == IS_DOWN || ikeys[keyIndex] == IS_HOLD)
				ikeys[keyIndex] = IS_UP;
			else
				ikeys[keyIndex] = IS_NORMAL;
		}
	}
}

void Mouse_editor::OnSignal( const InputSignalType idSignal, void* data, const uint playerID /*= 0 */ )
{
	if ( playerID != m_playerID ) return;

	switch ( idSignal )
	{
	case IST_SET_OS_CURSOR:
		if ( data )
		{
			int2* pos = (int2*)data;			
			SetCursorPos( pos->x, pos->y );
		}
		break;

	case IST_SET_RECT:
		if ( data )
		{
			int4* rect = (int4*)data;
			m_rect = *rect;
		}
		break;
	}
}

