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


#include "../../sxLib/Lib.h"

#define SX_GUI_MAX_ELEMENT				4

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

//! bounding conditions
#define SX_GUI_MINIMUM_ALPHA			0.001f			//!	minimum alpha value that controls can be shown
#define SX_GUI_MINIMUM_SCALE			0.0001f			//! minimum scale value that controls can be shown

//! prevent rubbing surfaces
#define SX_GUI_Z_BIAS					-0.15f

//! these are types of GUI controls
enum uiType
{
	UT_NONE = 0,
	UT_PANEL,
	UT_BUTTON,
	UT_CHECKBOX,
	UT_SCROLL,
	UT_PROGRESS,
	UT_LABEL,
	UT_EDITBOX,
	UT_LISTBOX,

	UT_32BITENUM = 0xffffffff
};

//! indicate that which primitive type should be used in batch system
enum uiElementType
{
	ET_NONE = 0,
	ET_LINES,			//! element contain list of lines
	ET_TRIANGLES,		//! element contain list of triangles
	ET_QUADS,			//! element contain list of quads

	BM_32BITENUM = 0xffffffff
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

//! describe state of mouse on a control
enum uiMouseState
{
	MS_NORMAL	= 0,
	MS_ENTERED,
	MS_DOWN,
	MS_UP,

	MS_32BITENUM = 0xffffffff
};

/*! input reporter reports that which input values used in other processes */
struct uiInputReport
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

	uiInputReport(void): ray( float3(0,0,0), float3(0,0,0) ), mouseLocked(0), keyboardLocked(0), 
		mouseLeft(MS_NORMAL), mouseRight(MS_NORMAL), mouseMidd(MS_NORMAL), mouseWheel(0), keycode(0), keychar(0), keycodeEx(0) {}
};


//! describe character information
struct uiFontChar
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

	uiFontChar(): id(0), x(0), y(0), width(0), height(0), xOffset(0), yOffset(0), xAdvance(0), page(0) {};
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
	void Clear( void );

	//! return the number of states
	uint Count( void ) const;

	//! add a new state and fill it by current state and return index of new state
	uint Add( void );

	//! remove an state by specified index
	void Remove( const sint index );

	//! return the index of current state
	uint GetIndex( void ) const;

	//! set index of the state
	void SetIndex( const sint index );

	//! return reference to the current state structure
	uiState* GetCurrent( void );

	//! return reference to state by index
	uiState* GetByIndex( const sint index );

	//! return reference of the blended state structure
	uiState* GetBlended( void );

	//! return true if the state is blending
	bool IsBlending( void ) const;

	//! TODO: move option from this to the uiControl body !! get options and update controller then return new option 
	void Update( const dword option, float elpsTime );

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

//!	basic element of a graphical user interface
class SEGAN_ENG_API uiElement
{
	SEGAN_STERILE_CLASS(uiElement);
public:
	uiElement( void );
	~uiElement( void );

	//! create vertices
	void CreateVertices( const uint count );

	//! clear all vertices
	void ClearVertices( void );

public:
	uiElementType	m_type;				//	type of data in the element
	uint			m_image;			//	image id helps the manager to compound elements
	uint			m_numVertices;		//	number of vertices
	float3*			m_pos;				//	positions
	float2*			m_uv;				//	UV coordinates
	Color2*			m_color;			//	colors
	float3*			m_posfinal;			//	use temporary positions to transform elements from local space to the world space
};

//! base class of forms
class Form
{
public:	
	virtual ~Form( void ) {};
};

//! this is the standard GUI callback event.
typedef void (Form::*GUICallback)(class uiControl* sender);

//! object structure used to assign GUI member functions
class GUIFuncPtr
{
public:
	GUIFuncPtr(): m_form(null), m_func(null) {};
	GUIFuncPtr( Form* form, GUICallback func ): m_form(form), m_func(func) {};
	void operator ()( class uiControl* sender )
	{
		if ( m_form && m_func )
			( m_form->*m_func)( sender );
	}

public:
	Form*			m_form;
	GUICallback		m_func;
};

//! basic GUI control
class SEGAN_ENG_API uiControl
{
	SEGAN_STERILE_CLASS(uiControl);
public:
	uiControl( void );
	~uiControl( void );

	//! set the parent of this control
	virtual void SetParent( uiControl* parent );

	//! set size of control
	virtual void SetSize( const float width, const float height );

	//! update the control
	virtual void Update( float elpsTime, const matrix& viewInverse, const matrix& viewProjection, const uint vpwidth, const uint vpheight );

	//! process input for this control
	virtual void ProcessInput( uiInputReport* inputReport );

	//! extract valid elements in the control
	void GetElements( Array<uiElement*> * elementArray, const bool traversChilds = true );

	/*! return index of element contacted by mouse ray and fill out uv point of intersection. return -1 if no contact */
	sint IntersectRay( const Ray* ray, const sint element = -1, OUT float2* uv = null );

public:

	uiType				m_type;								//!	type of control
	String				m_name;								//!	name of control
	uint				m_id;
	dword				m_option;							//!	include control options started with SX_GUI_
	float2				m_size;								//!	control dimensions
	uiStateController	m_state;							//!	state of control include origin, orientation, scale, ...
	float3				m_position_offset;					//!	additional position offset
	float3				m_rotation_offset;					//!	additional rotation offset
	float3				m_scale_offset;						//!	additional scale offset
	matrix				m_matrix;							//!	final matrix computed from states
	uiElement			m_element[SX_GUI_MAX_ELEMENT];		//!	elements of control
	uiControl*			m_parent;							//!	parent of this control
	Array<uiControl*>	m_child;							//!	array of children

	uiMouseState		m_mouseState;						//!	hold the state of mouse
	GUIFuncPtr			m_onClick;							//!	callback function for mouse click
	GUIFuncPtr			m_onEnter;							//!	callback function for mouse enter
	GUIFuncPtr			m_onExit;							//!	callback function for mouse leave
	GUIFuncPtr			m_onMove;							//!	callback function for mouse move
	GUIFuncPtr			m_onWheel;							//!	callback function for mouse wheel
	GUIFuncPtr			m_onKeyDown;						//!	callback function for key down

};


//! the Panel Control. this object has only one element as background
class SEGAN_ENG_API uiPanel: public uiControl
{
	SEGAN_STERILE_CLASS(uiPanel);
public:
	uiPanel( void );
	~uiPanel( void );

	//! set size of control
	virtual void SetSize( const float width, const float height );

};

//! uiDevice contain some additional functions
class SEGAN_ENG_API uiDevice
{
	SEGAN_STERILE_CLASS(uiDevice);
public:

	uiDevice( void );
	~uiDevice( void );

	//! create and return a GUI by given type
	uiControl* CreateContorl( const uiType type );

	//! copy the src element to the dest element in the given index position
	void Copy( uiElement* dest, uint& index, const uiElement* src );

	//! prepare for batching elements. pass count 0 to compute number of batch automatically
	void BeginBatch( const uint count );

	//! add an element to the batch and return false if can't batch the element with another
	bool AddBatch( const uiElement* elem );

	//! return number of vertices need to batch all added elements
	uint GetBatchVertexCount( void );

	//! end patch and insert them to dest element. the last data in dest will be lost
	void EndBatch( uiElement* dest );

public:

	Array<uiElement*>	m_batches;		//!	us in batch system
	bool				m_editor;		//! indicates that GUI is running in editor mode
};


//! GUI post properties
#define _SX_GUI_NOT_VISIBLE_			0x10000000
#define _SX_GUI_NOT_ENABLE_				0x20000000
#define _SX_GUI_IN_3DSPACE_				0x40000000

//! return true if the ray intersect with element
SEGAN_ENG_API bool sx_intersect( const Ray* ray, const uiElement* element, OUT float2* uv = null );

#endif	//	GUARD_uiDevice_HEADER_FILE
