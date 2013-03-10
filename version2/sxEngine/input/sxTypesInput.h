/********************************************************************
	created:	2010/11/18
	filename: 	sxInputTypes.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain 
*********************************************************************/
#ifndef GUARD_sxInputTypes_HEADER_FILE
#define GUARD_sxInputTypes_HEADER_FILE

#include "sxInput_def.h"

//! maximum number of players can use this input system
#define SX_INPUT_PLAYER_MAXID	4

//! input device capabilities
#define SX_INPUT_CAPS_CURSOR	0x00000001
#define SX_INPUT_CAPS_SHOCK		0x00000002
#define SX_INPUT_CAPS_LCD		0x00000004
#define SX_INPUT_CAPS_LED		0x00000008

//! input buttons state
#define SX_INPUT_STATE_DOUBLE	-2
#define SX_INPUT_STATE_UP		-1
#define SX_INPUT_STATE_NORMAL	0
#define SX_INPUT_STATE_DOWN		1
#define SX_INPUT_STATE_HOLD		2


//! signals that will received by input devices
enum InputSignalType
{
	IST_SET_CURSOR,			//  this will comes by a vector to set device's cursor
	IST_SET_SIZE,			//  change input box size. data will be a pointer to int2
	IST_SET_SPEED,			//	change input speed. data will be a pointer to float
	IST_SHOCK,				//  this will comes by specified data to shock the input device
	IST_TEXT,				//  this will comes by specified text data to set device's LCD screen text
	IST_LED					//  this will comes by specified data to set device's LED on/off
};
#define IST_

//! structure of Cursor used in Input class
struct Input_State_Cursor
{
	float x;
	float y;
};

//! structure of Analog used in Input class
struct Input_State_Analog
{
	bool	state;	//	false=up / true=down
	float	x;		//  relative signed value of x
	float	y;		//  relative signed value of y
};

//! structure of Buttons (Keyboard/Mouse/Joystick/...) used in Input class
struct Input_State_Buttons
{
	/*
	value:0 normal / value:1 down value:2 still down/ value:-1 up. key up lives 1 frame and after that the value will became 0
	NOTE: Keys[SX_INPUT_KEY_MOUSE_WHEEL] contain wheel data in range [-1..0..1]
	*/
	char	Keys[256];
};

//! signal structure for set cursor
struct Input_Signal_Cursor {
	float x;
	float y;
};

//! signal structure for shock/vibrate joystick
struct Input_Signal_Vibration {
	WORD wLeftMotorSpeed;
	WORD wRightMotorSpeed;
};
typedef Input_Signal_Vibration SID_Shock;

//! signal structure for set text on LCD
struct Input_Signal_Text {
	DWORD	wTextOption;
	char	cString[256];
};

//! signal structure for set LED
struct Input_Signal_LED {
	WORD	wIndex;
	WORD	wOption;	
};

//////////////////////////////////////////////////////////////////////////
// INPUT KEYBOARD SCAN CODES
//////////////////////////////////////////////////////////////////////////
#define SX_INPUT_KEY_ESCAPE          0x01
#define SX_INPUT_KEY_1               0x02
#define SX_INPUT_KEY_2               0x03
#define SX_INPUT_KEY_3               0x04
#define SX_INPUT_KEY_4               0x05
#define SX_INPUT_KEY_5               0x06
#define SX_INPUT_KEY_6               0x07
#define SX_INPUT_KEY_7               0x08
#define SX_INPUT_KEY_8               0x09
#define SX_INPUT_KEY_9               0x0A
#define SX_INPUT_KEY_0               0x0B
#define SX_INPUT_KEY_MINUS           0x0C    /* - on main keyboard */
#define SX_INPUT_KEY_EQUALS          0x0D
#define SX_INPUT_KEY_BACK            0x0E    /* backspace */
#define SX_INPUT_KEY_TAB             0x0F
#define SX_INPUT_KEY_Q               0x10
#define SX_INPUT_KEY_W               0x11
#define SX_INPUT_KEY_E               0x12
#define SX_INPUT_KEY_R               0x13
#define SX_INPUT_KEY_T               0x14
#define SX_INPUT_KEY_Y               0x15
#define SX_INPUT_KEY_U               0x16
#define SX_INPUT_KEY_I               0x17
#define SX_INPUT_KEY_O               0x18
#define SX_INPUT_KEY_P               0x19
#define SX_INPUT_KEY_LBRACKET        0x1A
#define SX_INPUT_KEY_RBRACKET        0x1B
#define SX_INPUT_KEY_RETURN          0x1C    /* Enter on main keyboard */
#define SX_INPUT_KEY_LCONTROL        0x1D
#define SX_INPUT_KEY_A               0x1E
#define SX_INPUT_KEY_S               0x1F
#define SX_INPUT_KEY_D               0x20
#define SX_INPUT_KEY_F               0x21
#define SX_INPUT_KEY_G               0x22
#define SX_INPUT_KEY_H               0x23
#define SX_INPUT_KEY_J               0x24
#define SX_INPUT_KEY_K               0x25
#define SX_INPUT_KEY_L               0x26
#define SX_INPUT_KEY_SEMICOLON       0x27
#define SX_INPUT_KEY_APOSTROPHE      0x28
#define SX_INPUT_KEY_GRAVE           0x29    /* accent grave */
#define SX_INPUT_KEY_LSHIFT          0x2A
#define SX_INPUT_KEY_BACKSLASH       0x2B
#define SX_INPUT_KEY_Z               0x2C
#define SX_INPUT_KEY_X               0x2D
#define SX_INPUT_KEY_C               0x2E
#define SX_INPUT_KEY_V               0x2F
#define SX_INPUT_KEY_B               0x30
#define SX_INPUT_KEY_N               0x31
#define SX_INPUT_KEY_M               0x32
#define SX_INPUT_KEY_COMMA           0x33
#define SX_INPUT_KEY_PERIOD          0x34    /* . on main keyboard */
#define SX_INPUT_KEY_SLASH           0x35    /* / on main keyboard */
#define SX_INPUT_KEY_RSHIFT          0x36
#define SX_INPUT_KEY_MULTIPLY        0x37    /* * on numeric keypad */
#define SX_INPUT_KEY_LMENU           0x38    /* left Alt */
#define SX_INPUT_KEY_SPACE           0x39
#define SX_INPUT_KEY_CAPITAL         0x3A
#define SX_INPUT_KEY_F1              0x3B
#define SX_INPUT_KEY_F2              0x3C
#define SX_INPUT_KEY_F3              0x3D
#define SX_INPUT_KEY_F4              0x3E
#define SX_INPUT_KEY_F5              0x3F
#define SX_INPUT_KEY_F6              0x40
#define SX_INPUT_KEY_F7              0x41
#define SX_INPUT_KEY_F8              0x42
#define SX_INPUT_KEY_F9              0x43
#define SX_INPUT_KEY_F10             0x44
#define SX_INPUT_KEY_NUMLOCK         0x45
#define SX_INPUT_KEY_SCROLL          0x46    /* Scroll Lock */
#define SX_INPUT_KEY_NUMPAD7         0x47
#define SX_INPUT_KEY_NUMPAD8         0x48
#define SX_INPUT_KEY_NUMPAD9         0x49
#define SX_INPUT_KEY_SUBTRACT        0x4A    /* - on numeric keypad */
#define SX_INPUT_KEY_NUMPAD4         0x4B
#define SX_INPUT_KEY_NUMPAD5         0x4C
#define SX_INPUT_KEY_NUMPAD6         0x4D
#define SX_INPUT_KEY_ADD             0x4E    /* + on numeric keypad */
#define SX_INPUT_KEY_NUMPAD1         0x4F
#define SX_INPUT_KEY_NUMPAD2         0x50
#define SX_INPUT_KEY_NUMPAD3         0x51
#define SX_INPUT_KEY_NUMPAD0         0x52
#define SX_INPUT_KEY_DECIMAL         0x53    /* . on numeric keypad */
#define SX_INPUT_KEY_OEM_102         0x56    /* <> or \| on RT 102-key keyboard (Non-U.S.) */
#define SX_INPUT_KEY_F11             0x57
#define SX_INPUT_KEY_F12             0x58
#define SX_INPUT_KEY_F13             0x64    /*                     (NEC PC98) */
#define SX_INPUT_KEY_F14             0x65    /*                     (NEC PC98) */
#define SX_INPUT_KEY_F15             0x66    /*                     (NEC PC98) */
#define SX_INPUT_KEY_KANA            0x70    /* (Japanese keyboard)            */
#define SX_INPUT_KEY_ABNT_C1         0x73    /* /? on Brazilian keyboard */
#define SX_INPUT_KEY_CONVERT         0x79    /* (Japanese keyboard)            */
#define SX_INPUT_KEY_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define SX_INPUT_KEY_YEN             0x7D    /* (Japanese keyboard)            */
#define SX_INPUT_KEY_ABNT_C2         0x7E    /* Numpad . on Brazilian keyboard */
#define SX_INPUT_KEY_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define SX_INPUT_KEY_PREVTRACK       0x90    /* Previous Track (SX_INPUT_KEY_CIRCUMFLEX on Japanese keyboard) */
#define SX_INPUT_KEY_AT              0x91    /*                     (NEC PC98) */
#define SX_INPUT_KEY_COLON           0x92    /*                     (NEC PC98) */
#define SX_INPUT_KEY_UNDERLINE       0x93    /*                     (NEC PC98) */
#define SX_INPUT_KEY_KANJI           0x94    /* (Japanese keyboard)            */
#define SX_INPUT_KEY_STOP            0x95    /*                     (NEC PC98) */
#define SX_INPUT_KEY_AX              0x96    /*                     (Japan AX) */
#define SX_INPUT_KEY_UNLABELED       0x97    /*                        (J3100) */
#define SX_INPUT_KEY_NEXTTRACK       0x99    /* Next Track */
#define SX_INPUT_KEY_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define SX_INPUT_KEY_RCONTROL        0x9D
#define SX_INPUT_KEY_MUTE            0xA0    /* Mute */
#define SX_INPUT_KEY_CALCULATOR      0xA1    /* Calculator */
#define SX_INPUT_KEY_PLAYPAUSE       0xA2    /* Play / Pause */
#define SX_INPUT_KEY_MEDIASTOP       0xA4    /* Media Stop */
#define SX_INPUT_KEY_VOLUMEDOWN      0xAE    /* Volume - */
#define SX_INPUT_KEY_VOLUMEUP        0xB0    /* Volume + */
#define SX_INPUT_KEY_WEBHOME         0xB2    /* Web home */
#define SX_INPUT_KEY_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define SX_INPUT_KEY_DIVIDE          0xB5    /* / on numeric keypad */
#define SX_INPUT_KEY_SYSRQ           0xB7
#define SX_INPUT_KEY_RMENU           0xB8    /* right Alt */
#define SX_INPUT_KEY_PAUSE           0xC5    /* Pause */
#define SX_INPUT_KEY_HOME            0xC7    /* Home on arrow keypad */
#define SX_INPUT_KEY_UP              0xC8    /* UpArrow on arrow keypad */
#define SX_INPUT_KEY_PRIOR           0xC9    /* PgUp on arrow keypad */
#define SX_INPUT_KEY_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define SX_INPUT_KEY_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define SX_INPUT_KEY_END             0xCF    /* End on arrow keypad */
#define SX_INPUT_KEY_DOWN            0xD0    /* DownArrow on arrow keypad */
#define SX_INPUT_KEY_NEXT            0xD1    /* PgDn on arrow keypad */
#define SX_INPUT_KEY_INSERT          0xD2    /* Insert on arrow keypad */
#define SX_INPUT_KEY_DELETE          0xD3    /* Delete on arrow keypad */
#define SX_INPUT_KEY_LWIN            0xDB    /* Left Windows key */
#define SX_INPUT_KEY_RWIN            0xDC    /* Right Windows key */
#define SX_INPUT_KEY_APPS            0xDD    /* AppMenu key */
#define SX_INPUT_KEY_POWER           0xDE    /* System Power */
#define SX_INPUT_KEY_SLEEP           0xDF    /* System Sleep */
#define SX_INPUT_KEY_WAKE            0xE3    /* System Wake */
#define SX_INPUT_KEY_WEBSEARCH       0xE5    /* Web Search */
#define SX_INPUT_KEY_WEBFAVORITES    0xE6    /* Web Favorites */
#define SX_INPUT_KEY_WEBREFRESH      0xE7    /* Web Refresh */
#define SX_INPUT_KEY_WEBSTOP         0xE8    /* Web Stop */
#define SX_INPUT_KEY_WEBFORWARD      0xE9    /* Web Forward */
#define SX_INPUT_KEY_WEBBACK         0xEA    /* Web Back */
#define SX_INPUT_KEY_MYCOMPUTER      0xEB    /* My Computer */
#define SX_INPUT_KEY_MAIL			0xEC    /* Mail */
#define SX_INPUT_KEY_MEDIASELECT		0xED    /* Media Select */


#define SX_INPUT_KEY_MOUSE_LEFT		0xEE    /* Mouse left button */
#define SX_INPUT_KEY_MOUSE_RIGHT		0xEF    /* Mouse right button */
#define SX_INPUT_KEY_MOUSE_MIDDLE	0xF0    /* Mouse middle button */
#define SX_INPUT_KEY_MOUSE_WHEEL		0xF1    /* Mouse wheel value */

#define SX_INPUT_KEY_BACKSPACE       SX_INPUT_KEY_BACK            /* backspace */
#define SX_INPUT_KEY_NUMPADSTAR      SX_INPUT_KEY_MULTIPLY        /* * on numeric keypad */
#define SX_INPUT_KEY_LALT            SX_INPUT_KEY_LMENU           /* left Alt */
#define SX_INPUT_KEY_CAPSLOCK        SX_INPUT_KEY_CAPITAL         /* CapsLock */
#define SX_INPUT_KEY_NUMPADMINUS     SX_INPUT_KEY_SUBTRACT        /* - on numeric keypad */
#define SX_INPUT_KEY_NUMPADPLUS      SX_INPUT_KEY_ADD             /* + on numeric keypad */
#define SX_INPUT_KEY_NUMPADPERIOD    SX_INPUT_KEY_DECIMAL         /* . on numeric keypad */
#define SX_INPUT_KEY_NUMPADSLASH     SX_INPUT_KEY_DIVIDE          /* / on numeric keypad */
#define SX_INPUT_KEY_RALT            SX_INPUT_KEY_RMENU           /* right Alt */
#define SX_INPUT_KEY_UPARROW         SX_INPUT_KEY_UP              /* UpArrow on arrow keypad */
#define SX_INPUT_KEY_PAGEUP          SX_INPUT_KEY_PRIOR           /* PgUp on arrow keypad */
#define SX_INPUT_KEY_LEFTARROW       SX_INPUT_KEY_LEFT            /* LeftArrow on arrow keypad */
#define SX_INPUT_KEY_RIGHTARROW      SX_INPUT_KEY_RIGHT           /* RightArrow on arrow keypad */
#define SX_INPUT_KEY_DOWNARROW       SX_INPUT_KEY_DOWN            /* DownArrow on arrow keypad */
#define SX_INPUT_KEY_PAGEDN          SX_INPUT_KEY_NEXT            /* PgDn on arrow keypad */


namespace sx { namespace io {

BYTE SEGAN_API InputButtonToVirtualKey(BYTE inputButton);
BYTE SEGAN_API InputButtonToUpperACCI(BYTE inputButton);
BYTE SEGAN_API InputButtonToLowerACCI(BYTE inputButton);
WORD SEGAN_API InputButtonToFarsiCode(BYTE inputButton);
WORD SEGAN_API InputButtonToFarsiCodeWithShift(BYTE inputButton);

}} // namespace sx { namespace io {

#endif	//	GUARD_sxInputTypes_HEADER_FILE