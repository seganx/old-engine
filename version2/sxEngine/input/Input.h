/********************************************************************
	created:	2013/03/10
	filename: 	Input.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain functions and classes of input system
*********************************************************************/
#ifndef GUARD_Input_HEADER_FILE
#define GUARD_Input_HEADER_FILE

#include "../../sxLib/Lib.h"

//! maximum number of players can use this input system
#define SX_IO_PLAYER_MAXID			4


//! input device capabilities
#define SX_IO_CAPS_CURSOR			0x00000001
#define SX_IO_CAPS_KEYS				0x00000002


//////////////////////////////////////////////////////////////////////////
// INPUT KEYBOARD SCAN CODES
//////////////////////////////////////////////////////////////////////////
enum InputKey
{
	IK_ESCAPE			=	1,
	IK_1           		=  	2,
	IK_2           		=  	3,
	IK_3           		=  	4,
	IK_4           		=  	5,
	IK_5           		=  	6,
	IK_6           		=  	7,
	IK_7           		=  	8,
	IK_8           		=  	9,
	IK_9           		=  	10,
	IK_0           		= 	11,
	IK_MINUS       		=   12,						/* - on main keyboard */
	IK_EQUALS      		=   13,
	IK_BACK        		=   14,						/* backspace */
	IK_TAB         		=   15,
	IK_Q           		=   16,
	IK_W           		=   17,
	IK_E           		=   18,
	IK_R           		=   19,
	IK_T           		=   20,
	IK_Y           		=   21,
	IK_U           		=   22,
	IK_I           		=   23,
	IK_O           		=   24,
	IK_P           		=   25,
	IK_LBRACKET    		=   26,
	IK_RBRACKET    		=   27,
	IK_RETURN      		=   28,  					/* Enter on main keyboard */
	IK_LCONTROL    		=   29,
	IK_A           		=   30,
	IK_S           		=   31,
	IK_D           		=   32,
	IK_F           		=   33,
	IK_G           		=   34,
	IK_H           		=   35,
	IK_J           		=   36,
	IK_K           		=   37,
	IK_L           		=   38,
	IK_SEMICOLON   		=   39,
	IK_APOSTROPHE  		=   40,
	IK_GRAVE       		=   41,    					/* accent grave */
	IK_LSHIFT      		=   42,
	IK_BACKSLASH   		=   43,
	IK_Z           		=   44,
	IK_X           		=   45,
	IK_C           		=   46,
	IK_V           		=   47,
	IK_B           		=   48,
	IK_N           		=   49,
	IK_M           		=   50,
	IK_COMMA       		=   51,
	IK_PERIOD      		=   52,    					/* . on main keyboard */
	IK_SLASH       		=   53,    					/* / on main keyboard */
	IK_RSHIFT      		=   54,
	IK_MULTIPLY    		=   55,    					/* * on numeric keypad */
	IK_LALT	       		=   56,    					/* left Alt */
	IK_SPACE       		=   57,
	IK_CAPITAL     		=   58,
	IK_F1          		=   59,
	IK_F2          		=   60,
	IK_F3          		=   61,
	IK_F4          		=   62,
	IK_F5          		=   63,
	IK_F6          		=   64,
	IK_F7          		=   65,
	IK_F8          		=   66,
	IK_F9          		=   67,
	IK_F10         		=   68,
	IK_NUMLOCK     		=   69,
	IK_SCROLL      		=   70,    					/* Scroll Lock */
	IK_NUMPAD7     		=   71,
	IK_NUMPAD8     		=   72,
	IK_NUMPAD9     		=   73,
	IK_SUBTRACT    		=   74,    					/* - on numeric keypad */
	IK_NUMPAD4     		=   75,
	IK_NUMPAD5     		=   76,
	IK_NUMPAD6     		=   77,
	IK_ADD         		=   78,    					/* + on numeric keypad */
	IK_NUMPAD1     		=   79,
	IK_NUMPAD2     		=   80,
	IK_NUMPAD3     		=   81,
	IK_NUMPAD0     		=   82,
	IK_DECIMAL     		=   83,    					/* . on numeric keypad */
	IK_84				=	84,						//	UNUSED
	IK_85				=	85,						//	UNUSED
	IK_OEM_102     		=   86,    					/* <> or \| on RT 102-key keyboard (Non-U.S.) */
	IK_F11         		=   87,
	IK_F12         		=   88,
	IK_89				=	89,						//	UNUSED
	IK_90				=	90,						//	UNUSED
	IK_91				=	91,						//	UNUSED
	IK_92				=	92,						//	UNUSED
	IK_93				=	93,						//	UNUSED
	IK_94				=	94,						//	UNUSED
	IK_95				=	95,						//	UNUSED
	IK_96				=	96,						//	UNUSED
	IK_97				=	97,						//	UNUSED
	IK_98				=	98,						//	UNUSED
	IK_99				=	99,						//	UNUSED
	IK_F13         		=   100,    				/*                     (NEC PC98) */
	IK_F14         		=   101,    				/*                     (NEC PC98) */
	IK_F15         		=   102,    				/*                     (NEC PC98) */
	IK_103				=	103,					//	UNUSED
	IK_104				=	104,					//	UNUSED
	IK_105				=	105,					//	UNUSED
	IK_106				=	106,					//	UNUSED
	IK_107				=	107,					//	UNUSED
	IK_108				=	108,					//	UNUSED
	IK_109				=	109,					//	UNUSED
	IK_110				=	110,					//	UNUSED
	IK_111				=	111,					//	UNUSED
	IK_KANA        		=   112,    				/* (Japanese keyboard)            */
	IK_113				=	113,					//	UNUSED
	IK_114				=	114,					//	UNUSED
	IK_ABNT_C1     		=   115,    				/* /? on Brazilian keyboard */
	IK_116				=	116,					//	UNUSED
	IK_117				=	117,					//	UNUSED
	IK_118				=	118,					//	UNUSED
	IK_119				=	119,					//	UNUSED
	IK_120				=	120,					//	UNUSED
	IK_CONVERT     		=   121,    				/* (Japanese keyboard)            */
	IK_122				=	122,					//	UNUSED
	IK_NOCONVERT   		=   123,    				/* (Japanese keyboard)            */
	IK_124				=	124,					//	UNUSED
	IK_YEN         		=   125,    				/* (Japanese keyboard)            */
	IK_ABNT_C2     		=   126,    				/* Numpad . on Brazilian keyboard */
	IK_127				=	127,					//	UNUSED
	IK_128				=	128,					//	UNUSED
	IK_129				=	129,					//	UNUSED
	IK_130				=	130,					//	UNUSED
	IK_131				=	131,					//	UNUSED
	IK_132				=	132,					//	UNUSED
	IK_133				=	133,					//	UNUSED
	IK_134				=	134,					//	UNUSED
	IK_135				=	135,					//	UNUSED
	IK_136				=	136,					//	UNUSED
	IK_137				=	137,					//	UNUSED
	IK_138				=	138,					//	UNUSED
	IK_139				=	139,					//	UNUSED
	IK_140				=	140,					//	UNUSED
	IK_NUMPADEQUALS		=   141,    				/* = on numeric keypad (NEC PC98) */
	IK_142				=	142,					//	UNUSED
	IK_143				=	143,					//	UNUSED
	IK_PREVTRACK   		=   144,    				/* Previous Track (SX_IO_IK_CIRCUMFLEX on Japanese keyboard) */
	IK_AT          		=   145,    				/*                     (NEC PC98) */
	IK_COLON       		=   146,    				/*                     (NEC PC98) */
	IK_UNDERLINE   		=   147,    				/*                     (NEC PC98) */
	IK_KANJI       		=   148,    				/* (Japanese keyboard)            */
	IK_STOP        		=   149,    				/*                     (NEC PC98) */
	IK_AX          		=   150,    				/*                     (Japan AX) */
	IK_UNLABELED   		=   151,    				/*                        (J3100) */
	IK_152				=	152,					//	UNUSED
	IK_NEXTTRACK   		=   153,    				/* Next Track */
	IK_154				=	154,					//	UNUSED
	IK_155				=	155,					//	UNUSED
	IK_NUMPADENTER 		=   156,    				/* Enter on numeric keypad */
	IK_RCONTROL    		=   157,
	IK_158				=	158,					//	UNUSED
	IK_159				=	159,					//	UNUSED
	IK_MUTE        		=   160,    				/* Mute */
	IK_CALCULATOR  		=   161,    				/* Calculator */
	IK_PLAYPAUSE   		=   162,    				/* Play / Pause */
	IK_163				=	163,					//	UNUSED
	IK_MEDIASTOP   		=   164,    				/* Media Stop */
	IK_165				=	165,					//	UNUSED
	IK_166				=	166,					//	UNUSED
	IK_167				=	167,					//	UNUSED
	IK_168				=	168,					//	UNUSED
	IK_169				=	169,					//	UNUSED
	IK_170				=	170,					//	UNUSED
	IK_171				=	171,					//	UNUSED
	IK_172				=	172,					//	UNUSED
	IK_173				=	173,					//	UNUSED
	IK_VOLUMEDOWN  		=   174,    				/* Volume - */
	IK_175				=	175,					//	UNUSED
	IK_VOLUMEUP    		=   176,    				/* Volume + */
	IK_177				=	177,					//	UNUSED
	IK_WEBHOME     		=   178,    				/* Web home */
	IK_NUMPADCOMMA 		=   179,    				/* , on numeric keypad (NEC PC98) */
	IK_180				=	180,					//	UNUSED
	IK_DIVIDE      		=   181,    				/* / on numeric keypad */
	IK_UNUSED64			=	182,					//	UNUSED
	IK_SYSRQ       		=   183,
	IK_RALT	       		=   184,    				/* right Alt */
	IK_185				=	185,					//	UNUSED
	IK_186				=	186,					//	UNUSED
	IK_187				=	187,					//	UNUSED
	IK_188				=	188,					//	UNUSED
	IK_189				=	189,					//	UNUSED
	IK_190				=	190,					//	UNUSED
	IK_191				=	191,					//	UNUSED
	IK_192				=	192,					//	UNUSED
	IK_193				=	193,					//	UNUSED
	IK_194				=	194,					//	UNUSED
	IK_195				=	195,					//	UNUSED
	IK_196				=	196,					//	UNUSED
	IK_PAUSE       		=   197,    				/* Pause */
	IK_198				=	198,					//	UNUSED
	IK_HOME        		=   199,    				/* Home on arrow keypad */
	IK_UP          		=   200,    				/* UpArrow on arrow keypad */
	IK_PAGEUP       	=   201,    				/* PgUp on arrow keypad */
	IK_202				=	202,					//	UNUSED
	IK_LEFT        		=   203,    				/* LeftArrow on arrow keypad */
	IK_204				=	204,					//	UNUSED
	IK_RIGHT       		=   205,    				/* RightArrow on arrow keypad */
	IK_206				=	206,					//	UNUSED
	IK_END         		=   207,    				/* End on arrow keypad */
	IK_DOWN        		=   208,    				/* DownArrow on arrow keypad */
	IK_PAGEDOWN     	=   209,    				/* PgDn on arrow keypad */
	IK_INSERT      		=   210,    				/* Insert on arrow keypad */
	IK_DELETE      		=   211,    				/* Delete on arrow keypad */
	IK_212				=	212,					//	UNUSED
	IK_213				=	213,					//	UNUSED
	IK_214				=	214,					//	UNUSED
	IK_215				=	215,					//	UNUSED
	IK_216				=	216,					//	UNUSED
	IK_217				=	217,					//	UNUSED
	IK_218				=	218,					//	UNUSED
	IK_LWIN        		=   219,    				/* Left Windows key */
	IK_RWIN        		=   220,    				/* Right Windows key */
	IK_APPS        		=   221,    				/* AppMenu key */
	IK_POWER       		=   222,    				/* System Power */
	IK_SLEEP       		=   223,    				/* System Sleep */
	IK_224				=	224,					//	UNUSED
	IK_225				=	225,					//	UNUSED
	IK_226				=	226,					//	UNUSED
	IK_WAKE        		=   227,    				/* System Wake */
	IK_228				=	228,					//	UNUSED
	IK_WEBSEARCH   		=   229,    				/* Web Search */
	IK_WEBFAVORITES		=   230,    				/* Web Favorites */
	IK_WEBREFRESH  		=   231,    				/* Web Refresh */
	IK_WEBSTOP     		=   232,    				/* Web Stop */
	IK_WEBFORWARD  		=   233,    				/* Web Forward */
	IK_WEBBACK     		=   234,    				/* Web Back */
	IK_MYCOMPUTER  		=   235,    				/* My Computer */
	IK_MAIL				=	236,    				/* Mail */
	IK_MEDIASELECT		=	237,    				/* Media Select */
	IK_MOUSE_LEFT		=	238,    				/* Mouse left button */
	IK_MOUSE_RIGHT		=	239,    				/* Mouse right button */
	IK_MOUSE_MIDDLE		=	240,    				/* Mouse middle button */
	IK_MOUSE_WHEEL		=	241,    				/* Mouse wheel value */

	IK_FORCE32			=	0xffffffff
};

//! input buttons state
enum InputState
{
	IS_NORMAL = 0,
	IS_DOWN,
	IS_HOLD,
	IS_UP,
	IS_DOUBLE,

	IS_FORCE32 = 0xffffffff
};


//! signals that will received by input devices
enum InputSignalType
{
	IST_SET_OS_CURSOR = 1,		//  this will comes by a vector to set device's cursor. use int2 as x and y
	IST_SET_RECT,				//  change input box size. data will be a pointer to int4 : x, y, width, height
	IST_SET_SPEED,				//	change input speed. data will be a pointer to float
};

//! structure of all float values in Input system { cursor, wheel, analogs, ... )
struct InputValues
{
	float abs_x;		//	absolute value in screen space
	float abs_y;		//	absolute value in screen space
	float rl_x;			//	relative value in screen space
	float rl_y;			//	relative value in screen space
	float wheel;		//	wheel value [ -1 , 0 , 1 ]
};

//! structure of key states in Input system { cursor, wheel, analogs, ... )
struct InputKeys
{
	union {
		struct {
			InputState key0;
			InputState scape;
			InputState n_1;              	
			InputState n_2;              	
			InputState n_3;              	
			InputState n_4;              	
			InputState n_5;              	
			InputState n_6;              	
			InputState n_7;              	
			InputState n_8;              	
			InputState n_9;              	
			InputState n_0;              	
			InputState minus;							/* - on main keyboard */
			InputState equals;							
			InputState backspace;          				/* backspace */
			InputState tab;		
			InputState Q;
			InputState W;
			InputState E;
			InputState R;
			InputState T;
			InputState Y;
			InputState U;
			InputState I;
			InputState O;
			InputState P;
			InputState bracket_left;					
			InputState bracket_right;					
			InputState enter;							/* Enter on main keyboard */
			InputState ctrl_left;						
			InputState A;
			InputState S;
			InputState D;
			InputState F;
			InputState G;
			InputState H;
			InputState J;
			InputState K;
			InputState L;
			InputState semicolon;						
			InputState apostrophe;						
			InputState grave;							/* accent grave */
			InputState shift_left;						
			InputState backslash;						
			InputState Z;								
			InputState X;								
			InputState C;								
			InputState V;		
			InputState B;								
			InputState N;								
			InputState M;								
			InputState camma;							
			InputState period;							/* . on main keyboard */
			InputState slash;							/* / on main keyboard */
			InputState shift_right;						
			InputState numpad_star;						/* * on numeric keypad */
			InputState alt_left;						/* left Alt */
			InputState space;							
			InputState capslock;						
			InputState F1;								
			InputState F2;								
			InputState F3;								
			InputState F4;								
			InputState F5;
			InputState F6;								
			InputState F7;								
			InputState F8;								
			InputState F9;								
			InputState F10;								
			InputState numlock;							
			InputState scroll;							/* Scroll Lock */
			InputState numpad7;							
			InputState numpad8;							
			InputState numpad9;							
			InputState numpad_minus;					/* - on numeric keypad */
			InputState numpad4;							
			InputState numpad5;							
			InputState numpad6;							
			InputState numpad_plus;						/* + on numeric keypad */
			InputState numpad1;	
			InputState numpad2;							
			InputState numpad3;							
			InputState numpad0;							
			InputState numpad_priod;					/* . on numeric keypad */
			InputState key84;
			InputState key85;
			InputState oem_102;							/* <> or \| on RT 102-key keyboard (Non-U.S.) */
			InputState F11;								
			InputState F12;								
			InputState key89;
			InputState key90;
			InputState key91;
			InputState key92;
			InputState key93;
			InputState key94;
			InputState key95;
			InputState key96;
			InputState key97;
			InputState key98;
			InputState key99;
			InputState F13;								/*                     (NEC PC98) */
			InputState F14;								/*                     (NEC PC98) */
			InputState F15;								/*                     (NEC PC98) */
			InputState key103;
			InputState key104;
			InputState key105;
			InputState key106;
			InputState key107;
			InputState key108;
			InputState key109;
			InputState key110;
			InputState key111;
			InputState kana;							/* (Japanese keyboard)            */
			InputState key113;
			InputState key114;
			InputState abnt_c1;							/* /? on Brazilian keyboard */
			InputState key116;
			InputState key117;
			InputState key118;
			InputState key119;
			InputState key120;
			InputState convert;							/* (Japanese keyboard)            */
			InputState key122;
			InputState noconvert;						/* (Japanese keyboard)            */
			InputState key123;
			InputState yen;								/* (Japanese keyboard)            */
			InputState abnt_c2;							/* Numpad . on Brazilian keyboard */
			InputState key127;
			InputState key128;
			InputState key129;
			InputState key130;
			InputState key131;
			InputState key132;
			InputState key133;
			InputState key134;
			InputState key135;
			InputState key136;
			InputState key137;
			InputState key138;
			InputState key139;
			InputState key140;
			InputState numpad_equals;					/* = on numeric keypad (NEC PC98) */
			InputState key142;
			InputState key143;
			InputState prevtrack;						/* Previous Track (SX_IO_KEY_CIRCUMFLEX on Japanese keyboard) */
			InputState at;								/*                     (NEC PC98) */
			InputState colon;         					/*                     (NEC PC98) */
			InputState underline;						/*                     (NEC PC98) */
			InputState kanji;							/* (Japanese keyboard)            */
			InputState stop;							/*                     (NEC PC98) */
			InputState ax;								/*                     (Japan AX) */
			InputState unlabeled;						/*                        (J3100) */
			InputState key152;
			InputState nexttrack;						/* Next Track */
			InputState key154;
			InputState key155;
			InputState numpad_enter;  					/* Enter on numeric keypad */
			InputState ctrl_right;    					
			InputState key158;
			InputState key159;
			InputState mute;							/* Mute */
			InputState calculator;						/* Calculator */
			InputState playpause;						/* Play / Pause */
			InputState key163;
			InputState mediastop;						/* Media Stop */
			InputState key165;
			InputState key166;
			InputState key167;
			InputState key168;
			InputState key169;
			InputState key170;
			InputState key171;
			InputState key172;
			InputState key173;
			InputState volumedown;						/* Volume - */
			InputState key175;
			InputState volumeup;						/* Volume + */
			InputState key177;
			InputState webhome;							/* Web home */
			InputState numpad_camma;					/* , on numeric keypad (NEC PC98) */
			InputState key180;
			InputState numpadslash;						/* / on numeric keypad */
			InputState key182;
			InputState sysrq;							
			InputState alt_right;						/* right Alt */
			InputState key185;
			InputState key186;
			InputState key187;
			InputState key188;
			InputState key189;
			InputState key190;
			InputState key191;
			InputState key192;
			InputState key193;
			InputState key194;
			InputState key195;
			InputState key196;
			InputState pause;							/* Pause */
			InputState key198;
			InputState home;							/* Home on arrow keypad */
			InputState up;								/* UpArrow on arrow keypad */
			InputState pageup;							/* PgUp on arrow keypad */
			InputState key202;
			InputState left;							/* LeftArrow on arrow keypad */
			InputState key204;
			InputState right;							/* RightArrow on arrow keypad */
			InputState key206;
			InputState end;								/* End on arrow keypad */
			InputState down;							/* DownArrow on arrow keypad */
			InputState pagedown;						/* PgDn on arrow keypad */
			InputState insert;							/* Insert on arrow keypad */
			InputState deleTe;							/* Delete on arrow keypad */
			InputState key212;
			InputState key213;
			InputState key214;
			InputState key215;
			InputState key216;
			InputState key217;
			InputState key218;
			InputState win_left;						/* Left Windows key */
			InputState win_right;     					/* Right Windows key */
			InputState apps;							/* AppMenu key */
			InputState power;							/* System Power */
			InputState sleep;							/* System Sleep */
			InputState key224;
			InputState key225;
			InputState key226;
			InputState wake;							/* System Wake */
			InputState key228;
			InputState websearch;						/* Web Search */
			InputState webfavorites;					/* Web Favorites */
			InputState webrefresh;						/* Web Refresh */
			InputState webstop;							/* Web Stop */
			InputState webforward;						/* Web Forward */
			InputState webback;							/* Web Back */
			InputState mycomputer;						/* My Computer */
			InputState mail;							/* Mail */
			InputState mediaselect;						/* Media Select */
			InputState mouse_left;						/* Mouse left button */
			InputState mouse_right;						/* Mouse right button */
			InputState mouse_middle;					/* Mouse middle button */
			InputState mouse_wheel;						/* Mouse wheel value */

			InputState unused[15];
		};

		InputState keys[256];
	};
};


/*!
this is the base of other input device. other input device classes should inherit from this class.
to handle signals for more that one device in same type (2 joystick) the application responsible to attach more
Input Devices to Input class with different playerID. ( each player need his separate devices )
*/
class SEGAN_ENG_API InputDevice
{
	SEGAN_STERILE_CLASS(InputDevice);

public:

	InputDevice( const uint playerID );
	virtual ~InputDevice();

	//! initialize this device
	virtual void Initialize( void ) = 0;

	//! finalize this device
	virtual void Finalize( void ) = 0;
	
	//! get the name of the input device
	virtual const wchar* GetName( void ) = 0;

	//! get description of the input device
	virtual const wchar* GetDesc( void ) = 0;

	//! return device's capabilities. cursor, shock, LCD, LED, etc
	virtual dword GetCapabilities( void ) = 0;

	//! update input device
	virtual void Update( float elapsTime ) = 0;

	/*!
	this may call by engine in game to send signal to shock, vibration, etc. 
	use playerID to specify which player should handle this signal.
	*/
	virtual void OnSignal( const InputSignalType idSignal, void* data, const uint playerID = 0 ) = 0;

public:
	class Input*	m_owner;		//	owner to the input system
	uint			m_playerID;		//  the player ID of this device
};



//////////////////////////////////////////////////////////////////////////
//! use this class to catch the user input and also send signal to other devices ( shock, vibration, LED, LCD, etc )
class SEGAN_ENG_API Input
{
	SEGAN_STERILE_CLASS(Input);

public:
	Input( void );
	~Input( void );

	//! attach a device to the input class
	void Attach( const InputDevice* device );

	//! return number of devices currently attached to engine
	uint GetDeviceCount( void ) const;

	//! return device by index. return null if the index was out of range
	InputDevice* GetDevice( sint index );

	/*!
	this may call by engine in game to send signal to shock, vibration, etc to all other devices
	use playerID to specify which player should handle this signal.
	*/
	void SendSignal( InputSignalType idSignal, void* data, const uint playerID = 0 );

	//! update input devices
	void Update( float elpsTime );

	//! return values of cursor structure
	InputValues* GetValues( const uint playerID = 0 );

	//! return buttons value
	InputKeys* GetKeys( const uint playerID = 0 );

	//! return true if the key state is first down
	bool KeyDown( const InputKey key, const uint playerID = 0 );

	//! return true if the key state is hold down
	bool KeyHold( const InputKey key, const uint playerID = 0 );

	//! return true if the key state is up
	bool KeyUp( const InputKey key, const uint playerID = 0 );

	//! return true if the key state is double up
	bool KeyDouble( const InputKey key, const uint playerID = 0 );

public:

	InputValues				m_values[SX_IO_PLAYER_MAXID];
	InputKeys				m_keys[SX_IO_PLAYER_MAXID];
	Array<InputDevice*>		m_devices;

};


#define sx_key_down(key, playerID)		g_engine->m_input->KeyDown( key, playerID )
#define sx_key_hold(key, playerID)		g_engine->m_input->KeyHold( key, playerID )
#define sx_key_up(key, playerID)		g_engine->m_input->KeyUp( key, playerID )
#define sx_key_double(key, playerID)	g_engine->m_input->KeyDouble( key, playerID )
#define sx_mouse_rlx(playerID)			g_engine->m_input->GetValues( playerID )->rl_x
#define sx_mouse_rly(playerID)			g_engine->m_input->GetValues( playerID )->rl_y
#define sx_mouse_absx(playerID)			g_engine->m_input->GetValues( playerID )->abs_x
#define sx_mouse_absy(playerID)			g_engine->m_input->GetValues( playerID )->abs_y

byte SEGAN_ENG_API sx_io_get_acci( InputKey inputButton );
byte SEGAN_ENG_API sx_io_get_acci_with_shift( InputKey inputButton );
word SEGAN_ENG_API sx_io_get_farsi( InputKey inputButton );
word SEGAN_ENG_API sx_io_get_farsi_with_shift( InputKey inputButton );

#endif	//	GUARD_Input_HEADER_FILE

