/********************************************************************
	created:	2011/04/10
	filename: 	Form_EditNode.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain form of edit node to edit node's properties
*********************************************************************/
#ifndef GUARD_Form_EditNode_HEADER_FILE
#define GUARD_Form_EditNode_HEADER_FILE

#include "BaseForm.h"
#include "NodeExplorer.h"
#include "EditorUndo.h"

/*!
node editor form to edit nodes properties
*/
class Form_EditNode: public BaseForm
{

public:
	Form_EditNode(void);
	virtual ~Form_EditNode(void);

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

public:

	//! assign node to edit. call it frequently in loop to check changes.
	void UpdateEditorData(sx::core::PNode node, sx::core::PNodeMember member, sx::core::Camera& camera, UINT mouseState);

	//! assign multiple nodes to edit. call it frequently in loop to check changes.
	void UpdateEditorData_multiple(sx::core::ArrayPNode& nodes, sx::core::Camera& camera, UINT mouseState);

	//! return pointer to the node explorer
	PNodeExplorer GetNodeExplorer(void);

	//! return selected node in node tree
	sx::core::PNode GetSelectedNode(void);

	//! return selected member
	sx::core::PNodeMember GetSelectedMember(void);

	//! set undo manager
	void SetUndoManager(UndoManager* undoManager);

	//! render simple pivot
	static void DrawPivot(sx::core::Camera& cam, sx::math::Matrix& matWorld, UINT selectedAxis, float fSize = 10.0f);

	//! return combination of mouse axis on pivot selected by user
	static UINT SelectPivotAxis(sx::core::Camera& cam, sx::math::Matrix& matWorld, float fSize = 10.0f);

private:

	void OnMouseWheel(sx::gui::PControl Sender);
	void OnParamChange(sx::gui::PControl Sender);

	sx::core::PNode			m_pNode;			// pointer to selected node to edit it
	sx::core::PNodeMember	m_pMember;			// pointer to selected member
	sx::gui::PLabel			m_pTitle;			// Title of the form
	sx::gui::PTextEdit		m_pEditName;		// text editor to edit name

	sx::gui::PTextEdit		m_drawOrder;		// text editor to edit draw order

	sx::gui::PLabel			m_pLabelPos;		// position label
	sx::gui::PTextEdit		m_pEditPos[3];		// Text edits to edit position

	sx::gui::PLabel			m_pLabelRot;		// rotation label
	sx::gui::PTextEdit		m_pEditRot[3];		// Text edits to edit rotation

	sx::gui::PButton		m_playSound;		// play all sounds
	sx::gui::PButton		m_sprayParticle;	// spray all particles

	sx::gui::PCheckBox		m_groupChange;		// change as local or group

	NodeExplorer			m_nodeExplorer;		// use to explore the node
	UndoManager	*			m_undoManager;		// pointer to undo manager of editor

	//  additional data
	sx::core::ArrayPNode*	m_Nodes;			// pointer to multiple nodes
	bool					m_applyChanges;		// boolean controller to set permission for apply changes of node to the editors objects
	UINT					m_mouseState;		// hold mouse states comes from editor

	sx::core::Camera		m_Camera;
};
typedef Form_EditNode *PForm_EditNode;

#endif	//	GUARD_Form_EditNode_HEADER_FILE
