#include "Form_SceneTree.h"
#include "sxItemUI.h"
#include "EditorUI.h"
#include "EditorSettings.h"
#include "EditorScene.h"

#define FORM_SCENETREE_WIDTH	300.0f		//  size of the node edit form
#define FORM_SCENETREE_HEIGHT	400.0f		//  size of the node edit form

static float updateTimes = 0;
static float selectTimes = 0;

Form_SceneTree::Form_SceneTree( void ): BaseForm()
{
	m_pTitle	= (sx::gui::PLabel)sx::gui::Create(GUI_LABEL);
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Scene nodes");

	m_nodeList.SetParent(m_pBack);
	m_nodeList.SetSize( FORM_SCENETREE_WIDTH-50.0f, FORM_SCENETREE_HEIGHT-40.0f);
	m_nodeList.Position().y = -20.0f;

	//  stop moving and resizing
	//SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);
	//SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);

	m_MinSize.x = 300.0f;	
	m_MinSize.y = 300.0f;
	SetSize( FORM_SCENETREE_WIDTH, FORM_SCENETREE_HEIGHT );

	//  set new states
	m_pBack->State_GetByIndex(0).Center		= float3(0.0f, 0.0f, 0.0f);
	m_pBack->State_GetByIndex(0).Position	= float3(0.0f, 0.0f, 0.0f);
	m_pBack->State_GetByIndex(0).Scale		= float3(1.0f, 0.0f, 1.0f);

	m_pBack->State_GetByIndex(1).Center		= float3(0.0f, 0.0f, 0.0f);
	m_pBack->State_GetByIndex(1).Position	= float3(0.0f, 0.0f, 0.0f);
	m_pBack->State_GetByIndex(1).Scale		= float3(1.0f, 1.0f, 1.0f);
}

Form_SceneTree::~Form_SceneTree( void )
{

}

void Form_SceneTree::SetSize( float width, float height )
{
	m_pTitle->SetSize( float2(width - 70.0f, 24.0f) );
	m_pTitle->Position().y = height*0.5f - 10.0f;

	BaseForm::SetSize(width, height);
}

void Form_SceneTree::ProcessInput( bool& result, int playerID /*= 0*/ )
{
	if (result) return;

	const char* keys = sx::io::Input::GetKeys(playerID);

	if (keys[SX_INPUT_KEY_ESCAPE] == SX_INPUT_STATE_UP)
	{
		Close();
		result = true;
		return;
	}

	BaseForm::ProcessInput(result, playerID);
}

void Form_SceneTree::Update( float elpsTime )
{
	m_nodeList.Update(elpsTime);
	BaseForm::Update(elpsTime);
}

void Form_SceneTree::UpdateEditorData( sx::core::ArrayPNode& nodeList, sx::core::PNode selectedNode, float elpsTime )
{	
	updateTimes += elpsTime;
	selectTimes += elpsTime;

	if (updateTimes > 1000)
	{
		updateTimes = 0;

		m_nodeList.Clear();
		for (int i=0; i<nodeList.Count(); i++)
		{
			m_nodeList.Add( nodeList[i]->GetName(), EditorUI::GetMemberTexture(NMT_UNKNOWN), nodeList[i] );
		}
	}

	if (selectTimes > 100)
	{
		selectTimes = 0;
		for (int i=0; i<m_nodeList.Count(); i++)
		{
			if ( m_nodeList[i].UserData == selectedNode )
			{
				m_nodeList.SetItemIndex(i);
				break;
			}
		}
	}
}

void Form_SceneTree::UpdateSoon( void )
{
	updateTimes = 999999.0f;
}

void Form_SceneTree::OnSelectNode( sx::gui::PControl Sender )
{
	int index = m_nodeList.GetItemIndex();
	if (index<0) return;

	EditorScene::selectedNode = (sx::core::PNode)m_nodeList[index].UserData;
}

