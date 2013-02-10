#include "Form_Ask.h"
#include "EditorUI.h"

#define ASK_WIDTH			200.0f		//  size of the ask form
#define ASK_HEIGHT			100.0f		//  size of the ask form

Form_Ask::Form_Ask( void ): BaseForm(), m_Sender(0)
{
	// invisible close button
	m_pClose->RemProperty( SX_GUI_PROPERTY_VISIBLE );

	//  simply disable any things
	sx::gui::PPanel panel = sx_new( sx::gui::Panel );
	panel->SetParent(m_pBack);
	panel->SetSize( float2(99999, 99999) );
	panel->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	panel->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.2f);

	//  title of the form
	m_pTitle = sx_new( sx::gui::Label );
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->AddProperty(SX_GUI_PROPERTY_MULTILINE);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Set name for new object");

	//  description of the form
	m_pDesc = sx_new( sx::gui::Label );
	m_pDesc->SetParent(m_pBack);
	m_pDesc->SetAlign(GTA_CENTER);
	m_pDesc->AddProperty(SX_GUI_PROPERTY_MULTILINE | SX_GUI_PROPERTY_AUTOSIZE);
	m_pDesc->SetFont( EditorUI::GetDefaultFont(10) );
	m_pDesc->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pDesc->GetElement(1)->Color() = D3DColor(1.0f, 0.9f, 0.9f, 1.0f);
	m_pDesc->SetText(L"HINT form\n here is the place to ask confirmation .");

	m_OK		= EditorUI::CreateButton( m_pBack, 100, L"OK" );
	m_Cancel	= EditorUI::CreateButton( m_pBack, 100, L"Cancel" );
	m_Yes		= EditorUI::CreateButton( m_pBack, 100, L"Yes" );
	m_No		= EditorUI::CreateButton( m_pBack, 100, L"No" );

	m_OK->SetUserTag(ASK_OK);
	m_Cancel->SetUserTag(ASK_CANCEL);
	m_Yes->SetUserTag(ASK_YES);
	m_No->SetUserTag(ASK_NO);

	SEGAN_GUI_SET_ONCLICK(m_OK, Form_Ask::OnClick);
	SEGAN_GUI_SET_ONCLICK(m_Cancel, Form_Ask::OnClick);
	SEGAN_GUI_SET_ONCLICK(m_Yes, Form_Ask::OnClick);
	SEGAN_GUI_SET_ONCLICK(m_No, Form_Ask::OnClick);

	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);

	SetSize( ASK_WIDTH, ASK_HEIGHT );
}

Form_Ask::~Form_Ask( void )
{
	//  for other things the parent will do everything :)
}

void Form_Ask::SetSize( float width, float height )
{
	float top = height/2 - 15.0f;
	float left = -width/2;

	m_pTitle->SetSize( float2(width - 70.0f, 40.0f) );
	m_pTitle->Position().y = top;
	
	top -= m_pDesc->GetSize().y / 2 + 10;
	m_pDesc->Position().y = top;
	top -= m_pDesc->GetSize().y / 2 + 30;

	m_OK->Position().y = top;
	m_Cancel->Position().y = top;
	m_Yes->Position().y = top;
	m_No->Position().y = top;

	int visibleCount = 0;
	sx::gui::PControl visibleContorl[4] = {0, 0, 0, 0};
	if ( m_OK->HasProperty(SX_GUI_PROPERTY_VISIBLE) )		visibleContorl[visibleCount++] = m_OK;
	if ( m_Yes->HasProperty(SX_GUI_PROPERTY_VISIBLE) )		visibleContorl[visibleCount++] = m_Yes;
	if ( m_No->HasProperty(SX_GUI_PROPERTY_VISIBLE) )		visibleContorl[visibleCount++] = m_No;
	if ( m_Cancel->HasProperty(SX_GUI_PROPERTY_VISIBLE) )	visibleContorl[visibleCount++] = m_Cancel;

	float maxWidth = visibleCount * 140.0f;
	for (int i=0; i<visibleCount; i++)
		visibleContorl[i]->Position().x = i * 140.0f - ( maxWidth / 2 ) + 70.0f;

	BaseForm::SetSize( maxWidth < width ? width : maxWidth, height );

	m_pBack->State_GetByIndex(0).Position.Set(0, 0, 0);
	m_pBack->State_GetByIndex(1).Position.Set(0, 0, 0);

}

void Form_Ask::Show( const WCHAR* caption, const WCHAR* desc, ASK_ DWORD buttons, sx::gui::PControl respondTo, sx::gui::PForm pForm, GUICallbackEvent ptrFunc )
{
	m_Sender = respondTo;
	m_onClick.m_pForm = pForm;
	m_onClick.m_pFunc = ptrFunc;

	if ( buttons & ASK_OK )
		m_OK->AddProperty(SX_GUI_PROPERTY_VISIBLE);
	else
		m_OK->RemProperty(SX_GUI_PROPERTY_VISIBLE);

	if ( buttons & ASK_CANCEL )
		m_Cancel->AddProperty(SX_GUI_PROPERTY_VISIBLE);
	else
		m_Cancel->RemProperty(SX_GUI_PROPERTY_VISIBLE);

	if ( buttons & ASK_YES )
		m_Yes->AddProperty(SX_GUI_PROPERTY_VISIBLE);
	else
		m_Yes->RemProperty(SX_GUI_PROPERTY_VISIBLE);

	if ( buttons & ASK_NO )
		m_No->AddProperty(SX_GUI_PROPERTY_VISIBLE);
	else
		m_No->RemProperty(SX_GUI_PROPERTY_VISIBLE);

	m_pTitle->SetText( caption );

	str2048 str = desc;
	if ( str.Length() > 160 )
		m_pDesc->SetAlign(GTA_LEFT);
	else
		m_pDesc->SetAlign(GTA_CENTER);
	m_pDesc->SetText( desc );

	SetSize( m_pDesc->GetSize().x + 20, m_pDesc->GetSize().y + 70 );

	m_pBack->SetFocused( true );
	m_pBack->State_GetByIndex(0).Position.Set(0, 0, 0);
	m_pBack->State_GetByIndex(1).Position.Set(0, 0, 0);
	m_pBack->State_GetBlended().Position.Set(0, 0, 0);

	BaseForm::Show();
}

void Form_Ask::OnClick( sx::gui::PControl Sender )
{
	DWORD oldUserTag = m_Sender->GetUserTag();
	m_Sender->SetUserTag( Sender->GetUserTag() );
	m_onClick( m_Sender );
	m_Sender->SetUserTag(oldUserTag);
	Close();
}
