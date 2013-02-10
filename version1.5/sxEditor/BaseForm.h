/********************************************************************
	created:	2011/02/07
	filename: 	BaseForm.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the base editor forms that contain
	two round corner and some properties to move by mouse
	and some things else
*********************************************************************/
#ifndef GUARD_EditorForm_HEADER_FILE
#define GUARD_EditorForm_HEADER_FILE

#include "ImportEngine.h"

#define FORM_OPTION_RESIZABLE	0x00000001			//  the form can resize by holding left mouse button on the bottom right
#define FORM_OPTION_MOVABLE		0x00000002			//  the form can move by holding left mouse button
#define FORM_OPTION_ALONE		0x00000004			//	the form will not arrange and manage with the others
#define FORM_OPTION_ONTOP		0x00000008			//	the form is always show on top

class BaseForm : public sx::gui::Form
{

public:

	BaseForm(void);
	virtual ~BaseForm(void);

	//! get current size of the form
	float2 GetSize(void);

	//! set size of the form
	virtual void SetSize(float width, float height);

	//! process input should be call before update
	virtual void ProcessInput(bool& inputHandled);

	//! update should be call after process input
	virtual void Update(float elpsTime);

	//! operate should be call before draw
	virtual void Operate(float elpstime, bool& inputHandled);

	//! draw the form and it's childes
	virtual void Draw(DWORD flag);

	//! show the form
	virtual void Show(void);

	//! close the form
	virtual void Close(void);

	//! return true if form is visible
	bool IsVisible(void);

	//! return the background panel
	sx::gui::PPanelEx GetBack(void);

private:

	void OnMove(sx::gui::PControl Sender);
	void OnClose(sx::gui::PControl Sender);

protected:
	
	DWORD					m_Option;		//  some options of the form
	sx::math::float2		m_MaxSize;		//  maximum size of form
	sx::math::float2		m_MinSize;		//  minimum size of form

	sx::gui::PPanelEx		m_pBack;		//  the main panel of the form
	sx::gui::PPanel			m_pTop;			//  the simple panel on the top
	sx::gui::PPanel			m_pBottom;		//  the simple panel on the bottom
	sx::gui::PPanel			m_pLeft;		//  the simple panel on the left
	sx::gui::PPanel			m_pRight;		//  the simple panel on the right
	sx::gui::PPanel			m_pTopLeft;		//  the simple panel on the top left corner
	sx::gui::PPanel			m_pTopRight;	//  the simple panel on the top right corner
	sx::gui::PPanel			m_pBotLeft;		//  the simple panel on the bottom left corner
	sx::gui::PPanel			m_pBotRight;	//  the simple panel on the bottom right corner

	sx::gui::PButton		m_pClose;		//  close button on top right corner

	//////////////////////////////////////////////////////////////////////////
	//	FORM MANAGER
	//////////////////////////////////////////////////////////////////////////
public:

	class Manager
	{
	public:

		//! process input should be call before update
		static void ProcessInput(bool& inputHandled);

		//! update should be call after process input
		static void Update(float elpsTime);

		//! operate should be call before draw
		static void Operate(float elpsTime, bool& inputHandled);

		//! draw the form and it's childes
		static void Draw(DWORD flag);
	};

};
typedef BaseForm *PBaseForm;

#endif	//	GUARD_EditorForm_HEADER_FILE