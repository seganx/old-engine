/********************************************************************
	created:	2015/01/16
	filename: 	Form_NodeLsit.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of form show all nodes in scene
*********************************************************************/
#ifndef GUARD_Form_NodeList_HEADER_FILE
#define GUARD_Form_NodeList_HEADER_FILE


#include "BaseForm.h"
#include "sxListBox.h"

/*!
use to edit sound
*/
class Form_NodeList: public BaseForm
{

public:
	Form_NodeList(void);
	virtual ~Form_NodeList(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form
	void Update(float elpsTime);

	//! draw the form
	void Show(void);

private:

	void OnParamChange( sx::gui::PControl sender );		//  apply changes of an parameters
	void OnListChange( sx::gui::PControl sander );		//  list changed
	void ReloadList( void );							//	reload list

private:
	sx::gui::TextEdit*		m_name;
	sx::gui::ListBox*		m_list;
};

#endif	//	GUARD_Form_NodeList_HEADER_FILE
