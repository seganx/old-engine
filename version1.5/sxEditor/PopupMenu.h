/********************************************************************
	created:	2011/06/23
	filename: 	PopupMenu.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple popup menu
*********************************************************************/
#ifndef GUARD_PopupMenu_HEADER_FILE
#define GUARD_PopupMenu_HEADER_FILE

#include "BaseForm.h"
#include "sxListBox.h"

//  use this class to present popup menus
class PopupMenu : public BaseForm
{

public:
	PopupMenu(void);
	virtual ~PopupMenu(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! update the form
	void Update(float elpsTime);

	//! set select event
	void SetOnSelect(sx::gui::PForm pForm, GUICallbackEvent pFunc);

	//! reference to the list box
	sx::gui::ListBox& Listbox(void);

	//! Popup the menu
	void Popup(float x, float y);

private:

	void OnMouseMove(sx::gui::PControl Sender);

	sx::gui::ListBox	m_listBox;

	//  additional parameters
	float m_fShowTime;
};
typedef PopupMenu *PPopupMenu;

#endif	//	GUARD_PopupMenu_HEADER_FILE