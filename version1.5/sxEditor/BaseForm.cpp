#include "BaseForm.h"
#include "EditorUI.h"

#define FORM_DEF_SIZE_X		400.0f
#define FORM_DEF_SIZE_Y		300.0f
#define FORM_BORDER_SIZE	16.0f

static Array<PBaseForm> s_formNormal;		//  normal forms
static Array<PBaseForm> s_formOnTop;		//  always on top forms

BaseForm::BaseForm( void ): sx::gui::Form(), m_Option(FORM_OPTION_MOVABLE | FORM_OPTION_RESIZABLE), m_MaxSize(9999.0f, 9999.0f), m_MinSize(20.0f, 15.0f), m_pBack(NULL)
{
	using namespace sx::gui;

	m_pBack		= (PPanelEx)sx::gui::Create(GUI_PANELEX);
	m_pTop		= (PPanel)sx::gui::Create(GUI_PANEL);
	m_pBottom	= (PPanel)sx::gui::Create(GUI_PANEL);
	m_pLeft		= (PPanel)sx::gui::Create(GUI_PANEL);
	m_pRight	= (PPanel)sx::gui::Create(GUI_PANEL);
	m_pTopLeft	= (PPanel)sx::gui::Create(GUI_PANEL);
	m_pTopRight	= (PPanel)sx::gui::Create(GUI_PANEL);
	m_pBotLeft	= (PPanel)sx::gui::Create(GUI_PANEL);
	m_pBotRight	= (PPanel)sx::gui::Create(GUI_PANEL);
	m_pClose	= (PButton)sx::gui::Create(GUI_BUTTON);

	m_pBack->GetElement(0)->SetTextureSrc(		EditorUI::GetFormTexture(L"Back")	);
	m_pTop->GetElement(0)->SetTextureSrc(		EditorUI::GetFormTexture(L"Side")	);
	m_pBottom->GetElement(0)->SetTextureSrc(	EditorUI::GetFormTexture(L"Side")	);
	m_pLeft->GetElement(0)->SetTextureSrc(		EditorUI::GetFormTexture(L"Side")	);
	m_pRight->GetElement(0)->SetTextureSrc(		EditorUI::GetFormTexture(L"Side")	);
	m_pTopLeft->GetElement(0)->SetTextureSrc(	EditorUI::GetFormTexture(L"Corner") );
	m_pTopRight->GetElement(0)->SetTextureSrc(	EditorUI::GetFormTexture(L"Angle")	);
	m_pBotLeft->GetElement(0)->SetTextureSrc(	EditorUI::GetFormTexture(L"Corner") );
	m_pBotRight->GetElement(0)->SetTextureSrc(	EditorUI::GetFormTexture(L"Corner") );
	m_pClose->GetElement(0)->SetTextureSrc(		EditorUI::GetFormTexture(L"Close0")	);
	m_pClose->GetElement(1)->SetTextureSrc(		EditorUI::GetFormTexture(L"Close1")	);
	m_pClose->GetElement(2)->SetTextureSrc(		EditorUI::GetFormTexture(L"Close2")	);

	m_pBack->AddProperty(SX_GUI_PROPERTY_ACTIVATE | SX_GUI_PROPERTY_BLENDCHILDS);
	m_pTop->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	m_pBottom->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	m_pLeft->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	m_pRight->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	m_pTopLeft->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	m_pTopRight->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	m_pBotLeft->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	m_pBotRight->AddProperty(SX_GUI_PROPERTY_ACTIVATE);

	D3DColor color = EditorUI::GetFormColor();
	m_pBack->State_GetCurrent().Color	= color;
	m_pTop->GetElement(0)->Color()		= color;
	m_pBottom->GetElement(0)->Color()	= color;
	m_pLeft->GetElement(0)->Color()		= color;
	m_pRight->GetElement(0)->Color()	= color;
	m_pTopLeft->GetElement(0)->Color()	= color;
	m_pTopRight->GetElement(0)->Color() = color;
	m_pBotLeft->GetElement(0)->Color()	= color;
	m_pBotRight->GetElement(0)->Color() = color;

	m_pTop->SetParent(		m_pBack	);
	m_pBottom->SetParent(	m_pBack	);
	m_pLeft->SetParent(		m_pBack	);
	m_pRight->SetParent(	m_pBack	);
	m_pTopLeft->SetParent(	m_pBack	);
	m_pTopRight->SetParent(	m_pBack	);
	m_pBotLeft->SetParent(	m_pBack	);
	m_pBotRight->SetParent(	m_pBack	);
	m_pClose->SetParent(	m_pBack );

	m_pTopLeft->SetSize(	float2(FORM_BORDER_SIZE, FORM_BORDER_SIZE)	);
	m_pTopRight->SetSize(	float2(FORM_BORDER_SIZE, FORM_BORDER_SIZE)	);
	m_pBotLeft->SetSize(	float2(FORM_BORDER_SIZE, FORM_BORDER_SIZE)	);
	m_pBotRight->SetSize(	float2(FORM_BORDER_SIZE, FORM_BORDER_SIZE)	);
	m_pClose->SetSize(		float2(16.0f, 16.0f) );

	m_pLeft->Rotation().z		=  sx::math::PIDIV2;
	m_pRight->Rotation().z		= -sx::math::PIDIV2;
	m_pBottom->Rotation().z		=  PI;
	m_pBotLeft->Rotation().z	=  sx::math::PIDIV2;
	m_pBotRight->Rotation().z	=  PI;

	m_pBack->SetOnMouseMove(	this, (GUICallbackEvent)&BaseForm::OnMove	);
	m_pTop->SetOnMouseMove(		this, (GUICallbackEvent)&BaseForm::OnMove	);
	m_pTopLeft->SetOnMouseMove(	this, (GUICallbackEvent)&BaseForm::OnMove	);
	m_pTopRight->SetOnMouseMove(this, (GUICallbackEvent)&BaseForm::OnMove	);
	m_pLeft->SetOnMouseMove(	this, (GUICallbackEvent)&BaseForm::OnMove	);
	m_pRight->SetOnMouseMove(	this, (GUICallbackEvent)&BaseForm::OnMove	);
	m_pBottom->SetOnMouseMove(	this, (GUICallbackEvent)&BaseForm::OnMove	);
	m_pBotLeft->SetOnMouseMove(	this, (GUICallbackEvent)&BaseForm::OnMove	);
	m_pBotRight->SetOnMouseMove(this, (GUICallbackEvent)&BaseForm::OnMove	);
	m_pClose->SetOnMouseClick(	this, (GUICallbackEvent)&BaseForm::OnClose);

	m_pBack->State_GetCurrent().Color.w = 0.0f;
	m_pBack->State_GetCurrent().Scale = float3(2.0f, 2.0f, 2.0f);
	m_pBack->State_GetCurrent().Blender.x = 0.05f;

	m_pBack->State_Add();
	m_pBack->State_GetByIndex(1).Color = color;
	m_pBack->State_GetByIndex(1).Scale = float3(1.0f, 1.0f, 1.0f);
	m_pBack->State_GetByIndex(1).Blender.x = 0.05f;

	m_pBack->RemProperty(SX_GUI_PROPERTY_BLENDSTATES);
	m_pBack->State_SetIndex(0);
	m_pBack->AddProperty(SX_GUI_PROPERTY_BLENDSTATES);

	SetSize(FORM_DEF_SIZE_X, FORM_DEF_SIZE_Y);

	s_formNormal.PushBack( this );
}

BaseForm::~BaseForm( void )
{
	SEGAN_GUI_DELETE(m_pBack);

	s_formNormal.Remove( this );
	s_formOnTop.Remove( this );
}

void BaseForm::SetSize( float width, float height )
{
	float2 curSize = GetSize();
	m_pBack->SetSize(	float2(width, height)	);

	// update position to simulate standard window behavior
	for (int i=0; i<m_pBack->State_Count(); i++)
	{
		m_pBack->State_GetByIndex(i).Position.x -= (curSize.x - width) / 2;
		m_pBack->State_GetByIndex(i).Position.y += (curSize.y - height) / 2;
	}

	m_pTop->SetSize(	float2(width,	16.0f)	);
	m_pBottom->SetSize(	float2(width,	16.0f)	);
	m_pLeft->SetSize(	float2(height,	16.0f)	);
	m_pRight->SetSize(	float2(height,	16.0f)	);

	#define offset		8.0f
	float w = width  *	0.5f;
	float h = height *	0.5f;

	m_pTop->Position()		= float3(0.0f		, h + offset, -SX_GUI_Z_BIAS);
	m_pBottom->Position()	= float3(0.0f		,-h - offset, -SX_GUI_Z_BIAS);
	m_pLeft->Position()		= float3(-w - offset, 0.0f		, -SX_GUI_Z_BIAS);
	m_pRight->Position()	= float3( w + offset, 0.0f		, -SX_GUI_Z_BIAS);
	m_pTopLeft->Position()	= float3(-w - offset, h + offset, -SX_GUI_Z_BIAS);
	m_pTopRight->Position()	= float3( w + offset, h + offset, -SX_GUI_Z_BIAS);
	m_pBotLeft->Position()	= float3(-w - offset,-h - offset, -SX_GUI_Z_BIAS);
	m_pBotRight->Position()	= float3( w + offset,-h - offset, -SX_GUI_Z_BIAS);
	m_pClose->Position()	= float3( w + offset - 8.0f, h + offset - 8.0f, -SX_GUI_Z_BIAS);

	//Update(0);
	m_pBack->Update(0);
}

float2 BaseForm::GetSize( void )
{
	return m_pBack->GetSize();
}

void BaseForm::ProcessInput( bool& inputHandled )
{
	return m_pBack->ProcessInput(inputHandled, 0);
}

void BaseForm::Update( float elpsTime )
{
	m_pBack->Update(elpsTime);
}

void BaseForm::Operate( float elpstime, bool& inputHandled )
{

}

void BaseForm::Draw( DWORD flag )
{
	static Matrix matView;
	sx::d3d::Device3D::Matrix_View_Get( matView );
	sx::d3d::Device3D::Texture_SetSampleState(SUV_CLAMP);

	m_pBack->Draw(flag);

	sx::d3d::Device3D::Texture_SetSampleState(SUV_WRAP);
	sx::d3d::Device3D::Matrix_View_Set( matView );
}

sx::gui::PPanelEx BaseForm::GetBack( void )
{
	return m_pBack;
}

void BaseForm::Show( void )
{
	m_pBack->AddProperty(SX_GUI_PROPERTY_ENABLE);
	m_pBack->State_SetIndex(1);
}

void BaseForm::Close( void )
{
	m_pBack->RemProperty(SX_GUI_PROPERTY_ENABLE);
	m_pBack->State_SetIndex(0);
}

void BaseForm::OnMove( sx::gui::PControl Sender )
{
	if (Sender == m_pTop	|| Sender == m_pTopLeft	|| Sender == m_pTopRight	||
		Sender == m_pBack	|| Sender == m_pLeft	|| Sender == m_pRight		||
		Sender == m_pBottom	|| Sender == m_pBotLeft)
	{
		if (m_Option & FORM_OPTION_MOVABLE && Sender->GetSelectedElement() && 
			sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_LEFT] == SX_INPUT_STATE_HOLD)
		{

			m_pBack->Position().x += sx::io::Input::GetCursor_RL(0)->x;
			m_pBack->Position().y -= sx::io::Input::GetCursor_RL(0)->y;
		}
	}

	else if (Sender == m_pBotRight)
	{
		if (m_Option & FORM_OPTION_RESIZABLE && m_pBotRight->GetSelectedElement() &&
			sx::io::Input::GetKeys(0)[SX_INPUT_KEY_MOUSE_LEFT] == SX_INPUT_STATE_HOLD)
		{
			float2 newSize = GetSize();
			
			newSize.x += sx::io::Input::GetCursor_RL(0)->x;// + (sx::d3d::Device3D::Viewport()->Width  % 2);
			newSize.y += sx::io::Input::GetCursor_RL(0)->y;// + !(sx::d3d::Device3D::Viewport()->Height % 2);

			SEGAN_CLAMP(newSize.x, m_MinSize.x, m_MaxSize.x);
			SEGAN_CLAMP(newSize.y, m_MinSize.y, m_MaxSize.y);

			//float2 curSize = GetSize();
			SetSize(newSize.x, newSize.y);
			//m_pBack->Position().x += (newSize.x - curSize.x)*0.5f;	
			//m_pBack->Position().y -= (newSize.y - curSize.y)*0.5f;
		}
	}
}

void BaseForm::OnClose( sx::gui::PControl Sender )
{
	Close();
}

bool BaseForm::IsVisible( void )
{
	return (m_pBack->State_GetIndex() > 0) || !m_pBack->HasProperty( _SX_GUI_NOT_VISIBLE_ );
}

//////////////////////////////////////////////////////////////////////////
//	FORM MANAGER
//////////////////////////////////////////////////////////////////////////
void BaseForm::Manager::ProcessInput( bool& inputHandled )
{
	//  rearrange forms
	for (int i=0; i<s_formNormal.Count(); i++)
	{
		PBaseForm form = s_formNormal[i];
		if ( form->m_Option & FORM_OPTION_ONTOP )
		{
			s_formOnTop.PushBack( form );
			s_formNormal.Remove( form );
			i--;
		}
	}

	//  process input for always on top forms
	int first = -1;
	for (int i=0; i<s_formOnTop.Count(); i++)
	{
		BaseForm* form = s_formOnTop[i];
		if ( form->IsVisible() )
		{
			form->ProcessInput(inputHandled);
			if ( inputHandled && first < 0 )
				first = i;
		}
	}
	if ( first > 0 )
		s_formOnTop.Move(first, 0);

	//  process input for normal forms
	first = -1;
	for (int i=0; i<s_formNormal.Count(); i++)
	{
		BaseForm* form = s_formNormal[i];
		if ( form->IsVisible() )
		{
			form->ProcessInput(inputHandled);
			if ( inputHandled && first < 0 )
				first = i;
		}
	}
	if ( first > 0 )
		s_formNormal.Move(first, 0);
}

void BaseForm::Manager::Update( float elpsTime )
{
	//  update always on top forms
	for (int i=0; i<s_formOnTop.Count(); i++)
	{
		s_formOnTop[i]->Update(elpsTime);
	}

	//  update normal forms
	for (int i=0; i<s_formNormal.Count(); i++)
	{
		s_formNormal[i]->Update(elpsTime);
	}
}

void BaseForm::Manager::Operate( float elpsTime, bool& inputHandled )
{
	//  operate always on top forms
	for (int i=0; i<s_formOnTop.Count(); i++)
	{
		s_formOnTop[i]->Operate(elpsTime, inputHandled);
	}

	//  operate normal forms
	for (int i=0; i<s_formNormal.Count(); i++)
	{
		s_formNormal[i]->Operate(elpsTime, inputHandled);
	}
}

void BaseForm::Manager::Draw( DWORD flag )
{
	//  draw normal forms
	int i = s_formNormal.Count() - 1;
	while ( i >= 0 )
	{
		s_formNormal[i--]->Draw(flag);
	}

	//  draw always on top forms
	for (int i=s_formOnTop.Count()-1; i>=0; i--)
	{
		s_formOnTop[i]->Draw(flag);
	}
}