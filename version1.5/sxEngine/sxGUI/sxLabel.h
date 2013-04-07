/********************************************************************
	created:	2010/12/06
	filename: 	sxLabel.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a label class to show texts in a box
*********************************************************************/
#ifndef GUARD_sxLabel_HEADER_FILE
#define GUARD_sxLabel_HEADER_FILE

#include "sxControl.h"

namespace sx { namespace gui {

	//! forward declaration
	typedef class	Font		*PFont;

	//! line structure used to hold line data in the label and text editors
	class SEGAN_API GUITextLine
	{
	public:
		GUITextLine();
		virtual ~GUITextLine();

		//! return the character from string by index. return 0 if index was out of bound.
		WCHAR GetChar(int index);

		//! return true if the text pointer is front of color code `0xffffff`
		bool IsColorCode(const WCHAR* str);

		//! recalculate the width of the line
		int UpdateWidth(PFont textFont, bool reversed);

		//! compute number of characters committed to buffer
		int GetNumCharToBuffer(PFont textFont);

	public:
		str1024		text;
		int			width;
	};
	typedef GUITextLine *PGUITextLine;
	typedef Array<GUITextLine*>	GUILineList;



	//! the Label control can show texts in a specified box
	class SEGAN_API Label: public Control
	{
		SEGAN_STERILE_CLASS(Label);

	protected:
		SEGAN_GUI_CLASS_IMPLEMENT();

	public:
		Label(void);
		virtual ~Label(void);

		void Save( Stream& stream );
		void Load( Stream& stream );

		void SetSize(float2 S);

		//! add a new property 'SX_GUI_PROPERTY_'
		void AddProperty(DWORD prop);

		//! remove a property from current property set 'SX_GUI_PROPERTY_'
		void RemProperty(DWORD prop);

		//! draw label
		void Draw(DWORD option);

		//! call device API draw call
		void DrawText(DWORD option);

		//! set new text to edit
		void SetText(const WCHAR* text);

		//! return the text in the label
		const WCHAR* GetText(void);

		//! return the pointer to the line structure. return NULL for invalid index
		PGUITextLine GetLine(const int Index);

		//! set callback function to call when the text in the text box has been changed
		void SetOnTextChange(PForm pForm, GUICallbackEvent pFunc);

		//! set callback function for scrolling the editor
		void SetOnScrollChanged(PForm pForm, GUICallbackEvent pFunc);

		//! set font source for this edit box
		void SetFont(const WCHAR* fontSrc);

		//! return the font source
		const WCHAR* GetFontSource(void);

		//! set text alignment
		void SetAlign(GUITextAlign align);

		//! return text alignment
		GUITextAlign GetAlign(void);

		//! set value of the vertical and horizontal text scroll
		virtual void SetScrollValue(int x, int y);

		//! return value of the vertical and horizontal text scroll
		PointI GetScrollValue(void);

		//! return MAXIMUM value of the vertical and horizontal text scroll
		PointI GetScrollMaxValue(void);

	protected:
		//! reverse blocks of right to left texts
		void ReverseRTLText(str1024& in_str);

		//! recreate lines
		void CreateLines(void);

		//! recreate buffers
		void CreateBuffer(int MaxCharacter);

		//! release buffers
		void ReleaseBuffer(void);

		//! return true if the control can burn buffer
		bool CanBurnBuffer(void);

		//! prepare and form the buffers by texts
		virtual void BurnBuffer(void);
		
		GUILineList		m_Lines;
		String			m_Text;
		PFont			m_Font;
		GUITextAlign	m_Align;
		PointI			m_Scroll;					//	scroll of X axis and Y axis

		GUICallbackFuncPtr		m_OnTextChange;		//  call back function will call when text has been changed
		GUICallbackFuncPtr		m_OnScrollChange;	//  call back function will call when vertical/horizontal scrolled

		int						m_numBuffer;
		int						m_numChar;
		PDirect3DVertexBuffer	m_VB0;
		PDirect3DVertexBuffer	m_VB1;
		PDirect3DIndexBuffer	m_IB;
	};
	typedef Label *PLabel;

}}	//	namespace sx { namespace gui {

#endif	//	GUARD_sxLabel_HEADER_FILE
