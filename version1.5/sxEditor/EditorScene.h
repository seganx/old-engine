/********************************************************************
	created:	2011/04/12
	filename: 	EditorScene.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain scene editor to edit scene of the level
*********************************************************************/
#ifndef GUARD_EditorScene_HEADER_FILE
#define GUARD_EditorScene_HEADER_FILE

#include "PopupMenu.h"
#include "Form_SceneToolbar.h"
#include "Form_EditNode.h"
#include "Form_EditMesh.h"
#include "Form_EditPath.h"
#include "Form_EditTerrain.h"
#include "Form_EditSound.h"
#include "Form_EditParticle.h"
#include "Form_NodeList.h"
#include "EditorUndo.h"

class EditorScene
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

public:

	//! return current camera
	static sx::core::Camera& GetCurrentCamera(void);

	//! reset camera position
	static void ResetCamera(void);

	//! reload first camera
	static void ReloadCamera(void);

	static PForm_SceneToolbar		frm_Toolbar;			//	use as toolbar of the application
	static PForm_EditNode			frm_EditNode;			//  use node editor to edit node transformation
	static PForm_EditMesh			frm_EditMesh;			//  use mesh editor to edit mesh
	static PForm_EditPath			frm_EditPath;			//  use path editor to edit path nodes
	static PForm_EditTerrain		frm_EditTrrn;			//  use path editor to edit terrain nodes
	static PForm_EditSound			frm_EditSound;			//	use to edit sounds
	static PForm_EditParticle		frm_EditParticle;		//	use to edit particle
	static Form_NodeList*			frm_NodeList;			//	list of nodes in the scene

	static PPopupMenu				pop_RightClick;			//  popup menu will show in right click

	static UndoManager				mng_UndoManager;		//  undo manager object
	static sx::core::ArrayPNode		selectedNodes;			//  selected node of object editor
	static sx::core::PNodeMember	selectedMember;			//  selected node of object editor

	static DWORD					draw_Option;			//  options of drawing node
	static DWORD					mouse_state;			//  state of mouse in editor
	static DWORD					mouse_select_type;		//  type of objects to select by mouse
};

#endif	//	GUARD_EditorScene_HEADER_FILE
