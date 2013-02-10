#include "Form_Hint.h"
#include "EditorUI.h"

#define HINT_WIDTH			200.0f		//  size of the hint form
#define HINT_HEIGHT			100.0f		//  size of the hint form



Form_Hint::Form_Hint( void ): BaseForm(),  m_pCaptured(NULL), m_fTime(0)
{
	m_pBack->State_GetByIndex(0) = m_pBack->State_GetByIndex(1);
	m_pBack->State_GetByIndex(0).Color.w = 0.0f;
	m_pBack->State_GetByIndex(0).Blender.x = 0.02f;
	m_pBack->State_GetByIndex(1).Color.w = 0.75f;
	m_pBack->State_GetByIndex(1).Blender.x = 0.1f;


	// invisible close button
	m_pClose->RemProperty( SX_GUI_PROPERTY_VISIBLE );

	//  title of the form
	m_pTitle = sx_new( sx::gui::Label );
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->AddProperty(SX_GUI_PROPERTY_MULTILINE | SX_GUI_PROPERTY_AUTOSIZE);
	m_pTitle->SetFont( EditorUI::GetDefaultFont(8) );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = D3DColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_pTitle->SetText(L"HINT form\n here is the place to show hint .");
	m_pTitle->Position().x = -2;

	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);

	SetSize( HINT_WIDTH, HINT_HEIGHT );
}

Form_Hint::~Form_Hint( void )
{
	//  for other things the parent will do everything :)
}

void Form_Hint::Update( float elpsTime )
{
	const float maxTime = 1500.0;

	sx::gui::PControl pCaptured = sx::gui::Control::GetCapturedControl();
	if ( pCaptured &&  pCaptured->GetHint() )
	{
		m_fTime += elpsTime;
		if ( m_fTime > maxTime && pCaptured != m_pCaptured )
		{
			str2048 str = pCaptured->GetHint();
			if ( str.Length() > 160 )
				m_pTitle->SetAlign(GTA_LEFT);
			else
				m_pTitle->SetAlign(GTA_CENTER);

			m_pTitle->SetText( pCaptured->GetHint() );
			SetSize( m_pTitle->GetSize().x - 15, m_pTitle->GetSize().y - 20 );

			Show();

			m_pCaptured = pCaptured;
			m_fTime = maxTime;
		}

		if ( m_fTime > 10000 )
		{
			Close();
		}
	}
	else
	{
		Close();

		if ( m_pBack->State_GetBlended().Color.w < 0.1f )
			m_fTime = 0;
		m_pCaptured = NULL;
	}

	BaseForm::Update(elpsTime);

	//  update position states
	float2 halfSize( m_pBack->GetSize().x / 2 + 32 , m_pBack->GetSize().y / 2 + 32 );
	float x = SEGAN_MOUSE_ABSX(0) + halfSize.x;
	float y = SEGAN_MOUSE_ABSY(0) + halfSize.y;
	if ( x + halfSize.x > SEGAN_VP_WIDTH )		x -= halfSize.x * 2;
	if ( y + halfSize.y > SEGAN_VP_HEIGHT )		y -= halfSize.y * 2;

	m_pBack->State_GetByIndex(1).Position.Set( x - SEGAN_VP_WIDTH/2, SEGAN_VP_HEIGHT/2 - y, 0 );
	m_pBack->State_GetByIndex(0).Position = m_pBack->State_GetByIndex(1).Position;

}
