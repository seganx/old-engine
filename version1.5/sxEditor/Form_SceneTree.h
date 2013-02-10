/********************************************************************
	created:	2011/04/15
	filename: 	Form_SceneTree.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a form to show scene tree nodes
*********************************************************************/
#ifndef GUARD_Form_SceneTree_HEADER_FILE
#define GUARD_Form_SceneTree_HEADER_FILE

#include "BaseForm.h"
#include "sxListBox.h"

/*!
scene tree show the nodes in the scene and represent some features to manage them
*/
class Form_SceneTree: public BaseForm
{
	SEGAN_IMPLEMENT_STERILE_CLASS(Form_SceneTree);

public:
	Form_SceneTree(void);
	virtual ~Form_SceneTree(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& result, int playerID = 0);

	//! update the form explorer
	void Update(float elpsTime);

	//! update list in scene tree
	void UpdateEditorData(sx::core::ArrayPNode& nodeList, sx::core::PNode selectedNode, float elpsTime);

	//! commit changes. use for remove nodes and etc
	void UpdateSoon(void);

private:

	void OnSelectNode(sx::gui::PControl Sender);

	sx::gui::PLabel			m_pTitle;			// Title of the form
	sx::gui::ListBox		m_nodeList;			// list of nodes

};
typedef Form_SceneTree *PForm_SceneTree;

#endif	//	GUARD_Form_SceneTree_HEADER_FILE
