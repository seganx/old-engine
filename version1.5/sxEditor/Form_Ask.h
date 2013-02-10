/********************************************************************
	created:	2012/01/22
	filename: 	Form_Ask.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of form to show a dialog box 
				to ask confirmation of operations
*********************************************************************/
#ifndef GUARD_Form_Ask_HEADER_FILE
#define GUARD_Form_Ask_HEADER_FILE

#include "BaseForm.h"

#define ASK_
#define ASK_OK		0x00000001	// show button OK on ask form
#define ASK_CANCEL	0x00000002	// show button Cancel on ask form
#define ASK_YES		0x00000004	// show button Yes on ask form
#define ASK_NO		0x00000008	// show button No on ask form

/*!
use to show an ask form to get confirm
*/
class Form_Ask: public BaseForm
{

public:
	Form_Ask(void);
	virtual ~Form_Ask(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! Show ask form
	void Show(const WCHAR* caption, const WCHAR* desc, ASK_ DWORD buttons, sx::gui::PControl respondTo, sx::gui::PForm pForm, GUICallbackEvent ptrFunc);

private:

	void OnClick(sx::gui::PControl Sender);

	sx::gui::PButton	m_OK;			//	ok button
	sx::gui::PButton	m_Cancel;		//	cancel button
	sx::gui::PButton	m_Yes;			//	yes button
	sx::gui::PButton	m_No;			//	no button

	sx::gui::PLabel		m_pTitle;		//	title of the form
	sx::gui::PLabel		m_pDesc;		//  description of confirmation

	sx::gui::PControl	m_Sender;		//  hold sender to respond to
	GUICallbackFuncPtr	m_onClick;		//  use to handle clicks

};
typedef Form_Ask *PForm_Ask;

#endif	//	GUARD_Form_Ask_HEADER_FILE
