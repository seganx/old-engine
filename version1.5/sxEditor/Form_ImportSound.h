/********************************************************************
	created:	2012/02/01
	filename: 	Form_ImportSound.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple form to import sound
*********************************************************************/
#ifndef GUARD_Form_ImportSound_HEADER_FILE
#define GUARD_Form_ImportSound_HEADER_FILE


#include "BaseForm.h"

/*!
use to import sound
*/
class Form_ImportSound: public BaseForm
{

public:
	Form_ImportSound(void);
	virtual ~Form_ImportSound(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form
	void Update(float elpsTime);

	//! call to import sound
	void Import(sx::gui::PForm pForm, GUICallbackEvent OnOkClick);

	//! return the address of imported file
	const WCHAR* GetImportedFileName(void);

private:

	void OnParamChange(sx::gui::PControl Sender);	//  apply changes of an parameters
	void LoadSound(sx::gui::PControl Sender);

private:

	sx::gui::PLabel			m_pTitle;		//	title of the form

	sx::gui::PCheckBox		m_ch_Stream;	//	import sound as music stream
	sx::gui::PCheckBox		m_ch_Sample;	//	import sound as sound sample
	sx::gui::PCheckBox		m_ch_is3D;		//	import 3d sound 

	sx::gui::PButton		m_btn_ok;		//  OK button
	sx::gui::PButton		m_btn_Cancel;	//  Cancel button

	String					m_fileName;		//  sound filename to import
	GUICallbackFuncPtr		m_OnOkClick;	//  call loading function
};
typedef Form_ImportSound *PForm_ImportSound;

#endif	//	GUARD_Form_ImportSound_HEADER_FILE
