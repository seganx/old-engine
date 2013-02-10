/********************************************************************
	created:	2010/11/24
	filename: 	sxTypesGUI.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some data types and enumeration types
				that used in gui.
*********************************************************************/
#ifndef GUARD_sxTypesGUI_HEADER_FILE
#define GUARD_sxTypesGUI_HEADER_FILE

#include "sxGUI_def.h"

//! these flags used as gui properties
#define SX_GUI_PROPERTY_VISIBLE			0x00000001		//  control is visible
#define SX_GUI_PROPERTY_ENABLE			0x00000002		//  control is enable
#define SX_GUI_PROPERTY_BINDTOPARENT	0x00000004		//  control binded to the parent
#define SX_GUI_PROPERTY_FREEZE			0x00000008		//  control is froze and can't be selected
#define SX_GUI_PROPERTY_3DSPACE			0x00000010		//  control will show in 3D space
#define SX_GUI_PROPERTY_PROCESSKEY		0x00000020		//	control can process keyboard input
#define SX_GUI_PROPERTY_WORDWRAP		0x00000040		//	label or edit control will break the long size lines to fit in box
#define SX_GUI_PROPERTY_PIXELALIGN		0x00000080		//	control origin will aligned to the pixels
#define SX_GUI_PROPERTY_MULTILINE		0x00000100		//	allow to the label / edit control to show multi line text
#define SX_GUI_PROPERTY_READONLY		0x00000200		//	the edit control can be selected but is read only and could not edit text
#define SX_GUI_PROPERTY_BLENDSTATES		0x00000300		//	allow to the multi state controls to blend states when change them
#define SX_GUI_PROPERTY_CLIPCHILDS		0x00000400		//  use clip plane to avoid drawing child out of control area
#define SX_GUI_PROPERTY_BLENDCHILDS		0x00000800		//  apply some current states to the childes. at this time only color alpha will be applied
#define SX_GUI_PROPERTY_IGNOREBLEND		0x00001000		//  ignore blending that comes from parent
#define SX_GUI_PROPERTY_ACTIVATE		0x00002000		//  make control active and can be selected my mouse.
#define SX_GUI_PROPERTY_AUTOSIZE		0x00004000		//	label or edit control will resize to fit the text.
#define SX_GUI_PROPERTY_PROGRESSCIRCLE	0x00008000		//	progress control will display as circle mode
#define SX_GUI_PROPERTY_PROGRESSUV		0x00010000		//	progress control will display as linear mode and UV alignment
#define SX_GUI_PROPERTY_BILLBOARD		0x00020000		//	control will display as billboard

//! these flags used in draw option of a gui control and it;s childes
#define SX_GUI_DRAW_SAVEMATRIX			0x00010000		//  save current view and projection matrices
#define SX_GUI_DRAW_JUST3D				0x00020000		//  just draw of gui has 3D space property
#define SX_GUI_DRAW_JUST2D				0x00040000		//  just draw if gui is 2D

//! some key code combination
#define SX_GUI_KEY_SHIFT				0x0001
#define SX_GUI_KEY_ALT					0x0002
#define SX_GUI_KEY_CTRL					0x0004
#define	SX_GUI_KEY_CAPITAL				0x0008

//! prevent rubbing surfaces
#define SX_GUI_Z_BIAS					-0.15f

//! gui post properties
#define _SX_GUI_NOT_VISIBLE_			0x10000000
#define _SX_GUI_NOT_ENABLE_				0x20000000
#define _SX_GUI_IN_3DSPACE_				0x40000000

//! gui bounding conditions
#define SX_GUI_MINIMUM_ALPHA			0.001f
#define SX_GUI_MINIMUM_SCALE			0.0001f

//! these are types of GUI controls
enum GUIControlType
{
	GUI_NONE,
	GUI_PANEL,
	GUI_BUTTON,
	GUI_CHECKBOX,
	GUI_TRACKBAR,
	GUI_PROGRESSBAR,
	GUI_LABEL,
	GUI_TEXTEDIT,
	GUI_PANELEX
};
#define GUI_

enum GUITextAlign {
	GTA_LEFT,
	GTA_RIGHT,
	GTA_CENTER
};
#define GTA_

//! the value of this enumerations should set with care. values lower than 0x01000000 used for LTR and the higher values used for RTL languages
enum GUIInputLanguage {
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
#define GIL_

//! describe font information
typedef struct GUIFontDesc
{
	int		Size;			//  size of the font
	int		CharCount;		//  number of characters
	int		Outline;		//  outline thickness
	int		LineHeight;		//	distance in pixels between each line of text  
	int		Base;			//	number of pixels from the absolute top of the line to the base of the characters

	GUIFontDesc(): Size(0), CharCount(0), Outline(0), LineHeight(0), Base(0) {};
}
*PGUIFontDesc;

//! describe character information
typedef struct GUIFontChar
{
	DWORD ID;
	int x, y, width, height, xOffset, yOffset, xAdvance, page;

	GUIFontChar(): ID(0), x(0), y(0), width(0), height(0),	xOffset(0),	yOffset(0),	xAdvance(0), page(0) {};
}
*PGUIFontChar;

// forward declaration
namespace sx { namespace gui {
	typedef class Control	*PControl;

	typedef class Form
	{
	public:	
		virtual ~Form(){};
	} *PForm;
}}	//	namespace sx { namespace gui {


//! this callback function will change input button to the character
typedef WORD (*GUICallbackInputToChar)(BYTE inputButton);

//! this is the standard GUI callback event.
typedef void (sx::gui::Form::*GUICallbackEvent)(sx::gui::PControl Sender);

//! this callback event would call when key down on GUI. return true if this event handled
//typedef bool (*GUICallbackKeyboard)(sx::gui::PControl Sender, DWORD KeyCode);
//typedef	GUICallbackKeyboard	GUICallbackKeyDown, GUICallbackKeyUp, GUICallbackKeyPress;

////! this is the callback event of mouse over on GUI
//typedef bool (*GUICallbackMouseOver)(sx::gui::Control* Sender, float X, float Y);

//! object structure used to assign gui member functions
struct SEGAN_API GUICallbackFuncPtr
{
	sx::gui::PForm		m_pForm;
	GUICallbackEvent	m_pFunc;

	GUICallbackFuncPtr();
	GUICallbackFuncPtr(sx::gui::PForm pForm, GUICallbackEvent pFunc);
	void operator ()(sx::gui::PControl pSender);

};


namespace sx { namespace gui {

	/*!
	use static variables of this class to control the gui behavior. it's very cool for editing.
	*/
	class SEGAN_API Globals
	{
	public:
		//! return the golden FOV number for GUI
		static float		Golden_FOV(void);

		//! return the golden Z for GUI
		static float		Golden_Z(void);

		//! reference to the offset of line in draw line ( control selected )
		static float&		Draw_Line_Offset();

		//!	reference to the color of the draw line ( control selected )
		static D3DColor&	Draw_Line_Color();

		//! referent to the gui system mode
		static bool&		Editor_Mode(void);
	};

	//! create and return gui control by given type
	PControl SEGAN_API Create(GUIControlType guiType);

	//! destroy and invalid gui control
	void SEGAN_API Destroy(PControl gui);

}} // namespace sx { namespace gui {

#define SEGAN_GUI_CLASS_IMPLEMENT()protected:			\
	friend PControl Create(GUIControlType guiType);		\
	friend void Destroy(PControl gui);					\
	
#define SEGAN_GUI_DELETE(gui) {Destroy(gui); gui=NULL;}

#define SEGAN_GUI_SET_ONSCROLL(control, ptrFunc)	control->SetOnScroll( (sx::gui::PForm)this, (GUICallbackEvent)&ptrFunc )
#define SEGAN_GUI_SET_ONCLICK(control, ptrFunc)		control->SetOnMouseClick( (sx::gui::PForm)this, (GUICallbackEvent)&ptrFunc )
#define SEGAN_GUI_SET_ONENTER(control, ptrFunc)		control->SetOnMouseEnter( (sx::gui::PForm)this, (GUICallbackEvent)&ptrFunc )
#define SEGAN_GUI_SET_ONEXIT(control, ptrFunc)		control->SetOnMouseExit(  (sx::gui::PForm)this, (GUICallbackEvent)&ptrFunc )
#define SEGAN_GUI_SET_ONTEXT(control, ptrFunc)		control->SetOnTextChange( (sx::gui::PForm)this, (GUICallbackEvent)&ptrFunc )
#define SEGAN_GUI_SET_ONWHEEL(control, ptrFunc)		control->SetOnMouseWheel( (sx::gui::PForm)this, (GUICallbackEvent)&ptrFunc )

#endif	//	GUARD_sxTypesGUI_HEADER_FILE