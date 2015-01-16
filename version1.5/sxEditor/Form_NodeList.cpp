#include "Form_NodeList.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "EditorScene.h"


Form_NodeList::Form_NodeList( void ): BaseForm()
{
	//  frequency of sound
	m_name = EditorUI::CreateEditBox( m_pBack, 0, 230, L"" );
	SEGAN_GUI_SET_ONTEXT( m_name, Form_NodeList::OnParamChange );

	//	list of sounds
	m_list = sx_new( sx::gui::ListBox );
	m_list->SetParent( m_pBack );
	m_list->SetSize( 230, 100.0f, 18.0f, false );
	m_list->SetFont( EditorUI::GetDefaultFont(8) );
	m_list->SetItemIndex( 0 );
	m_list->SetOnSelect( this, (GUICallbackEvent)&Form_NodeList::OnListChange );

	//  stop resizing
	//SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	m_MinSize.Set( 200, 200 );
	SetSize( 250, 500 );
}

Form_NodeList::~Form_NodeList( void )
{
	sx_delete_and_null( m_list );
	//  for other things the parent will do everything :)
}

void Form_NodeList::SetSize( float width, float height )
{
	float top = height * 0.5f - 16.0f;
	m_name->SetSize( float2( width - 16.0f, 18.0f ) );
	m_name->Position().Set( 0.0f, top, 0.0f );

	m_list->SetSize( width - 16.0f, height - 40.0f, 18.0f, false );
	m_list->Position().Set( 0.0f, -10.0f, 0.0f );

	BaseForm::SetSize(width, height);
}

void Form_NodeList::ProcessInput( bool& inputHandled )
{
	if ( !IsVisible() || inputHandled ) return;

	if ( SEGAN_KEYUP(0, SX_INPUT_KEY_ESCAPE) )
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
}

void Form_NodeList::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	m_list->Update(elpsTime);
	BaseForm::Update(elpsTime);

	//  update position states
//	m_pBack->State_GetByIndex(0).Position = m_pBack->State_GetByIndex(1).Position;	
}


//////////////////////////////////////////////////////////////////////////
//	PRIVATE ZOON

void Form_NodeList::OnParamChange( sx::gui::PControl Sender )
{
	if ( Sender == m_name )
	{
		ReloadList();
	}
}

void Form_NodeList::OnListChange( sx::gui::PControl Sander )
{
	int index = m_list->GetItemIndex();
	EditorScene::selectedNodes.Clear();
	EditorScene::selectedMember = NULL;
	for ( int i=0; i<m_list->Count(); ++i )
	{
		if ( m_list->GetItem(i).IsSelected() || index == i )
			EditorScene::selectedNodes.PushBack( (sx::core::PNode)m_list->GetItem(i).UserData );
	}
}

void Form_NodeList::ReloadList( void )
{
	m_list->Clear();

	str512 nodeName, currName = m_name->GetText();
	sx::core::ArrayPNode nodes;
	sx::core::Scene::GetAllNodes( nodes );
	for ( int i=0; i<nodes.Count(); ++i )
	{
		sx::core::Node* node = nodes[i];
		if ( node->MemberTypeExist( EditorScene::mouse_select_type ) )
		{
			if ( currName.Length() )
			{
				nodeName = node->GetName();
				if ( currName.Length() == 1 )
				{
					if ( nodeName[0] == currName[0] )
					{
						m_list->Add( nodeName, null, node );
					}
				}
				else if ( nodeName.Find( currName ) > -1 )
				{
					m_list->Add( nodeName, null, node );
				}
			}
			else
			{
				m_list->Add( nodes[i]->GetName(), null, node );
			}
		}
	}
}

void Form_NodeList::Show( void )
{
	float x = SEGAN_MOUSE_ABSX(0) - SEGAN_VP_WIDTH/2;
	float y = SEGAN_VP_HEIGHT/2 - SEGAN_MOUSE_ABSY(0) - GetSize().y * 0.5f - 41.5f;
	m_pBack->State_GetByIndex(0).Position.Set( x, y, 0 );
	m_pBack->State_GetByIndex(1).Position.Set( x, y, 0 );
	ReloadList();
	BaseForm::Show();
}
