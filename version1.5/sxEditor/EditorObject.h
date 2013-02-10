/********************************************************************
	created:	2011/04/17
	filename: 	EditorObject.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain object editor to import/edit objects
*********************************************************************/
#ifndef GUARD_EditorObject_HEADER_FILE
#define GUARD_EditorObject_HEADER_FILE

#include "PopupMenu.h"
#include "Form_SetName.h"
#include "Form_ObjectToolbar.h"
#include "Form_EditNode.h"
#include "Form_EditMesh.h"
#include "Form_EditParticle.h"
#include "Form_EditSound.h"
#include "Form_EditAnimator.h"

class EditorObject
{
public:
	//! initialize the editor will called from editor.cpp
	static void Initialize(void);

	//! finalize the editor will called from editor.cpp
	static void Finalize(void);

	//! will called when window size has been changed to recreate necessary resources
	static void Resize(int width, int height);

	//! update the editor 
	static void Update(float elpsTime, bool& inputHandled);

	//! render the editor
	static void Render(float elpsTime);

	//! reset camera position
	static void ResetCamera(void);

	//! copy from existing object
	static void Copy(sx::core::PNode node, sx::core::PNodeMember member);

	//! create new object and set node as parent
	static void Paste(sx::core::PNode& node);

public:

	static PForm_ObjectToolbar		frm_Toolbar;			//	use as toolbar of the application
	static PForm_EditNode			frm_EditNode;			//  use node editor to edit node transformation
	static PForm_EditMesh			frm_EditMesh;			//	use mesh editor to edit materials of meshes
	static PForm_EditParticle		frm_EditParticle;		//	use particle editor to edit particle
	static PForm_EditSound			frm_EditSound;			//	use to edit sounds
	static PForm_EditAnimator		frm_EditAnimator;		//	use to edit animator

	static PPopupMenu				pop_Menu1;				//  list of some operation. copy / paste / move ...

	static sx::core::PNode			node_Root;				//  root node of the object editor
	static sx::core::PNode			selectedNode;			//  selected node of object editor
	static sx::core::PNodeMember	selectedMember;			//  selected node member of selected node

	static DWORD					draw_Option;			//  options of drawing node
	static DWORD					mouse_state;			//  state of mouse in editor
	static DWORD					mouse_select_type;		//  type of objects to select by mouse

	static sx::core::Shadow_direction m_Shadow;				//  use simple 512x512 shadow map
};

#endif	//	GUARD_EditorObject_HEADER_FILE