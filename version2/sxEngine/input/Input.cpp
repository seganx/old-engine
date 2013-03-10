#include "Input.h"


byte sx_io_key_to_virtualkey( byte inputButton )
{
	switch ( inputButton )
	{
	case SX_IO_KEY_SPACE			: return VK_SPACE	;
	case SX_IO_KEY_BACK				: return VK_BACK	;
	case SX_IO_KEY_DELETE			: return VK_DELETE	;
	case SX_IO_KEY_RETURN			: return VK_RETURN	;
	case SX_IO_KEY_NUMPADENTER		: return VK_RETURN	;
	case SX_IO_KEY_UP				: return VK_UP		;
	case SX_IO_KEY_LEFT				: return VK_LEFT	;
	case SX_IO_KEY_RIGHT			: return VK_RIGHT	;
	case SX_IO_KEY_DOWN				: return VK_DOWN	;
	case SX_IO_KEY_ESCAPE			: return VK_ESCAPE	;
	case SX_IO_KEY_TAB				: return VK_TAB		;
	case SX_IO_KEY_MULTIPLY			: return VK_MULTIPLY;
	case SX_IO_KEY_CAPITAL			: return VK_CAPITAL	;
	case SX_IO_KEY_F1				: return VK_F1		;
	case SX_IO_KEY_F2				: return VK_F2		;
	case SX_IO_KEY_F3				: return VK_F3		;
	case SX_IO_KEY_F4				: return VK_F4		;
	case SX_IO_KEY_F5				: return VK_F5		;
	case SX_IO_KEY_F6				: return VK_F6		;
	case SX_IO_KEY_F7				: return VK_F7		;
	case SX_IO_KEY_F8				: return VK_F8		;
	case SX_IO_KEY_F9				: return VK_F9		;
	case SX_IO_KEY_F10				: return VK_F10		;
	case SX_IO_KEY_NUMLOCK			: return VK_NUMLOCK	;
	case SX_IO_KEY_SCROLL			: return VK_SCROLL	;
	case SX_IO_KEY_NUMPAD7      	: return VK_NUMPAD7	;
	case SX_IO_KEY_NUMPAD8      	: return VK_NUMPAD8	;
	case SX_IO_KEY_NUMPAD9      	: return VK_NUMPAD9	;
	case SX_IO_KEY_SUBTRACT     	: return VK_SUBTRACT;
	case SX_IO_KEY_NUMPAD4      	: return VK_NUMPAD4	;
	case SX_IO_KEY_NUMPAD5      	: return VK_NUMPAD5	;
	case SX_IO_KEY_NUMPAD6      	: return VK_NUMPAD6	;
	case SX_IO_KEY_ADD          	: return VK_ADD		;
	case SX_IO_KEY_NUMPAD1      	: return VK_NUMPAD1	;
	case SX_IO_KEY_NUMPAD2      	: return VK_NUMPAD2	;
	case SX_IO_KEY_NUMPAD3      	: return VK_NUMPAD3	;
	case SX_IO_KEY_NUMPAD0      	: return VK_NUMPAD0	;
	case SX_IO_KEY_DECIMAL      	: return VK_DECIMAL	;
	case SX_IO_KEY_OEM_102      	: return VK_OEM_102	;
	case SX_IO_KEY_F11          	: return VK_F11		;
	case SX_IO_KEY_F12          	: return VK_F12		;
	case SX_IO_KEY_F13          	: return VK_F13		;
	case SX_IO_KEY_F14          	: return VK_F14		;
	case SX_IO_KEY_F15          	: return VK_F15		;
	case SX_IO_KEY_DIVIDE       	: return VK_DIVIDE	;
	case SX_IO_KEY_PAUSE        	: return VK_PAUSE	;
	case SX_IO_KEY_END          	: return VK_END		;
	case SX_IO_KEY_HOME         	: return VK_HOME	;
	case SX_IO_KEY_PRIOR        	: return VK_PRIOR	;
	case SX_IO_KEY_NEXT         	: return VK_NEXT	;
	case SX_IO_KEY_INSERT       	: return VK_INSERT	;
	case SX_IO_KEY_APPS				: return VK_APPS	;
	case SX_IO_KEY_SLEEP			: return VK_SLEEP	;
	case SX_IO_KEY_LSHIFT			: return VK_SHIFT	;
	case SX_IO_KEY_RSHIFT			: return VK_SHIFT	;
	case SX_IO_KEY_LALT				: return VK_MENU	;
	case SX_IO_KEY_RALT				: return VK_MENU	;
	default							: return 0;
	}
}

byte sx_io_virtualkey_to_key( byte inputButton )
{
	switch ( inputButton )
	{
	case VK_SPACE		: return SX_IO_KEY_SPACE		;
	case VK_BACK		: return SX_IO_KEY_BACK			;
	case VK_DELETE		: return SX_IO_KEY_DELETE		;
	case VK_RETURN		: return SX_IO_KEY_RETURN		;
	case VK_RETURN		: return SX_IO_KEY_NUMPADENTER	;
	case VK_UP			: return SX_IO_KEY_UP			;
	case VK_LEFT		: return SX_IO_KEY_LEFT			;
	case VK_RIGHT		: return SX_IO_KEY_RIGHT		;
	case VK_DOWN		: return SX_IO_KEY_DOWN			;
	case VK_ESCAPE		: return SX_IO_KEY_ESCAPE		;
	case VK_TAB			: return SX_IO_KEY_TAB			;
	case VK_MULTIPLY	: return SX_IO_KEY_MULTIPLY		;
	case VK_CAPITAL		: return SX_IO_KEY_CAPITAL		;
	case VK_F1			: return SX_IO_KEY_F1			;
	case VK_F2			: return SX_IO_KEY_F2			;
	case VK_F3			: return SX_IO_KEY_F3			;
	case VK_F4			: return SX_IO_KEY_F4			;
	case VK_F5			: return SX_IO_KEY_F5			;
	case VK_F6			: return SX_IO_KEY_F6			;
	case VK_F7			: return SX_IO_KEY_F7			;
	case VK_F8			: return SX_IO_KEY_F8			;
	case VK_F9			: return SX_IO_KEY_F9			;
	case VK_F10			: return SX_IO_KEY_F10			;
	case VK_NUMLOCK		: return SX_IO_KEY_NUMLOCK		;
	case VK_SCROLL		: return SX_IO_KEY_SCROLL		;
	case VK_NUMPAD7		: return SX_IO_KEY_NUMPAD7      ;
	case VK_NUMPAD8		: return SX_IO_KEY_NUMPAD8      ;
	case VK_NUMPAD9		: return SX_IO_KEY_NUMPAD9      ;
	case VK_SUBTRACT	: return SX_IO_KEY_SUBTRACT     ;
	case VK_NUMPAD4		: return SX_IO_KEY_NUMPAD4      ;
	case VK_NUMPAD5		: return SX_IO_KEY_NUMPAD5      ;
	case VK_NUMPAD6		: return SX_IO_KEY_NUMPAD6      ;
	case VK_ADD			: return SX_IO_KEY_ADD          ;
	case VK_NUMPAD1		: return SX_IO_KEY_NUMPAD1      ;
	case VK_NUMPAD2		: return SX_IO_KEY_NUMPAD2      ;
	case VK_NUMPAD3		: return SX_IO_KEY_NUMPAD3      ;
	case VK_NUMPAD0		: return SX_IO_KEY_NUMPAD0      ;
	case VK_DECIMAL		: return SX_IO_KEY_DECIMAL      ;
	case VK_OEM_102		: return SX_IO_KEY_OEM_102      ;
	case VK_F11			: return SX_IO_KEY_F11          ;
	case VK_F12			: return SX_IO_KEY_F12          ;
	case VK_F13			: return SX_IO_KEY_F13          ;
	case VK_F14			: return SX_IO_KEY_F14          ;
	case VK_F15			: return SX_IO_KEY_F15          ;
	case VK_DIVIDE		: return SX_IO_KEY_DIVIDE       ;
	case VK_PAUSE		: return SX_IO_KEY_PAUSE        ;
	case VK_END			: return SX_IO_KEY_END          ;
	case VK_HOME		: return SX_IO_KEY_HOME         ;
	case VK_PRIOR		: return SX_IO_KEY_PRIOR        ;
	case VK_NEXT		: return SX_IO_KEY_NEXT         ;
	case VK_INSERT		: return SX_IO_KEY_INSERT       ;
	case VK_APPS		: return SX_IO_KEY_APPS			;
	case VK_SLEEP		: return SX_IO_KEY_SLEEP		;
	case VK_SHIFT		: return SX_IO_KEY_LSHIFT		;
	case VK_SHIFT		: return SX_IO_KEY_RSHIFT		;
	case VK_MENU		: return SX_IO_KEY_LALT			;
	case VK_MENU		: return SX_IO_KEY_RALT			;
	default				: return 0;
	}
}

byte sx_io_get_acci( byte inputButton )
{
	switch ( inputButton )
	{
	case SX_IO_KEY_Q        	: return 'q'		;
	case SX_IO_KEY_W        	: return 'w'		;
	case SX_IO_KEY_E        	: return 'e'		;
	case SX_IO_KEY_R        	: return 'r'		;
	case SX_IO_KEY_T        	: return 't'		;
	case SX_IO_KEY_Y        	: return 'y'		;
	case SX_IO_KEY_U        	: return 'u'		;
	case SX_IO_KEY_I        	: return 'i'		;
	case SX_IO_KEY_O        	: return 'o'		;
	case SX_IO_KEY_P        	: return 'p'		;
	case SX_IO_KEY_A        	: return 'a'		;
	case SX_IO_KEY_S        	: return 's'		;
	case SX_IO_KEY_D        	: return 'd'		;
	case SX_IO_KEY_F        	: return 'f'		;
	case SX_IO_KEY_G        	: return 'g'		;
	case SX_IO_KEY_H        	: return 'h'		;
	case SX_IO_KEY_J        	: return 'j'		;
	case SX_IO_KEY_K        	: return 'k'		;
	case SX_IO_KEY_L        	: return 'l'		;
	case SX_IO_KEY_Z        	: return 'z'		;
	case SX_IO_KEY_X        	: return 'x'		;
	case SX_IO_KEY_C        	: return 'c'		;
	case SX_IO_KEY_V        	: return 'v'		;
	case SX_IO_KEY_B        	: return 'b'		;
	case SX_IO_KEY_N        	: return 'n'		;
	case SX_IO_KEY_M        	: return 'm'		;
	case SX_IO_KEY_0			: return '0'		;
	case SX_IO_KEY_NUMPAD0		: return '0'		;
	case SX_IO_KEY_1			: return '1'		;
	case SX_IO_KEY_NUMPAD1		: return '1'		;
	case SX_IO_KEY_2			: return '2'		;
	case SX_IO_KEY_NUMPAD2		: return '2'		;
	case SX_IO_KEY_3			: return '3'		;
	case SX_IO_KEY_NUMPAD3		: return '3'		;
	case SX_IO_KEY_4			: return '4'		;
	case SX_IO_KEY_NUMPAD4		: return '4'		;
	case SX_IO_KEY_5			: return '5'		;
	case SX_IO_KEY_NUMPAD5		: return '5'		;
	case SX_IO_KEY_6			: return '6'		;
	case SX_IO_KEY_NUMPAD6		: return '6'		;
	case SX_IO_KEY_7			: return '7'		;
	case SX_IO_KEY_NUMPAD7		: return '7'		;
	case SX_IO_KEY_8			: return '8'		;
	case SX_IO_KEY_NUMPAD8		: return '8'		;
	case SX_IO_KEY_9			: return '9'		;
	case SX_IO_KEY_NUMPAD9		: return '9'		;
	case SX_IO_KEY_MINUS		: return '-'		;
	case SX_IO_KEY_EQUALS		: return '='		;
	case SX_IO_KEY_LBRACKET		: return '['		;
	case SX_IO_KEY_RBRACKET		: return ']'		;
	case SX_IO_KEY_SEMICOLON	: return ';'		;
	case SX_IO_KEY_APOSTROPHE	: return '\''		;
	case SX_IO_KEY_BACKSLASH	: return '\\'		;
	case SX_IO_KEY_COMMA		: return ','		;
	case SX_IO_KEY_PERIOD		: return '.'		;
	case SX_IO_KEY_NUMPADPERIOD	: return '.'		;
	case SX_IO_KEY_SLASH		: return '/'		;
	case SX_IO_KEY_MULTIPLY		: return '*'		;
	case SX_IO_KEY_SPACE		: return ' '		;
	case SX_IO_KEY_SUBTRACT		: return '-'		;
	case SX_IO_KEY_ADD			: return '+'		;
	case SX_IO_KEY_NUMPADEQUALS	: return '='		;
	case SX_IO_KEY_AT			: return '@'		;
	case SX_IO_KEY_COLON		: return ':'		;
	case SX_IO_KEY_UNDERLINE	: return '_'		;
	case SX_IO_KEY_NUMPADCOMMA	: return ','		;
	case SX_IO_KEY_DIVIDE		: return '/'		;
	case SX_IO_KEY_GRAVE		: return '`'		;
	case SX_IO_KEY_NUMPADENTER  : return VK_RETURN	;
	case SX_IO_KEY_RETURN       : return VK_RETURN	;
	case SX_IO_KEY_BACK         : return VK_BACK	;
	default						: return 0			;	
	}

	return 0;
}

byte sx_io_get_acci_with_shift( byte inputButton )
{

	switch ( inputButton )
	{
	case SX_IO_KEY_Q			: return 'Q'		;
	case SX_IO_KEY_W			: return 'W'		;
	case SX_IO_KEY_E			: return 'E'		;
	case SX_IO_KEY_R			: return 'R'		;
	case SX_IO_KEY_T			: return 'T'		;
	case SX_IO_KEY_Y			: return 'Y'		;
	case SX_IO_KEY_U			: return 'U'		;
	case SX_IO_KEY_I			: return 'I'		;
	case SX_IO_KEY_O			: return 'O'		;
	case SX_IO_KEY_P			: return 'P'		;
	case SX_IO_KEY_A			: return 'A'		;
	case SX_IO_KEY_S			: return 'S'		;
	case SX_IO_KEY_D			: return 'D'		;
	case SX_IO_KEY_F			: return 'F'		;
	case SX_IO_KEY_G			: return 'G'		;
	case SX_IO_KEY_H			: return 'H'		;
	case SX_IO_KEY_J			: return 'J'		;
	case SX_IO_KEY_K			: return 'K'		;
	case SX_IO_KEY_L			: return 'L'		;
	case SX_IO_KEY_Z			: return 'Z'		;
	case SX_IO_KEY_X			: return 'X'		;
	case SX_IO_KEY_C			: return 'C'		;
	case SX_IO_KEY_V			: return 'V'		;
	case SX_IO_KEY_B			: return 'B'		;
	case SX_IO_KEY_N			: return 'N'		;
	case SX_IO_KEY_M			: return 'M'		;
	case SX_IO_KEY_0			: return ')'		;
	case SX_IO_KEY_NUMPAD0		: return ')'		;
	case SX_IO_KEY_1			: return '!'		;
	case SX_IO_KEY_NUMPAD1		: return '!'		;
	case SX_IO_KEY_2			: return '@'		;
	case SX_IO_KEY_NUMPAD2		: return '@'		;
	case SX_IO_KEY_3			: return '#'		;
	case SX_IO_KEY_NUMPAD3		: return '#'		;
	case SX_IO_KEY_4			: return '$'		;
	case SX_IO_KEY_NUMPAD4		: return '$'		;
	case SX_IO_KEY_5			: return '%'		;
	case SX_IO_KEY_NUMPAD5		: return '%'		;
	case SX_IO_KEY_6			: return '^'		;
	case SX_IO_KEY_NUMPAD6		: return '^'		;
	case SX_IO_KEY_7			: return '&'		;
	case SX_IO_KEY_NUMPAD7		: return '&'		;
	case SX_IO_KEY_8			: return '*'		;
	case SX_IO_KEY_NUMPAD8		: return '*'		;
	case SX_IO_KEY_9			: return '('		;
	case SX_IO_KEY_NUMPAD9		: return '('		;
	case SX_IO_KEY_MINUS		: return '_'		;
	case SX_IO_KEY_EQUALS		: return '+'		;
	case SX_IO_KEY_LBRACKET 	: return '{'		;
	case SX_IO_KEY_RBRACKET 	: return '}'		;
	case SX_IO_KEY_SEMICOLON	: return ':'		;
	case SX_IO_KEY_APOSTROPHE  	: return '"'		;
	case SX_IO_KEY_BACKSLASH   	: return '|'		;
	case SX_IO_KEY_COMMA       	: return '<'		;
	case SX_IO_KEY_PERIOD      	: return '>'		;
	case SX_IO_KEY_SLASH       	: return '?'		;
	case SX_IO_KEY_MULTIPLY    	: return '*'		;
	case SX_IO_KEY_SPACE       	: return ' '		;
	case SX_IO_KEY_SUBTRACT    	: return '-'		;
	case SX_IO_KEY_ADD         	: return '+'		;
	case SX_IO_KEY_NUMPADEQUALS	: return '='		;
	case SX_IO_KEY_AT          	: return '@'		;
	case SX_IO_KEY_COLON       	: return ':'		;
	case SX_IO_KEY_UNDERLINE   	: return '_'		;
	case SX_IO_KEY_NUMPADCOMMA 	: return ','		;
	case SX_IO_KEY_DIVIDE      	: return '/'		;
	case SX_IO_KEY_GRAVE		: return '~'		;
	case SX_IO_KEY_NUMPADENTER 	: return VK_RETURN	;
	case SX_IO_KEY_RETURN      	: return VK_RETURN	;
	case SX_IO_KEY_BACK        	: return VK_BACK	;
	default						: return 0;	
	}

	return 0;
}

word sx_io_get_farsi( byte inputButton )
{
	switch ( inputButton )
	{
	case SX_IO_KEY_Q          	: return 1590		;
	case SX_IO_KEY_W          	: return 1589		;
	case SX_IO_KEY_E          	: return 1579		;
	case SX_IO_KEY_R          	: return 1602		;
	case SX_IO_KEY_T          	: return 1601		;
	case SX_IO_KEY_Y          	: return 1594		;
	case SX_IO_KEY_U          	: return 1593		;
	case SX_IO_KEY_I          	: return 1607		;
	case SX_IO_KEY_O          	: return 1582		;
	case SX_IO_KEY_P          	: return 1581		;
	case SX_IO_KEY_A          	: return 1588		;
	case SX_IO_KEY_S          	: return 1587		;
	case SX_IO_KEY_D          	: return 1740		;
	case SX_IO_KEY_F          	: return 1576		;
	case SX_IO_KEY_G          	: return 1604		;
	case SX_IO_KEY_H          	: return 1575		;
	case SX_IO_KEY_J          	: return 1578		;
	case SX_IO_KEY_K          	: return 1606		;
	case SX_IO_KEY_L          	: return 1605		;
	case SX_IO_KEY_Z          	: return 1592		;
	case SX_IO_KEY_X          	: return 1591		;
	case SX_IO_KEY_C          	: return 1586		;
	case SX_IO_KEY_V          	: return 1585		;
	case SX_IO_KEY_B          	: return 1584		;
	case SX_IO_KEY_N          	: return 1583		;
	case SX_IO_KEY_M          	: return 1574		;
	case SX_IO_KEY_0			: return 1632		;
	case SX_IO_KEY_NUMPAD0		: return 1632		;
	case SX_IO_KEY_1			: return 1633		;
	case SX_IO_KEY_NUMPAD1		: return 1633		;
	case SX_IO_KEY_2			: return 1634		;
	case SX_IO_KEY_NUMPAD2		: return 1634		;
	case SX_IO_KEY_3			: return 1635		;
	case SX_IO_KEY_NUMPAD3		: return 1635		;
	case SX_IO_KEY_4			: return 1636		;
	case SX_IO_KEY_NUMPAD4		: return 1636		;
	case SX_IO_KEY_5			: return 1637		;
	case SX_IO_KEY_NUMPAD5		: return 1637		;
	case SX_IO_KEY_6			: return 1638		;
	case SX_IO_KEY_NUMPAD6		: return 1638		;
	case SX_IO_KEY_7			: return 1639		;
	case SX_IO_KEY_NUMPAD7		: return 1639		;
	case SX_IO_KEY_8			: return 1640		;
	case SX_IO_KEY_NUMPAD8		: return 1640		;
	case SX_IO_KEY_9			: return 1641		;
	case SX_IO_KEY_NUMPAD9		: return 1641		;
	case SX_IO_KEY_MINUS		: return '-'		;
	case SX_IO_KEY_EQUALS		: return '='		;
	case SX_IO_KEY_LBRACKET     : return 1580		;
	case SX_IO_KEY_RBRACKET     : return 1670		;
	case SX_IO_KEY_SEMICOLON    : return 1705		;
	case SX_IO_KEY_APOSTROPHE   : return 1711		;
	case SX_IO_KEY_BACKSLASH    : return 1688		;
	case SX_IO_KEY_COMMA        : return 1608		;
	case SX_IO_KEY_PERIOD       : return '.'		;
	case SX_IO_KEY_NUMPADPERIOD	: return '.'		;
	case SX_IO_KEY_SLASH        : return '/'		;
	case SX_IO_KEY_MULTIPLY     : return '*'		;
	case SX_IO_KEY_SPACE        : return ' '		;
	case SX_IO_KEY_SUBTRACT     : return '-'		;
	case SX_IO_KEY_ADD          : return '+'		;
	case SX_IO_KEY_NUMPADEQUALS : return '='		;
	case SX_IO_KEY_AT           : return '@'		;
	case SX_IO_KEY_COLON        : return ':'		;
	case SX_IO_KEY_UNDERLINE    : return '_'		;
	case SX_IO_KEY_NUMPADCOMMA  : return ','		;
	case SX_IO_KEY_DIVIDE       : return '/'		;
	case SX_IO_KEY_GRAVE		: return 1662		;
	case SX_IO_KEY_NUMPADENTER  : return VK_RETURN	;
	case SX_IO_KEY_RETURN       : return VK_RETURN	;
	case SX_IO_KEY_BACK         : return VK_BACK	;
	default						: return 0;	
	}

	return 0;
}

word sx_io_get_farsi_with_shift( byte inputButton )
{
	switch ( inputButton )
	{
	case SX_IO_KEY_Q			: return 1590		;
	case SX_IO_KEY_W			: return 1589		;
	case SX_IO_KEY_E			: return 1579		;
	case SX_IO_KEY_R			: return 1602		;
	case SX_IO_KEY_T			: return 1601		;
	case SX_IO_KEY_Y			: return 1594		;
	case SX_IO_KEY_U			: return 1593		;
	case SX_IO_KEY_I			: return ']'		;
	case SX_IO_KEY_O			: return '['		;
	case SX_IO_KEY_P			: return '\\'		;
	case SX_IO_KEY_A			: return 1588		;
	case SX_IO_KEY_S			: return 1587		;
	case SX_IO_KEY_D			: return 1740		;
	case SX_IO_KEY_F			: return 1576		;
	case SX_IO_KEY_G			: return 1604		;
	case SX_IO_KEY_H			: return 1570		;
	case SX_IO_KEY_J			: return 1578		;
	case SX_IO_KEY_K			: return '<'		;
	case SX_IO_KEY_L			: return '>'		;
	case SX_IO_KEY_Z			: return 1592		;
	case SX_IO_KEY_X			: return 1591		;
	case SX_IO_KEY_C			: return 1688		;
	case SX_IO_KEY_V			: return 1585		;
	case SX_IO_KEY_B			: return 1584		;
	case SX_IO_KEY_N			: return 1583		;
	case SX_IO_KEY_M			: return 1569		;
	case SX_IO_KEY_0			: return 1632		;
	case SX_IO_KEY_NUMPAD0		: return 1632		;
	case SX_IO_KEY_1			: return 1633		;
	case SX_IO_KEY_NUMPAD1		: return 1633		;
	case SX_IO_KEY_2			: return 1634		;
	case SX_IO_KEY_NUMPAD2		: return 1634		;
	case SX_IO_KEY_3			: return 1635		;
	case SX_IO_KEY_NUMPAD3		: return 1635		;
	case SX_IO_KEY_4			: return 1636		;
	case SX_IO_KEY_NUMPAD4		: return 1636		;
	case SX_IO_KEY_5			: return 1637		;
	case SX_IO_KEY_NUMPAD5		: return 1637		;
	case SX_IO_KEY_6			: return 1638		;
	case SX_IO_KEY_NUMPAD6		: return 1638		;
	case SX_IO_KEY_7			: return 1639		;
	case SX_IO_KEY_NUMPAD7		: return 1639		;
	case SX_IO_KEY_8			: return 1640		;
	case SX_IO_KEY_NUMPAD8		: return 1640		;
	case SX_IO_KEY_9			: return 1641		;
	case SX_IO_KEY_NUMPAD9		: return 1641		;
	case SX_IO_KEY_MINUS		: return '-'		;
	case SX_IO_KEY_EQUALS		: return '='		;
	case SX_IO_KEY_LBRACKET     : return '{'		;
	case SX_IO_KEY_RBRACKET     : return '}'		;
	case SX_IO_KEY_SEMICOLON    : return ':'		;
	case SX_IO_KEY_APOSTROPHE   : return '"'		;
	case SX_IO_KEY_BACKSLASH    : return '|'		;
	case SX_IO_KEY_COMMA        : return '<'		;
	case SX_IO_KEY_PERIOD       : return '>'		;
	case SX_IO_KEY_SLASH        : return '?'		;
	case SX_IO_KEY_MULTIPLY     : return '*'		;
	case SX_IO_KEY_SPACE        : return ' '		;
	case SX_IO_KEY_SUBTRACT     : return '-'		;
	case SX_IO_KEY_ADD          : return '+'		;
	case SX_IO_KEY_NUMPADEQUALS : return '='		;
	case SX_IO_KEY_AT           : return '@'		;
	case SX_IO_KEY_COLON        : return ':'		;
	case SX_IO_KEY_UNDERLINE    : return '_'		;
	case SX_IO_KEY_NUMPADCOMMA  : return ','		;
	case SX_IO_KEY_NUMPADPERIOD	: return '.'		;
	case SX_IO_KEY_DIVIDE		: return '/'		;
	case SX_IO_KEY_GRAVE		: return '~'		;
	case SX_IO_KEY_NUMPADENTER	: return VK_RETURN	;
	case SX_IO_KEY_RETURN		: return VK_RETURN	;
	case SX_IO_KEY_BACK			: return VK_BACK	;
	default						: return 0;	
	}

	return 0;
}

