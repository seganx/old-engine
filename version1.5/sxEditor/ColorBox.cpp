#include "ColorBox.h"
#include "EditorUI.h"

#define COLORBOX_MAX_SHOW_TIME		3000.0f
#define COLORBOX_WIDTH				200.0f
#define COLORBOX_HEIGHT				80.0f


Colorbox::Colorbox( void ): BaseForm(), m_pColor(0), m_fShowTime(0)
{

	float top = COLORBOX_HEIGHT * 0.5f;
	float left = - COLORBOX_WIDTH * 0.5f;
	for (int i=0; i<4; i++)
	{
		m_tr_color[i] = EditorUI::CreateTrackbar(m_pBack, 130, 0.0f, 255.0f);
		m_tr_color[i]->Position().Set(left + 70.0f, top - 10.0f - i*20.0f, 0.0f);
		SEGAN_GUI_SET_ONSCROLL(m_tr_color[i], Colorbox::OnScrollColor);

		m_ed_color[i] = EditorUI::CreateEditBox(m_pBack, 150.0f, 50.0f, L"0.00");
		m_ed_color[i]->Position().Set(left + 170.0f, top - 10.0f - i*20.0f, 0.0f);
	}

	m_pClose->RemProperty(SX_GUI_PROPERTY_VISIBLE);
	SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);

	SetSize( COLORBOX_WIDTH, COLORBOX_HEIGHT );
}

Colorbox::~Colorbox( void )
{

}

void Colorbox::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	m_fShowTime -= elpsTime;
	if ( m_fShowTime<0 && !m_pBack->GetFocused() )
		Close();

	BaseForm::Update(elpsTime);

	if (m_pColor)
	{
		m_pColor->r = m_tr_color[0]->GetBlendingValue() / 255.0f;
		m_pColor->g = m_tr_color[1]->GetBlendingValue() / 255.0f;
		m_pColor->b = m_tr_color[2]->GetBlendingValue() / 255.0f;
		m_pColor->a = m_tr_color[3]->GetBlendingValue() / 255.0f;
	}

}

void Colorbox::Show( float x, float y, PD3DColor color )
{
	m_pColor = color;
	if (m_pColor)
	{
		m_tr_color[0]->SetValue( m_pColor->r * 255.0f );
		m_tr_color[1]->SetValue( m_pColor->g * 255.0f );
		m_tr_color[2]->SetValue( m_pColor->b * 255.0f );
		m_tr_color[3]->SetValue( m_pColor->a * 255.0f );
	}

	m_fShowTime = COLORBOX_MAX_SHOW_TIME;

	m_pBack->State_GetByIndex(1).Position.Set(x, y, 0);
	m_pBack->State_GetByIndex(0).Position.Set(x, y, 0);

	m_pBack->RemProperty(SX_GUI_PROPERTY_BLENDSTATES);
	m_pBack->BlendStates(0);
	m_pBack->AddProperty(SX_GUI_PROPERTY_BLENDSTATES);

	//m_pBack->SetFocused(true);

	BaseForm::Show();
}

void Colorbox::Close( void )
{
	m_pColor = NULL;
	BaseForm::Close();
}

void Colorbox::OnScrollColor( sx::gui::PControl Sender )
{
	for (int i=0; i<4; i++)
	{
		if ( Sender == m_tr_color[i] )
		{
			m_ed_color[i]->SetOnTextChange(NULL, NULL);
			m_ed_color[i]->SetText(FloatToStr( m_tr_color[i]->GetValue() ) );
			SEGAN_GUI_SET_ONTEXT(m_ed_color[i], Colorbox::OnEditText);
			break;
		}
	}
}

void Colorbox::OnEditText( sx::gui::PControl Sender )
{
	for (int i=0; i<4; i++)
	{
		if ( Sender == m_ed_color[i] )
		{
			m_tr_color[i]->SetOnScroll(NULL, NULL);
			m_tr_color[i]->SetValue( str128::StrToFloat( m_ed_color[i]->GetText() ) );
			SEGAN_GUI_SET_ONSCROLL(m_tr_color[i], Colorbox::OnScrollColor);
			break;
		}
	}	
}