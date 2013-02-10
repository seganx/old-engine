#include "Form_EditTerrain.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "EditorScene.h"
#include "EditorUndo.h"

#define EDITTERRAIN_WIDTH			300.0f		//  size of the edit mesh form
#define EDITTERRAIN_HEIGHT			300.0f		//  size of the edit mesh form

#define EDITTERRAIN_MODE_NONE		0			//  do nothing
#define EDITTERRAIN_MODE_CREATE		1
#define EDITTERRAIN_MODE_DELETE		2

static float editorWidth = 0, editorHeight = 0;

Form_EditTerrain::Form_EditTerrain( void ): BaseForm(), m_Mode(0), m_heightPlus(0)
{
	//  title of the form
	m_pTitle = sx_new( sx::gui::Label );
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->AddProperty(SX_GUI_PROPERTY_MULTILINE);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Edit Terrain");

	m_btnCreate	= EditorUI::CreateButton(m_pBack, 100.0f, L"Create");
	SEGAN_GUI_SET_ONCLICK( m_btnCreate, Form_EditTerrain::OnParamChange );
	m_btnCreate->SetHint(L"CREATE GEOMETRY : \n Create geometry for terrain where the is no geometry. ");

	m_btnDelete	= EditorUI::CreateButton(m_pBack, 100.0f, L"Delete");
	SEGAN_GUI_SET_ONCLICK( m_btnDelete, Form_EditTerrain::OnParamChange );
	m_btnDelete->SetHint(L"DELETE GEOMETRY : \n delete terrain geometry. ");

	m_trLOD	= EditorUI::CreateTrackbar(m_pBack, 170.0f, 5.0f, 20.0f);
	m_trLOD->SetValue( sx::core::Terrain::Manager::LOD_Threshold() );
	SEGAN_GUI_SET_ONSCROLL( m_trLOD, Form_EditTerrain::OnParamChange );
	m_trLOD->SetHint(L"TERRAIN LOD THRESHOLD : \n Change the parameter of LOD selection will increase/decrease details/performance while rendering the terrain. ");

	m_lbLOD	= EditorUI::CreateLabel(m_trLOD, 170.0f, 8, L"LOD Threshold : 2.2");
	m_lbLOD->SetHint(L"TERRAIN LOD THRESHOLD : \n Change the parameter of LOD selection will increase/decrease details/performance while rendering the terrain. ");
	m_lbLOD->Position().y = 18.0f;
	

	m_EditMaterial.SetParent(m_pBack);
	m_EditMaterial.SetOnSizeChanged(this, (GUICallbackEvent)&Form_EditTerrain::OnMaterialResized);

	m_btnFinish	= EditorUI::CreateButton(m_pBack, 100.0f, L"Finish");
	SEGAN_GUI_SET_ONCLICK( m_btnFinish, Form_EditTerrain::OnParamChange );

	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	m_pBack->State_Add();
	m_pBack->State_GetByIndex(2) = m_pBack->State_GetByIndex(1);

	SetSize( EDITTERRAIN_WIDTH, EDITTERRAIN_HEIGHT );
}

Form_EditTerrain::~Form_EditTerrain( void )
{
	//  for other things the parent will do everything :)
}

void Form_EditTerrain::SetSize( float width, float height )
{
	float top = height*0.5f - 16.0f;
	float bot = -height*0.5f + 16.0f;
	float left = -width*0.5f + 16.0f;
	float right = width*0.5f - 16.0f;

	m_pTitle->SetSize( float2(width - 70.0f, 50.0f) );
	m_pTitle->Position().y = top;
	top -= 30.0f;

	left += 40.0f;
	m_btnCreate->Position().Set( left, top, 0.0f);
	m_btnDelete->Position().Set( left+110.0f, top, 0.0f);	top -= 50.0f;
	m_trLOD->Position().Set( 0.0f, top, 0.0f);				top -= 30.0f;

	m_heightPlus = 170;
	m_EditMaterial.Position().y = top;
	m_EditMaterial.SetSize(width, height);

	right -= 40.0f;
	m_btnFinish->Position().Set( right, bot, 0.0f); bot += 35.0f;
	
	BaseForm::SetSize(width, height);
}

void Form_EditTerrain::ProcessInput( bool& inputHandled )
{
	if ( !IsVisible() || inputHandled ) return;

	const char* keys = sx::io::Input::GetKeys(0);

	if (keys[SX_INPUT_KEY_ESCAPE] == SX_INPUT_STATE_UP)
	{
		sx::gui::ArrayPControl clist;
		m_pBack->GetChildren(clist);
		for (int i=0; i<clist.Count(); i++)
		{
			if ( clist[i]->GetFocused() )
			{
				if ( m_pBack->State_GetIndex() == 2 )
					OnParamChange( m_btnFinish );
				else
					Close();

				inputHandled = true;
				return;
			}
		}
	}

	BaseForm::ProcessInput(inputHandled);
}

void Form_EditTerrain::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	if (m_Mode != EDITTERRAIN_MODE_NONE)
		m_btnFinish->AddProperty(SX_GUI_PROPERTY_VISIBLE);
	else
		m_btnFinish->RemProperty(SX_GUI_PROPERTY_VISIBLE);

	BaseForm::Update(elpsTime);

	sx::core::Terrain::Manager::LOD_Threshold() = m_trLOD->GetBlendingValue();
	str256 str;
	str.Format(L"LOD Threshold : %.2f", m_trLOD->GetBlendingValue());
	m_lbLOD->SetText( str );

	m_EditMaterial.Update(elpsTime);

	//  update position states
	m_pBack->State_GetByIndex(0).Position = m_pBack->State_GetByIndex(1).Position;
	if ( m_pBack->State_GetIndex() == 2 )
	{
		SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);
	}
	else
	{
		SEGAN_SET_ADD(m_Option, FORM_OPTION_MOVABLE);
	}
	
	//  update material list
	m_EditMaterial.SetMaterialToEdit( sx::core::Terrain::Manager::GetMaterial(0) );

}

void Form_EditTerrain::Draw( DWORD flag )
{
	BaseForm::Draw(flag);
}

void Form_EditTerrain::OnResize( int EditorWidth, int EditorHeight )
{
 	float offsetW = (float)(EditorWidth  % 2);
 	float offsetH = (float)(EditorHeight  % 2);
 	editorWidth = (float)EditorWidth + offsetW;
 	editorHeight = (float)EditorHeight + offsetH;

	 
	m_pBack->State_GetByIndex(2).Position.x = - editorWidth * 0.5f  - EDITTERRAIN_WIDTH  / 2 + 120.0f;
	m_pBack->State_GetByIndex(2).Position.y =   editorHeight * 0.5f + BaseForm::GetSize().y /2 - 40.0f;
}


//////////////////////////////////////////////////////////////////////////
//	PRIVATE ZOON

void Form_EditTerrain::OnMaterialResized( sx::gui::PControl Sender )
{
	//  update form size depend of height of material editor
	float2 cursize = GetSize();
	cursize.y = m_heightPlus + m_EditMaterial.GetHeight();
	SetSize( cursize.x, cursize.y );

	m_pBack->State_GetByIndex(2).Position.x = - editorWidth * 0.5f  - cursize.x  / 2 + 120.0f;
	m_pBack->State_GetByIndex(2).Position.y =   editorHeight * 0.5f + cursize.y /2 - 40.0f;
}

void Form_EditTerrain::OnParamChange( sx::gui::PControl Sender )
{
	if (!Sender) return;

	if ( Sender == m_btnCreate )
	{
		m_pBack->State_SetIndex(2);
		EditorScene::frm_Toolbar->Close();

		m_Mode = EDITTERRAIN_MODE_CREATE;
		EditorScene::mouse_state = EDITOR_MOUSESTATE_SELECT;
	}

	if ( Sender == m_btnDelete )
	{
		m_pBack->State_SetIndex(2);
		EditorScene::frm_Toolbar->Close();

		m_Mode = EDITTERRAIN_MODE_DELETE;
		EditorScene::mouse_state = EDITOR_MOUSESTATE_SELECT;
	}

	if ( Sender == m_btnFinish )
	{
		m_pBack->State_SetIndex(1);
		EditorScene::frm_Toolbar->Show();

		m_Mode = EDITTERRAIN_MODE_NONE;
	}

}

void Form_EditTerrain::Operate( bool& inputHandled )
{
	if ( !inputHandled && m_Mode != EDITTERRAIN_MODE_NONE )
	{
		inputHandled = true;
		bool localHandled = false;

		//  at first create most necessary object
		Matrix matView, matProj;
		EditorScene::GetCurrentCamera().GetViewMatrix(matView);
		EditorScene::GetCurrentCamera().GetProjectionMatrix(matProj);
		sx::math::Ray ray( SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0), SEGAN_VP_WIDTH, SEGAN_VP_HEIGHT, matView, matProj );

		//  after first check the creation mode
		if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_LEFT) )
		{
			if ( m_Mode == EDITTERRAIN_MODE_CREATE )
				localHandled = CreateNode(ray);
			else if ( m_Mode == EDITTERRAIN_MODE_DELETE )
				localHandled = DeleteNode(ray);
		}
		
		//  now select object if mouse state is in select mode
		if (!localHandled	&&
			SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT)	&& 
			!SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL)	&& 
			!SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL) )
		{
			msg_IntersectRay msgRay( NMT_PATHNODE, ray, msg_IntersectRay::GEOMETRY, NULL);
			sx::core::Scene::GetNodeByRay(msgRay);
			sx::core::PNode snode = (sx::core::PNode)msgRay.results[0].node;
			EditorScene::selectedMember = (sx::core::PNodeMember)msgRay.results[0].member;

			//  received node may be a child. we should get parent here
			while ( snode && snode->GetParent() )
				snode = snode->GetParent();

			//  set selected node
			EditorScene::selectedNodes.Clear();
			if ( snode ) EditorScene::selectedNodes.PushBack( snode );
		}
		
	}
}

void Form_EditTerrain::Show( void )
{
	BaseForm::Show();

	//  force to close node / path editor to avoid conflicts
	EditorScene::frm_EditNode->Close();
	EditorScene::frm_EditPath->Close();
}

bool Form_EditTerrain::CreateNode( Ray& ray )
{
	if( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) ||
		SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL) ||
		SEGAN_KEYHOLD(0, SX_INPUT_KEY_LALT)		||
		SEGAN_KEYHOLD(0, SX_INPUT_KEY_RALT)
		)
		return false;

	bool doCreate = false;
	msg_IntersectRay msgRay(NMT_TERRAIN | NMT_MESH, ray, msg_IntersectRay::GEOMETRY, NULL);
	sx::core::Scene::GetNodeByRay(msgRay);
	float3 terrPos = msgRay.results[0].position;


	if ( msgRay.results[0].member )
	{
		doCreate = false;
	}
	else
	{
		Plane p; p.Make(sx::math::VEC3_ZERO, sx::math::VEC3_Y);
		doCreate = ray.Intersect_Plane(p, &terrPos);
	}

	if ( doCreate && terrPos.Distance_sqr( sx::core::Renderer::GetCamera()->Eye ) < 700000 )
	{
		terrPos.x = sx::cmn::Round(terrPos.x / 32.0f) * 32.0f;
		terrPos.y = 0.0f;
		terrPos.z = sx::cmn::Round(terrPos.z / 32.0f) * 32.0f;

		sx::core::PNode			node = sx::core::Scene::CreateNode();
		sx::core::PTerrain		terr = sx_new( sx::core::Terrain );
		terr->SetOwner( node );
		node->SetName( L"terrain" );
		node->UpdateBoundingVolumes();
		node->SetPosition( terrPos );
		sx::core::Scene::AddNode( node );

		EditorScene::selectedNodes.Clear();
		if ( node )
		{
			EditorScene::selectedNodes.PushBack( node );
			EditorScene::mng_UndoManager.AddAction( UAT_CREATE, &node, 1);
		}
		EditorScene::selectedMember = terr;

		return true;
	}
	else return false;
	
}

bool Form_EditTerrain::DeleteNode( Ray& ray )
{
	if( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) ||
		SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL) ||
		SEGAN_KEYHOLD(0, SX_INPUT_KEY_LALT)		||
		SEGAN_KEYHOLD(0, SX_INPUT_KEY_RALT)
		)
		return false;

	bool doCreate = false;
	msg_IntersectRay msgRay(NMT_TERRAIN, ray, msg_IntersectRay::GEOMETRY, NULL);
	sx::core::Scene::GetNodeByRay(msgRay);
	float3 terrPos = msgRay.results[0].position;


	if ( msgRay.results[0].member )
	{
		sx::core::PNode node = sx::core::PNode(msgRay.results[0].node);
		if ( node )
			EditorScene::mng_UndoManager.DeleteNode( &node, 1 );
		EditorScene::selectedNodes.Clear();
		EditorScene::selectedMember = NULL;
		return true;
	}
	else return false;
}
