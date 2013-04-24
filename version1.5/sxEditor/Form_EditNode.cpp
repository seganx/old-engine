#include "Form_EditNode.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "EditorUI.h"
#include "EditorSettings.h"

#define FORM_EDITNODE_WIDTH			250.0f		//  size of the node edit form
#define FORM_EDITNODE_HEIGHT		400.0f		//  size of the node edit form
#define FORM_EDITNODE_MAX_WIDTH		500.0f		//  maximum width


#define PIVOT_SCALE_PERCENT		60

UINT pivot_selectedAxis = 0;
int		s_multi_nodeCount	= 0;
float3	s_multi_pos			= float3(0, 0, 0);
float3	s_multi_rot			= float3(0, 0, 0);


Form_EditNode::Form_EditNode( void ): BaseForm(), m_applyChanges(true), m_mouseState(0), m_pNode(0), m_Nodes(0), m_undoManager(0)
{
	m_pTitle	= (sx::gui::PLabel)sx::gui::Create(GUI_LABEL);
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Edit Node");

	//  create name text editor
	m_pEditName = (sx::gui::PTextEdit)sx::gui::Create(GUI_TEXTEDIT);
	m_pEditName->SetParent(m_pBack);
	m_pEditName->SetAlign(GTA_CENTER);
	m_pEditName->SetFont( EditorUI::GetFormCaptionFont() );
	m_pEditName->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pEditName->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pEditName->SetText(L"NoName");
	SEGAN_GUI_SET_ONTEXT( m_pEditName, Form_EditNode::OnParamChange );

	m_drawOrder = EditorUI::CreateLabeldEditBox( m_pBack, 16.0f, 50.0f, 70.0f, L"Draw Order : ", NULL );
	SEGAN_GUI_SET_ONTEXT( m_drawOrder, Form_EditNode::OnParamChange );

	//  create simple labels
	m_pLabelPos = EditorUI::CreateLabel(m_pBack, 100.0f, 8, L"Position :");
	m_pLabelRot = EditorUI::CreateLabel(m_pBack, 100.0f, 8, L"Rotation :");

	//	create additional button
	m_playSound	= EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture( L"ButtonPlay" ) );
	m_sprayParticle = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture( L"ButtonSpray" ) );
	SEGAN_GUI_SET_ONCLICK( m_playSound, Form_EditNode::OnParamChange );
	SEGAN_GUI_SET_ONCLICK( m_sprayParticle, Form_EditNode::OnParamChange );

	// create check box for group change
	m_groupChange = EditorUI::CreateCheckBox( m_pBack, 50, 50, L"As Group" );

	//  create position text editor
	for (int i=0; i<3; i++)
	{
		m_pEditPos[i] = EditorUI::CreateEditBox(m_pBack, 0, FORM_EDITNODE_WIDTH/4, L"0.00");
		SEGAN_GUI_SET_ONTEXT(m_pEditPos[i], Form_EditNode::OnParamChange );
		SEGAN_GUI_SET_ONWHEEL(m_pEditPos[i], Form_EditNode::OnMouseWheel );
		
		m_pEditRot[i] = EditorUI::CreateEditBox(m_pBack, 0, FORM_EDITNODE_WIDTH/4, L"0.00");
		SEGAN_GUI_SET_ONTEXT(m_pEditRot[i], Form_EditNode::OnParamChange );
		SEGAN_GUI_SET_ONWHEEL(m_pEditRot[i], Form_EditNode::OnMouseWheel );
	}

	m_nodeExplorer.SetParent(m_pBack);

	//  stop moving and resizing
	//SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	m_MinSize.Set( FORM_EDITNODE_WIDTH, FORM_EDITNODE_WIDTH );
	m_MaxSize.Set( FORM_EDITNODE_MAX_WIDTH, FORM_EDITNODE_HEIGHT*1.5 );

	SetSize( FORM_EDITNODE_WIDTH, FORM_EDITNODE_HEIGHT );
}

Form_EditNode::~Form_EditNode( void )
{
	//  for other things the parent will do everything :)
}

void Form_EditNode::SetSize( float width, float height )
{
	float top = height/2 - 10.0f;
	float left = -width/2;

	m_pTitle->SetSize( float2(width - 40.0f, 30.0f) );
	m_pTitle->Position().y = top;	top -= 15.0f;

	m_pEditName->SetSize( float2(width - 40.0f, 30.0f) );
	m_pEditName->Position().y = top;	top -= 30.0f;

	m_drawOrder->Position().Set( left + 95.0f, top, 0.0f );

	m_playSound->Position().Set( left + 150.0f, top, 0.0f );
	m_sprayParticle->Position().Set( left + 200.0f, top, 0.0f );
	top -= 35.0f;

	float2 editSize(40.0f, 20.0f);
	m_pLabelPos->Position().Set(left + 50.0f, top, 0.0f);
	for (int i=0; i<3; i++)
	{
		m_pEditPos[i]->SetSize( editSize );
		m_pEditPos[i]->Position().x = left + 80.0f + i*(editSize.x + 20.0f);
		m_pEditPos[i]->Position().y = top;
	}
	top -= 20.0f;

	m_pLabelRot->Position().Set(left + 50.0f, top, 0.0f);
	for (int i=0; i<3; i++)
	{
		m_pEditRot[i]->SetSize( editSize );
		m_pEditRot[i]->Position().x = left + 80.0f + i*(editSize.x + 20.0f);
		m_pEditRot[i]->Position().y = top;
	}
	top -= 20.0f;

	m_groupChange->Position().Set( left + 10.0f, top, 0.0f );

	top += 10.0f;

	m_nodeExplorer.SetSize( width, height / 2.0f + top - 30.0f );
	m_nodeExplorer.Position().y = top - ( height / 2.0f + top ) * 0.5f - 12.0f;

	BaseForm::SetSize( width, height );
}

void Form_EditNode::ProcessInput( bool& inputHandled )
{
	if (inputHandled) return;

	const char* keys = sx::io::Input::GetKeys(0);

	if (keys[SX_INPUT_KEY_ESCAPE] == SX_INPUT_STATE_UP)
	{
		sx::gui::ArrayPControl clist;
		m_pBack->GetChildren(clist);
		for (int i=0; i<clist.Count(); i++)
		{
			if ( clist[i]->GetFocused() )
			{
				Close();
				inputHandled = true;
				return;
			}
		}
	}

	BaseForm::ProcessInput(inputHandled);
	if (inputHandled || ( !m_pNode && !m_Nodes ) ) return;

	//  collect info
	bool keyhold_Shift = SEGAN_KEYHOLD(0, SX_INPUT_KEY_LSHIFT) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RSHIFT);
	bool keyhold_Crlt = SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL);
	bool keyhold_mouseLeft = SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_LEFT);
	bool keyhold_mouseRight = SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_RIGHT);
	bool keyhold_mouseMidd = SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_MIDDLE);
	bool keyhold_Move = SEGAN_KEYHOLD(0, SX_INPUT_KEY_W) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_S) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_A) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_D);

	//  manage undo/redo
	if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) && pivot_selectedAxis && m_undoManager)
	{
		if ( m_pNode )
				m_undoManager->AddAction( UAT_MOVE, &m_pNode, 1 );
		if ( m_Nodes && m_Nodes->Count() )
				m_undoManager->AddAction( UAT_MOVE, &m_Nodes->At(0), m_Nodes->Count() );
	}

	sx::math::Matrix matPivot;

	if ( m_pNode )
	{
		if ( !m_pNode->GetParent() )
		{
			matPivot.Translation( m_pNode->GetPosition_world().x, m_pNode->GetPosition_world().y, m_pNode->GetPosition_world().z );
		}
		else
		{
			matPivot = m_pNode->GetParent()->GetMatrix_world();
			matPivot.SetTranslation( m_pNode->GetPosition_world().x, m_pNode->GetPosition_world().y, m_pNode->GetPosition_world().z );
		}
	}

	if ( m_Nodes && m_Nodes->Count() )
	{
		float3 cen(0,0,0);

		for (int i=0; i<m_Nodes->Count(); i++)
		{
			cen += m_Nodes->At(i)->GetPosition_world();
		}
		cen.x /=  m_Nodes->Count();
		cen.y /=  m_Nodes->Count();
		cen.z /=  m_Nodes->Count();
		matPivot.Translation( cen.x, cen.y, cen.z );
	}

	//  select an axis of the pivot
	if ( (m_mouseState & EDITOR_MOUSESTATE_MOVE) && !keyhold_Crlt && !keyhold_Shift && !keyhold_Move )
	{
		if ( pivot_selectedAxis && keyhold_mouseLeft )
		{
			//  collect view information
			float dis = m_Camera.Eye.Distance(float3(matPivot._41, matPivot._42, matPivot._43));
			Matrix matView;
			m_Camera.GetViewMatrix( matView );
			matView.Inverse( matView );

			// make changes vector
			float3 vecView( -SEGAN_MOUSE_RLX(0)/SEGAN_VP_HEIGHT, SEGAN_MOUSE_RLY(0)/SEGAN_VP_HEIGHT, 0.0f );
			
			// transform vector to the view space
			vecView.Transform_Norm( vecView, matView );

			//  transform vector to the world space
			if ( m_pNode && m_pNode->GetParent() )
			{
				Matrix m; m.Inverse( m_pNode->GetParent()->GetMatrix_world() );
				vecView.Transform_Norm( vecView, m );
			}

			//  move node
			vecView *= dis;

			if ( m_pNode )
			{
				float3 pos = m_pNode->GetPosition_local();
				if (pivot_selectedAxis & EDITOR_MOUSEAXIS_X) pos.x -= vecView.x;
				if (pivot_selectedAxis & EDITOR_MOUSEAXIS_Y) pos.y -= vecView.y;
				if (pivot_selectedAxis & EDITOR_MOUSEAXIS_Z) pos.z -= vecView.z;
				m_pNode->SetPosition(pos);

				//  show changes in editor
				m_applyChanges = false;
				for (int i=0; i<3; i++)
					m_pEditPos[i]->SetText(FloatToStr(	pos[i] ) );
				m_applyChanges = true;

			}

			if ( m_Nodes && m_Nodes->Count() )
			{
				for (int i=0; i<m_Nodes->Count(); i++)
				{
					float3 pos = m_Nodes->At(i)->GetPosition_local();
					if (pivot_selectedAxis & EDITOR_MOUSEAXIS_X) pos.x -= vecView.x;
					if (pivot_selectedAxis & EDITOR_MOUSEAXIS_Y) pos.y -= vecView.y;
					if (pivot_selectedAxis & EDITOR_MOUSEAXIS_Z) pos.z -= vecView.z;
					m_Nodes->At(i)->SetPosition(pos);

					m_applyChanges = false;
 						for ( int i=0; i<3; i++ )
 							m_pEditPos[i]->SetText( L"0.0" );
						s_multi_pos.Set(0,0,0);
					m_applyChanges = true;
				}
			}

		}
		else
		{
			pivot_selectedAxis = SelectPivotAxis( m_Camera, matPivot, 10.0f );
		}

		if (pivot_selectedAxis)
		{
			inputHandled = true;

			//  disable focused gui
			if ( sx::gui::Control::GetFocusedControl() )
				sx::gui::Control::GetFocusedControl()->SetFocused(false);
		}
	}
}

void Form_EditNode::Update( float elpsTime )
{
	m_nodeExplorer.Update(elpsTime);

	BaseForm::Update(elpsTime);

	//  update position states
	m_pBack->State_GetByIndex(0).Position = m_pBack->State_GetByIndex(1).Position;

}

void Form_EditNode::Draw( DWORD flag )
{
	if ( (m_pNode || (m_Nodes && m_Nodes->Count())) && (m_mouseState & EDITOR_MOUSESTATE_MOVE) )
	{
		sx::math::Matrix matPivot;

		if ( m_pNode )
		{
			if ( !m_pNode->GetParent() )
			{
				matPivot.Translation( m_pNode->GetPosition_world().x, m_pNode->GetPosition_world().y, m_pNode->GetPosition_world().z );
			}
			else
			{
				matPivot = m_pNode->GetParent()->GetMatrix_world();
				matPivot.SetTranslation( m_pNode->GetPosition_world().x, m_pNode->GetPosition_world().y, m_pNode->GetPosition_world().z );
			}
		}

		if ( m_Nodes && m_Nodes->Count() )
		{
			float3 cen(0,0,0);

			for (int i=0; i<m_Nodes->Count(); i++)
			{
				cen += m_Nodes->At(i)->GetPosition_world();
			}
			cen.x /=  m_Nodes->Count();
			cen.y /=  m_Nodes->Count();
			cen.z /=  m_Nodes->Count();
			matPivot.Translation( cen.x, cen.y, cen.z );
		}

		DrawPivot(m_Camera, matPivot, pivot_selectedAxis, 10.0f);
	}

	BaseForm::Draw(0);
}

void Form_EditNode::OnResize( int EditorWidth, int EditorHeight )
{
	//
}

void Form_EditNode::UpdateEditorData_multiple( sx::core::ArrayPNode& nodes, sx::core::Camera& camera, UINT mouseState )
{
	m_Nodes = &nodes;

	m_nodeExplorer.UpdateData(NULL, NULL);

	//  update additional data
	m_Camera = camera;
	m_mouseState = mouseState;
	m_pNode = NULL;

	m_applyChanges = false;
	
	if ( s_multi_nodeCount != nodes.Count() )
	{
		for ( int i=0; i<3; i++ )
		{
			m_pEditPos[i]->SetText( L"0.0" );
			m_pEditRot[i]->SetText( L"0.0" );
		}
		s_multi_nodeCount = nodes.Count();

		s_multi_pos.Set(0,0,0);
		s_multi_rot.Set(0,0,0);
	}
	str1024 str = L"Selected ";
	str << nodes.Count() << L" nodes";
	m_pEditName->SetText( str );

	m_applyChanges = true;

	//Close();
}

void Form_EditNode::UpdateEditorData( sx::core::PNode node, sx::core::PNodeMember member, sx::core::Camera& camera, UINT mouseState )
{
	m_Nodes = NULL;

	m_nodeExplorer.UpdateData(node, member);

	//  verify the mesh
	if ( !node )
	{
		m_pNode = NULL;
		Close();
		return;
	}
	
	s_multi_nodeCount = 0;

	//  update additional data
	m_Camera = camera;
	m_mouseState = mouseState;

	m_applyChanges = false; //  to avoid applying changes by editors	
	if ( m_pNode != node )
	{
		// get position and rotation of the node
		float3 rotation, position= node->GetPosition_local();
		node->GetMatrix_local().GetRoationXYZ(rotation.x, rotation.y, rotation.z);

		for (int i=0; i<3; i++)
		{
			m_pEditPos[i]->SetText(FloatToStr( position[i] ) );
			m_pEditRot[i]->SetText(FloatToStr(	sx::math::RadToDeg(rotation[i]) ) );
		}
	}

	//  update title
	if ( member )
	{
		if ( member != m_pMember )
			m_pEditName->SetText( member->GetName() ? member->GetName() : L"NONAME" );
	}
	else if ( node )
	{
		m_pEditName->SetText( node->GetName() );
		m_drawOrder->SetText( IntToStr( node->m_drawOrder ) );
	}

	m_applyChanges = true;

	m_pNode = node;
	m_pMember = member;
}

PNodeExplorer Form_EditNode::GetNodeExplorer( void )
{
	return &m_nodeExplorer;
}

sx::core::PNode Form_EditNode::GetSelectedNode( void )
{
	return m_nodeExplorer.GetSelectedNode();
}

sx::core::PNodeMember Form_EditNode::GetSelectedMember( void )
{
	return m_nodeExplorer.GetSelectedMember();
}

void Form_EditNode::SetUndoManager( UndoManager* undoManager )
{
	m_undoManager = undoManager;
}

void Form_EditNode::DrawPivot( sx::core::Camera& cam, sx::math::Matrix& matWorld, UINT selectedAxis, float fSize /*= 10.0f*/ )
{
	using namespace sx;

	d3d::UI3D::ReadyToDebug( 0xffffffff );

	//  get world matrix and world position
	Matrix matScale;
// 	if ( !matParent )
// 	{
// 		matWorld.Translation( matPivot->_41, matPivot->_42, matPivot->_43 );
// 	}
// 	else
// 	{
// 		matWorld = matParent;
// 		matWorld.SetTranslation( matPivot->_41, matPivot->_42, matPivot->_43 );
// 	}

	float3 d( matWorld._41 - cam.Eye.x, matWorld._42 - cam.Eye.y, matWorld._43 - cam.Eye.z );
	float dis = d.Length(); 
	if (dis<EPSILON) dis = EPSILON;
	dis *= fSize/PIVOT_SCALE_PERCENT;
	matScale.Scale(dis, dis, dis);
	matWorld.Multiply( matScale, matWorld );
	d3d::Device3D::Matrix_World_Set(matWorld);

	// draw selected plane
	d3d::Device3D::RS_ZEnabled(false);
	d3d::Device3D::RS_Culling(false);
	d3d::Device3D::RS_Alpha( SX_MATERIAL_ALPHABLEND );

	if ( selectedAxis & EDITOR_MOUSEAXIS_X && selectedAxis & EDITOR_MOUSEAXIS_Y )
	{
		d3d::Device3D::SetMaterialColor( 0x77ffff00 );
		d3d::UI3D::DrawRectangle( float3(0,0.4f,0), float3(0.4f,0.4f,0), float3(0.4f,0,0), math::VEC3_ZERO );
	}
	else if ( selectedAxis & EDITOR_MOUSEAXIS_Z && selectedAxis & EDITOR_MOUSEAXIS_Y )
	{
		d3d::Device3D::SetMaterialColor( 0x77ffff00 );
		d3d::UI3D::DrawRectangle( float3(0,0.4f,0), float3(0,0.4f,0.4f), float3(0,0,0.4f), math::VEC3_ZERO );
	}
	else if ( selectedAxis & EDITOR_MOUSEAXIS_X && selectedAxis & EDITOR_MOUSEAXIS_Z )
	{
		d3d::Device3D::SetMaterialColor( 0x77ffff00 );
		d3d::UI3D::DrawRectangle( float3(0,0,0.4f), float3(0.4f,0,0.4f), float3(0.4f,0,0), math::VEC3_ZERO );
	}

	d3d::Device3D::RS_Alpha(0);
	d3d::Device3D::RS_Culling(true);

	//  draw axis
	d3d::Device3D::SetMaterialColor( (selectedAxis & EDITOR_MOUSEAXIS_X) ? 0xffffff00 : 0xffff0000);
	d3d::UI3D::DrawLine(math::VEC3_ZERO, math::VEC3_X);
	d3d::Device3D::SetMaterialColor( (selectedAxis & EDITOR_MOUSEAXIS_X && selectedAxis & EDITOR_MOUSEAXIS_Y) ? 0xffffff00 : 0xffff0000);
	d3d::UI3D::DrawLine(float3(0.4f,0,0), float3(0.4f,0.4f,0));
	d3d::Device3D::SetMaterialColor( (selectedAxis & EDITOR_MOUSEAXIS_X && selectedAxis & EDITOR_MOUSEAXIS_Z) ? 0xffffff00 : 0xffff0000);
	d3d::UI3D::DrawLine(float3(0.4f,0,0), float3(0.4f,0,0.4f));

	d3d::Device3D::SetMaterialColor( (selectedAxis & EDITOR_MOUSEAXIS_Y) ? 0xffffff00 : 0xff00ff00);
	d3d::UI3D::DrawLine(math::VEC3_ZERO, math::VEC3_Y);
	d3d::Device3D::SetMaterialColor( (selectedAxis & EDITOR_MOUSEAXIS_Y && selectedAxis & EDITOR_MOUSEAXIS_X) ? 0xffffff00 : 0xff00ff00);
	d3d::UI3D::DrawLine(float3(0,0.4f,0), float3(0.4f,0.4f,0));
	d3d::Device3D::SetMaterialColor( (selectedAxis & EDITOR_MOUSEAXIS_Y && selectedAxis & EDITOR_MOUSEAXIS_Z) ? 0xffffff00 : 0xff00ff00);
	d3d::UI3D::DrawLine(float3(0,0.4f,0), float3(0,0.4f,0.4f));

	d3d::Device3D::SetMaterialColor( (selectedAxis & EDITOR_MOUSEAXIS_Z) ? 0xffffff00 : 0xff0000ff);
	d3d::UI3D::DrawLine(math::VEC3_ZERO, math::VEC3_Z);
	d3d::Device3D::SetMaterialColor( (selectedAxis & EDITOR_MOUSEAXIS_Z && selectedAxis & EDITOR_MOUSEAXIS_X) ? 0xffffff00 : 0xff0000ff);
	d3d::UI3D::DrawLine(float3(0,0,0.4f), float3(0.4f,0,0.4f));
	d3d::Device3D::SetMaterialColor( (selectedAxis & EDITOR_MOUSEAXIS_Z && selectedAxis & EDITOR_MOUSEAXIS_Y) ? 0xffffff00 : 0xff0000ff);
	d3d::UI3D::DrawLine(float3(0,0,0.4f), float3(0,0.4f,0.4f));

	Matrix pvtWorld;
	matScale.Scale(0.08f, 0.4f, 0.08f);

	//  draw cone of x axis
	pvtWorld.RotationZ(-math::PIDIV2);
	pvtWorld.Multiply( matScale, pvtWorld );
	pvtWorld._41 = 1.0f;
	pvtWorld.Multiply( pvtWorld, matWorld );
	d3d::Device3D::Matrix_World_Set(pvtWorld);
	d3d::UI3D::DrawCone((selectedAxis == EDITOR_MOUSEAXIS_X) ? 0xffffff00 : 0xffff0000);

	//  draw cone of y axis
	pvtWorld.Identity();
	pvtWorld.Multiply( matScale, pvtWorld );
	pvtWorld._42 = 1.0f;
	pvtWorld.Multiply( pvtWorld, matWorld );
	d3d::Device3D::Matrix_World_Set(pvtWorld);
	d3d::UI3D::DrawCone((selectedAxis == EDITOR_MOUSEAXIS_Y) ? 0xffffff00 : 0xff00ff00);

	//  draw cone of z axis
	pvtWorld.RotationX(math::PIDIV2);
	pvtWorld.Multiply( matScale, pvtWorld );
	pvtWorld._43 = 1.0f;
	pvtWorld.Multiply( pvtWorld, matWorld );
	d3d::Device3D::Matrix_World_Set(pvtWorld);
	d3d::UI3D::DrawCone((selectedAxis == EDITOR_MOUSEAXIS_Z) ? 0xffffff00 : 0xff0000ff);

	d3d::Device3D::RS_ZEnabled(true);

}

UINT Form_EditNode::SelectPivotAxis( sx::core::Camera& cam, sx::math::Matrix& matWorld, float fSize /*= 10.0f*/ )
{
	using namespace sx;

	UINT result = 0;

	//  create ray for intersection test
	float3 hitPointRay;
	Matrix matView, matProj;
	cam.GetViewMatrix( matView );
	cam.GetProjectionMatrix( matProj );
	math::Ray ray( SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0), SEGAN_VP_WIDTH, SEGAN_VP_HEIGHT, matView, matProj );

	//  get world matrix and world position
	Matrix matScale;
	float3 d( matWorld._41 - cam.Eye.x, matWorld._42 - cam.Eye.y, matWorld._43 - cam.Eye.z );
	float dis = d.Length(); 
	if (dis<EPSILON) dis = EPSILON;
	dis *= fSize/PIVOT_SCALE_PERCENT;
	matScale.Scale(dis, dis, dis);
	matWorld.Multiply( matScale, matWorld );


	Matrix pvtWorld;
	matScale.Scale(0.08f, 0.4f, 0.08f);

	//  test x axis
	pvtWorld.RotationZ(-math::PIDIV2);
	pvtWorld.Multiply( matScale, pvtWorld );
	pvtWorld._41 = 1.0f;
	pvtWorld.Multiply( pvtWorld, matWorld );
	if ( d3d::UI3D::IntersectCone( pvtWorld, ray, hitPointRay ) )
		result |= EDITOR_MOUSEAXIS_X;


	//  test y axis
	pvtWorld.Identity();
	pvtWorld.Multiply( matScale, pvtWorld );
	pvtWorld._42 = 1.0f;
	pvtWorld.Multiply( pvtWorld, matWorld );
	if ( d3d::UI3D::IntersectCone( pvtWorld, ray, hitPointRay ) )
		result |= EDITOR_MOUSEAXIS_Y;

	//  test z axis
	pvtWorld.RotationX(math::PIDIV2);
	pvtWorld.Multiply( matScale, pvtWorld );
	pvtWorld._43 = 1.0f;
	pvtWorld.Multiply( pvtWorld, matWorld );
	if ( d3d::UI3D::IntersectCone( pvtWorld, ray, hitPointRay ) )
		result |= EDITOR_MOUSEAXIS_Z;

	//  test planes
	if ( !result )
	{
		float disXY=FLT_MAX, disZY=FLT_MAX, disXZ=FLT_MAX;
		if ( d3d::UI3D::IntersectRect(Rect3D(float3(0,0.4f,0),float3(0.4f,0.4f,0),float3(0.4f,0,0),math::VEC3_ZERO), matWorld, ray, hitPointRay, true) )
			disXY = hitPointRay.Distance( cam.Eye );
		if ( d3d::UI3D::IntersectRect(Rect3D(float3(0,0.4f,0),float3(0,0.4f,0.4f),float3(0,0,0.4f),math::VEC3_ZERO), matWorld, ray, hitPointRay, true) )
			disZY = hitPointRay.Distance( cam.Eye );
		if ( d3d::UI3D::IntersectRect(Rect3D(float3(0,0,0.4f),float3(0.4f,0,0.4f),float3(0.4f,0,0),math::VEC3_ZERO), matWorld, ray, hitPointRay, true) )
			disXZ = hitPointRay.Distance( cam.Eye );

		//  specialize XZ plane to select
		if ( disXZ < FLT_MAX )
			result |= EDITOR_MOUSEAXIS_X | EDITOR_MOUSEAXIS_Z;
		else if ( (disXY < FLT_MAX) && (disXY < disZY) && (disXY < disXZ) )
			result |= EDITOR_MOUSEAXIS_X | EDITOR_MOUSEAXIS_Y;
		else if ( (disZY < FLT_MAX) && (disZY < disXY) && (disZY < disXZ) )
			result |= EDITOR_MOUSEAXIS_Z | EDITOR_MOUSEAXIS_Y;
	}

	return result;
}

void Form_EditNode::OnMouseWheel( sx::gui::PControl Sender )
{
	if (!Sender || !m_applyChanges) return;

	switch ( Sender->GetType() )
	{
	case GUI_TEXTEDIT:
		{
			for (int i=0; i<3; i++)
			{
				sx::gui::PTextEdit pEdit = m_pEditRot[i];
				if ( Sender == pEdit && pEdit->GetFocused() )
				{
					float r = str128::StrToFloat( pEdit->GetText() );
					r += sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_WHEEL] * 5.0f;
					pEdit->SetText(FloatToStr(	r ) );
					break;
				}

				pEdit = m_pEditPos[i];
				if ( Sender == pEdit && pEdit->GetFocused() )
				{
					float r = str128::StrToFloat( pEdit->GetText() );
					r += sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_WHEEL];
					pEdit->SetText(FloatToStr(	r ) );
					break;
				}
			}
		}
	}
}

void Form_EditNode::OnParamChange( sx::gui::PControl Sender )
{
	if (!Sender || !m_applyChanges || m_pBack->State_GetIndex()==0 ) return;

	if ( m_pNode && Sender ==  m_pEditName )
	{
		if ( m_pMember )
			m_pMember->SetName( m_pEditName->GetText() );
		else
			m_pNode->SetName( m_pEditName->GetText() );

		m_nodeExplorer.UpdateData(NULL, NULL);

		return;
	}

	if ( Sender == m_drawOrder )
	{
		int value = str256		::StrToInt( m_drawOrder->GetText() );

		if ( m_pNode )
			m_pNode->m_drawOrder = value;

		for ( int i=0; m_Nodes && i<m_Nodes->Count(); i++ )
			m_Nodes->At(i)->m_drawOrder = value;

		return;
	}

	if( Sender == m_playSound )
	{
		msg_SoundPlay sndmsg( true );
		if( m_pNode )
			m_pNode->MsgProc( MT_SOUND_PLAY, &sndmsg );

		if( m_Nodes && m_Nodes->Count() )
		{
			for ( int i=0; i<m_Nodes->Count(); i++ )
			{
				m_Nodes->At( i )->MsgProc( MT_SOUND_PLAY, &sndmsg );
			}
		}
	}

	if( Sender == m_sprayParticle )
	{
		msg_Particle msgpar( SX_PARTICLE_SPRAY );
		if( m_pNode )
			m_pNode->MsgProc( MT_PARTICLE, &msgpar );

		if( m_Nodes && m_Nodes->Count() )
		{
			for ( int i=0; i<m_Nodes->Count(); i++ )
			{
				m_Nodes->At( i )->MsgProc( MT_PARTICLE, &msgpar );
			}
		}
	}

	switch ( Sender->GetType() )
	{
	case GUI_TEXTEDIT:
		if ( m_pNode || m_Nodes->Count() )
		{
			if( m_undoManager )
			{
				if ( m_pNode )
					m_undoManager->AddAction( UAT_MOVE, &m_pNode, 1);
				else
					m_undoManager->AddAction( UAT_MOVE, &m_Nodes->At(0), m_Nodes->Count() );
			}

			if ( m_pNode )
			{
				float3 pos;
				pos[0] = str128::StrToFloat( m_pEditPos[0]->GetText() );
				pos[1] = str128::StrToFloat( m_pEditPos[1]->GetText() );
				pos[2] = str128::StrToFloat( m_pEditPos[2]->GetText() );
				m_pNode->SetPosition(pos);

				m_pNode->SetRotation(
					sx::math::DegToRad( str128::StrToFloat( m_pEditRot[0]->GetText() ) ),
					sx::math::DegToRad( str128::StrToFloat( m_pEditRot[1]->GetText() ) ),
					sx::math::DegToRad( str128::StrToFloat( m_pEditRot[2]->GetText() ) )
					);
			}
			else
			{
				float3 editValue;
				bool asgroup = m_groupChange->Checked();
				
				//  position
				editValue[0] = str128::StrToFloat( m_pEditPos[0]->GetText() );
				editValue[1] = str128::StrToFloat( m_pEditPos[1]->GetText() );
				editValue[2] = str128::StrToFloat( m_pEditPos[2]->GetText() );
				for ( int i=0; i<m_Nodes->Count(); i++ )
				{
					float3 pos = m_Nodes->At(i)->GetPosition_local();
					if ( asgroup )
					{
						pos.x += editValue[0] - s_multi_pos[0];
						pos.y += editValue[1] - s_multi_pos[1];
						pos.z += editValue[2] - s_multi_pos[2];
					}
					else
					{
						if ( Sender == m_pEditPos[0] ) pos.x = editValue[0];
						if ( Sender == m_pEditPos[1] ) pos.y = editValue[1];
						if ( Sender == m_pEditPos[2] ) pos.z = editValue[2];
					}
					m_Nodes->At(i)->SetPosition(pos);
				}

				if ( asgroup )
				{
					s_multi_pos.x = editValue.x;
					s_multi_pos.y = editValue.y;
					s_multi_pos.z = editValue.z;
				}

				//	rotate
				editValue[0] = str128::StrToFloat( m_pEditRot[0]->GetText() );
				editValue[1] = str128::StrToFloat( m_pEditRot[1]->GetText() );
				editValue[2] = str128::StrToFloat( m_pEditRot[2]->GetText() );
				for ( int i=0; i<m_Nodes->Count(); i++ )
				{
					// get rotation of the node
					float3 rotation;
					m_Nodes->At(i)->GetMatrix_local().GetRoationXYZ( rotation.x, rotation.y, rotation.z );
					if ( asgroup )
					{
						rotation.x += sx::math::DegToRad( editValue[0] - s_multi_rot[0] );
						rotation.y += sx::math::DegToRad( editValue[1] - s_multi_rot[1] );
						rotation.z += sx::math::DegToRad( editValue[2] - s_multi_rot[2] );
					}
					else
					{
						if ( Sender == m_pEditRot[0] ) rotation.x = sx::math::DegToRad( editValue[0] );
						if ( Sender == m_pEditRot[1] ) rotation.y = sx::math::DegToRad( editValue[1] );
						if ( Sender == m_pEditRot[2] ) rotation.z = sx::math::DegToRad( editValue[2] );
					}

					m_Nodes->At(i)->SetRotation( rotation.x, rotation.y, rotation.z );
				}
				
				if ( asgroup )
				{
					s_multi_rot.x = editValue.x;
					s_multi_rot.y = editValue.y;
					s_multi_rot.z = editValue.z;
				}
			}
		}
		break;
	}
}

