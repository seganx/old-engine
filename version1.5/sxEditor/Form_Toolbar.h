/********************************************************************
	created:	2011/04/15
	filename: 	Form_Toolbar.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain class toolbar form which contain some
				buttons to manage application
*********************************************************************/
#ifndef GUARD_Form_Toolbar_HEADER_FILE
#define GUARD_Form_Toolbar_HEADER_FILE

#include "BaseForm.h"

#define FORM_TOOLBAR_SIZE_H	50.0f		//  size of the toolbar form

/*!
tool bar form will use in upper part of windows application to manage editor
*/
class Form_Toolbar: public BaseForm
{

public:
	Form_Toolbar(void);
	virtual ~Form_Toolbar(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form explorer
	void Update(float elpsTime);

	//! will call by editor to set the size of the from depend of size of the editor
	void OnResize(int EditorWidth, int EditorHeight);

private:

};
typedef Form_Toolbar *PForm_Toolbar;

#endif	//	GUARD_Form_Toolbar_HEADER_FILE

