#include "PopupMenu.h"

#define POPUP_MAX_SHOW_TIME		100

PopupMenu::PopupMenu( void ): BaseForm(), m_fShowTime(0)
{
	m_listBox.SetParent(m_pBack);

	m_pClose->RemProperty(SX_GUI_PROPERTY_VISIBLE);
	SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	SetSize(10.0f, 10.0f);

	m_pBack->State_GetByIndex(0).Scale.Set(0.3f, 0.3f, 0.3f);
	//m_pBack->State_GetByIndex(1).Blender.Set(0.5f, 0.4f);

	m_listBox.SelectItemByMouseMove(true);
}

PopupMenu::~PopupMenu( void )
{

}

void PopupMenu::SetSize( float width, float height )
{
	BaseForm::SetSize(width, height);
	m_listBox.SetSize(width+8.0f, height+8.0f);
}

void PopupMenu::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

// 	if ( m_pBack->State_IsBlending() )
// 		m_listBox.SelectItemByMouseMove(false);
// 	else
// 		m_listBox.SelectItemByMouseMove(true);

	m_fShowTime -= elpsTime;
	if ( m_fShowTime<0 && !m_listBox.GetFocused() )
	{
		BaseForm::Close();
	}
	else
	{
		m_listBox.SetFocused();
	}

	m_listBox.Update(elpsTime);

	BaseForm::Update(elpsTime);
}


void PopupMenu::SetOnSelect( sx::gui::PForm pForm, GUICallbackEvent pFunc )
{
	m_listBox.SetOnMouseClick(pForm, pFunc);
}

sx::gui::ListBox& PopupMenu::Listbox( void )
{
	return m_listBox;
}

void PopupMenu::Popup( float x, float y )
{
	m_fShowTime = POPUP_MAX_SHOW_TIME;
	
	m_pBack->State_GetByIndex(1).Position.Set(x, y, 0);
	m_pBack->State_GetByIndex(0).Position.Set(x, y, 0);

	m_pBack->RemProperty(SX_GUI_PROPERTY_BLENDSTATES);
	m_pBack->BlendStates(0);
	m_pBack->AddProperty(SX_GUI_PROPERTY_BLENDSTATES);

	Show();
}