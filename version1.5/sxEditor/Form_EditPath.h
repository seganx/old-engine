/********************************************************************
	created:	2011/07/07
	filename: 	Form_EditPath.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of form to edit path nodes
*********************************************************************/
#ifndef GUARD_Form_EditPath_HEADER_FILE
#define GUARD_Form_EditPath_HEADER_FILE

#include "BaseForm.h"
#include "sxListBox.h"

/*!
use to edit path nodes
*/
class Form_EditPath: public BaseForm
{

public:
	Form_EditPath(void);
	virtual ~Form_EditPath(void);

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

	//! assign path node to edit. call it frequently in loop to check changes.
	void SetPathToEdit(sx::core::PNodeMember path);

	//! this can be called frequently in suit place to do do the operation.
	void Operate(bool& inputHandled);

	//! show the window
	void Show(void);

private:

	void OnParamChange(sx::gui::PControl Sender);		//  apply changes of an parameters
	void OnLayerChange(sx::gui::PControl Sander);		//  layer changed
	bool CreateNode(Ray& ray);

private:

	sx::gui::PLabel			m_pTitle;			//	title of the form
	sx::gui::PButton		m_btnCreate;		//  use to create path
	sx::gui::PButton		m_btnLink;			//  use to link path
	sx::gui::PButton		m_btnUnlink;		//  use to unlink path
	sx::gui::PButton		m_btnFinish;		//  use to finish job
	sx::gui::PListBox		m_lsbLayer;			//  layer of path
	sx::core::PPathNode		m_path;				//  path to edit

	//! additional parameters
	DWORD					m_Mode;
};
typedef Form_EditPath *PForm_EditPath;

#endif	//	GUARD_Form_EditPath_HEADER_FILE
