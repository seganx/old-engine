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
	UT_PANELEX,
	UT_LISTBOX,

	UT_32BITENUM = 0xffffffff
};

//! indicate that which primitive type should be used in batch system
enum uiBatchMode
{
	BM_SIMPLE = 0,
	BM_TRIANGLES,
	BM_QUADS_CCW,
	MB_QUADS_CW,

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
	void Remove( const uint index );

	//! return the index of current state
	uint GetIndex( void ) const;

	//! set index of the state
	void SetIndex( const uint index );

	//! return reference to the current state structure
	uiState* GetCurrent( void );

	//! return reference to state by index
	uiState* GetByIndex( const uint index );

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
	void ClearVertives( void );

public:

	uint		m_image;			//	image id helps the manager to compound elements
	uint		m_numVertices;		//	number of vertices
	float3*		m_pos;				//	positions
	float2*		m_uv;				//	UV coordinates
	Color*		m_color;			//	colors
	float3*		m_posfinal;				//	use temporary positions to transform elements from local space to the world space
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

public:

	uiType				m_type;								//!	type of control
	str128				m_name;								//!	name of control
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
	void Copy( uiElement* dest, uint& index, const uiElement* src, const uiBatchMode mode );

	//! prepare for batching elements. if mode was not simple the only element's with 4 vertices will accepted to the batch
	void BeginBatchElements( const uiBatchMode mode, const uint count );

	//! add an element to the batch and return false if can't batch given element with another
	bool AddBatchElements( const uiElement* elem );

	//! end patch and append them to the end of dest element
	void EndBatchElements( uiElement* dest );

public:

	uiBatchMode			m_batchMode;	//! batch mode 
	Array<uiElement*>	m_batches;		//!	us in batch system
	float				m_zbias;		//! prevent rubbing surfaces of GUI controls in 3D mode
	bool				m_editor;		//! indicates that GUI is running in editor mode
};


//! GUI post properties
#define _SX_GUI_NOT_VISIBLE_			0x10000000
#define _SX_GUI_NOT_ENABLE_				0x20000000
#define _SX_GUI_IN_3DSPACE_				0x40000000



#endif	//	GUARD_uiDevice_HEADER_FILE
