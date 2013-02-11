#include "com_HUD.h"
#include "Game.h"
#include "GameGUI.h"
#include "Entity.h"

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES
static int s_displayHealthBar = 2;


com_HUD::com_HUD( void ): Component()
	, m_addPos(0,0,0)
	, m_healthBar(0)
	//, m_healthBar_Time(0)
{
	sx_callstack();

	m_name = L"HUD";
}

com_HUD::~com_HUD( void )
{
	sx_callstack();
	Finalize();
}

void com_HUD::Initialize( void )
{
	sx_callstack();

	m_healthBar = sx_new( sx::gui::ProgressBar );
	m_healthBar->SetSize( float2(256, 32) );
	m_healthBar->AddProperty( SX_GUI_PROPERTY_PROGRESSUV );
	m_healthBar->AddProperty( SX_GUI_PROPERTY_BILLBOARD );
	m_healthBar->AddProperty( SX_GUI_PROPERTY_3DSPACE );

	//m_healthBar->GetElement(0)->Color() = D3DColor(0, 0, 0, 1.0f);
	m_healthBar->GetElement(0)->SetTextureSrc( L"gui_healthBar0.txr" );
	m_healthBar->GetElement(1)->SetTextureSrc( L"gui_healthBar1.txr" );

	sx::math::AABox meshBox;
	m_owner->m_mesh->MsgProc( MT_GETBOX_LOCAL, &meshBox );
	m_addPos.Set( 0, meshBox.Max.y + 1.0f, 0 );
	m_healthBar->Position() = m_owner->GetPosition() + m_addPos;

	g_game->m_gui->Add_Back( m_healthBar );
}

void com_HUD::Finalize( void )
{
	sx_callstack();

	g_game->m_gui->Remove( m_healthBar );	
	sx_delete_and_null( m_healthBar );
}

void com_HUD::Update( float elpsTime )
{
	sx_callstack();

	if ( m_owner && m_owner->m_health.icur > 0 )
	{
		m_healthBar->Position() = m_owner->GetPosition() + m_addPos;
		m_healthBar->SetMax( (float)m_owner->m_health.imax );
		m_healthBar->SetValue( (float)m_owner->m_health.icur );

		switch ( s_displayHealthBar )
		{
		case 0:
			{
				m_healthBar->RemProperty( SX_GUI_PROPERTY_VISIBLE );
			}
			break;

		case 1:
			{
				m_healthBar->AddProperty( SX_GUI_PROPERTY_VISIBLE );
				//m_healthBar->GetElement(0)->Color().a = 1.0f;
				//m_healthBar->GetElement(1)->Color().a = 1.0f;
			}
			break;

		case 2:
			{
				//m_healthBar_Time += elpsTime;
				if ( m_owner->m_health.icur < m_owner->m_health.imax )
					m_healthBar->AddProperty( SX_GUI_PROPERTY_VISIBLE );
				else
					m_healthBar->RemProperty( SX_GUI_PROPERTY_VISIBLE );
			}
			break;
		}
	}
	else
	{
		g_game->m_gui->Remove( m_healthBar );
	}
}

void com_HUD::MsgProc( UINT msg, void* data )
{
	sx_callstack();

// 	switch ( msg )
// 	{
// 	case GMT_DAMAGE:	m_healthBar_Time = 0;	break;
// 	}
}

Component* com_HUD::Clone( void )
{
	sx_callstack();
	return sx_new( com_HUD );
}

int& com_HUD::Display_HealthBar( void )
{
	return s_displayHealthBar;
}
