/********************************************************************
	created:	2012/05/28
	filename: 	Form_EditAnimator.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a form to edit animator
*********************************************************************/
#ifndef GUARD_Form_EditAnimator_HEADER_FILE
#define GUARD_Form_EditAnimator_HEADER_FILE


#include "BaseForm.h"
#include "sxListBox.h"

/*!
use to edit animator
*/
class Form_EditAnimator: public BaseForm
{

public:
	Form_EditAnimator(void);
	virtual ~Form_EditAnimator(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form
	void Update(float elpsTime);

	//! draw the form
	void Draw(DWORD flag);

	//! will call by editor to set the size of the from depend of size of the editor
	void OnResize(int EditorWidth, int EditorHeight);

	//! assign animator to edit. call it frequently in loop to check changes.
	void SetAnimatorToEdit(sx::core::PNodeMember anim);

	//! this can be called frequently in suit place to do do the operation.
	void Operate(bool& inputHandled);

	//! show the window
	void Show(void);

private:

	void ReloadList( void );
	void OnParamChange( sx::gui::PControl Sender );		//  apply changes of an parameters
	void OnListChange( sx::gui::PControl Sander );		//  list changed
	void OnOpenAnimation( sx::gui::PControl Sander );
	void OnMouseWheel(sx::gui::PControl Sender);


private:

	sx::gui::PLabel			m_pTitle;			//	title of the form

	sx::gui::TextEdit*		m_animSpeed;
	sx::gui::TextEdit*		m_blendTime;
	sx::gui::ListBox*		m_animlist;
	sx::gui::PButton		m_addAnim;
	sx::gui::PButton		m_remAnim;
	sx::gui::PButton		m_clearList;
	sx::gui::PButton		m_moveUp;
	sx::gui::PButton		m_moveDown;
	sx::gui::PCheckBox		m_loop;

	sx::core::PAnimator		m_animator;

	bool					m_applyChanges;
};
typedef Form_EditAnimator *PForm_EditAnimator;

#endif	//	GUARD_Form_EditAnimator_HEADER_FILE
