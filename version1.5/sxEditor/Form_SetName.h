/********************************************************************
	created:	2011/06/19
	filename: 	Form_SetName.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a form to choose a name for new created object
*********************************************************************/
#ifndef GUARD_Form_SetName_HEADER_FILE
#define GUARD_Form_SetName_HEADER_FILE


#include "BaseForm.h"

/*! form of set name to set a name to nodes and members */
class Form_SetName: public BaseForm
{

public:
	Form_SetName(void);
	virtual ~Form_SetName(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form
	void Update(float elpsTime);

	//! draw the form and it's childes
	void Draw(DWORD flag);

	//! will call by editor to set the size of the from depend of size of the editor
	void OnResize(int EditorWidth, int EditorHeight);

	//! show the form
	void Show(void);

	//! close the form
	void Close(void);

	//! assign member to edit. call it frequently in loop to check changes.
	void UpdateEditorData(sx::core::PNode node, sx::core::PNodeMember member);

private:

	void OnParamChange(sx::gui::PControl Sender);

private:

	sx::core::PNode				m_pNode;			// pointer to selected node
	sx::core::PNodeMember		m_pMember;			// pointer to selected member

	sx::gui::PPanel				m_pPanel;			// use to lock background
	sx::gui::PLabel				m_pTitle;			// Title of the form

	sx::gui::PLabel				m_lb_memName;		// label of member name
	sx::gui::PTextEdit			m_ed_memName;		// set member name edit box
	sx::gui::PCheckBox			m_ch_newNode;		//  check box for create new node
	sx::gui::PLabel				m_lb_nodName;		// label of name name
	sx::gui::PTextEdit			m_ed_nodName;		// set node name edit box
	sx::gui::PButton			m_btn_Ok;			//  ok button

public:

	//! prepare name of objects
	static const WCHAR* CorrectName(const WCHAR* ObjectName);
};
typedef Form_SetName *PForm_SetName;

#endif	//	GUARD_Form_SetName_HEADER_FILE
