#include "GameGUI.h"
#include "Game.h"
#include "GameConfig.h"
#include "Entity.h"
#include "Player.h"
#include "Scripter.h"
#include "GameStrings.h"

#define GAMETIPS_MAXTIME		10
#define GAMETIPS_ICON_SIZE		64
#define GAMETIPS_ICON_SIZE_div2	32


//////////////////////////////////////////////////////////////////////////
//	gold & people
//////////////////////////////////////////////////////////////////////////
GoldAndPeople::GoldAndPeople( void ): Form(), m_time(0)
{
	//	create back panel
	m_back = sx_new( sx::gui::PanelEx );
	m_back->SetSize( float2(1, 1) );
	m_back->State_GetByIndex(0).Blender.Set( 0.5f, 0.4f );
	m_back->State_GetByIndex(0).Align.Set( 0.5f, 0.5f );
	m_back->State_GetByIndex(0).Position.Set( -136.0f, 200.0f, 0 );
	m_back->State_Add();
	m_back->State_GetByIndex(1).Position.Set( -136.0f, -34.0f, 0 );

	//  create gold and people panel
	sx::gui::PPanel ground = sx_new( sx::gui::Panel );
	ground->SetParent( m_back );
	ground->SetSize( float2(512, 128) );
	ground->GetElement(0)->SetTextureSrc( L"gui_pnlGold.txr" );
	ground->Position().y = -28.0f;

	//  gold clipper
	sx::gui::PPanel goldClipper = sx_new( sx::gui::Panel );
	goldClipper->SetParent( ground );
	goldClipper->SetSize( float2(82, 27) );
	goldClipper->AddProperty( SX_GUI_PROPERTY_CLIPCHILDS );
	goldClipper->GetElement(0)->Color() = D3DColor( 0, 0, 0, 0.1f );
	goldClipper->Position().Set( 41.0f, 21.0f, 0 );

	//  people clipper
	sx::gui::PPanel peopleClipper = sx_new( sx::gui::Panel );
	peopleClipper->SetParent( ground );
	peopleClipper->SetSize( float2(65, 21) );
	peopleClipper->AddProperty( SX_GUI_PROPERTY_CLIPCHILDS );
	peopleClipper->GetElement(0)->Color() = D3DColor( 0, 0, 0, 0.1f );
	peopleClipper->Position().Set( -52.0f, 34.0f, 0 );

	//	digits
	for ( int i=0; i<8; i++ )
	{
		m_digit[i] = sx_new( sx::gui::PanelEx );
		m_digit[i]->State_GetByIndex(0).Color.Set(0,0,0,1);

		if ( i < 4 )
		{
			float2 sz = goldClipper->GetSize();
			float w = sz.x * 0.25f;
			float x = - sz.x * 0.5f + w * 0.5f;
			m_digit[i]->SetParent( goldClipper );
			m_digit[i]->State_GetByIndex(0).Position.x = x + w * i;
			m_digit[i]->GetElement(0)->SetTextureSrc( L"gui_gold_digits.txr" );
			m_digit[i]->SetSize( float2(18, 239) );
		}
		else
		{
			float2 sz = peopleClipper->GetSize();
			float w = sz.x * 0.25f;
			float x = - sz.x * 0.5f + w * 0.5f;
			m_digit[i]->SetParent( peopleClipper );
			m_digit[i]->State_GetByIndex(0).Position.x = x + w * (i - 4.0f);
			m_digit[i]->GetElement(0)->SetTextureSrc( L"gui_people_digits.txr" );
			m_digit[i]->SetSize( float2(12, 196) );
		}

		for ( int n=0; n<10; n++ )
		{
			if ( n ) m_digit[i]->State_Add();
			m_digit[i]->State_GetByIndex(n).Center.Set( 0, -0.45f + 0.1f * n, 0 );
		}
	}

	//	create alarm panel
	m_alarm = sx_new( sx::gui::PanelEx );
	m_alarm->SetParent( m_back );
	m_alarm->SetSize( float2( 64, 64 ) );
	m_alarm->GetElement(0)->SetTextureSrc( L"gui_pnlGold_alarm.txr" );
	m_alarm->Position().Set( -194.0f, 8.0f, 0.0f );
	m_alarm->State_Add();
	m_alarm->State_GetByIndex(0).Color.Set( 1.0f, 1.0f, 1.0f, 0.0f );
	m_alarm->State_GetByIndex(1).Blender.Set( 0.2f, 1.0f );

	m_info = sx_new( sx::gui::PanelEx );
	m_info->SetParent( ground );
	m_info->SetSize( float2(64, 64) );
	m_info->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_info->GetElement(0)->SetTextureSrc( L"gui_pnlGold_info.txr" );
	m_info->Position().Set( 101.0f, -33.0f, 0.0f );
	m_info->State_Add();
	m_info->State_GetByIndex(0).Color.w = 0.05f;
	m_info->State_GetByIndex(1).Scale.Set( 1.05f, 1.05f, 1.0f );
	m_info->State_GetByIndex(1).Blender.Set( 0.1f, 1.0f );
	SEGAN_GUI_SET_ONCLICK( m_info, GoldAndPeople::OnClick );
	SEGAN_GUI_SET_ONENTER( m_info, GoldAndPeople::OnEnter );
	SEGAN_GUI_SET_ONEXIT( m_info, GoldAndPeople::OnExit );
}

GoldAndPeople::~GoldAndPeople( void )
{
	sx_delete_and_null( m_back );
}

void GoldAndPeople::ProcessInput( bool& inputHandled, float elpsTime )
{
	if ( !g_game->m_game_currentLevel || g_game->m_game_paused || g_game->m_app_Loading || g_game->m_mouseMode != MS_Null ) return;
	sx_callstack();

	if ( !inputHandled && SEGAN_KEYUP( 0, SX_INPUT_KEY_I ) )
	{
		OnClick( m_info );
		inputHandled = true;
	}

	m_info->ProcessInput( inputHandled );
}

void GoldAndPeople::Update( float elpsTime )
{
	int gold = g_game->m_player->m_gold;
	m_digit[0]->State_SetIndex( gold / 1000 );	gold %= 1000;
	m_digit[1]->State_SetIndex( gold / 100 );	gold %= 100;
	m_digit[2]->State_SetIndex( gold / 10 );	gold %= 10;
	m_digit[3]->State_SetIndex( gold );

	int people = g_game->m_player->m_people;
	m_digit[4]->State_SetIndex( people / 1000 );	people %= 1000;
	m_digit[5]->State_SetIndex( people / 100 );		people %= 100;
	m_digit[6]->State_SetIndex( people / 10 );		people %= 10;
	m_digit[7]->State_SetIndex( people );

	if ( m_time > 0 )
	{
		m_time -= elpsTime;
		if ( m_time <= 0 )
		{
			m_alarm->State_SetIndex( 0 );
		}
	}

	m_back->Update( elpsTime );
}

void GoldAndPeople::Alarm( void )
{
	m_time = 5000;
	m_alarm->State_SetIndex( 1 );
}

void GoldAndPeople::OnClick( sx::gui::PControl sender )
{
	if ( sender == m_info )
	{
		g_game->m_gui->m_info->Show();

		msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
		g_game->m_gui->m_info->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

		return;
	}
}

void GoldAndPeople::OnEnter( sx::gui::PControl sender )
{
	if ( sender->GetType() == GUI_PANELEX )
		sx::gui::PPanelEx( sender )->State_SetIndex(1);

	msg_SoundPlay msg( true, 0, 0, L"mouseHover" );
	g_game->m_gui->m_info->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void GoldAndPeople::OnExit( sx::gui::PControl sender )
{
	sx::gui::PPanelEx( sender )->State_SetIndex(0);
}

//////////////////////////////////////////////////////////////////////////
//	panel to show game guid
//////////////////////////////////////////////////////////////////////////
GameGuide::GameGuide( void ): m_time(0), m_pos(0,0)//, m_used(false)
{
	m_back = sx_new( sx::gui::PanelEx );
	m_back->AddProperty( SX_GUI_PROPERTY_BLENDCHILDS );
	m_back->State_Add();
	m_back->State_GetByIndex(0).Blender.Set(0.5f, 0.4f);
	m_back->State_GetByIndex(0).Scale.Set(0.9f, 0.9f, 1);
	m_back->State_GetByIndex(0).Color.Set(0, 0, 0, 0);
	m_back->State_GetByIndex(1).Blender.Set(0.5f, 0.4f);
	m_back->State_GetByIndex(1).Scale.Set(1, 1, 1);
	m_back->State_GetByIndex(1).Color.Set(0.0f, 0.0f, 0.0f, 0.6f);
	m_back->State_SetIndex(0);

	m_indic = sx_new( sx::gui::Panel );
	m_indic->SetParent( m_back );
	m_indic->GetElement(0)->SetTextureSrc(L"gui_guid_pointer.txr");
	m_indic->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 1.0f );
	m_indic->SetSize( float2(32.0f, 32.0f) );

	m_title = create_label( m_back, 644, 100, 100, 0, 0 );
	m_title->RemProperty( SX_GUI_PROPERTY_WORDWRAP );
	m_title->AddProperty( SX_GUI_PROPERTY_AUTOSIZE );
	m_title->AddProperty( SX_GUI_PROPERTY_IGNOREBLEND );
	m_title->GetElement(1)->Color() = D3DColor(1,1,0.2f,1);

	m_desc = create_label( m_back, 645, 100, 100, 0, 0 );
	m_desc->RemProperty( SX_GUI_PROPERTY_WORDWRAP );
	m_desc->AddProperty( SX_GUI_PROPERTY_AUTOSIZE );
	m_desc->AddProperty( SX_GUI_PROPERTY_MULTILINE );
	m_desc->AddProperty( SX_GUI_PROPERTY_IGNOREBLEND );
}

GameGuide::~GameGuide( void )
{
	m_back->SetParent( null );
	g_game->m_gui->Remove( m_back );
	sx_delete_and_null( m_back );
}

void GameGuide::SetText( const uint title )
{
	const uint desc = title + 1;
	m_currHint = title;
	m_time = 0;

	float2 panelSize, titleSize, descSize;
	int index = 0;
	if ( desc )
	{
		update_label( m_title, title );
		update_label( m_desc, desc );

		titleSize = m_title->GetSize();
		descSize = m_desc->GetSize();
	}
	else
	{
		update_label( m_title, title );
		m_desc->SetText( NULL );

		titleSize = m_title->GetSize();
		descSize.Set(0,0);
	}

	panelSize.x = sx_max_f( titleSize.x, descSize.x ) + 32;
	panelSize.y = titleSize.y + descSize.y + 32;
	m_back->SetSize( panelSize );
	m_title->Position().Set( 0, (panelSize.y/2) - (titleSize.y/2) - 16, 0 );
	m_desc->Position().Set( 0, - (panelSize.y/2) + (descSize.y/2) + 16, 0 );
}


void GameGuide::Show( const CORNER corner, const float x, const float y, const float lifetime /*= 10*/ )
{
	m_time = lifetime;

	if ( m_currHint )
	{
		float2 pos( x, y );
		float2 siz = m_back->GetSize();

		switch ( corner )
		{
		case TOPLEFT:
			{
				m_indic->Position().Set( - siz.x * 0.5f, siz.y * 0.5f, 0.0f );
				m_indic->Rotation().z = PI_DIV_2;
				pos.x += siz.x * 0.5f + 32.0f;
				pos.y -= siz.y * 0.5f + 32.0f;
			}
			break;

		case TOPRIGHT:
			{
				m_indic->Position().Set( siz.x * 0.5f, siz.y * 0.5f, 0.0f );
				pos.x -= siz.x * 0.5f + 32.0f;
				pos.y -= siz.y * 0.5f + 32.0f;
			}
			break;

		case BOTTOMRIGHT:
			{
				m_indic->Position().Set( siz.x * 0.5f, - siz.y * 0.5f, 0.0f );
				m_indic->Rotation().z = - PI_DIV_2;
				pos.x -= siz.x * 0.5f + 32.0f;
				pos.y += siz.y * 0.5f + 32.0f;
			}
			break;

		case BOTTOMLEFT:
			{
				m_indic->Position().Set( - siz.x * 0.5f, - siz.y * 0.5f, 0.0f );
				m_indic->Rotation().z = PI;
				pos.x += siz.x * 0.5f + 32.0f;
				pos.y += siz.y * 0.5f + 32.0f;
			}
			break;
		}

		m_back->State_GetByIndex(0).Position.Set( pos.x, pos.y, 0 );
		m_back->State_GetByIndex(1).Position.Set( pos.x, pos.y, 0 );
		m_back->State_SetIndex(1);
	}
	else
	{
		m_back->State_SetIndex(0);
	}
}

void GameGuide::Hide( void )
{
	m_back->State_SetIndex(0);
}

void GameGuide::Update( const float elpsTime )
{
	if ( m_time > 0.0f )
	{
		m_time -= elpsTime * 0.001f;
		if ( m_time <= 0.0f )
		{
			m_back->State_SetIndex(0);
		}
	}
}

bool GameGuide::IsVisible( void )
{
	return m_back->State_GetIndex() > 0;
}


//////////////////////////////////////////////////////////////////////////
//	game tips
//////////////////////////////////////////////////////////////////////////
class GameTips
{
public:

	GameTips(void): m_age(GAMETIPS_MAXTIME), m_posY(0), m_panel(0), m_label(0), m_dead(false) 
	{

	}

	~GameTips(void)
	{
		if ( m_panel )
		{
			g_game->m_gui->Remove( m_panel );
			sx_delete_and_null( m_panel );
		}
		if ( m_label )
		{
			g_game->m_gui->Remove( m_label );
			sx_delete_and_null( m_label );
		}
	}

	void Create(const uint tipText, const WCHAR* tipIcon)
	{

		float width		= (float)sx::d3d::Device3D::Viewport()->Width;
		float height	= (float)sx::d3d::Device3D::Viewport()->Height;
		float left		= 50.0f		- width * 0.5f;
		m_posY			= 150.0f	- height * 0.5f;

		if ( tipIcon )
		{
			m_posY += GAMETIPS_ICON_SIZE_div2 * 0.5f;

			m_panel = sx_new( sx::gui::Panel );
			m_panel->SetSize( float2(GAMETIPS_ICON_SIZE, GAMETIPS_ICON_SIZE) );
			m_panel->GetElement(0)->SetTextureSrc( tipIcon );
			m_panel->Position().Set( left + GAMETIPS_ICON_SIZE_div2, m_posY, 0 );
			left += GAMETIPS_ICON_SIZE;

			g_game->m_gui->Add_Back( m_panel );
		}

		if ( tipText )
		{
			m_label = create_label( null, tipText, 100.0f, GAMETIPS_ICON_SIZE_div2, 0, 0 );
			m_label->RemProperty( SX_GUI_PROPERTY_WORDWRAP );
			m_label->AddProperty( SX_GUI_PROPERTY_AUTOSIZE );
			m_label->Position().Set( left + m_label->GetSize().x * 0.5f, m_posY, 0 );

			g_game->m_gui->Add_Back( m_label );
		}		
	}

	void Update(float elpsTime)
	{
		m_age -= elpsTime * 0.001f;
		if ( m_age < 0 )
		{
			m_dead = true;
			return;
		}

		if ( m_panel )
		{
			m_panel->Position().y = m_posY;
			if ( m_age < 1 ) m_panel->GetElement(0)->Color().a = m_age;
		}

		if ( m_label )
		{
			m_label->Position().y = m_posY;
			if ( m_age < 1 ) m_label->GetElement(1)->Color().a = m_age;
		}
	}

public:

	float					m_age;
	float					m_posY;
	sx::gui::Panel*			m_panel;
	sx::gui::Label*			m_label;
	bool					m_dead;

};


//////////////////////////////////////////////////////////////////////////
//	game hints
//////////////////////////////////////////////////////////////////////////
class GameHint
{
public:

	GameHint(void): m_time(0), m_currCnrtl(0), m_back(0), m_title(0), m_desc(0)
	{
		m_back = sx_new( sx::gui::PanelEx );
		m_back->AddProperty( SX_GUI_PROPERTY_BLENDCHILDS );
		m_back->State_Add();
		m_back->State_GetByIndex(0).Blender.Set(0.5f, 0.4f);
		m_back->State_GetByIndex(0).Scale.Set(0.7f, 0.7f, 1);
		m_back->State_GetByIndex(0).Color.Set(0, 0, 0, 0);
		m_back->State_GetByIndex(1).Blender.Set(0.5f, 0.4f);
		m_back->State_GetByIndex(1).Scale.Set(1, 1, 1);
		m_back->State_GetByIndex(1).Color.Set(0.0f, 0.0f, 0.0f, 0.6f);
		
		m_title = create_label( m_back, 644, 100, 100, 0, 0 );
		m_title->RemProperty( SX_GUI_PROPERTY_WORDWRAP );
		m_title->AddProperty( SX_GUI_PROPERTY_AUTOSIZE );
		m_title->AddProperty( SX_GUI_PROPERTY_IGNOREBLEND );
		m_title->GetElement(1)->Color() = D3DColor(1,1,0.2f,1);

		m_desc = create_label( m_back, 645, 100, 100, 0, 0 );
		m_desc->RemProperty( SX_GUI_PROPERTY_WORDWRAP );
		m_desc->AddProperty( SX_GUI_PROPERTY_AUTOSIZE );
		m_desc->AddProperty( SX_GUI_PROPERTY_MULTILINE );
		m_desc->AddProperty( SX_GUI_PROPERTY_IGNOREBLEND );
	}

	~GameHint(void)
	{
		sx_delete_and_null( m_back );
	}

	void SetText(const wchar* hint)
	{
		m_currHint = hint;
		m_time = 0;

		float2 panelSize, titleSize, descSize;
		String tmp_title = hint;
		int index = 0;
		if ( ( index = tmp_title.Find(L"\n") ) > -1 )
		{
			String tmp_desc;
			tmp_title.CopyTo( tmp_desc, index + 1, 99999 );
			tmp_title.Delete( index, 99999 );

			update_label( m_title, sx_str_to_uint( tmp_title ) );
			update_label( m_desc, sx_str_to_uint( tmp_desc ) );

			titleSize = m_title->GetSize();
			descSize = m_desc->GetSize();
		}
		else
		{
			update_label( m_title, sx_str_to_uint( tmp_title ) );
			m_desc->SetText( NULL );

			titleSize = m_title->GetSize();
			descSize.Set(0,0);
		}

		panelSize.x = sx_max_f( titleSize.x, descSize.x ) + 32;
		panelSize.y = titleSize.y + descSize.y + 32;
		m_back->SetSize( panelSize );
		m_title->Position().Set( 0, (panelSize.y/2) - (titleSize.y/2) - 16, 0 );
		m_desc->Position().Set( 0, - (panelSize.y/2) + (descSize.y/2) + 16, 0 );

	}

	void SetCurrentContorl(sx::gui::PControl pCurrent)
	{
		if ( !pCurrent || !pCurrent->GetHint() || !pCurrent->HasProperty( SX_GUI_PROPERTY_ACTIVATE ) )
		{
			m_back->State_SetIndex(0);
			m_currCnrtl = pCurrent;
			return;
		}
		if ( m_currHint == pCurrent->GetHint() && m_currCnrtl == pCurrent ) return;

		m_currCnrtl = pCurrent;
		SetText( m_currCnrtl->GetHint() );
		m_back->State_SetIndex(1);
	}

	void Update(float elpsTime)
	{
		m_time += elpsTime;
		if ( m_time > 7000 )
		{
			m_back->State_SetIndex(0);
		}

		SetCurrentContorl( sx::gui::Control::GetCapturedControl() );

		float2 siz = m_back->GetSize();
		float2 pos( SEGAN_MOUSE_ABSX(0) - SEGAN_VP_WIDTH/2, SEGAN_VP_HEIGHT/2 - SEGAN_MOUSE_ABSY(0) );
		pos.x += (pos.x > 0) ? -siz.x/2 : siz.x/2;
		pos.y += (pos.y > 0) ? -siz.y/2 : siz.y/2;

		m_back->State_GetCurrent().Position.Set( pos.x, pos.y, 0 );

		m_back->Update( elpsTime );
	}


public:
	float					m_time;			//  time of display
	str64					m_currHint;		//  current hint numbers
	sx::gui::PControl		m_currCnrtl;	//	current control
	sx::gui::PPanelEx		m_back;			//	use as background
	sx::gui::PLabel			m_title;		//  title
	sx::gui::PLabel			m_desc;			//  description
	
};


//////////////////////////////////////////////////////////////////////////
//	game gui
//////////////////////////////////////////////////////////////////////////
GameGUI::GameGUI( void ): Form()
, m_hint(0)
, m_status(0)
, m_main(0)
, m_map(0)
, m_profile(0)
#if USE_STEAM_SDK
#else
, m_achivements(0)
#endif
, m_settings(0)
, m_credits(0)
, m_confirmExit(0)
, m_pause(0)
, m_victory(0)
, m_gameOver(0)
, m_cinematic(0)
{

}

GameGUI::~GameGUI( void )
{

}

void GameGUI::Initialize( void )
{
	//  create power attack panel
	m_powerAttaks = sx_new( sx::gui::PanelEx );
	m_powerAttaks->SetSize( float2(512, 128) );
	m_powerAttaks->GetElement(0)->SetTextureSrc( L"gui_pnlPowerAttacks.txr" );
	m_powerAttaks->State_GetByIndex(0).Align.Set( -0.5f, -0.5f );
	m_powerAttaks->State_GetByIndex(0).Center.Set( 0.205f, -0.5f, 0 );
	m_powerAttaks->State_GetByIndex(0).Blender.Set( 0.4f, 0.5f );
	m_powerAttaks->State_Add();
	m_powerAttaks->State_GetByIndex(1).Align.Set( -0.5f, -0.5f );
	m_powerAttaks->State_GetByIndex(1).Center.Set( 0.205f, 0.5f, 0 );
	m_powerAttaks->State_GetByIndex(1).Blender.Set( 0.4f, 0.5f );

	//	add empty buttons to show the unlocked feature
	{
		for ( uint i=0; i<5; ++i )
		{
			sx::gui::Panel* p = sx_new( sx::gui::Panel );
			p->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
			p->SetParent( m_powerAttaks );
			p->SetSize( float2( 40, 40 ) );
			p->GetElement(0)->Color() = D3DColor( 0, 0, 0, 0.1f );
			
			float left, top = -11.0f;
			switch ( i )
			{
			case 0:		left = -30.0f;	break;
			case 1:		left = 27.0f;	break;
			case 2:		left = 84.0f;	break;
			case 3:		left = 140.0f;	break;
			case 4:		left = 198.0f;	break;
			default:	left = -30 + i * 80.0f;
			}
			p->Position().Set( left, top, 0.0f );

			p->SetHint( L"182\n184" );
		}
	}
	
	
	//  create game speed controller
	m_gameSpeed = sx_new( sx::gui::TrackBar );
	m_gameSpeed->SetParent( m_powerAttaks );
	m_gameSpeed->SetSize( float2( 150, 32) );
	m_gameSpeed->GetElement(0)->Color().a = 0;
	m_gameSpeed->GetElement(1)->SetTextureSrc( L"gui_trkGameSpeed.txr" );
	m_gameSpeed->Position().Set( 92.0f, 40.0f, 0.0f );
	m_gameSpeed->SetMin(0.05f);
	m_gameSpeed->SetMax(2.0f);
	m_gameSpeed->SetValue( Config::GetData()->game_speed );
	
	//	create minimum and maximum value for game speed
	sx::gui::Panel* panel = sx_new( sx::gui::Panel );
	panel->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	panel->SetParent( m_gameSpeed );
	panel->SetSize( float2( 24, 15 ) );
	panel->GetElement(0)->Color() = D3DColor(0,0,0,0.02f);
	panel->Position().Set( -75.0f - 12.0f, -5.0f, 0 );
	panel->SetUserTag( 1 );
	SEGAN_GUI_SET_ONCLICK( panel, GameGUI::OnClick );

	panel = (sx::gui::Panel*)panel->Clone();
	panel->SetParent( m_gameSpeed );
	panel->Position().Set( 75.0f + 12.0f, -5.0f, 0 );
	panel->SetUserTag( 2 );
	SEGAN_GUI_SET_ONCLICK( panel, GameGUI::OnClick );

	//	create default indicator
	m_gameSpeedDefault = sx_new( sx::gui::Panel );
	m_gameSpeedDefault->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_gameSpeedDefault->SetParent( m_gameSpeed );
	m_gameSpeedDefault->SetSize( float2(32, 24) );
	m_gameSpeedDefault->GetElement(0)->SetTextureSrc( L"gui_trkGameSpeed_default.txr" );
	m_gameSpeedDefault->Position().y = - 24.0f;
	m_gameSpeedDefault->SetUserTag(3);
	SEGAN_GUI_SET_ONCLICK( m_gameSpeedDefault, GameGUI::OnClick );

	m_guide = sx_new( GameGuide );
	m_guide->m_back->SetParent( m_gameSpeed );

	m_goldPeople = sx_new( GoldAndPeople );
	m_hint = sx_new( GameHint );
	m_status = sx_new ( MenuStatus );
	m_main = sx_new ( MenuMain );
	m_map = sx_new ( MenuMap );
	m_profile = sx_new( MenuProfile );
#if USE_STEAM_SDK
#else
	m_achivements = sx_new( MenuAchievements );
#endif
	m_settings = sx_new( MenuSettings );
	m_credits = sx_new( MenuCredits );
	m_confirmExit = sx_new( MenuConfirmExit );
	m_pause = sx_new( MenuPause );
	m_victory = sx_new( MenuVictory );
	m_gameOver = sx_new( MenuGameOver );
	m_cinematic = sx_new( MenuCinematic );
	m_info = sx_new( MenuInfo );
	m_upgradePanel = sx_new( MenuUpgrade );

	m_status->Initialize();
	m_main->Initialize();
	m_map->Initialize();
	m_profile->Initialize();
#if USE_STEAM_SDK
#else
	m_achivements->Initialize();
#endif
	m_settings->Initialize();
	m_credits->Initialize();
	m_confirmExit->Initialize();
	m_pause->Initialize();
	m_victory->Initialize();
	m_gameOver->Initialize();
	m_cinematic->Initialize();
	m_info->Initialize();
	m_upgradePanel->Initialize();
}

void GameGUI::Finalize( void )
{
	m_guide->m_back->SetParent(null);
	sx_delete_and_null( m_guide );

	m_info->Finalize();
	m_cinematic->Finalize();
	m_gameOver->Finalize();
	m_victory->Finalize();
	m_pause->Finalize();
	m_confirmExit->Finalize();
#if USE_STEAM_SDK
#else
	m_achivements->Finalize();
#endif
	m_settings->Finalize();
	m_credits->Finalize();
	m_profile->Finalize();
	m_map->Finalize();
	m_main->Finalize();
	m_status->Finalize();
	m_upgradePanel->Finalize();


	sx_delete_and_null( m_upgradePanel );
	sx_delete_and_null( m_info );
	sx_delete_and_null( m_goldPeople );
	sx_delete_and_null( m_hint );
	sx_delete_and_null( m_cinematic );
	sx_delete_and_null( m_gameOver );
	sx_delete_and_null( m_victory );
	sx_delete_and_null( m_pause );
	sx_delete_and_null( m_confirmExit );
#if USE_STEAM_SDK
#else
	sx_delete_and_null( m_achivements );
#endif
	sx_delete_and_null( m_settings );
	sx_delete_and_null( m_credits );
	sx_delete_and_null( m_profile );
	sx_delete_and_null( m_map );
	sx_delete_and_null( m_main );
	sx_delete_and_null( m_status );
	sx_delete_and_null( m_powerAttaks );
	m_gameSpeed = NULL;
}

void GameGUI::ProcessInput( bool& inputHandled, float elpsTime )
{
	sx_callstack();

	m_info->ProcessInput( inputHandled, elpsTime );
	m_confirmExit->ProcessInput( inputHandled, elpsTime );
	m_status->ProcessInput( inputHandled, elpsTime );
	m_upgradePanel->ProcessInput( inputHandled, elpsTime );
	m_victory->ProcessInput( inputHandled, elpsTime );
	m_gameOver->ProcessInput( inputHandled, elpsTime );
	m_goldPeople->ProcessInput( inputHandled, elpsTime );
	m_settings->ProcessInput( inputHandled, elpsTime );
	m_pause->ProcessInput( inputHandled, elpsTime );
#if USE_STEAM_SDK
#else
	m_achivements->ProcessInput( inputHandled, elpsTime );
#endif
	m_credits->ProcessInput( inputHandled, elpsTime );
	m_profile->ProcessInput( inputHandled, elpsTime );
	m_map->ProcessInput( inputHandled, elpsTime );
	m_main->ProcessInput( inputHandled, elpsTime );

	if ( !g_game->m_game_paused )
	{
		m_gameSpeed->ProcessInput( inputHandled );
	}
	else
	{
		m_guide->Hide();
	}

	if ( !inputHandled )
		m_powerAttaks->ProcessInput( inputHandled );
}

void GameGUI::Update( float elpsTime )
{
	for (int i=0; i<m_gui.Count(); i++)
		m_gui[i]->Update(elpsTime);

	for (int i=0; i<m_tips.Count(); i++)
	{
		GameTips* tips = m_tips[i];
		
		tips->Update(elpsTime);
		if ( tips->m_dead )
		{
			sx_delete_and_null( tips );
			m_tips.RemoveByIndex(i);
			i--;
		}
	}

	m_info->Update( elpsTime );
	m_goldPeople->Update( elpsTime );
	m_hint->Update(elpsTime);
	m_status->Update( elpsTime );
	m_upgradePanel->Update( elpsTime );
	m_cinematic->Update( elpsTime );
	m_gameOver->Update( elpsTime );
	m_victory->Update( elpsTime );
	m_main->Update( elpsTime );
	m_profile->Update( elpsTime );
	m_confirmExit->Update( elpsTime );
#if USE_STEAM_SDK
#else
	m_achivements->Update( elpsTime );
#endif
	m_settings->Update( elpsTime );
	m_credits->Update( elpsTime );
	m_map->Update( elpsTime );
	m_pause->Update( elpsTime );
	m_powerAttaks->Update( elpsTime );
	m_guide->Update( elpsTime );
	
	if ( m_guide->m_back->State_GetIndex() && sx_abs_f( m_gameSpeed->GetBlendingValue() - Config::GetData()->game_speed ) > 0.01f )
		m_guide->Hide();

	Config::GetData()->game_speed = m_gameSpeed->GetBlendingValue();
	
	if ( !g_game->m_game_currentLevel || g_game->m_game_paused )
	{
		m_powerAttaks->State_SetIndex(0);
		m_goldPeople->m_back->State_SetIndex(0);
	}
	else
	{
		if ( g_game->m_mouseMode != MS_Null )
		{
			m_powerAttaks->State_SetIndex(0);
			m_goldPeople->m_back->State_SetIndex(0);
		}
		else
		{
			m_powerAttaks->State_SetIndex(1);
			m_goldPeople->m_back->State_SetIndex(1);
		}
	}
}


void GameGUI::Draw( DWORD flag )
{
	sx_callstack();

	for (int i=0; i<m_gui.Count(); i++)
	{
		m_gui[i]->Draw( flag | SX_GUI_DRAW_SAVEMATRIX );
	}

	m_goldPeople->m_back->Draw( flag );
	m_powerAttaks->Draw( flag );
	m_hint->m_back->Draw( flag );
	m_pause->Draw( flag );
	m_gameOver->Draw( flag );
	m_victory->Draw( flag );
	m_main->Draw( flag );
	m_map->Draw( flag );
	m_profile->Draw( flag );
#if USE_STEAM_SDK
#else
	m_achivements->Draw( flag );
#endif
	m_settings->Draw( flag );
	m_credits->Draw( flag );
	m_upgradePanel->Draw(flag);
	m_status->Draw( flag );
	m_info->Draw( flag );
	m_confirmExit->Draw( flag );
	m_cinematic->Draw( flag );
}

void GameGUI::MsgProc( UINT recieverID, UINT msg, void* data )
{
	sx_callstack_param(GameGUI::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

	if ( !m_main ) return;

	switch ( msg )
	{
	case GMT_LEVEL_CLEAR:
	case GMT_GAME_END:
	case GMT_GAME_RESETING:
		{
			for ( int i=0; i<m_tips.Count(); i++ )
			{
				GameTips* tips = m_tips[i];
				sx_delete_and_null( tips );
			}
			m_tips.Clear();
		}
	case GMT_GAME_RESET:
	case GMT_LEVEL_LOAD:
		{
			if ( g_game->m_game_currentLevel == 0 )
				m_main->Show();

			m_gameSpeed->SetValue(1.0f);
		}
		break;

	case GMT_GAME_START:
		{
#if 0
			if ( g_game->m_game_currentLevel != 0 )
			{
				ShowTips(L" Game Started !", 0xffff0000);
				//m_info->AddTutorial( L"TEST", L"This is simple test", L"level6_gr05.txr" );
			}
#endif
			m_gameSpeed->SetValue(1.0f);
		}
		break;
	}

	switch ( msg )
	{
	case GMT_GAME_START:
	case GMT_GAME_RESET:
	case GMT_LEVEL_LOAD:
		//	compute position of default panel 
		{
			float smin = 1.0f - g_game->m_upgrades.general_time_keeper;
			m_gameSpeed->SetMin( smin );

			float w = m_gameSpeed->GetSize().y / 2.0f;
			float p = 1.0f - smin;
			float m = m_gameSpeed->GetMax() - smin;
			if ( m < 0.001f ) m = 0.001f;
			m_gameSpeedDefault->Position().x = m_gameSpeed->GetRect().x1 + w + ( p / m ) * ( m_gameSpeed->GetSize().x - w * 2.0f ) + 0.5f;
		}
		break;
	}

	m_pause->MsgProc( recieverID, msg, data );
	m_main->MsgProc( recieverID, msg, data );
	m_map->MsgProc( recieverID, msg, data );
#if USE_STEAM_SDK
#else
	m_achivements->MsgProc( recieverID, msg, data );
#endif
	m_settings->MsgProc( recieverID, msg, data );
	m_credits->MsgProc( recieverID, msg, data );
	m_profile->MsgProc( recieverID, msg, data );
	m_info->MsgProc( recieverID, msg, data );
	m_upgradePanel->MsgProc( recieverID, msg, data );
}

void GameGUI::ShowTips( const uint tipText, const WCHAR* tipIcon /*= NULL*/ )
{
	if ( !tipText ) return;

	GameTips* newTips = sx_new( GameTips );
	newTips->Create( tipText, tipIcon );

	for (int i=0; i<m_tips.Count(); i++)
		m_tips[i]->m_posY += tipIcon ? GAMETIPS_ICON_SIZE : GAMETIPS_ICON_SIZE_div2;

	m_tips.PushBack( newTips );
}

void GameGUI::Add_Front( sx::gui::PControl pGUI )
{
	m_gui.PushBack( pGUI );
}

void GameGUI::Remove( sx::gui::PControl pGUI )
{
	m_gui.Remove( pGUI );
}

void GameGUI::Add_Back( sx::gui::PControl pGUI )
{
	m_gui.PushFront( pGUI );
}

void GameGUI::OnClick( sx::gui::Control* sender )
{
	if ( !sender ) return;

	switch ( sender->GetUserTag() )
	{
	case 1:	m_gameSpeed->SetValue( m_gameSpeed->GetMin() ); break;
	case 2:	m_gameSpeed->SetValue( m_gameSpeed->GetMax() ); break;
	case 3:	m_gameSpeed->SetValue( 1.0f ); break;
	}
}
