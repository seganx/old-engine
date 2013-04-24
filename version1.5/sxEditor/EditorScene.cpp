#include "EditorScene.h"
#include "MainEditor.h"
#include "EditorObject.h"
#include "EditorUI.h"
#include "EditorSettings.h"
#include "EditorCamera.h"

using namespace sx;

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES
//////////////////////////////////////////////////////////////////////////
static int						camIndex = 0;
static EditorCamera				cam[8];			//  cameras of this scene

//////////////////////////////////////////////////////////////////////////
//	CLASS AND FUNCTION IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////
PForm_SceneToolbar		EditorScene::frm_Toolbar		= NULL;
PForm_EditNode			EditorScene::frm_EditNode		= NULL;
PForm_EditMesh			EditorScene::frm_EditMesh		= NULL;
PForm_EditPath			EditorScene::frm_EditPath		= NULL;
PForm_EditTerrain		EditorScene::frm_EditTrrn		= NULL;
PForm_EditSound			EditorScene::frm_EditSound		= NULL;
PForm_EditParticle		EditorScene::frm_EditParticle	= NULL;
PPopupMenu				EditorScene::pop_RightClick		= NULL;

sx::core::PNodeMember	EditorScene::selectedMember		= NULL;
sx::core::ArrayPNode	EditorScene::selectedNodes;

DWORD	EditorScene::draw_Option			= 0;//SX_DRAW_DEBUG | SX_DRAW_BOUNDING_BOX /*| SX_DRAW_BOUNDING_SPHERE*/ /*| SX_DRAW_BONES*/;
DWORD	EditorScene::mouse_state			= EDITOR_MOUSESTATE_MOVE;
DWORD	EditorScene::mouse_select_type		= NMT_ALL;

//////////////////////////////////////////////////////////////////////////
//	INTERNAL CLASSES
//////////////////////////////////////////////////////////////////////////
void callbackUndo( UndoManager::UndoAction* action )
{
	switch (action->type)
	{
	case UAT_CREATE:
		{
			if( action->nodes.Count() )
			{
				EditorScene::selectedNodes.Clear();
				EditorScene::selectedMember = NULL;
			}

			for (int i=0; i<action->nodes.Count(); i++)
			{
				sx::core::PNodeMember pmember = action->nodes[i]->GetMemberByIndex(0);
				if ( pmember && pmember->GetType() == NMT_PATHNODE )
				{
					sx::core::PPathNode ppath = (sx::core::PPathNode)pmember;
					if ( ppath->m_Neighbor.Count() )
					{
						ppath = ppath->m_Neighbor[0];
						EditorScene::selectedMember = ppath;
						EditorScene::selectedNodes.PushBack( ppath->GetOwner() );
					}
				}
				sx::core::Scene::DeleteNode( action->nodes[i] );
			}
		}
		break;

	case UAT_DELETE:
		{
			if( action->nodes.Count() )
			{
				EditorScene::selectedNodes.Clear();
				EditorScene::selectedMember = NULL;
			}

			for (int i=0; i<action->nodes.Count(); i++)
			{
				sx::core::Scene::AddNode( action->nodes[i] );
				EditorScene::selectedNodes.PushBack( action->nodes[i] );

				//  additional operation
				msg_SoundPlay msgSound(true, 0, SX_SOUND_PLAYONLOAD);
				action->nodes[i]->MsgProc( MT_SOUND_PLAY, &msgSound );
			}
		}
		break;
	}
}
UndoManager EditorScene::mng_UndoManager(callbackUndo);

static MemoryStream	copyStream;		//  use to copy node and members
class Internal_Form_Scene : public sx::gui::Form
{
public:
	void OnNodeExplorerSelect(gui::PControl Sender)
	{
		EditorScene::selectedMember	= EditorScene::frm_EditNode->GetSelectedMember();
	}

	void popCopy(void)
	{
		copyStream.Clear();

		int n = EditorScene::selectedNodes.Count();
		if ( n > 1 )		//  copy multiple nodes
		{
			//  write number of nodes
			SEGAN_STREAM_WRITE(copyStream, n);

			for (int i=0; i<n; i++)
			{
				EditorScene::selectedNodes[i]->Save( copyStream );
			}
		}
		else if ( n == 1 )	//  copy one node
		{
			EditorObject::Copy( EditorScene::selectedNodes[0], NULL );
		}

	}

	void popPaste(void)
	{
		sx::core::PNode	node = NULL;
		EditorScene::selectedNodes.Clear();

		if ( copyStream.Size() )	//  paste multiple nodes
		{
			copyStream.SetPos(0);
			int n=0;

			//  read number of nodes
			SEGAN_STREAM_READ(copyStream, n);
			
			for (int i=0; i<n; i++)
			{
				node = sx_new( sx::core::Node );
				node->Load(copyStream);

				//  add selected node
				core::Scene::AddNode( node );
				EditorScene::selectedNodes.PushBack( node );
				EditorScene::selectedMember = NULL;
			}

			if ( n )
				EditorScene::mng_UndoManager.AddAction( UAT_CREATE, &EditorScene::selectedNodes[0], EditorScene::selectedNodes.Count() );
		}
		else
		{
			EditorObject::Paste( node );
			if (node)
			{
				core::Scene::AddNode( node );
				EditorScene::selectedNodes.PushBack( node );
				EditorScene::selectedMember = NULL;
				EditorScene::mng_UndoManager.AddAction( UAT_CREATE, &node, 1);
			}
		}
	}

	void OnPopupRightClick(gui::PControl Sender)
	{
		int index = EditorScene::pop_RightClick->Listbox().GetItemIndex();

		switch (index)
		{
		case 0:	//	copy node
			{
				popCopy();
			}
			break;

		case 1:	//	paste node
			{
				popPaste();
			}
			break;
		}

		EditorScene::pop_RightClick->Close();
	}

};
Internal_Form_Scene frm_Internal;



//////////////////////////////////////////////////////////////////////////
//	CLASS AND FUNCTION IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////

void EditorScene::Initialize( void )
{
	frm_Toolbar	=		sx_new( Form_SceneToolbar );
	frm_EditNode =		sx_new( Form_EditNode );
	frm_EditMesh =		sx_new( Form_EditMesh );
	frm_EditPath =		sx_new( Form_EditPath );
	frm_EditTrrn =		sx_new( Form_EditTerrain );
	frm_EditSound =		sx_new( Form_EditSound );
	frm_EditParticle =	sx_new( Form_EditParticle );

	//  set functions
	frm_EditNode->GetNodeExplorer()->m_Listbox.SetOnMouseClick( &frm_Internal, (GUICallbackEvent)&Internal_Form_Scene::OnNodeExplorerSelect );
	frm_EditNode->GetNodeExplorer()->m_Listbox.SetOnKeyDown( &frm_Internal, (GUICallbackEvent)&Internal_Form_Scene::OnNodeExplorerSelect );

	//  create popup for right click
	pop_RightClick = sx_new( PopupMenu );
	pop_RightClick->SetSize( 100.0f, 60.0f );
	pop_RightClick->Listbox().Add( L"Copy",		EditorUI::GetTexture(L"Copy") );
	pop_RightClick->Listbox().Add( L"Paste",	EditorUI::GetTexture(L"Paste") );
	pop_RightClick->SetOnSelect( &frm_Internal, (GUICallbackEvent)&Internal_Form_Scene::OnPopupRightClick );

	frm_EditNode->SetUndoManager( &mng_UndoManager );
}

void EditorScene::Finalize( void )
{
	sx::core::Scene::Cleanup();

	sx_delete( pop_RightClick );

	sx_delete( frm_Toolbar );
	sx_delete( frm_EditNode );
	sx_delete( frm_EditMesh );
	sx_delete( frm_EditPath );
	sx_delete( frm_EditTrrn );
	sx_delete( frm_EditSound );
	sx_delete( frm_EditParticle );
}

void EditorScene::Resize( int width, int height )
{
	//  resize forms which needs to be resized
	frm_Toolbar->OnResize(	width, height );
	frm_EditNode->OnResize( width, height );
	frm_EditMesh->OnResize( width, height );
	frm_EditPath->OnResize( width, height );
	frm_EditTrrn->OnResize( width, height );
}

void EditorScene::Update( float elpsTime, bool& inputHandled )
{

	//////////////////////////////////////////////////////////////////////////
	//	UPDATE INPUT
	sx::gui::PControl focusedControl = sx::gui::Control::GetFocusedControl();
	if ( focusedControl && focusedControl->GetType() == GUI_TEXTEDIT )
	{
		bool iohandled = false;
		focusedControl->ProcessInput( iohandled );
		char* keys = (char*)sx::io::Input::GetKeys(0);//(char*)pKeys;
		keys[SX_INPUT_KEY_MOUSE_RIGHT] = SX_INPUT_STATE_NORMAL;
		keys[SX_INPUT_KEY_N] = keys[SX_INPUT_KEY_E] = 
			keys[SX_INPUT_KEY_LCONTROL] = keys[SX_INPUT_KEY_RCONTROL] = 
			keys[SX_INPUT_KEY_LSHIFT] = keys[SX_INPUT_KEY_RSHIFT] = 
			keys[SX_INPUT_KEY_LALT] = keys[SX_INPUT_KEY_RALT] = 
			SX_INPUT_STATE_NORMAL;
	}

	if ( !inputHandled )
	{
		if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_A) )
		{
			selectedNodes.Clear();
			sx::core::Scene::GetAllNodes(selectedNodes);
			for ( int i=0; i<selectedNodes.Count(); i++ )
			{
				if ( !selectedNodes[i]->MemberTypeExist( mouse_select_type ) )
				{
					selectedNodes.RemoveByIndex(i);
					i--;
				}
			}
		}
		else if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_C) )
		{
			frm_Internal.popCopy();
			inputHandled = true;
		}
		else if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_V) )
		{
			frm_Internal.popPaste();
			inputHandled = true;
		}
		else if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_Z) )
		{
			mng_UndoManager.Undo();
			inputHandled = true;
		}
		else if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_R) )
		{
			ResetCamera();
			inputHandled = true;
		}
	}

	if ( !inputHandled && SEGAN_KEYUP(0, SX_INPUT_KEY_MOUSE_RIGHT) ) 	//  show the popup menu
	{
		pop_RightClick->Popup( SEGAN_MOUSE_ABSX(0) - SEGAN_VP_WIDTH/2,  SEGAN_VP_HEIGHT/2 - SEGAN_MOUSE_ABSY(0) );
		inputHandled = true;
	}

	if ( !inputHandled )
	{
		//  edit selected objects
		if (selectedNodes.Count()==1 && SEGAN_KEYDOWN(0, SX_INPUT_KEY_N) )
		{
			frm_EditNode->Show();
		}

		if ( selectedMember && SEGAN_KEYDOWN(0, SX_INPUT_KEY_E) )
		{
			if ( selectedMember->GetType() == NMT_MESH )
				frm_EditMesh->Show();
			else if ( selectedMember->GetType() == NMT_PATHNODE )
				frm_EditPath->Show();
			else if ( selectedMember->GetType() == NMT_SOUND )
				frm_EditSound->Show();
			else if ( selectedMember->GetType() == NMT_PARTICLE )
				frm_EditParticle->Show();
		}
	}

	pop_RightClick->ProcessInput(inputHandled);
	frm_Toolbar->ProcessInput(inputHandled);
	frm_EditMesh->ProcessInput(inputHandled);
	frm_EditParticle->ProcessInput(inputHandled);
	frm_EditSound->ProcessInput(inputHandled);
	frm_EditPath->ProcessInput(inputHandled);
	frm_EditTrrn->ProcessInput(inputHandled);
	frm_EditNode->ProcessInput(inputHandled);


	cam[camIndex].Update_multi(elpsTime, inputHandled, selectedNodes, selectedMember);
	
	//////////////////////////////////////////////////////////////////////////
	//	SELECT OBJECT FROM SCENE
	frm_EditPath->Operate(inputHandled);
	frm_EditTrrn->Operate(inputHandled);

	if ( !inputHandled )
	{
		// select object from scene
		if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT)	&& 
			!SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL)	&& 
			!SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL)	)
		{
			//  create ray for intersection test
			Matrix matView, matProj;
			cam[camIndex]().GetViewMatrix(matView);
			cam[camIndex]().GetProjectionMatrix(matProj);
			math::Ray ray( SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0), SEGAN_VP_WIDTH, SEGAN_VP_HEIGHT, matView, matProj );
			
			//  select object if mouse state is in select mode
			if ( (mouse_state & EDITOR_MOUSESTATE_SELECT) || (mouse_state & EDITOR_MOUSESTATE_MOVE) )
			{
				msg_IntersectRay msgRay(mouse_select_type, ray, msg_IntersectRay::GEOMETRY, NULL);
				sx::core::Scene::GetNodeByRay(msgRay);
				sx::core::PNode snode = (sx::core::PNode)msgRay.results[0].node;
				sx::core::PNodeMember smember = (sx::core::PNodeMember)msgRay.results[0].member;

				//  TODO : 
				selectedMember = smember;

				if ( snode )
				{
					//  received node may be a child. we should get parent here
					while ( snode && snode->GetParent() )
						snode = snode->GetParent();

					//  multiple select ?
					if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LSHIFT) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RSHIFT) )
					{
						if ( selectedNodes.IndexOf( snode )>-1 )
							selectedNodes.Remove( snode );
						else
							selectedNodes.PushBack( snode );
					}
					else
					{
						selectedNodes.Clear();
						selectedNodes.PushBack(snode);
					}
				}
				else
					selectedNodes.Clear();
			}
		}// if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) )

	}//  if ( !inputHandled ) 

	//////////////////////////////////////////////////////////////////////////
	//  UPDATE SCENE

	//  update other forms and objects
	if( selectedNodes.Count() > 1 )
	{
		frm_EditSound->SetSoundToEdit(NULL);
		frm_EditParticle->SetParticleToEdit(NULL);
		frm_EditMesh->SetMeshToEdit(NULL);
		frm_EditNode->UpdateEditorData_multiple( selectedNodes, cam[camIndex](), mouse_state );
	}
	else if( selectedNodes.Count() == 1 )
	{
		frm_EditSound->SetSoundToEdit(selectedMember);
		frm_EditParticle->SetParticleToEdit(selectedMember);
		frm_EditMesh->SetMeshToEdit(selectedMember);
		frm_EditNode->UpdateEditorData( selectedNodes[0], NULL, cam[camIndex](), mouse_state );
	}
	else
	{
		frm_EditNode->UpdateEditorData( NULL, NULL, cam[camIndex](), mouse_state );
		frm_EditMesh->SetMeshToEdit(NULL);
		frm_EditSound->SetSoundToEdit(NULL);
		frm_EditParticle->SetParticleToEdit(NULL);
	}

	//  update gui
	pop_RightClick->Update(elpsTime);
	frm_Toolbar->Update(elpsTime);
	frm_EditMesh->Update(elpsTime);
	frm_EditNode->Update(elpsTime);
	frm_EditPath->Update(elpsTime);
	frm_EditTrrn->Update(elpsTime);
	frm_EditSound->Update(elpsTime);
	frm_EditParticle->Update(elpsTime);

	// TEST
	{
		float3 vcam = cam[camIndex]().At - cam[camIndex]().Eye;
		str1024 str;
		str.Format
			(
			L"camera dis: %.2f | %s | nodes: %d | selected: %s", 
			vcam.Length(),
			Editor::dbg_Lable->GetText(),
			sx::core::Scene::Count(),
			selectedNodes.Count()==1 ? selectedNodes[0]->GetName() : L"..."
			);
		Editor::dbg_Lable->SetText(str);
	}

	core::Renderer::Update(elpsTime);
}

void EditorScene::Render( float elpsTime )
{
	FogDesc fog;
	sx::d3d::Device3D::GetFogDesc(fog);
	sx::d3d::Device3D::Clear_Screen(fog.Color);

	// set selected camera
	sx::core::PCamera pCamera = &cam[camIndex]();
	pCamera->Far = 6000.0f;
	pCamera->Aspect = SEGAN_VP_WIDTH / SEGAN_VP_HEIGHT;
	pCamera->FOV = PI / (1.7f + pCamera->Aspect);
	sx::core::Renderer::SetCamera( pCamera );
	SoundListener soundListener( pCamera->Eye, pCamera->GetDirection(), pCamera->Up );
	sx::snd::Device::SetListener( soundListener );

	sx::d3d::Device3D::RS_AntialiasLine(false);

	//  render the meshes in the scene
	if ( draw_Option & SX_DRAW_WIRED )
		core::Renderer::RenderScene( SX_DRAW_MESH | SX_DRAW_TERRAIN | SX_SHADER_SHADOW | SX_DRAW_WIRED );
	else
		core::Renderer::RenderScene( SX_DRAW_PARTICLE | SX_DRAW_MESH | SX_DRAW_TERRAIN | SX_SHADER_SHADOW );

	//  draw grid if needed
	if ( EditorSettings::GetGridVisible() )
		sx::d3d::UI3D::DrawGrid( EditorSettings::GetGridSize(), EditorSettings::GetGridColor() );

	//  render other things
	sx::d3d::Device3D::RS_ZWritable(false);
	sx::d3d::Device3D::RS_AntialiasLine(true);

	if ( frm_EditPath->IsVisible() )
	{
		SEGAN_SET_ADD( draw_Option, SX_DRAW_PATH);
	}
	else
	{
		SEGAN_SET_REM( draw_Option, SX_DRAW_PATH);
	}


	if ( draw_Option )
		core::Renderer::RenderScene( draw_Option | SX_DRAW_WIRED );

	sx::d3d::Device3D::RS_AntialiasLine(false);
	sx::d3d::Device3D::RS_ZWritable(true);
	

	//  render scene manager system
	//core::Scene::Debug_Draw( float3(0,0,0), 500.0f, SX_DRAW_BOUNDING_SPHERE | SX_DRAW_WIRED );

	if ( selectedNodes.Count() )
	{
		//  draw some debug information of selected node
		sx::d3d::Device3D::RS_ZWritable(false);
		for (int i=0; i<selectedNodes.Count(); i++)
		{
			selectedNodes[i]->Draw( SX_DRAW_DEBUG | SX_DRAW_BOUNDING_BOX | SX_DRAW_WIRED | SX_DRAW_SELECTED | SX_DRAW_DEBUG );
		}
		sx::d3d::Device3D::RS_ZWritable(true);
	}

	Editor::RenderCompass(elpsTime);

	// draw forms
	frm_EditNode->Draw(0);
	frm_EditSound->Draw(0);
	frm_EditParticle->Draw(0);
	frm_EditMesh->Draw(0);
	frm_EditPath->Draw(0);
	frm_EditTrrn->Draw(0);
	frm_Toolbar->Draw(0);
	pop_RightClick->Draw(0);

	static UINT iTime = 0;
	iTime++;
	if (iTime==20)
	{
		frm_Toolbar->Show();
	}

}

sx::core::Camera& EditorScene::GetCurrentCamera( void )
{
	return cam[camIndex]();
}

void EditorScene::ResetCamera( void )
{
	cam[camIndex].Reset();
}

void EditorScene::ReloadCamera( void )
{
	cam[0].Reload();
}
