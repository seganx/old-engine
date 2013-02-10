/********************************************************************
	created:	2010/12/17
	filename: 	sxEditBox.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of edit box. this class will
				inherit from label to show the text and some additional
				function to update text by the user input
*********************************************************************/
#ifndef GUARD_sxEditBox_HEADER_FILE
#define GUARD_sxEditBox_HEADER_FILE

#include "sxLabel.h"

namespace sx { namespace gui {

	//! the TextEdit control can show texts in a specified box and some feature to edit text by user input
	class SEGAN_API TextEdit: public Label
	{
		SEGAN_STERILE_CLASS(TextEdit);

	protected:
		SEGAN_GUI_CLASS_IMPLEMENT();

	public:
		TextEdit(void);
		virtual ~TextEdit(void);

		void SetSize(float2 S);

		void Update(float elpsTime);

		void Draw(DWORD option);

		void DrawOutline(void);

		//! set new text to edit
		void SetText(const WCHAR* text);

		//! draw a simple cursor to show the caret position
		void DrawCaret(DWORD option);

		//! draw selection boxes to show selected characters
		void DrawSelection(DWORD option);

		//! set a new caret position
		void  SetCaretPos(int Ln, int Col);

		//! return the point of caret position
		PointI GetCaretPos(void);

		//! set input language. it can be one or more combination of GIL_
		void SetInputLanguages(GUIInputLanguage inputLang);

		//! insert text to the text editor
		void TextInsert(PointI& p, const WCHAR* text);

		//! delete text from text editor
		void TextDelete(PointI& p);

		//! delete text from text editor
		void TextDelete(PointI from, PointI to);

		//! set value of the vertical and horizontal text scroll
		void SetScrollValue(int x, int y);

		//! copy whole/selected text
		void Copy(void);

		//! insert pasted text to the current position
		void Paste(void);

	protected:
		/*return index of element contacted by mouse ray. return -1 if no contact
		NOTE: this function will called on update.*/
		int	MouseOver(float absX, float absY);

		/*! call this function to pass keyboard events to gui
		set KeyCode in LOWORD and set extended KeyCode in HIWORD */
		DWORD	OnKeyDown(DWORD keyCode);

		/*! call this function to pass keyboard events to gui
		set KeyCode in LOWORD and set extended KeyCode in HIWORD */
		DWORD	OnKeyPress(DWORD KeyCode);

		//! process keys from hardware input device
		int	ProcessKeyboardInput(const char* pKeys, GUICallbackInputToChar InputCharFunc = NULL);
		void processInputMouse(bool& result, int playerID);

		//! this function traverse the text in the line to recalculate line width and cut/add the tail of the text
		void UpdateLine(int LineIndex);

		//! prepare and form the buffers by texts
		void BurnBuffer(void);

		//! set selection points
		void SetSelection(const int2 start, const int2 end);

	private:
		PointI GetCaretFromPixel(float px, float py);
		bool   CaretIsOnRTLText(PointI& p, OUT int& fromIndex, OUT int& toIndex);
		void   UpdateCaretOnPixel(void);

		PointI		m_CaretPos;		//  the position of the caret
		PointI		m_selStart;		//  the start position of the selection
		PointI		m_selEnd;		//	the end position of the selection
		int			m_selCount;		//  number of character that has been selected
		DWORD		m_Language;		//  hold the allowed language for input text. use the first byte to denote language index

	};
	typedef TextEdit *PTextEdit;

}}	//	namespace sx { namespace gui {

#endif	//	GUARD_sxEditBox_HEADER_FILE