#include "EditorObject.h"
#include "MainEditor.h"
#include "EditorUI.h"
#include "EditorSettings.h"
#include "ImportMS3D.h"
#include "ImportTexture.h"
#include "EditorCamera.h"

using namespace sx;

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES
//////////////////////////////////////////////////////////////////////////
static int			camIndex = 0;
static EditorCamera cam[8];		//  cameras of this scene

static float3	hitPoint(0,0,0);				//  hit point of selection object
static float3	hitNorml(0,0,0);				//  normal vector of selection object

static MemoryStream	copyStream;		//  use to copy node and members

//////////////////////////////////////////////////////////////////////////
//	CLASS AND FUNCTION IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////

PForm_ObjectToolbar			EditorObject::frm_Toolbar			= NULL;
PForm_EditNode				EditorObject::frm_EditNode			= NULL;
PForm_EditMesh				EditorObject::frm_EditMesh			= NULL;
PForm_EditParticle			EditorObject::frm_EditParticle		= NULL;
PForm_EditSound				EditorObject::frm_EditSound			= NULL;
PForm_EditAnimator			EditorObject::frm_EditAnimator		= NULL;
PPopupMenu					EditorObject::pop_Menu1				= NULL;
sx::core::PNode				EditorObject::node_Root				= NULL;
sx::core::PNode				EditorObject::selectedNode			= NULL;
sx::core::PNodeMember		EditorObject::selectedMember		= NULL;

DWORD	EditorObject::draw_Option				= SX_DRAW_DEBUG | SX_DRAW_BOUNDING_BOX /*| SX_DRAW_BOUNDING_SPHERE*/ /*| SX_DRAW_BONES*/;
DWORD	EditorObject::mouse_state				= EDITOR_MOUSESTATE_MOVE;
DWORD	EditorObject::mouse_select_type			= NMT_MESH;

sx::core::Shadow_direction	EditorObject::m_Shadow;

//////////////////////////////////////////////////////////////////////////
//  internal data structures
class Internal_Form_Object : public sx::gui::Form
{
public:
	void OnNodeExplorerSelect(gui::PControl Sender)
	{
		EditorObject::selectedNode		=	EditorObject::frm_EditNode->GetSelectedNode();
		EditorObject::selectedMember	=	EditorObject::frm_EditNode->GetSelectedMember();
	}

	void OnPopupMenu1Click(gui::PControl Sender)
	{
		int index = EditorObject::pop_Menu1->Listbox().GetItemIndex();

		switch (index)
		{
		case 0:	//  copy member
			{
				EditorObject::Copy( NULL, EditorObject::selectedMember );
			}
			break;

		case 1:	//	copy node
			{
				EditorObject::Copy( EditorObject::selectedNode, NULL );
			}
			break;

		case 2:	//	paste
			{
				if ( EditorObject::selectedNode )
					EditorObject::Paste( EditorObject::selectedNode );
				else
					EditorObject::Paste( EditorObject::node_Root );
			}
			break;
		}

		EditorObject::pop_Menu1->Close();
	}
};
static Internal_Form_Object frm_Internal;

void EditorObject::Initialize( void )
{
	sx_callstack();

	//  create objects
	frm_Toolbar			= sx_new( Form_ObjectToolbar );
	frm_EditNode		= sx_new( Form_EditNode );
	frm_EditMesh		= sx_new( Form_EditMesh );
	frm_EditParticle	= sx_new( Form_EditParticle );
	frm_EditSound		= sx_new( Form_EditSound );
	frm_EditAnimator	= sx_new( Form_EditAnimator );

	//  set functions
	frm_EditNode->GetNodeExplorer()->m_Listbox.SetOnMouseClick( &frm_Internal, (GUICallbackEvent)&Internal_Form_Object::OnNodeExplorerSelect );
	frm_EditNode->GetNodeExplorer()->m_Listbox.SetOnKeyDown( &frm_Internal, (GUICallbackEvent)&Internal_Form_Object::OnNodeExplorerSelect );

	//  create popup
	pop_Menu1 = sx_new( PopupMenu );
	pop_Menu1->SetSize( 100.0f, 60.0f );
	pop_Menu1->Listbox().Add( L"Copy Member",	EditorUI::GetTexture(L"Copy") );
	pop_Menu1->Listbox().Add( L"Copy Node",		EditorUI::GetTexture(L"Copy") );
	pop_Menu1->Listbox().Add( L"Paste",			EditorUI::GetTexture(L"Paste") );
	pop_Menu1->SetOnSelect( &frm_Internal, (GUICallbackEvent)&Internal_Form_Object::OnPopupMenu1Click );

	m_Shadow.CreateResources(512);
}

void EditorObject::Finalize( void )
{
	sx_delete( node_Root );

	m_Shadow.DestroyResources();

	sx_delete( pop_Menu1 );

	sx_delete( frm_EditAnimator );
	sx_delete( frm_EditSound );
	sx_delete( frm_EditParticle );
	sx_delete( frm_EditMesh );
	sx_delete( frm_EditNode );
	sx_delete( frm_Toolbar );
}

void EditorObject::Resize( int width, int height )
{
	frm_Toolbar->OnResize(width, height);
	frm_EditNode->OnResize(width, height);
	frm_EditMesh->OnResize(width, height);
	frm_EditParticle->OnResize(width, height);
}

void EditorObject::Update( float elpsTime, bool& inputHandled )
{
	//////////////////////////////////////////////////////////////////////////
	//	VALIDATE INFO
	if ( !selectedNode ) selectedNode = node_Root;

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

	//  show the popup menu
	if ( !inputHandled && SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_RIGHT) )
		pop_Menu1->Popup( SEGAN_MOUSE_ABSX(0) - SEGAN_VP_WIDTH/2,  SEGAN_VP_HEIGHT/2 - SEGAN_MOUSE_ABSY(0) );

	if ( !inputHandled )
	{
		//  edit selected objects
		if (selectedNode)
		{
			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_N) )
				frm_EditNode->Show();
		}
		if (selectedMember && SEGAN_KEYDOWN(0, SX_INPUT_KEY_E) )
		{
			//  if member is mesh user can show mesh editor
			if ( selectedMember->GetType() == NMT_MESH )
				frm_EditMesh->Show();

			//  if member is particle user can show particle editor
			if ( selectedMember->GetType() == NMT_PARTICLE )
				frm_EditParticle->Show();

			//  if member is sound user can show sound editor
			if ( selectedMember->GetType() == NMT_SOUND )
				frm_EditSound->Show();

			//  if member is sound user can show animator editor
			if ( selectedMember->GetType() == NMT_ANIMATOR )
				frm_EditAnimator->Show();
		}
	}

#if 1
	pop_Menu1->ProcessInput(inputHandled);
	frm_Toolbar->ProcessInput(inputHandled);
	frm_EditParticle->ProcessInput(inputHandled);
	frm_EditMesh->ProcessInput(inputHandled);
	frm_EditSound->ProcessInput(inputHandled);
	frm_EditAnimator->ProcessInput(inputHandled);
	frm_EditNode->ProcessInput(inputHandled);
#endif

	cam[camIndex].Update(elpsTime, inputHandled, selectedNode, selectedMember);

	//////////////////////////////////////////////////////////////////////////
	//  select object from the root
	if ( !inputHandled )
	{
		if ( node_Root )
		{
			// select object from root
			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) )
			{
				if ( !SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && !SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL) )
				{
					//  create ray for intersection test
					Matrix matView, matProj;
					cam[camIndex]().GetViewMatrix(matView);
					cam[camIndex]().GetProjectionMatrix(matProj);
					math::Ray ray( SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0), SEGAN_VP_WIDTH, SEGAN_VP_HEIGHT, matView, matProj );

					//  select object if mouse state is on select mode
					if ( (mouse_state & EDITOR_MOUSESTATE_SELECT) || (mouse_state & EDITOR_MOUSESTATE_MOVE) )
					{
						//  test intersection
						if ( ray.Intersect_OBBox( node_Root->GetBox_world() ) )
						{
							if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_RALT) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_LALT) )
							{
								selectedNode = node_Root;
							}
							else
							{
								msg_IntersectRay msgRay(mouse_select_type, ray, msg_IntersectRay::GEOMETRY, NULL);
								node_Root->MsgProc(MT_INTERSECT_RAY, &msgRay);
								selectedNode = (sx::core::PNode)(msgRay.results[0].node);
								selectedMember = (sx::core::PNodeMember)(msgRay.results[0].member);
							}
						}
					}
				}
			}
		}
		else // if ( node_Root )
		{
			selectedNode = NULL;
			selectedMember = NULL;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//  UPDATE SCENE

	//  update other forms and objects
	frm_EditNode->UpdateEditorData( selectedNode ? selectedNode : node_Root, selectedMember, cam[camIndex](), mouse_state );
	frm_EditMesh->SetMeshToEdit(selectedMember);
	frm_EditParticle->SetParticleToEdit(selectedMember);
	frm_EditSound->SetSoundToEdit(selectedMember);
	frm_EditAnimator->SetAnimatorToEdit(selectedMember);
	//Editor::frm_SetName->UpdateEditorData(selectedNode, selectedMember);

	//  update gui
#if 1
	pop_Menu1->Update(elpsTime);
	frm_Toolbar->Update(elpsTime);
	frm_EditMesh->Update(elpsTime);
	frm_EditParticle->Update(elpsTime);
	frm_EditSound->Update(elpsTime);
	frm_EditAnimator->Update(elpsTime);
	frm_EditNode->Update(elpsTime);
#endif

	// TEST
	{
		float3 vcam = cam[camIndex]().At - cam[camIndex]().Eye;
		str1024 str;
		str.Format( L"Camera dis: %.2f | %s", vcam.Length(), Editor::dbg_Lable->GetText() );
		Editor::dbg_Lable->SetText(str);
	}
}

void EditorObject::Render( float elpsTime )
{
	//  some simple useful data
	static UINT iFrame = 0;
	iFrame++;
	if (iFrame==20) frm_Toolbar->Show();

	sx::d3d::Device3D::Clear_Screen(0xffaaaaca);

	// set selected camera
	sx::core::PCamera pCamera = &cam[camIndex]();
	pCamera->Far = 6000.0f;
	pCamera->Aspect = SEGAN_VP_WIDTH / SEGAN_VP_HEIGHT;
	pCamera->FOV = PI / (1.7f + pCamera->Aspect);
	sx::core::Renderer::SetCamera( pCamera );
	SoundListener soundListener( pCamera->Eye, pCamera->GetDirection(), pCamera->Up );
	sx::snd::Device::SetListener( soundListener );

	//  update sun light
	d3d::ShaderPool::SetLight( core::Renderer::GetSunLight() );
	d3d::ShaderPool::Update(elpsTime);

	//  update shadow
	if ( node_Root && !SEGAN_SET_HAS(draw_Option, SX_DRAW_WIRED) && sx::core::Settings::GetOption_Shadow()->HasPermission() )
	{
		//  check settings of the shadow
		static int shadowLevel = 1;
		if ( shadowLevel != sx::core::Settings::GetOption_Shadow()->GetLevel() )
		{
			shadowLevel = sx::core::Settings::GetOption_Shadow()->GetLevel();
			m_Shadow.DestroyResources();

			//  choose texture size of shadow map
			m_Shadow.CreateResources( shadowLevel==0 ? 2048 : (shadowLevel==1 ? 1024 : (shadowLevel==2 ? 512 : 256)) );
		}

		{
			float lightCamPhase;
			float lightCamRad = node_Root->GetSphere_local().r;
			core::Camera lightcam = cam[camIndex]();
			lightcam.GetSpherical(NULL, &lightCamPhase, NULL);
			lightcam.SetSpherical(lightCamRad, lightCamPhase, 0.0f);
			static Frustum frustum;
			static sx::core::ArrayPNode nodes;
			if (nodes.Count()<1) nodes.PushBack(node_Root);
			nodes[0] = node_Root;
			m_Shadow.ComputeLightFrustum(
				frustum, 
				lightcam, 
				float3( (float*)core::Renderer::GetSunLight() ),
				lightCamRad
				);
			m_Shadow.RenderToShadowMaps( nodes );
		}
	}

	//  draw grid if needed
	if ( EditorSettings::GetGridVisible() )
		sx::d3d::UI3D::DrawGrid( EditorSettings::GetGridSize(), EditorSettings::GetGridColor() );

	//  draw hit point and hit normal
// 	sx::d3d::Device3D::Matrix_World_Set(math::MTRX_IDENTICAL);
// 	sx::d3d::UI3D::DrawSphere( Sphere(hitPoint, 0.1f), 0xff005555 );
// 	sx::d3d::Device3D::Matrix_World_Set(math::MTRX_IDENTICAL);
// 	sx::d3d::UI3D::DrawLine(hitPoint, float3(hitPoint+hitNorml));

	//  verify that scene is not empty
	if (node_Root)
	{
		sx::d3d::Device3D::RS_Fog( true );

		//  draw meshes in nodes with specified options
		core::Renderer::SetRenderTarget( core::Renderer::GetRenderTarget() );
		d3d::Device3D::RS_WireFrame( SEGAN_SET_HAS(draw_Option, SX_DRAW_WIRED) );
		node_Root->Update(elpsTime);
		if ( sx::core::Settings::GetOption_Shadow()->HasPermission() )
		{
			m_Shadow.SetShadowMapsToDevice();		
			node_Root->Draw(SX_DRAW_PARTICLE | SX_DRAW_MESH | SX_SHADER_SHADOW);
			node_Root->Draw(SX_DRAW_ALPHA | SX_DRAW_PARTICLE | SX_DRAW_MESH | SX_SHADER_SHADOW);
		}
		else
		{
			node_Root->Draw(SX_DRAW_PARTICLE | SX_DRAW_MESH);
			node_Root->Draw(SX_DRAW_ALPHA | SX_DRAW_PARTICLE | SX_DRAW_MESH);
		}

		d3d::Device3D::RS_WireFrame(false);

		sx::d3d::Device3D::RS_Fog( false );

		// draw other things in node
		sx::d3d::Device3D::RS_ZWritable(false);
		node_Root->Draw(draw_Option | SX_DRAW_WIRED);
		sx::d3d::Device3D::RS_ZWritable(true);
	}

	if (selectedNode)
	{
		//  draw some debug information of selected node
		sx::d3d::Device3D::RS_ZWritable(false);
		sx::d3d::Device3D::Matrix_World_Set( selectedNode->GetMatrix_world() );
		sx::d3d::UI3D::DrawWiredAABox( selectedNode->GetBox_local(), 0xffffffff );
		sx::d3d::Device3D::RS_ZWritable(true);
	}

	if ( selectedMember )
	{
		//  draw some debug information of selected member
		sx::d3d::Device3D::RS_ZWritable(false);
		
		selectedMember->Draw( draw_Option | SX_DRAW_WIRED | SX_DRAW_SELECTED );

		//  try to draw a box around of selected member
		OBBox box;
		if ( !selectedMember->MsgProc(MT_GETBOX_WORLD, &box) )
		{
			sx::d3d::Device3D::Matrix_World_Set(math::MTRX_IDENTICAL);
			sx::d3d::UI3D::DrawWiredOBBox(box, 0xffff0000);
		}

		sx::d3d::Device3D::RS_ZWritable(true);

	}


	Editor::RenderCompass(elpsTime);

	// draw forms
#if 1
	frm_EditNode->Draw(0);
	frm_EditSound->Draw(0);
	frm_EditAnimator->Draw(0);
	frm_EditMesh->Draw(0);
	frm_EditParticle->Draw(0);
	frm_Toolbar->Draw(0);
	pop_Menu1->Draw(0);
#endif
	
}

void EditorObject::ResetCamera( void )
{
	cam[camIndex].Reset();
}

void EditorObject::Copy( sx::core::PNode node, sx::core::PNodeMember member )
{
	copyStream.Clear();

	if ( node )
	{
		NodeMemberType nmt = NMT_UNKNOWN;
		SEGAN_STREAM_WRITE(copyStream, nmt);
		node->Save(copyStream);
		return;
	}

	if ( member )
	{
		NodeMemberType nmt = member->GetType();
		SEGAN_STREAM_WRITE(copyStream, nmt);
		member->Save(copyStream);
		return;
	}
}

void EditorObject::Paste( sx::core::PNode& node )
{
	NodeMemberType nmt = NMT_NULL;

	copyStream.SetPos(0);
	SEGAN_STREAM_READ(copyStream, nmt);
	if (nmt == NMT_NULL) return;

	switch (nmt)
	{
	case NMT_UNKNOWN:
		{
			sx::core::PNode pNode = sx_new( sx::core::Node );
			pNode->Load(copyStream);
		
			if (node)
				pNode->SetParent(node);
			else
				node = pNode;

			float f = 0;
			node->MsgProc(MT_ACTIVATE, &f);
		}
		break;

	default:
		{
			if ( !node )
				node = sx_new( sx::core::Node );

			sx::core::PNodeMember pMember = sx::core::NodeMember::Create(nmt);
			pMember->SetOwner(node);
			pMember->Load(copyStream);

			float f = 0;
			node->MsgProc(MT_ACTIVATE, &f);
		}
	}
}
