#include "Form_EditPath.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "EditorScene.h"

#define EDITPATH_WIDTH			250.0f		//  size of the edit mesh form
#define EDITPATH_HEIGHT			200.0f		//  size of the edit mesh form

#define EDITPATH_MODE_NONE		0			//  do nothing
#define EDITPATH_MODE_CREATE	1
#define EDITPATH_MODE_LINK		2
#define EDITPATH_MODE_UNLINK	3

//////////////////////////////////////////////////////////////////////////
//	SOME STATIC VARIABLE
static sx::core::PPathNode s_link_from_Path = NULL;	//	use to connect nodes
static sx::core::PPathNode s_link_to_Path = NULL;	//	use to connect nodes

Form_EditPath::Form_EditPath( void ): BaseForm(), m_path(NULL), m_Mode(0)
{
	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	m_pBack->State_Add();
	m_pBack->State_GetByIndex(2) = m_pBack->State_GetByIndex(1);

	//  title of the form
	m_pTitle = sx_new( sx::gui::Label );
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->AddProperty(SX_GUI_PROPERTY_MULTILINE);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Edit Path");

	m_btnCreate	= EditorUI::CreateButton(m_pBack, 100.0f, L"Create");
	SEGAN_GUI_SET_ONCLICK( m_btnCreate, Form_EditPath::OnParamChange );

	m_btnLink	= EditorUI::CreateButton(m_pBack, 100.0f, L"Link");
	SEGAN_GUI_SET_ONCLICK( m_btnLink, Form_EditPath::OnParamChange );

	m_btnUnlink = EditorUI::CreateButton(m_pBack, 100.0f, L"Unlink");
	SEGAN_GUI_SET_ONCLICK( m_btnUnlink, Form_EditPath::OnParamChange );

	m_btnFinish	= EditorUI::CreateButton(m_pBack, 100.0f, L"Finish");
	SEGAN_GUI_SET_ONCLICK( m_btnFinish, Form_EditPath::OnParamChange );

	m_btnCreate->SetHint(L"CREATE\n Click anywhere to create a path node. \n NOTE: Hold ' Shift ' to connect nodes automatically. ");
	m_btnLink->SetHint(L"LINK \n Connect two selected nodes together. ");
	m_btnUnlink->SetHint(L"UNLINK \n Disconnect two connected nodes. ");

	//  shader quality
	m_lsbLayer = sx_new( sx::gui::ListBox );
	m_lsbLayer->SetSize(120, 80.0f, 18.0f, false);
	m_lsbLayer->SetParent( m_pBack );
	m_lsbLayer->SetFont( EditorUI::GetDefaultFont(8) );
	m_lsbLayer->SetItemIndex( sx::core::PathNode::Manager::DebugLayer() );
	m_lsbLayer->SetOnSelect( this, (GUICallbackEvent)&Form_EditPath::OnLayerChange );
	sx::gui::PLabel lbl = EditorUI::CreateLabel(m_lsbLayer->GetBack(), 120.0f, 8, L"Path Layer :");
	lbl->Position().y = 50.0f;
	
	m_lsbLayer->Add( L"General", NULL );
	m_lsbLayer->Add( L"Yellow", NULL );
	m_lsbLayer->Add( L"Green", NULL );
	m_lsbLayer->Add( L"Sky", NULL );
	m_lsbLayer->Add( L"Blue", NULL );
	m_lsbLayer->Add( L"Woman", NULL );
	m_lsbLayer->Add( L"Strawberry", NULL );
	m_lsbLayer->Add( L"cantaloupe", NULL );

	SetSize( EDITPATH_WIDTH, EDITPATH_HEIGHT );

}

Form_EditPath::~Form_EditPath( void )
{
	//  for other things the parent will do everything :)
	sx_delete( m_lsbLayer );
}

void Form_EditPath::SetSize( float width, float height )
{
	float top = height*0.5f - 16.0f;
	float bot = -height*0.5f + 16.0f;
	float left = -width*0.5f + 16.0f;
	float right = width*0.5f - 16.0f;

	m_pTitle->SetSize( float2(width - 70.0f, 50.0f) );
	m_pTitle->Position().y = top;
	top -= 30.0f;

	left += 40.0f;
	m_btnCreate->Position().Set( left, top, 0.0f);	top -= 35.0f;
	m_lsbLayer->Position().Set( 50, top-5, 0 );		
	m_btnLink->Position().Set( left, top, 0.0f);	top -= 35.0f;
	m_btnUnlink->Position().Set( left, top, 0.0f);	top -= 50.0f;

	right -= 40.0f;
	m_btnFinish->Position().Set( right, bot, 0.0f); bot += 35.0f;
	
	BaseForm::SetSize(width, height);
}

void Form_EditPath::ProcessInput( bool& inputHandled )
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

void Form_EditPath::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	if (m_Mode != EDITPATH_MODE_NONE)
		m_btnFinish->AddProperty(SX_GUI_PROPERTY_VISIBLE);
	else
		m_btnFinish->RemProperty(SX_GUI_PROPERTY_VISIBLE);

	m_lsbLayer->Update(elpsTime);
	BaseForm::Update(elpsTime);

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
	
}

void Form_EditPath::Draw( DWORD flag )
{
	if ( (m_Mode == EDITPATH_MODE_LINK || m_Mode == EDITPATH_MODE_UNLINK) && s_link_from_Path && !s_link_to_Path )
	{
		Sphere s( s_link_from_Path->GetPosition(), 0.25f );
		sx::d3d::Device3D::Matrix_World_Set( sx::math::MTRX_IDENTICAL );
		sx::d3d::UI3D::DrawSphere(s, 0xffffff00);
	}

	BaseForm::Draw(flag);
}

void Form_EditPath::OnResize( int EditorWidth, int EditorHeight )
{
 	float offsetW = (float)(EditorWidth  % 2);
 	float offsetH = (float)(EditorHeight  % 2);
 	float width = (float)EditorWidth + offsetW;
 	float height = (float)EditorHeight + offsetH;

	m_pBack->State_GetByIndex(2).Position.x = - width/2 - EDITPATH_WIDTH/2 + 120.0f;
	m_pBack->State_GetByIndex(2).Position.y = height*0.5f + EDITPATH_HEIGHT/2 - 40.0f;
}

void Form_EditPath::SetPathToEdit( sx::core::PNodeMember path )
{
	if ( !IsVisible() ) return;

	//  verify the path
	if ( !path || path->GetType() != NMT_PATHNODE )
	{
		m_path = NULL;
		Close();
		return;
	}
	if ( m_path == path ) return;
	m_path = (sx::core::PPathNode)path;

	//  set title
	if ( m_path->GetName() )
	{
		str1024 str = L"Edit Path\n";
		str << m_path->GetName();
		m_pTitle->SetText( str );
	}
	else m_pTitle->SetText(L"Edit Path - [no name]");

}

//////////////////////////////////////////////////////////////////////////
//	PRIVATE ZOON
void Form_EditPath::OnParamChange( sx::gui::PControl Sender )
{
	if (!Sender) return;

	if ( Sender == m_btnCreate )
	{
		m_pBack->State_SetIndex(2);
		EditorScene::frm_Toolbar->Close();

		m_Mode = EDITPATH_MODE_CREATE;
		s_link_to_Path = NULL;
		s_link_from_Path = NULL;
	}

	else if ( Sender == m_btnLink )
	{
		m_pBack->State_SetIndex(2);
		EditorScene::frm_Toolbar->Close();

		m_Mode = EDITPATH_MODE_LINK;
		s_link_to_Path = NULL;
		s_link_from_Path = NULL;
	}

	else if ( Sender == m_btnUnlink )
	{
		m_pBack->State_SetIndex(2);
		EditorScene::frm_Toolbar->Close();

		m_Mode = EDITPATH_MODE_UNLINK;
		s_link_to_Path = NULL;
		s_link_from_Path = NULL;
	}

	else if ( Sender == m_btnFinish )
	{
		m_pBack->State_SetIndex(1);
		EditorScene::frm_Toolbar->Show();

		m_Mode = EDITPATH_MODE_NONE;
		s_link_from_Path = NULL;
		s_link_to_Path = NULL;
	}
}

void Form_EditPath::OnLayerChange( sx::gui::PControl Sander )
{
	sx::core::PathNode::Manager::DebugLayer() = m_lsbLayer->GetItemIndex();
}

bool Form_EditPath::CreateNode( Ray& ray )
{
	if( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) ||
		SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL) ||
		SEGAN_KEYHOLD(0, SX_INPUT_KEY_LALT)		||
		SEGAN_KEYHOLD(0, SX_INPUT_KEY_RALT)
		)
		return false;

	bool doCreate = false;
	msg_IntersectRay msgRay(NMT_MESH, ray, msg_IntersectRay::GEOMETRY, NULL);
	sx::core::Scene::GetNodeByRay(msgRay);
	float3 pathPos = msgRay.results[0].position;

	if ( msgRay.results[0].member )
	{
		doCreate = true;
	}
	else
	{
		Plane p; p.Make(sx::math::VEC3_ZERO, sx::math::VEC3_Y);
		doCreate = ray.Intersect_Plane(p, &pathPos);
	}

	if ( doCreate /*&& sx::core::PathNode::FindNeighbor(pathPos) == NULL*/ )
	{
		sx::core::PNode			node = sx::core::Scene::CreateNode();
		sx::core::PPathNode		path = sx_new( sx::core::PathNode );
		path->SetOwner( node );
		node->SetName( path->GetName() );
		node->UpdateBoundingVolumes();
		node->SetPosition(pathPos);
		sx::core::Scene::AddNode( node );
		EditorScene::mng_UndoManager.AddAction( UAT_CREATE, &node, 1 );

		if (EditorScene::selectedMember								&&
			EditorScene::selectedMember->GetType()==NMT_PATHNODE	&&
			( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LSHIFT) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RSHIFT) ))
		{
			sx::core::PPathNode lastpath = (sx::core::PPathNode)EditorScene::selectedMember;
			path->ConnectTo(lastpath);
		}

		EditorScene::selectedNodes.Clear();
		EditorScene::selectedNodes.PushBack( node );
		EditorScene::selectedMember = path;
		
		return true;
	}
	else return false;
	
}

void Form_EditPath::Operate( bool& inputHandled )
{
	if ( !inputHandled && m_Mode != EDITPATH_MODE_NONE )
	{
		inputHandled = true;
		bool localHandled = false;

		//  at first create most necessary object
		Matrix matView, matProj;
		EditorScene::GetCurrentCamera().GetViewMatrix(matView);
		EditorScene::GetCurrentCamera().GetProjectionMatrix(matProj);
		sx::math::Ray ray( SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0), SEGAN_VP_WIDTH, SEGAN_VP_HEIGHT, matView, matProj );

		//  at first check the creation mode
		if ( m_Mode == EDITPATH_MODE_CREATE && SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) )
		{
			localHandled = CreateNode(ray);
		}
		
		//  now select object if mouse state is in select mode
		if (!localHandled	&&
			SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT)	&& 
			!SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL)	&& 
			!SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL) )
		{
			msg_IntersectRay msgRay( NMT_PATHNODE, ray, msg_IntersectRay::GEOMETRY, NULL );
			sx::core::Scene::GetNodeByRay(msgRay);
			sx::core::PNode snode = (sx::core::PNode)msgRay.results[0].node;
			EditorScene::selectedMember = (sx::core::PNodeMember)msgRay.results[0].member;

			//  received node may be a child. we should get parent here
			while ( snode && snode->GetParent() )
				snode = snode->GetParent();

			EditorScene::selectedNodes.Clear();
			if ( snode ) EditorScene::selectedNodes.PushBack( snode );

			if ( m_Mode == EDITPATH_MODE_LINK || m_Mode == EDITPATH_MODE_UNLINK )
			{
				if ( !s_link_from_Path )		s_link_from_Path = (sx::core::PPathNode)EditorScene::selectedMember;
				else if ( !s_link_to_Path)		s_link_to_Path = (sx::core::PPathNode)EditorScene::selectedMember;

				if ( !EditorScene::selectedMember )
				{
					s_link_from_Path = NULL;
					s_link_to_Path = NULL;
				}
			}
		}

		//  operate link mode
		if ( m_Mode == EDITPATH_MODE_LINK )
		{
			if ( s_link_from_Path && s_link_to_Path )
			{
				s_link_from_Path->ConnectTo( s_link_to_Path );

				EditorScene::selectedMember = s_link_from_Path;

				EditorScene::selectedNodes.Clear();
				EditorScene::selectedNodes.PushBack( s_link_from_Path->GetOwner() );

				s_link_from_Path = NULL;
				s_link_to_Path = NULL;
			}
		}
		
		//
		if ( m_Mode == EDITPATH_MODE_UNLINK )
		{
			if ( s_link_from_Path && s_link_to_Path )
			{
				s_link_from_Path->DisconnectFrom( s_link_to_Path );

				EditorScene::selectedMember = s_link_from_Path;

				EditorScene::selectedNodes.Clear();
				EditorScene::selectedNodes.PushBack( s_link_from_Path->GetOwner() );

				s_link_from_Path = NULL;
				s_link_to_Path = NULL;
			}
		}
	}
}

void Form_EditPath::Show( void )
{
	BaseForm::Show();

	//  force to close node / terrain editor to avoid conflicts
	EditorScene::frm_EditNode->Close();
	EditorScene::frm_EditTrrn->Close();

	//  path node can be select
	EditorScene::mouse_select_type = NMT_PATHNODE;
}

