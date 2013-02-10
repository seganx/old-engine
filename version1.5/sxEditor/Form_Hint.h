/********************************************************************
	created:	2011/12/10
	filename: 	Form_Hint.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a from to show hint in the mouse position
*********************************************************************/
#ifndef GUARD_Form_Hint_HEADER_FILE
#define GUARD_Form_Hint_HEADER_FILE


#include "BaseForm.h"

/*!
use to show hint of objects
*/
class Form_Hint: public BaseForm
{

public:
	Form_Hint(void);
	virtual ~Form_Hint(void);

	//! update the form
	void Update(float elpsTime);

private:

	sx::gui::PControl		m_pCaptured;		//  captured control to show hint
	sx::gui::PLabel			m_pTitle;			//	title of the form
	float					m_fTime;			//  time of display

};
typedef Form_Hint *PForm_Hint;

#endif	//	GUARD_Form_Hint_HEADER_FILE
