/********************************************************************
	created:	2013/03/15
	filename: 	uiDevice.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain GUI manager for manage GUI system
*********************************************************************/
#ifndef GUARD_uiDevice_HEADER_FILE
#define GUARD_uiDevice_HEADER_FILE


#include "../Engine_def.h"
#include "../math/Math.h"

//! these flags used as GUI properties
#define SX_GUI_VISIBLE					0x00000001		//!	control is visible
#define SX_GUI_ENABLE					0x00000002		//!	control is enable
#define SX_GUI_BLENDSTATES				0x00000004		//!	control can blend between states
#define SX_GUI_FREEZE					0x00000008		//!	control is froze and can't be selected. this is useful for editors
#define SX_GUI_PROCESSKEY				0x00000010		//!	control can process keyboard input
#define SX_GUI_PROCESSMOUSE				0x00000020		//!	make control active and can be selected by mouse.
#define SX_GUI_CLIPCHILDS				0x00000040		//!	use clip plane to avoid drawing child out of control area
#define SX_GUI_BILLBOARD				0x00000080		//!	control will display as billboard
#define SX_GUI_3DSPACE					0x00000100		//!	control will show in 3D space
#define SX_GUI_DISCRETE					0x00000200		//!	Scroll : indicator will scroll in discrete values
#define SX_GUI_MULTILINE				0x00000400		//!	Label/EditBox : allow to the label or edit control to show multi line text
#define SX_GUI_WORDWRAP					0x00000800		//!	Label/EditBox : label or edit control will break the long size lines to fit in the box
#define SX_GUI_AUTOSIZE					0x00001000		//!	Label/EditBox : label or edit control will resize to fit the text
#define SX_GUI_PROGRESSCIRCLE			0x00002000		//!	Progress : control will display as circle mode
#define SX_GUI_PROGRESSUV				0x00004000		//!	Progress : control will display as linear mode and UV alignment

//! GUI post properties
#define _SX_GUI_NOT_VISIBLE_			0x10000000
#define _SX_GUI_NOT_ENABLE_				0x20000000
#define _SX_GUI_IN_3DSPACE_				0x40000000

//! bounding conditions
#define SX_GUI_MINIMUM_ALPHA			0.001f			//!	minimum alpha value that controls can be shown
#define SX_GUI_MINIMUM_SCALE			0.0001f			//! minimum scale value that controls can be shown

//! prevent rubbing surfaces
#define SX_GUI_Z_BIAS					-0.15f



//! describe state of mouse on a control
enum uiMouseState
{
	MS_NORMAL	= 0,
	MS_ENTERED,
	MS_DOWN,
	MS_UP
};

/*! input also reports that which input values used in other processes */
struct uiInput
{
	Ray				ray;				//!	ray comes from the mouse
	uint			mouseLocked;		//!	contain the id of object who locked mouse
	uint			keyboardLocked;		//!	contain the id of object who locked keyboard
	uiMouseState	mouseLeft;			//!	state of left mouse button 
	uiMouseState	mouseRight;			//!	state of right mouse button
	uiMouseState	mouseMidd;			//!	state of middle mouse button
	char			mouseWheel;			//! value of mouse wheel [ -1 , 0 , 1 ]
	word			keycode;			//!	key down code
	word			keychar;			//!	key down character
	dword			keycodeEx;			//!	additional extended key code

	uiInput(void): ray( float3(0,0,0), float3(0,0,0) ), mouseLocked(0), keyboardLocked(0), 
		mouseLeft(MS_NORMAL), mouseRight(MS_NORMAL), mouseMidd(MS_NORMAL), mouseWheel(0), keycode(0), keychar(0), keycodeEx(0) {}
};

//! describe text alignment
enum uiTextAlign
{
	TA_LEFT,
	TA_RIGHT,
	TA_CENTER,

	TA_32BITENUM = 0xffffffff
};

//! the value of this enumerations should set with care. values lower than 0x01000000 used for LTR and the higher values used for RTL languages
enum uiInputLanguage
{
	IL_ENGLISH	= 0x00000100,	//	|	
	IL_GERMAN	= 0x00000200,	//	|
	IL_FRENCH	= 0x00000400,	//	|
	IL_SPANISH	= 0x00000800,	//	|
	IL_LANG5	= 0x00001000,	//	|
	IL_LANG6	= 0x00002000,	//	|
	IL_LANG7	= 0x00004000,	//	|
	IL_LANG8	= 0x00008000,	//	|
	IL_LANG9	= 0x00010000,	//	|===> Left To Right Languages
	IL_LANG10	= 0x00020000,	//	|
	IL_LANG11	= 0x00040000,	//	|
	IL_LANG12	= 0x00080000,	//	|
	IL_LANG13	= 0x00100000,	//	|
	IL_LANG14	= 0x00200000,	//	|
	IL_LANG15	= 0x00400000,	//	|
	IL_LANG16	= 0x00800000,	//	|

	IL_LANG17	= 0x01000000,	//	|
	IL_LANG18	= 0x02000000,	//	|
	IL_LANG19	= 0x04000000,	//	|
	IL_LANG20	= 0x08000000,	//	|===> Right To Left Languages
	IL_LANG21	= 0x10000000,	//	|
	IL_LANG22	= 0x20000000,	//	|
	LI_ARABIC	= 0x40000000,	//	|
	IL_PERSIAN	= 0x80000000	//	|
};

//! describe character information
struct uiChar
{
	dword	id;
	sint	x;
	sint	y;
	sint	width;
	sint	height;
	sint	xOffset;
	sint	yOffset;
	sint	xAdvance;
	sint	page;

	uiChar(): id(0), x(0), y(0), width(0), height(0), xOffset(0), yOffset(0), xAdvance(0), page(0) {};
};

//! describe font information
struct uiFontDesc
{
	sint	size;				//  size of the font
	sint	charCount;			//  number of characters
	sint	outline;			//  outline thickness
	sint	lineHeight;			//	distance in pixels between each line of text  
	sint	charBase;			//	number of pixels from the absolute top of the line to the base of the characters

	uiFontDesc(): size(0), charCount(0), outline(0), lineHeight(0), charBase(0) {};
};

//! state of control
struct uiState
{
	float2		align;			//  align system of the control
	float3		center;			//  center of the control
	float3		position;		//  position of the control
	float3		rotation;		//  rotation of the control
	float3		scale;			//  scale of the control
	float4		color;			//  color of the control
	float2		blender;		//  blending weights for velocity and amplitude

	uiState():	align(0,0), center(0,0,0), position(0,0,0), rotation(0,0,0), scale(1,1,1), color(1,1,1,1), blender(0.7f, 0.5f) {}
};


//! state controller can manage and update states. state controller has one state at least
class SEGAN_ENG_API uiStateController
{
	SEGAN_STERILE_CLASS(uiStateController);

public:

	uiStateController( void );
	~uiStateController( void );

	//! clear all states except one first state
	void clear( void );

	//! add a new state and fill it by current state and return index of new state
	uint add( void );

	//! remove an state by specified index
	void remove( const sint index );

	//! set index of the state
	void set_current( const sint index );

	//! return true if the state is blending
	bool is_blending( void ) const;

	//! TODO: move option from this to the uiControl body !! get options and update controller then return new option 
	void update( const dword option, float elpsTime );

public:

	Array<uiState>	m_states;	//	array of states
	uiState			m_curr;		//	current state
	uiState			m_last;		//	last state
	sint			m_index;	//	index of current state

	struct Blender
	{
		float vel;
		float amp;
		float w;
		float a;
		float v;
	} 				m_blender;	//	simple blending system to blend between states
};

//!	basic context of a graphical user interface
class SEGAN_ENG_API uiContext
{
	SEGAN_STERILE_CLASS(uiContext);
public:
	uiContext( void );
	~uiContext( void );

	//! create vertices
	void create_vertices( const uint count );

	//! clear all vertices
	void clear_vertices( void );

public:

	uint			m_image;			//	image id indicates the texture id in texture manager. also this can helps the painter to compound elements
	uint			m_vcount;			//	number of vertices
	float3*			m_pos;				//	positions
	float2*			m_uv;				//	UV coordinates
	Color2*			m_color;			//	colors

};

//! base class of forms
class uiForm
{
public:	
	virtual ~uiForm( void ) {};
};

//! this is the standard GUI callback event.
typedef void (uiForm::*CB_uiControl)(class uiControl* sender);

//! object structure used to assign GUI member functions
class uiFunction
{
public:
	uiFunction(): m_form(null), m_func(null) {};
	uiFunction( uiForm* form, CB_uiControl func ): m_form(form), m_func(func) {};
	void operator ()( class uiControl* sender )
	{
		if ( m_form && m_func )
			( m_form->*m_func)( sender );
	}

public:
	uiForm*			m_form;
	CB_uiControl	m_func;
};

//! basic GUI control
class SEGAN_ENG_API uiControl
{
	SEGAN_STERILE_CLASS(uiControl);
public:
	uiControl( void );
	virtual ~uiControl( void );

	//! set the parent of this control
	virtual void set_parent( uiControl* parent );

	//! set size of control
	virtual void set_size( const float width, const float height );

	//! update the control
	virtual void update( float elpsTime, const uint vpwidth, const uint vpheight );

	//! paint the control to the context
	virtual void paint( void ) = 0;

	//! process input for this control
	virtual void process_input( uiInput* inputReport );

public:

	char				m_type[8];							//!	type of control
	uint				m_id;
	String				m_name;								//!	name of control
	dword				m_option;							//!	include control options started with SX_GUI_
	float2				m_size;								//!	control dimensions
	uiStateController	m_state;							//!	state of control include origin, orientation, scale, ...
	float3				m_position_offset;					//!	additional position offset
	float3				m_rotation_offset;					//!	additional rotation offset
	float3				m_scale_offset;						//!	additional scale offset
	matrix				m_matrix;							//!	final matrix computed from states
	uiContext			m_context;							//!	context of control
	uiControl*			m_parent;							//!	parent of this control
	Array<uiControl*>	m_child;							//!	array of children

	uiMouseState		m_mouseState;						//!	last mouse state on this control
	uiFunction			m_on_enter;							//!	callback function for mouse enter
	uiFunction			m_on_exit;							//!	callback function for mouse leave
	uiFunction			m_on_move;							//!	callback function for mouse move
	uiFunction			m_on_wheel;							//!	callback function for mouse wheel
	uiFunction			m_on_click;							//!	callback function for mouse click
	uiFunction			m_on_keydown;						//!	callback function for key down
};


//! the Panel Control. this object has only one element as background
class SEGAN_ENG_API uiPanel: public uiControl
{
	SEGAN_STERILE_CLASS(uiPanel);
public:
	uiPanel( void );
	virtual ~uiPanel( void );

	//! set size of control
	virtual void set_size( const float width, const float height );

	//! paint the control to the context
	virtual void paint( void );

};

/*! return index of first vertex of triangle which contacted by mouse ray and fill out uv point of intersection. return -1 if no contact */
SEGAN_ENG_API sint sx_intersect( const Ray* ray, const uiContext* element, OUT float2* uv = null );

//! add a rectangle to the context
SEGAN_ENG_API void sx_add_rectangle( uiContext* context, const float2& xy1, const float2& xy2, const float2& uv1, const float2& uv2 );

//! add a circle to the context
SEGAN_ENG_API void sx_add_circle( uiContext* context, const float2& center, const float radius, const float numslice );

//! extract all contexts in the control and his children
SEGAN_ENG_API void sx_get_contexts( const uiControl* control, Array<uiContext*> * contexts, const bool traverschilds = true );

#endif	//	GUARD_uiDevice_HEADER_FILE
