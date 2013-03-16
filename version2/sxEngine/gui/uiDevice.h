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
#define SX_GUI_VISIBLE					0x00000001		//  control is visible
#define SX_GUI_ENABLE					0x00000002		//  control is enable
#define SX_GUI_FREEZE					0x00000004		//  control is froze and can't be selected. this is useful for editors
#define SX_GUI_PROCESSKEY				0x00000008		//	control can process keyboard input
#define SX_GUI_PROCESSMOUSE				0x00000010		//  make control active and can be selected by mouse.
#define SX_GUI_CLIPCHILDS				0x00000020		//  use clip plane to avoid drawing child out of control area
#define SX_GUI_TOPLEFT					0x00000040		//  control will show in top / left 2D system
#define SX_GUI_BILLBOARD				0x00000080		//	control will display as billboard
#define SX_GUI_3DSPACE					0x00000100		//  control will show in 3D space
#define SX_GUI_DISCRETE					0x00000200		//	Scroll : indicator will scroll in discrete values
#define SX_GUI_MULTILINE				0x00000400		//	Label/EditBox : allow to the label or edit control to show multi line text
#define SX_GUI_WORDWRAP					0x00000800		//	Label/EditBox : label or edit control will break the long size lines to fit in the box
#define SX_GUI_AUTOSIZE					0x00001000		//	Label/EditBox : label or edit control will resize to fit the text
#define SX_GUI_PROGRESSCIRCLE			0x00002000		//	Progress : control will display as circle mode
#define SX_GUI_PROGRESSUV				0x00004000		//	Progress : control will display as linear mode and UV alignment

//! these are types of GUI controls
enum GUIType
{
	GUI_NONE = 0,
	GUI_PANEL,
	GUI_BUTTON,
	GUI_CHECKBOX,
	GUI_Scroll,
	GUI_PROGRESS,
	GUI_LABEL,
	GUI_EDITBOX,
	GUI_PANELEX,
	GUI_LISTBOX,

	GUI_32BITENUM = 0xffffffff
};

//! indicate that which primitive type should be used in batch system
enum GUIBatchMode
{
	GBM_SIMPLE = 0,
	GBM_TRIANGLES,
	GBM_QUADS_CCW,
	GMB_QUADS_CW,

	GBM_32BITENUM = 0xffffffff
};

//! describe text alignment
enum GUITextAlign
{
	GTA_LEFT,
	GTA_RIGHT,
	GTA_CENTER,

	GTA_32BITENUM = 0xffffffff
};

//! the value of this enumerations should set with care. values lower than 0x01000000 used for LTR and the higher values used for RTL languages
enum GUIInputLanguage
{
	GIL_ENGLISH	= 0x00000100,	//	|	
	GIL_GERMAN	= 0x00000200,	//	|
	GIL_FRENCH	= 0x00000400,	//	|
	GIL_SPANISH	= 0x00000800,	//	|
	GIL_LANG5	= 0x00001000,	//	|
	GIL_LANG6	= 0x00002000,	//	|
	GIL_LANG7	= 0x00004000,	//	|
	GIL_LANG8	= 0x00008000,	//	|
	GIL_LANG9	= 0x00010000,	//	|===> Left To Right Languages
	GIL_LANG10	= 0x00020000,	//	|
	GIL_LANG11	= 0x00040000,	//	|
	GIL_LANG12	= 0x00080000,	//	|
	GIL_LANG13	= 0x00100000,	//	|
	GIL_LANG14	= 0x00200000,	//	|
	GIL_LANG15	= 0x00400000,	//	|
	GIL_LANG16	= 0x00800000,	//	|

	GIL_LANG17	= 0x01000000,	//	|
	GIL_LANG18	= 0x02000000,	//	|
	GIL_LANG19	= 0x04000000,	//	|
	GIL_LANG20	= 0x08000000,	//	|===> Right To Left Languages
	GIL_LANG21	= 0x10000000,	//	|
	GIL_LANG22	= 0x20000000,	//	|
	GLI_ARABIC  = 0x40000000,	//	|
	GIL_PERSIAN = 0x80000000	//	|
};

//! describe font information
struct GUIFontDesc
{
	sint	size;			//  size of the font
	sint	charCount;		//  number of characters
	sint	outline;		//  outline thickness
	sint	lineHeight;		//	distance in pixels between each line of text  
	sint	charBase;		//	number of pixels from the absolute top of the line to the base of the characters

	GUIFontDesc(): size(0), charCount(0), outline(0), lineHeight(0), charBase(0) {};
};

//! describe character information
struct GUIFontChar
{
	dword id;
	sint x, y, width, height, xOffset, yOffset, xAdvance, page;

	GUIFontChar(): id(0), x(0), y(0), width(0), height(0),	xOffset(0),	yOffset(0),	xAdvance(0), page(0) {};
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
	virtual void Update( float elpsTime );

public:

	GUIType				m_type;
	str128				m_name;
	dword				m_option;
	float2				m_size;
	float3				m_position;
	float3				m_position_offset;
	float3				m_rotation;
	float3				m_rotation_offset;
	float3				m_scale;
	float3				m_scale_offset;
	uiElement			m_element[SX_GUI_MAX_ELEMENT];
	uiControl*			m_parent;
	Array<uiControl*>	m_child;

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
	uiControl* CreateContorl( const GUIType type );

	//! copy the src element to the dest element in the given index position
	void Copy( uiElement* dest, uint& index, const uiElement* src, const GUIBatchMode mode );

	//! prepare for batching elements. if mode was not simple the only element's with 4 vertices will accepted to the batch
	void BeginBatchElements( const GUIBatchMode mode, const uint count );

	//! add an element to the batch and return false if can't batch given element with another
	bool AddBatchElements( const uiElement* elem );

	//! end patch and append them to the end of dest element
	void EndBatchElements( uiElement* dest );

public:

	GUIBatchMode		m_batchMode;	//! batch mode 
	Array<uiElement*>	m_batches;		//!	us in batch system
	float				m_minAlpha;		//!	minimum alpha value that controls can be shown
	float				m_minScale;		//! minimum scale value that controls can be shown
	float				m_zbias;		//! prevent rubbing surfaces of GUI controls in 3D mode
	bool				m_editor;		//! indicates that GUI is running in editor mode
};


//! GUI post properties
#define _SX_GUI_NOT_VISIBLE_			0x10000000
#define _SX_GUI_NOT_ENABLE_				0x20000000
#define _SX_GUI_IN_3DSPACE_				0x40000000



#endif	//	GUARD_uiDevice_HEADER_FILE
