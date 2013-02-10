#include "Form_SetName.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "EditorUI.h"
#include "EditorSettings.h"

#include "EditorObject.h"
#include "EditorScene.h"

#define FORM_SETNAME_WIDTH		300.0f		//  size of the form
#define FORM_SETNAME_HEIGHT		180.0f		//  size of the form

static float s_Shake = 0.0f;

Form_SetName::Form_SetName( void ): BaseForm(), m_pMember(0), m_pNode(0)
{
	// invisible close button
	m_pClose->RemProperty( SX_GUI_PROPERTY_VISIBLE );

	m_pTitle = sx_new( sx::gui::Label );
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->AddProperty(SX_GUI_PROPERTY_MULTILINE);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Set name for new object");

	m_pPanel = sx_new( sx::gui::Panel );
	m_pPanel->SetParent(m_pBack);
	m_pPanel->SetSize( float2(99999, 99999) );
	m_pPanel->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	m_pPanel->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.1f);

	m_lb_memName	= EditorUI::CreateLabel(m_pPanel, FORM_SETNAME_WIDTH, 8, L"Set a name for new object :");
	m_ed_memName	= EditorUI::CreateEditBox(m_pPanel, 0.0f, FORM_SETNAME_WIDTH-60.0f);
	m_ch_newNode	= EditorUI::CreateCheckBox(m_pPanel, -FORM_SETNAME_WIDTH/2 + 20.0f, FORM_SETNAME_WIDTH-60.0f, L"Create new node for this member");
	m_lb_nodName	= EditorUI::CreateLabel(m_pPanel, FORM_SETNAME_WIDTH, 8, L"Set a name for new node :");
	m_ed_nodName	= EditorUI::CreateEditBox(m_pPanel, 0.0f, FORM_SETNAME_WIDTH-60.0f);
	m_btn_Ok		= EditorUI::CreateButtonEx(m_pPanel, 64.0f, 32.0f, EditorUI::GetButtonTexture(L"OK", 0) );
	m_btn_Ok->SetSize( float2(64.0f, 32.0f) );
	SEGAN_GUI_SET_ONCLICK(m_btn_Ok, Form_SetName::OnParamChange);

	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);

	SetSize( FORM_SETNAME_WIDTH, FORM_SETNAME_HEIGHT );
}

Form_SetName::~Form_SetName( void )
{
	//  for other things the parent will do everything :)
}

void Form_SetName::SetSize( float width, float height )
{
	float top = height/2 - 15.0f;
	float left = -width/2;

	m_pTitle->SetSize( float2(width - 70.0f, 40.0f) );
	m_pTitle->Position().y = top;			top -= 25.0f;

	m_lb_memName->Position().y = top;		top -= 18.0f;
	m_ed_memName->Position().y = top;		top -= 30.0f;
	m_ch_newNode->Position().y = top;		top -= 30.0f;
	m_lb_nodName->Position().y = top;		top -= 18.0f;
	m_ed_nodName->Position().y = top;		top -= 35.0f;
	m_btn_Ok->Position().y = top;			top -= 15.0f;
	m_btn_Ok->Position().x = -left - 50.0f;

	BaseForm::SetSize( width, height );
}

void Form_SetName::ProcessInput( bool& inputHandled )
{
	if ( !IsVisible() || inputHandled ) return;
	BaseForm::ProcessInput(inputHandled);
}

void Form_SetName::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	if ( m_ch_newNode->Checked() )
	{
		m_lb_nodName->AddProperty(SX_GUI_PROPERTY_VISIBLE);
		m_ed_nodName->AddProperty(SX_GUI_PROPERTY_VISIBLE);
	}
	else
	{
		m_lb_nodName->RemProperty(SX_GUI_PROPERTY_VISIBLE);
		m_ed_nodName->RemProperty(SX_GUI_PROPERTY_VISIBLE);
	}

	//  update position states
	if (s_Shake>0)
	{
		s_Shake -= elpsTime;
		m_pBack->State_GetByIndex(1).Position.Set(10.0f - sx::cmn::Random(20.0f),0,0);
	}
	else	
		m_pBack->State_GetByIndex(1).Position.Set(0,0,0);

	m_pBack->State_GetByIndex(0).Position.Set(0,0,0);

	BaseForm::Update(elpsTime);

	//  force to close other editor window to avoid conflicts
// 	EditorScene::frm_EditMesh->Close();
// 	EditorObject::frm_EditMesh->Close();
// 	EditorScene::frm_EditTrrn->Close();
// 	EditorScene::frm_EditPath->Close();

}

void Form_SetName::Draw( DWORD flag )
{
	BaseForm::Draw(0);
}

void Form_SetName::OnResize( int EditorWidth, int EditorHeight )
{
	//
}

void Form_SetName::UpdateEditorData( sx::core::PNode node, sx::core::PNodeMember member )
{
	//  verify the member
	if ( !member && !node )
	{
		m_pNode = NULL;
		m_pMember = NULL;

		Close();
		return;
	}
	if (m_pNode == node && m_pMember == member) return;

	m_pNode = node;
	m_pMember = member;

	if (node && member)
	{
		m_ch_newNode->Checked() = false;
		m_ch_newNode->GetElement(0)->Color() = D3DColor( 1.0f, 0.6f, 0.6f, 1.0f );
		m_ch_newNode->AddProperty(SX_GUI_PROPERTY_ENABLE);
		m_ch_newNode->GetChild(0)->GetElement(1)->Color() = EditorUI::GetDefaultFontColor();
	}
	else
	{
		m_ch_newNode->Checked() = true;
		m_ch_newNode->GetElement(0)->Color() = 0xaaaaaaaa;
		m_ch_newNode->RemProperty(SX_GUI_PROPERTY_ENABLE);
		m_ch_newNode->GetChild(0)->GetElement(1)->Color() = 0xaaaaaaaa;
	}

	if (member)
	{
		m_ed_memName->AddProperty(SX_GUI_PROPERTY_ENABLE);
		m_ed_memName->GetElement(1)->Color() = EditorUI::GetDefaultFontColor();
	}
	else
	{
		m_ed_memName->RemProperty(SX_GUI_PROPERTY_ENABLE);
		m_ed_memName->GetElement(1)->Color() = 0xaaaaaaaa;
	}
}

void Form_SetName::OnParamChange( sx::gui::PControl Sender )
{
	if ( !Sender ) return;

	if ( Sender == m_btn_Ok )
	{
		//  collect info
		sx::core::PNode node = NULL;
		str1024 memName = CorrectName( m_ed_memName->GetText() );
		str1024 nodName = CorrectName( m_ed_nodName->GetText() );

		//  validate names
		if ( m_pMember && !memName.Text() )
		{
			m_ed_memName->SetFocused(true);
			s_Shake = 100.0f;
			return;
		}

		if ( (!m_pMember && m_pNode && !nodName.Text()) || (m_ch_newNode->Checked() && !nodName.Text()) )
		{
			m_ed_nodName->SetFocused(true);
			s_Shake = 100.0f;
			return;
		}

		//  apply changes
		if ( m_pMember )
		{
			m_pMember->SetName( memName );
			if ( m_ch_newNode->Checked() )
			{
				node = sx::core::Scene::CreateNode();
				node->SetName( nodName );
				m_pMember->SetOwner( node );
			}

			//  now we have 4 state
			if (m_pNode && node)
			{
				node->SetParent( m_pNode );
			}
			else if (m_pNode && !node)
			{
				m_pMember->SetOwner( m_pNode );
			}
			else if (!m_pNode && node)
			{
				switch ( Editor::GetSpace() )
				{
				case EWS_OBJECT:
					{
						if ( EditorObject::node_Root )
							node->SetParent(EditorObject::node_Root);
						else
							EditorObject::node_Root = node;

						EditorObject::selectedNode = node;
						EditorObject::selectedMember = m_pMember;
					}
					break;

				case EWS_SCENE:
					{
						node->SetPosition(sx::core::Renderer::GetCamera()->At);
						sx::core::Scene::AddNode( node );
						EditorScene::mng_UndoManager.AddAction( UAT_CREATE, &node, 1);
					}
					break;
				}
			}
			else
			{
				s_Shake = 100.0f;
				return;
			}

		}
		else if ( m_pNode )
		{
			m_pNode->SetName( nodName );
			
			switch ( Editor::GetSpace() )
			{
			case EWS_OBJECT:
				{
					if ( EditorObject::selectedNode )
						m_pNode->SetParent( EditorObject::selectedNode );
					else if ( EditorObject::node_Root )
						m_pNode->SetParent( EditorObject::node_Root );
					else
						EditorObject::node_Root = m_pNode;

					EditorObject::selectedNode = m_pNode;
				}
				break;
			}
		}

		//  finally close dialog
		Close();

	}
}

const WCHAR* Form_SetName::CorrectName( const WCHAR* ObjectName )
{
	static String		 str;

	if (!ObjectName) return NULL;

	str = ObjectName;
	str.Trim();
	if ( !str.Length() ) return NULL;

	for (int i=0; i<str.Length(); i++)
	{
		WCHAR ch = str[i];

		bool isOk = SEGAN_BETWEEN(ch, 'a', 'z') || SEGAN_BETWEEN(ch, 'A', 'Z') || SEGAN_BETWEEN(ch, '0', '9') || ch=='_' || ch=='@';
		if (!isOk) str[i] = '_';
	}

	return str;
}

void Form_SetName::Show( void )
{
	BaseForm::Show();

	//  hide toolbar
	EditorScene::frm_Toolbar->Close();
	EditorObject::frm_Toolbar->Close();
}

void Form_SetName::Close( void )
{
	m_pNode = NULL;
	m_pMember = NULL;

	BaseForm::Close();

	//  show toolbar & node editor
	EditorScene::frm_Toolbar->Show();
	EditorObject::frm_Toolbar->Show();
}