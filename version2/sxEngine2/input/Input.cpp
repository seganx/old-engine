#include "Input.h"

//////////////////////////////////////////////////////////////////////////
//	InputDevice implementation
//////////////////////////////////////////////////////////////////////////
InputDevice::InputDevice( uint playerID ): m_playerID(0) {}
InputDevice::~InputDevice() {}


//////////////////////////////////////////////////////////////////////////
//	Input implementation
//////////////////////////////////////////////////////////////////////////
Input::Input( void )
{
	sx_mem_set( m_values, 0, sizeof(m_values) );
	sx_mem_set( m_keys, 0, sizeof(m_keys) );
}

Input::~Input( void )
{

}

void Input::Attach( const InputDevice* device )
{
	if ( !device ) return;
	InputDevice* dev = (InputDevice*)device;
	m_devices.push_back( dev );
	dev->m_owner = this;
}

uint Input::GetDeviceCount( void ) const
{
	return m_devices.m_count;
}

InputDevice* Input::GetDevice( sint index )
{
	if ( index < 0 || index >= m_devices.m_count ) return null;
	return m_devices[index];
}

void Input::SendSignal( InputSignalType idSignal, void* data, const uint playerID /*= 0 */ )
{
	for ( int i=0; i<m_devices.m_count; ++i )
	{
		m_devices[i]->OnSignal( idSignal, data, playerID );
	}
}

void Input::Update( float elpsTime )
{
	for ( int i=0; i<m_devices.m_count; ++i )
	{
		m_devices[i]->Update( elpsTime );
	}
}

SEGAN_INLINE InputValues* Input::GetValues( const uint playerID /*= 0 */ )
{
	return &m_values[ playerID ];
}

SEGAN_INLINE InputKeys* Input::GetKeys( const uint playerID /*= 0 */ )
{
	return &m_keys[ playerID ];
}

SEGAN_INLINE bool Input::KeyDown( const InputKey key, const uint playerID /*= 0 */ )
{
	return ( m_keys[playerID].keys[key] == IS_DOWN );
}

SEGAN_INLINE bool Input::KeyHold( const InputKey key, const uint playerID /*= 0 */ )
{
	return ( m_keys[playerID].keys[key] == IS_HOLD );
}

SEGAN_INLINE bool Input::KeyUp( const InputKey key, const uint playerID /*= 0 */ )
{
	return ( m_keys[playerID].keys[key] == IS_UP );
}

SEGAN_INLINE bool Input::KeyDouble( const InputKey key, const uint playerID /*= 0 */ )
{
	return ( m_keys[playerID].keys[key] == IS_DOUBLE );
}



//////////////////////////////////////////////////////////////////////////
//	UTILITY FUNCTIONS
//////////////////////////////////////////////////////////////////////////
byte sx_io_get_acci( InputKey inputButton )
{
	switch ( inputButton )
	{
	case IK_Q        			: return 'q'	;
	case IK_W        			: return 'w'	;
	case IK_E        			: return 'e'	;
	case IK_R        			: return 'r'	;
	case IK_T        			: return 't'	;
	case IK_Y        			: return 'y'	;
	case IK_U        			: return 'u'	;
	case IK_I        			: return 'i'	;
	case IK_O        			: return 'o'	;
	case IK_P        			: return 'p'	;
	case IK_A        			: return 'a'	;
	case IK_S        			: return 's'	;
	case IK_D        			: return 'd'	;
	case IK_F        			: return 'f'	;
	case IK_G        			: return 'g'	;
	case IK_H        			: return 'h'	;
	case IK_J        			: return 'j'	;
	case IK_K        			: return 'k'	;
	case IK_L        			: return 'l'	;
	case IK_Z        			: return 'z'	;
	case IK_X        			: return 'x'	;
	case IK_C        			: return 'c'	;
	case IK_V        			: return 'v'	;
	case IK_B        			: return 'b'	;
	case IK_N        			: return 'n'	;
	case IK_M        			: return 'm'	;
	case IK_0					: return '0'	;
	case IK_NUMPAD0				: return '0'	;
	case IK_1					: return '1'	;
	case IK_NUMPAD1				: return '1'	;
	case IK_2					: return '2'	;
	case IK_NUMPAD2				: return '2'	;
	case IK_3					: return '3'	;
	case IK_NUMPAD3				: return '3'	;
	case IK_4					: return '4'	;
	case IK_NUMPAD4				: return '4'	;
	case IK_5					: return '5'	;
	case IK_NUMPAD5				: return '5'	;
	case IK_6					: return '6'	;
	case IK_NUMPAD6				: return '6'	;
	case IK_7					: return '7'	;
	case IK_NUMPAD7				: return '7'	;
	case IK_8					: return '8'	;
	case IK_NUMPAD8				: return '8'	;
	case IK_9					: return '9'	;
	case IK_NUMPAD9				: return '9'	;
	case IK_MINUS				: return '-'	;
	case IK_EQUALS				: return '='	;
	case IK_LBRACKET			: return '['	;
	case IK_RBRACKET			: return ']'	;
	case IK_SEMICOLON			: return ';'	;
	case IK_APOSTROPHE			: return '\''	;
	case IK_BACKSLASH			: return '\\'	;
	case IK_COMMA				: return ','	;
	case IK_PERIOD				: return '.'	;
	case IK_DECIMAL				: return '.'	;
	case IK_SLASH				: return '/'	;
	case IK_MULTIPLY			: return '*'	;
	case IK_SPACE				: return ' '	;
	case IK_SUBTRACT			: return '-'	;
	case IK_ADD					: return '+'	;
	case IK_NUMPADEQUALS		: return '='	;
	case IK_AT					: return '@'	;
	case IK_COLON				: return ':'	;
	case IK_UNDERLINE			: return '_'	;
	case IK_NUMPADCOMMA			: return ','	;
	case IK_DIVIDE				: return '/'	;
	case IK_GRAVE				: return '`'	;
	case IK_NUMPADENTER 		: return 0x0D	;
	case IK_RETURN      		: return 0x0D	;
	case IK_BACK        		: return 0x08	;
	default						: return 0		;	
	}

	return 0;
}

byte sx_io_get_acci_with_shift( InputKey inputButton )
{

	switch ( inputButton )
	{
	case IK_Q				: return 'Q'	;
	case IK_W				: return 'W'	;
	case IK_E				: return 'E'	;
	case IK_R				: return 'R'	;
	case IK_T				: return 'T'	;
	case IK_Y				: return 'Y'	;
	case IK_U				: return 'U'	;
	case IK_I				: return 'I'	;
	case IK_O				: return 'O'	;
	case IK_P				: return 'P'	;
	case IK_A				: return 'A'	;
	case IK_S				: return 'S'	;
	case IK_D				: return 'D'	;
	case IK_F				: return 'F'	;
	case IK_G				: return 'G'	;
	case IK_H				: return 'H'	;
	case IK_J				: return 'J'	;
	case IK_K				: return 'K'	;
	case IK_L				: return 'L'	;
	case IK_Z				: return 'Z'	;
	case IK_X				: return 'X'	;
	case IK_C				: return 'C'	;
	case IK_V				: return 'V'	;
	case IK_B				: return 'B'	;
	case IK_N				: return 'N'	;
	case IK_M				: return 'M'	;
	case IK_0				: return ')'	;
	case IK_NUMPAD0			: return ')'	;
	case IK_1				: return '!'	;
	case IK_NUMPAD1			: return '!'	;
	case IK_2				: return '@'	;
	case IK_NUMPAD2			: return '@'	;
	case IK_3				: return '#'	;
	case IK_NUMPAD3			: return '#'	;
	case IK_4				: return '$'	;
	case IK_NUMPAD4			: return '$'	;
	case IK_5				: return '%'	;
	case IK_NUMPAD5			: return '%'	;
	case IK_6				: return '^'	;
	case IK_NUMPAD6			: return '^'	;
	case IK_7				: return '&'	;
	case IK_NUMPAD7			: return '&'	;
	case IK_8				: return '*'	;
	case IK_NUMPAD8			: return '*'	;
	case IK_9				: return '('	;
	case IK_NUMPAD9			: return '('	;
	case IK_MINUS			: return '_'	;
	case IK_EQUALS			: return '+'	;
	case IK_LBRACKET 		: return '{'	;
	case IK_RBRACKET 		: return '}'	;
	case IK_SEMICOLON		: return ':'	;
	case IK_APOSTROPHE  	: return '"'	;
	case IK_BACKSLASH   	: return '|'	;
	case IK_COMMA       	: return '<'	;
	case IK_PERIOD      	: return '>'	;
	case IK_SLASH       	: return '?'	;
	case IK_MULTIPLY    	: return '*'	;
	case IK_SPACE       	: return ' '	;
	case IK_SUBTRACT    	: return '-'	;
	case IK_ADD         	: return '+'	;
	case IK_NUMPADEQUALS	: return '='	;
	case IK_AT          	: return '@'	;
	case IK_COLON       	: return ':'	;
	case IK_UNDERLINE   	: return '_'	;
	case IK_NUMPADCOMMA 	: return ','	;
	case IK_DIVIDE      	: return '/'	;
	case IK_GRAVE			: return '~'	;
	case IK_NUMPADENTER 	: return 0x0D	;
	case IK_RETURN      	: return 0x0D	;
	case IK_BACK        	: return 0x08	;
	default					: return 0		;	
	}

	return 0;
}

word sx_io_get_farsi( InputKey inputButton )
{
	switch ( inputButton )
	{
	case IK_Q          			: return 1590	;
	case IK_W          			: return 1589	;
	case IK_E          			: return 1579	;
	case IK_R          			: return 1602	;
	case IK_T          			: return 1601	;
	case IK_Y          			: return 1594	;
	case IK_U          			: return 1593	;
	case IK_I          			: return 1607	;
	case IK_O          			: return 1582	;
	case IK_P          			: return 1581	;
	case IK_A          			: return 1588	;
	case IK_S          			: return 1587	;
	case IK_D          			: return 1740	;
	case IK_F          			: return 1576	;
	case IK_G          			: return 1604	;
	case IK_H          			: return 1575	;
	case IK_J          			: return 1578	;
	case IK_K          			: return 1606	;
	case IK_L          			: return 1605	;
	case IK_Z          			: return 1592	;
	case IK_X          			: return 1591	;
	case IK_C          			: return 1586	;
	case IK_V          			: return 1585	;
	case IK_B          			: return 1584	;
	case IK_N          			: return 1583	;
	case IK_M          			: return 1574	;
	case IK_0					: return 1632	;
	case IK_NUMPAD0				: return 1632	;
	case IK_1					: return 1633	;
	case IK_NUMPAD1				: return 1633	;
	case IK_2					: return 1634	;
	case IK_NUMPAD2				: return 1634	;
	case IK_3					: return 1635	;
	case IK_NUMPAD3				: return 1635	;
	case IK_4					: return 1636	;
	case IK_NUMPAD4				: return 1636	;
	case IK_5					: return 1637	;
	case IK_NUMPAD5				: return 1637	;
	case IK_6					: return 1638	;
	case IK_NUMPAD6				: return 1638	;
	case IK_7					: return 1639	;
	case IK_NUMPAD7				: return 1639	;
	case IK_8					: return 1640	;
	case IK_NUMPAD8				: return 1640	;
	case IK_9					: return 1641	;
	case IK_NUMPAD9				: return 1641	;
	case IK_MINUS				: return '-'	;
	case IK_EQUALS				: return '='	;
	case IK_LBRACKET    		: return 1580	;
	case IK_RBRACKET    		: return 1670	;
	case IK_SEMICOLON   		: return 1705	;
	case IK_APOSTROPHE  		: return 1711	;
	case IK_BACKSLASH   		: return 1688	;
	case IK_COMMA       		: return 1608	;
	case IK_PERIOD      		: return '.'	;
	case IK_DECIMAL				: return '.'	;
	case IK_SLASH       		: return '/'	;
	case IK_MULTIPLY    		: return '*'	;
	case IK_SPACE       		: return ' '	;
	case IK_SUBTRACT    		: return '-'	;
	case IK_ADD         		: return '+'	;
	case IK_NUMPADEQUALS		: return '='	;
	case IK_AT          		: return '@'	;
	case IK_COLON       		: return ':'	;
	case IK_UNDERLINE   		: return '_'	;
	case IK_NUMPADCOMMA 		: return ','	;
	case IK_DIVIDE      		: return '/'	;
	case IK_GRAVE				: return 1662	;
	case IK_NUMPADENTER 		: return 0x0D	;
	case IK_RETURN      		: return 0x0D	;
	case IK_BACK        		: return 0x08	;
	default						: return 0		;	
	}

	return 0;
}

word sx_io_get_farsi_with_shift( InputKey inputButton )
{
	switch ( inputButton )
	{
	case IK_Q				: return 1590	;
	case IK_W				: return 1589	;
	case IK_E				: return 1579	;
	case IK_R				: return 1602	;
	case IK_T				: return 1601	;
	case IK_Y				: return 1594	;
	case IK_U				: return 1593	;
	case IK_I				: return ']'	;
	case IK_O				: return '['	;
	case IK_P				: return '\\'	;
	case IK_A				: return 1588	;
	case IK_S				: return 1587	;
	case IK_D				: return 1740	;
	case IK_F				: return 1576	;
	case IK_G				: return 1604	;
	case IK_H				: return 1570	;
	case IK_J				: return 1578	;
	case IK_K				: return '<'	;
	case IK_L				: return '>'	;
	case IK_Z				: return 1592	;
	case IK_X				: return 1591	;
	case IK_C				: return 1688	;
	case IK_V				: return 1585	;
	case IK_B				: return 1584	;
	case IK_N				: return 1583	;
	case IK_M				: return 1569	;
	case IK_0				: return 1632	;
	case IK_NUMPAD0			: return 1632	;
	case IK_1				: return 1633	;
	case IK_NUMPAD1			: return 1633	;
	case IK_2				: return 1634	;
	case IK_NUMPAD2			: return 1634	;
	case IK_3				: return 1635	;
	case IK_NUMPAD3			: return 1635	;
	case IK_4				: return 1636	;
	case IK_NUMPAD4			: return 1636	;
	case IK_5				: return 1637	;
	case IK_NUMPAD5			: return 1637	;
	case IK_6				: return 1638	;
	case IK_NUMPAD6			: return 1638	;
	case IK_7				: return 1639	;
	case IK_NUMPAD7			: return 1639	;
	case IK_8				: return 1640	;
	case IK_NUMPAD8			: return 1640	;
	case IK_9				: return 1641	;
	case IK_NUMPAD9			: return 1641	;
	case IK_MINUS			: return '-'	;
	case IK_EQUALS			: return '='	;
	case IK_LBRACKET     	: return '{'	;
	case IK_RBRACKET     	: return '}'	;
	case IK_SEMICOLON    	: return ':'	;
	case IK_APOSTROPHE   	: return '"'	;
	case IK_BACKSLASH    	: return '|'	;
	case IK_COMMA        	: return '<'	;
	case IK_PERIOD       	: return '>'	;
	case IK_SLASH        	: return '?'	;
	case IK_MULTIPLY     	: return '*'	;
	case IK_SPACE        	: return ' '	;
	case IK_SUBTRACT     	: return '-'	;
	case IK_ADD          	: return '+'	;
	case IK_NUMPADEQUALS 	: return '='	;
	case IK_AT           	: return '@'	;
	case IK_COLON        	: return ':'	;
	case IK_UNDERLINE    	: return '_'	;
	case IK_NUMPADCOMMA  	: return ','	;
	case IK_DECIMAL			: return '.'	;
	case IK_DIVIDE			: return '/'	;
	case IK_GRAVE			: return '~'	;
	case IK_NUMPADENTER 	: return 0x0D	;
	case IK_RETURN      	: return 0x0D	;
	case IK_BACK        	: return 0x08	;
	default					: return 0		;	
	}

	return 0;
}


