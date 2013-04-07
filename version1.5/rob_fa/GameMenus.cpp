#include "GameMenus.h"
#include "Game.h"
#include "Player.h"
#include "Entity.h"
#include "GameGUI.h"
#include "GameConfig.h"
#include "Scripter.h"

//////////////////////////////////////////////////////////////////////////
//	menu 
//////////////////////////////////////////////////////////////////////////
Menu::Menu()
: m_back(0)
, m_soundNode(0)
{

}

Menu::~Menu()
{

}

void Menu::Initialize( void )
{
	m_back = sx_new( sx::gui::PanelEx );
	m_back->AddProperty( SX_GUI_PROPERTY_BLENDCHILDS );
	m_back->State_Add();

	m_back->State_GetByIndex(0).Color.Set( 1, 1, 1, 0 );
	m_back->State_GetByIndex(0).Blender.Set( 0.9f, 0.2f );

	m_back->State_GetByIndex(1).Color.Set( 1, 1, 1, 1 );

	//	get sound node
	m_soundNode = sx_new( sx::core::Node );

	PStream pfile = NULL;
	if ( sx::sys::FileManager::File_Open( L"gui_sounds.node", SEGAN_PACKAGENAME_DRAFT, pfile ) )
	{
		m_soundNode->Load(*pfile);
		sx::sys::FileManager::File_Close(pfile);
	}
}

void Menu::Finalize( void )
{
	sx_delete_and_null( m_soundNode );
	sx_delete_and_null( m_back );
}

void Menu::ProcessInput( bool& inputHandled, float elpsTime )
{
	m_back->ProcessInput( inputHandled, 0 );
}

void Menu::Update( float elpsTime )
{
	m_back->Update( elpsTime );
}

void Menu::Draw( DWORD flag )
{
	sx_callstack();

	m_back->Draw( flag );
}

void Menu::MsgProc( UINT recieverID, UINT msg, void* data )
{
	
}

void Menu::Show( void )
{
	m_back->State_SetIndex(1);
}

void Menu::Hide( void )
{
	m_back->State_SetIndex(0);
}

bool Menu::IsVisible( void )
{
	return m_back->State_GetIndex() > 0;
}

void Menu::OnEnter( sx::gui::PControl sender )
{
	if ( sender->GetType() == GUI_PANELEX )
		sx::gui::PPanelEx( sender )->State_SetIndex(1);
	
	msg_SoundPlay msg( true, 0, 0, L"mouseHover" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void Menu::OnExit( sx::gui::PControl sender )
{
	sx::gui::PPanelEx( sender )->State_SetIndex(0);
}


//////////////////////////////////////////////////////////////////////////
//	main menu
//////////////////////////////////////////////////////////////////////////
void MenuMain::Initialize( void )
{
	Menu::Initialize();
	m_time = 0;
	m_back->SetSize( float2(1,1) );
	m_back->State_GetByIndex(1).Color.Set( 0.0f, 0.0f, 0.0f, 0.1f );
	m_back->RemProperty( SX_GUI_PROPERTY_BLENDCHILDS );
	m_back->State_GetByIndex(0).Position.Set( 0.0f, 0.0f, 0.0f );
	m_back->State_GetByIndex(1).Position.Set( 0.0f, 0.0f, 0.0f );

	m_mainBack = sx_new( sx::gui::PanelEx );
	m_mainBack->SetParent( m_back );
	m_mainBack->SetSize( float2( 1024.0f, 1024.0f ) );
	m_mainBack->GetElement(0)->SetTextureSrc( L"gui_mainBack.txr" );
	m_mainBack->State_Add();
	m_mainBack->State_Add();
	m_mainBack->State_Add();
	m_mainBack->State_Add();
	m_mainBack->State_GetByIndex(0).Blender.Set( 0.1f, 0.5f );
	m_mainBack->State_GetByIndex(0).Position.Set( -148.0f, 0.0f, 0.0f );
	m_mainBack->State_GetByIndex(1).Blender.Set( 0.043f, 0.5f );
	m_mainBack->State_GetByIndex(1).Position.Set( -250.0f, 0.0f, 0.0f );
	m_mainBack->State_GetByIndex(2).Blender.Set( 0.2f, 0.5f );
	m_mainBack->State_GetByIndex(2).Position.Set( -250.0f, 0.0f, 0.0f );
	m_mainBack->State_GetByIndex(3).Blender.Set( 0.1f, 0.5f );
	m_mainBack->State_GetByIndex(3).Position.Set( -360.0f, 0.0f, 0.0f );
	m_mainBack->State_GetByIndex(4).Blender.Set( 0.06f, 0.7f );
	m_mainBack->State_GetByIndex(4).Position.Set( -510.0f, 0.0f, 0.0f );

	sx::gui::Panel* pnl = sx_new( sx::gui::Panel );
	pnl->SetParent( m_mainBack );
	pnl->SetSize( float2( 360.0f, 656.0f ) );
	pnl->Position().Set( 380.0f, 0.0f, 0.0f );
	pnl->GetElement(0)->Color() = D3DColor( 0, 0, 0, 0 );

	m_slantBack = sx_new( sx::gui::PanelEx );
	m_slantBack->SetParent( m_back );
	m_slantBack->SetSize( float2( 1024, 1024) );
	m_slantBack->GetElement(0)->SetTextureSrc( L"gui_mainSlant.txr" );
	m_slantBack->State_Add();
	m_slantBack->State_GetByIndex(0).Color.w = 0.0f;
	m_slantBack->State_GetByIndex(0).Blender.Set( 0.1f, 0.5f );
	m_slantBack->State_GetByIndex(0).Position.Set( 222.0f, 0.0f, 0.0f );
	m_slantBack->State_GetByIndex(1).Blender.Set( 0.13f, 0.5f );
	m_slantBack->State_GetByIndex(1).Position.Set( 185.0f, 0.0f, 0.0f );

	for ( int i=0; i<5; i++ )
	{
		m_btn[i] = sx_new( sx::gui::PanelEx );
		m_btn[i]->SetParent( m_slantBack );
		m_btn[i]->SetSize( float2( 512, 64 ) );
		m_btn[i]->State_Add();
		m_btn[i]->State_Add();
		m_btn[i]->State_GetByIndex(0).Color.w = 0.0f;
		m_btn[i]->State_GetByIndex(0).Blender.Set( 0.12f, 0.6f );
		m_btn[i]->State_GetByIndex(0).Position.Set( 230.0f, -700.0f, 0 );
		m_btn[i]->State_GetByIndex(2).Blender.Set( 0.1f, 0.6f );
		m_btn[i]->State_GetByIndex(2).Color.Set( 1.0f, 0.78f, 0.0f, 1.0f );

		switch ( i )
		{
		case 0:	m_btn[i]->State_GetByIndex(1).Blender.Set( 0.100f, 0.6f );	break;
		case 1:	m_btn[i]->State_GetByIndex(1).Blender.Set( 0.095f, 0.6f );	break;
		case 2:	m_btn[i]->State_GetByIndex(1).Blender.Set( 0.090f, 0.6f );	break;
		case 3:	m_btn[i]->State_GetByIndex(1).Blender.Set( 0.085f, 0.6f );	break;
		case 4:	m_btn[i]->State_GetByIndex(1).Blender.Set( 0.080f, 0.6f );	break;
		}

		switch ( i )
		{
		case 0:	m_btn[i]->State_GetByIndex(1).Position.Set( 22.0f, 96.0f, 0 );	break;
		case 1:	m_btn[i]->State_GetByIndex(1).Position.Set( 34.0f, 48.0f, 0 );	break;
		case 2:	m_btn[i]->State_GetByIndex(1).Position.Set( 46.0f, 0.0f, 0 );	break;
		case 3:	m_btn[i]->State_GetByIndex(1).Position.Set( 58.0f, -48.5f, 0 );	break;
		case 4:	m_btn[i]->State_GetByIndex(1).Position.Set( 70.0f, -96.0f, 0 );	break;
		}

		switch ( i )
		{
		case 0:	m_btn[i]->State_GetByIndex(2).Position.Set( 16.0f, 96.0f, 0 );	break;
		case 1:	m_btn[i]->State_GetByIndex(2).Position.Set( 28.0f, 48.0f, 0 );	break;
		case 2:	m_btn[i]->State_GetByIndex(2).Position.Set( 40.0f, 0.0f, 0 );	break;
		case 3:	m_btn[i]->State_GetByIndex(2).Position.Set( 52.0f, -48.5f, 0 );	break;
		case 4:	m_btn[i]->State_GetByIndex(2).Position.Set( 60.0f, -96.0f, 0 );	break;
		}

		switch ( i )
		{
		case 0:	m_btn[i]->GetElement(0)->SetTextureSrc( L"gui_mainPlay.txr" );		break;
		case 1:	m_btn[i]->GetElement(0)->SetTextureSrc( L"gui_mainProfile.txr" );	break;
		case 2:	m_btn[i]->GetElement(0)->SetTextureSrc( L"gui_mainAch.txr" );		break;
		case 3:	m_btn[i]->GetElement(0)->SetTextureSrc( L"gui_mainSettings.txr" );	break;
		case 4:	m_btn[i]->GetElement(0)->SetTextureSrc( L"gui_mainCredits.txr" );	break;
		}

		sx::gui::Panel* pnl = sx_new( sx::gui::Panel );
		pnl->SetParent( m_btn[i] );
		pnl->SetSize( float2( 252, 30 ) );
		pnl->Position().x = -100.0f;
		pnl->GetElement(0)->Color().a = 0.001f;
		pnl->SetUserTag( i );
		SEGAN_GUI_SET_ONCLICK( pnl, MenuMain::OnClick );
		SEGAN_GUI_SET_ONENTER( pnl, MenuMain::OnEnter );
		SEGAN_GUI_SET_ONEXIT( pnl, MenuMain::OnExit );
	}

}

void MenuMain::ProcessInput( bool& inputHandled, float elpsTime )
{
	if ( inputHandled ) return;
	sx_callstack();

	if ( g_game->m_game_currentLevel == 0 && m_slantBack->State_GetIndex() )
	{
		if ( SEGAN_KEYUP( 0, SX_INPUT_KEY_ESCAPE ) )
		{
			g_game->m_gui->m_confirmExit->Show();
		}

		for ( int i=0; i<5; i++ )
		{
			bool ih = false;
			m_btn[i]->GetChild(0)->ProcessInput( ih );
			if ( ih ) inputHandled = true;
		}
	}
}

void MenuMain::Update( float elpsTime )
{
	static int firstTime = 1;
	if ( elpsTime > 100 ) return;

	if ( g_game->m_game_currentLevel == 0 && !g_game->m_app_Loading )
	{
		m_time += elpsTime;

		const float maxTime = firstTime==1 ? 2000.0f : 500.0f;
		if ( m_time > maxTime && m_time < 4000.0f )
		{
			m_mainBack->State_SetIndex( firstTime );
			m_slantBack->State_SetIndex(1);
			for ( int i=0; i<5; i++ )
				m_btn[i]->State_SetIndex(1);
			m_time = 4750.0f;
			firstTime = 2;
		}

		if ( m_time > 5000.0f && m_time < 6000.0f )
		{
			m_time = 7000.0f;

			g_game->m_gui->m_status->Show();

			for ( int i=0; i<5; i++ )
				m_btn[i]->GetChild(0)->AddProperty( SX_GUI_PROPERTY_ACTIVATE );

			sx::gui::PPanel( m_mainBack->GetChild(0) )->GetElement(0)->Color().a = 1;
		}
		
	}
	else
	{
		m_mainBack->State_SetIndex(0);
		m_slantBack->State_SetIndex(0);
		for ( int i=0; i<5; i++ )
			m_btn[i]->State_SetIndex(0);
	}

	Menu::Update( elpsTime );
}

void MenuMain::OnClick( sx::gui::PControl sender )
{
	if ( !sender ) return;

	switch ( sender->GetUserTag() )
	{
	case 0: // play
		{
			m_slantBack->State_SetIndex(0);
			for ( int i=0; i<5; i++ )
			{
				m_btn[i]->GetChild(0)->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
				m_btn[i]->State_SetIndex(0);
			}

			m_mainBack->State_SetIndex(4);
			g_game->m_gui->m_map->Show();
			g_game->m_gui->m_status->Hide();
		}
		break;

	case 1: // profile
		{
			m_slantBack->State_SetIndex(0);
			for ( int i=0; i<5; i++ )
			{
				m_btn[i]->GetChild(0)->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
				m_btn[i]->State_SetIndex(0);
			}

			m_mainBack->State_SetIndex(3);
			g_game->m_gui->m_profile->Show();
		}
		break;

	case 2: // achievements
		{
			m_slantBack->State_SetIndex(0);
			for ( int i=0; i<5; i++ )
			{
				m_btn[i]->GetChild(0)->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
				m_btn[i]->State_SetIndex(0);
			}

			m_mainBack->State_SetIndex(3);
			g_game->m_gui->m_achivements->Show();
		}
		break;

	case 3: // settings
		{
			m_slantBack->State_SetIndex(0);
			for ( int i=0; i<5; i++ )
			{
				m_btn[i]->GetChild(0)->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
				m_btn[i]->State_SetIndex(0);
			}

			m_mainBack->State_SetIndex(3);
			g_game->m_gui->m_settings->Show();
		}
		break;

	case 4: // credits
		{
			
		}
		break;
	}

	msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void MenuMain::OnEnter( sx::gui::PControl sender )
{
	if ( !sender ) return;
	sx::gui::PPanelEx( sender->GetParent() )->State_SetIndex(2);

	msg_SoundPlay msg( true, 0, 0, L"mouseHover" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void MenuMain::OnExit( sx::gui::PControl sender )
{
	if ( !sender ) return;
	sx::gui::PPanelEx( sender->GetParent() )->State_SetIndex(1);
}

void MenuMain::Show( void )
{
	Menu::Show();
	m_time = 200;
}

void MenuMain::Hide( void )
{
	Menu::Hide();
	g_game->m_gui->m_status->Hide();
}


//////////////////////////////////////////////////////////////////////////
//	main map ( level selection )
//////////////////////////////////////////////////////////////////////////
void MenuMap::Initialize( void )
{
	Menu::Initialize();
	m_selectedLevel = 1;
	m_frame = 0;

	m_back->SetSize( float2( 1024, 1024 ) );
	m_back->GetElement(0)->SetTextureSrc( L"gui_menu_map.txr" );
	m_back->State_GetByIndex(0).Position.Set( 160.0f, -76.0f, 0.0f );
	m_back->State_GetByIndex(0).Blender.Set( 0.3f, 0.3f );
	m_back->State_GetByIndex(1).Position.Set( 0.0f, -76.0f, 0.0f );
	m_back->State_GetByIndex(1).Blender.Set( 0.07f, 0.6f );

	//	create level chooser
	m_chooser = sx_new( sx::gui::PanelEx );
	m_chooser->SetParent( m_back );
	m_chooser->SetSize( float2(128, 128) );
	m_chooser->GetElement(0)->SetTextureSrc( L"gui_menu_map_chooser.txr" );
	m_chooser->RemProperty( SX_GUI_PROPERTY_BLENDSTATES );

	//	create level indicators
	for ( int i=0; i<10; i++ )
	{
		Level_GUI* guil = &( m_levels[i] );

		//	create roads
		guil->m_road = sx_new( sx::gui::PanelEx );
		guil->m_road->SetParent( m_back );
		str256 str; str.Format( L"gui_menu_map_road%d.txr", i );
		guil->m_road->GetElement(0)->SetTextureSrc( str );
		switch ( i )
		{
		case 0:		guil->m_road->SetSize( float2( 128, 128 ) );	break;
		case 1:		guil->m_road->SetSize( float2( 256, 128 ) );	break;
		case 2:		guil->m_road->SetSize( float2( 128, 256 ) );	break;
		case 3:		guil->m_road->SetSize( float2( 128, 128 ) );	break;
		case 4:		guil->m_road->SetSize( float2( 256, 128 ) );	break;
		case 5:		guil->m_road->SetSize( float2( 256, 128 ) );	break;
		case 6:		guil->m_road->SetSize( float2( 128, 256 ) );	break;
		case 7:		guil->m_road->SetSize( float2( 256, 64  ) );	break;
		case 8:		guil->m_road->SetSize( float2( 256, 128 ) );	break;
		case 9:		guil->m_road->SetSize( float2( 32 , 128 ) );	break;
		}
		switch ( i )
		{
		case 0:		guil->m_road->State_GetByIndex(0).Position.Set( -98.0f, -101.0f, 0.0f );	break;
		case 1:		guil->m_road->State_GetByIndex(0).Position.Set( 11.0f, -5.0f, 0.0f );		break;
		case 2:		guil->m_road->State_GetByIndex(0).Position.Set( 166.0f, -87.0f, 0.0f );		break;
		case 3:		guil->m_road->State_GetByIndex(0).Position.Set( 272.0f, -106.0f, 0.0f );	break;
		case 4:		guil->m_road->State_GetByIndex(0).Position.Set( 237.0f, 3.0f, 0.0f );		break;
		case 5:		guil->m_road->State_GetByIndex(0).Position.Set( 69.0f, 78.0f, 0.0f );		break;
		case 6:		guil->m_road->State_GetByIndex(0).Position.Set( -56.0f, 181.0f, 0.0f );		break;
		case 7:		guil->m_road->State_GetByIndex(0).Position.Set( 39.0f, 252.0f, 0.0f );		break;
		case 8:		guil->m_road->State_GetByIndex(0).Position.Set( 232.0f, 160.0f, 0.0f );		break;
		case 9:		guil->m_road->State_GetByIndex(0).Position.Set( 330.0f, 188.0f, 0.0f );		break;
		}

		guil->m_road->State_GetByIndex(0).Color.Set(0,0,0,1);
		guil->m_road->State_Add();
		guil->m_road->State_GetByIndex(0).Color.Set(0,0,0,0);
	}

	for ( int i=0; i<10; i++ )
	{
		Level_GUI* guil = &( m_levels[i] );

		//	create level indicator
		guil->m_area = sx_new( sx::gui::PanelEx );
		guil->m_area->SetUserTag( i );
		guil->m_area->SetParent( m_back );
		guil->m_area->SetSize( float2(32, 32) );
		guil->m_area->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
		guil->m_area->GetElement(0)->SetTextureSrc( L"gui_menu_map_level.txr" );
		switch ( i )
		{
		case 0:		guil->m_area->State_GetByIndex(0).Position.Set( -75.0f, -55.0f, 0.0f );		break;
		case 1:		guil->m_area->State_GetByIndex(0).Position.Set( 104.0f, -3.0f, 0.0f );		break;
		case 2:		guil->m_area->State_GetByIndex(0).Position.Set( 229.0f, -165.0f, 0.0f );	break;
		case 3:		guil->m_area->State_GetByIndex(0).Position.Set( 309.0f, -44.0f, 0.0f );		break;
		case 4:		guil->m_area->State_GetByIndex(0).Position.Set( 158.0f, 44.0f, 0.0f );		break;
		case 5:		guil->m_area->State_GetByIndex(0).Position.Set( -20.0f, 116.0f, 0.0f );		break;
		case 6:		guil->m_area->State_GetByIndex(0).Position.Set( -67.0f, 249.0f, 0.0f );		break;
		case 7:		guil->m_area->State_GetByIndex(0).Position.Set( 137.0f, 227.0f, 0.0f );		break;
		case 8:		guil->m_area->State_GetByIndex(0).Position.Set( 330.0f, 130.0f, 0.0f );		break;
		case 9:		guil->m_area->State_GetByIndex(0).Position.Set( 292.0f, 293.0f, 0.0f );		break;
		}
		guil->m_area->State_Add();
		guil->m_area->State_GetByIndex(1).Color.Set( 1.0f, 0.0f, 0.0f, 1.0f );
		guil->m_area->State_GetByIndex(1).Blender.Set( 0.1f, 0.7f );
		SEGAN_GUI_SET_ONENTER( guil->m_area, Menu::OnEnter );
		SEGAN_GUI_SET_ONEXIT( guil->m_area, Menu::OnExit );
		SEGAN_GUI_SET_ONCLICK( guil->m_area, MenuMap::OnClick );
		guil->m_area->State_Add();
		guil->m_area->State_GetByIndex(2).Scale.Set( 0.0f, 0.0f, 0.0f );
		guil->m_area->State_GetByIndex(2).Blender.Set( 0.1f, 0.7f );
		guil->m_area->State_SetIndex(2);

		//	create flag of level
		guil->m_flag = sx_new( sx::gui::PanelEx );
		guil->m_flag->SetParent( guil->m_area );
		guil->m_flag->SetSize( float2(128, 128) );
		guil->m_flag->GetElement(0)->SetTextureSrc( L"gui_menu_map_flag.txr" );
		guil->m_flag->State_Add();
		guil->m_flag->State_GetByIndex(1).Position.Set( 0.0f, 30.0f, 0.0f );
		guil->m_flag->State_GetByIndex(0).Scale.Set( 0.0f, 0.0f, 0.0f );
		for ( int s=0; s<3; s++ )
		{
			guil->m_star[s] = sx_new( sx::gui::PanelEx );
			guil->m_star[s]->SetParent( guil->m_flag );
			guil->m_star[s]->SetSize( float2(16, 16) );
			guil->m_star[s]->GetElement(0)->SetTextureSrc( L"gui_menu_map_star.txr" );
			switch ( s )
			{
			case 0:	guil->m_star[s]->State_GetByIndex(0).Position.Set( 0.0f, -3.0f, 0.0f );	break;
			case 1:	guil->m_star[s]->State_GetByIndex(0).Position.Set( 0.0f,  8.0f, 0.0f );	break;
			case 2:	guil->m_star[s]->State_GetByIndex(0).Position.Set( 0.0f, 19.0f, 0.0f );	break;
			}
			guil->m_star[s]->State_Add();
			guil->m_star[s]->State_GetByIndex(0).Scale.Set( 0.0f, 0.0f, 0.0f );
		}

		m_chooser->State_Add();
		m_chooser->State_GetByIndex(i+1).Position = guil->m_area->State_GetByIndex(0).Position;
	}
	m_chooser->State_GetByIndex(0).Color.Set(0,0,0,0);


	//	create buttons
	m_playGame = sx_new( sx::gui::PanelEx );
	m_playGame->SetUserTag( 10 );
	m_playGame->SetParent( m_back );
	m_playGame->SetSize( float2(128, 128) );
	m_playGame->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_playGame->GetElement(0)->SetTextureSrc( L"gui_menu_map_play.txr" );
	m_playGame->State_GetByIndex(0).Position.Set( -340.0f, 192.0f, 0.0f );
	m_playGame->State_Add();
	m_playGame->State_GetByIndex(1).Scale.Set( 1.15f, 1.15f, 1.0f );
	m_playGame->State_GetByIndex(1).Blender.Set( 0.1f, 0.7f );
	SEGAN_GUI_SET_ONENTER( m_playGame, Menu::OnEnter );
	SEGAN_GUI_SET_ONEXIT( m_playGame, Menu::OnExit );
	SEGAN_GUI_SET_ONCLICK( m_playGame, MenuMap::OnClick );


	m_miniGame = sx_new( sx::gui::PanelEx );
	m_miniGame->SetUserTag( 11 );
	m_miniGame->SetParent( m_back );
	m_miniGame->SetSize( float2(64, 64) );
	m_miniGame->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_miniGame->GetElement(0)->SetTextureSrc( L"gui_menu_map_mini.txr" );
	m_miniGame->State_GetByIndex(0).Position.Set( -340.0f, 112.0f, 0.0f );
	m_miniGame->State_Add();
	m_miniGame->State_GetByIndex(1).Scale.Set( 1.15f, 1.15f, 1.0f );
	m_miniGame->State_GetByIndex(1).Blender.Set( 0.1f, 0.7f );
	SEGAN_GUI_SET_ONENTER( m_miniGame, Menu::OnEnter );
	SEGAN_GUI_SET_ONEXIT( m_miniGame, Menu::OnExit );
	SEGAN_GUI_SET_ONCLICK( m_miniGame, MenuMap::OnClick );

	m_upgrade = sx_new( sx::gui::PanelEx );
	m_upgrade->SetUserTag( 12 );
	m_upgrade->SetParent( m_back );
	m_upgrade->SetSize( float2(64, 64) );
	m_upgrade->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_upgrade->GetElement(0)->SetTextureSrc( L"gui_menu_map_upgrade.txr" );
	m_upgrade->State_GetByIndex(0).Position.Set( -324.0f, 52.0f, 0.0f );
	m_upgrade->State_Add();
	m_upgrade->State_GetByIndex(1).Scale.Set( 1.15f, 1.15f, 1.0f );
	m_upgrade->State_GetByIndex(1).Blender.Set( 0.1f, 0.7f );
	SEGAN_GUI_SET_ONENTER( m_upgrade, Menu::OnEnter );
	SEGAN_GUI_SET_ONEXIT( m_upgrade, Menu::OnExit );
	SEGAN_GUI_SET_ONCLICK( m_upgrade, MenuMap::OnClick );

	//	create back button
	m_goback = sx_new( sx::gui::Button );
	m_goback->SetUserTag( 13 );
	m_goback->SetParent( m_back );
	m_goback->SetSize( float2( 128, 32 ) );
	m_goback->Position().Set( -274.0f, -11.0f, 0.0f );
	m_goback->GetElement(0)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(0)->Color().a = 0.01f;
	m_goback->GetElement(1)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(1)->Color().a = 1.0f;
	m_goback->GetElement(2)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(2)->Color().a = 0.5f;
	SEGAN_GUI_SET_ONCLICK( m_goback, MenuMap::OnClick );
	SEGAN_GUI_SET_ONENTER( m_goback, Menu::OnEnter );

	//	create difficulty
	m_diff_scroll = sx_new( sx::gui::TrackBar );
	m_diff_scroll->SetMax(2);
	m_diff_scroll->SetUserTag( 1 );
	m_diff_scroll->SetParent( m_back );
	m_diff_scroll->SetSize( float2(130, 32) );
	m_diff_scroll->AddProperty( SX_GUI_PROPERTY_AUTOSIZE );
	m_diff_scroll->GetElement(0)->Color().a = 0.0f;
	m_diff_scroll->GetElement(1)->Color().a = 0.01f;
	m_diff_scroll->Position().Set( -317.0f, 265.0f, 0.0f );
	SEGAN_GUI_SET_ONSCROLL( m_diff_scroll, MenuMap::OnScroll );

	sx::gui::Panel* panel = sx_new( sx::gui::Panel );
	panel->SetParent( m_diff_scroll );
	panel->SetSize( float2( 80, 32 ) );
	panel->GetElement(0)->SetTextureSrc( L"gui_menu_map_diff.txr" );

	//	create label to show people
	m_diff_label = sx_new( sx::gui::Label );
	m_diff_label->SetParent( panel );
	m_diff_label->SetSize( float2(80, 20) );
	m_diff_label->SetAlign( GTA_CENTER );
	m_diff_label->GetElement(0)->Color().a = 0.0f;
	m_diff_label->GetElement(1)->Color() = 0xaaffffff;
	m_diff_label->SetFont( L"Font_map_diff.fnt" );
	m_diff_label->AddProperty( SX_GUI_PROPERTY_MULTILINE );
	m_diff_label->AddProperty( SX_GUI_PROPERTY_WORDWRAP );

	panel = sx_new( sx::gui::Panel );
	panel->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	panel->SetParent( m_diff_scroll );
	panel->SetSize( float2( 32, 32 ) );
	panel->GetElement(0)->Color().a = 0.01f;
	panel->SetUserTag( 2 );
	panel->Position().x = m_diff_scroll->GetSize().x * 0.5f + 16;
	SEGAN_GUI_SET_ONCLICK( panel, MenuMap::OnScroll );

	panel = sx_new( sx::gui::Panel );
	panel->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	panel->SetParent( m_diff_scroll );
	panel->SetSize( float2( 32, 32 ) );
	panel->GetElement(0)->Color().a = 0.01f;
	panel->SetUserTag( 3 );
	panel->Position().x = - m_diff_scroll->GetSize().x * 0.5f - 16;
	SEGAN_GUI_SET_ONCLICK( panel, MenuMap::OnScroll );

	//	load some default value
	String str = sx::sys::FileManager::Project_GetDir();
	str << L"strings.txt";
	Scripter script;
	script.Load( str );
	for (int i=0; i<script.GetObjectCount(); i++)
	{
		str512 tmpStr;
		if ( script.GetString(i, L"Type", tmpStr) && tmpStr == L"Strings" )
		{
			if ( !script.GetString(i, L"Name", tmpStr) )
				continue;

			if ( tmpStr == L"DifficultyButton" )
			{
				script.GetString( i, L"norm",	m_diff_norm_text	);
				script.GetString( i, L"hard",	m_diff_hard_text	);
				script.GetString( i, L"insane", m_diff_insane_text	);
				break;
			}
		}
	}
}

void MenuMap::Finalize( void )
{
	Menu::Finalize();
}

void MenuMap::ProcessInput( bool& inputHandled, float elpsTime )
{
	if ( !m_back->State_GetIndex() ) return;
	sx_callstack();

	if ( !inputHandled && SEGAN_KEYUP( 0, SX_INPUT_KEY_ESCAPE ) )
	{
		Hide();
		g_game->m_gui->m_main->Show();
		inputHandled = true;
		return;
	}
	else if ( !inputHandled && SEGAN_KEYUP( 0, SX_INPUT_KEY_RETURN ) )
	{
		OnClick( m_playGame );
		inputHandled = true;
		return;
	}

	Menu::ProcessInput( inputHandled, elpsTime );
}

void MenuMap::Update( float elpsTime )
{
	if ( g_game->m_game_currentLevel == 0 )
	{
		static float rotatez = 0;
		rotatez += elpsTime * 0.002f;
		m_chooser->Scale().x = 1 + 0.05f * sin( rotatez );
		m_chooser->Scale().y = 1 + 0.05f * sin( rotatez );

		m_levels[ m_selectedLevel-1 ].m_area->State_SetIndex(1);

		if ( m_back->State_GetIndex() && m_frame < 150 )
		{
			m_frame++;

			if ( g_game->m_player->m_profile.level > 0 && m_frame ==  5 )	m_levels[0].m_area->State_SetIndex(0);
			if ( g_game->m_player->m_profile.level > 1 && m_frame == 10 )	m_levels[1].m_area->State_SetIndex(0);
			if ( g_game->m_player->m_profile.level > 2 && m_frame == 15 )	m_levels[2].m_area->State_SetIndex(0);
			if ( g_game->m_player->m_profile.level > 3 && m_frame == 20 )	m_levels[3].m_area->State_SetIndex(0);
			if ( g_game->m_player->m_profile.level > 4 && m_frame == 25 )	m_levels[4].m_area->State_SetIndex(0);
			if ( g_game->m_player->m_profile.level > 5 && m_frame == 30 )	m_levels[5].m_area->State_SetIndex(0);
			if ( g_game->m_player->m_profile.level > 6 && m_frame == 35 )	m_levels[6].m_area->State_SetIndex(0);
			if ( g_game->m_player->m_profile.level > 7 && m_frame == 40 )	m_levels[7].m_area->State_SetIndex(0);
			if ( g_game->m_player->m_profile.level > 8 && m_frame == 45 )	m_levels[8].m_area->State_SetIndex(0);
			if ( g_game->m_player->m_profile.level > 8 && m_frame == 50 )	m_levels[9].m_area->State_SetIndex(0);

			if ( g_game->m_player->m_profile.level > 0 && m_frame ==  5 )	m_levels[0].m_road->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 1 && m_frame == 10 )	m_levels[1].m_road->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 2 && m_frame == 15 )	m_levels[2].m_road->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 3 && m_frame == 20 )	m_levels[3].m_road->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 4 && m_frame == 25 )	m_levels[4].m_road->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 5 && m_frame == 30 )	m_levels[5].m_road->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 6 && m_frame == 35 )	m_levels[6].m_road->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 7 && m_frame == 40 )	m_levels[7].m_road->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 8 && m_frame == 45 )	m_levels[8].m_road->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 8 && m_frame == 50 )	m_levels[9].m_road->State_SetIndex(1);

			if ( g_game->m_player->m_profile.level > 1 && m_frame == 30 )	m_levels[0].m_flag->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 2 && m_frame == 35 )	m_levels[1].m_flag->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 3 && m_frame == 40 )	m_levels[2].m_flag->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 4 && m_frame == 45 )	m_levels[3].m_flag->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 5 && m_frame == 50 )	m_levels[4].m_flag->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 6 && m_frame == 55 )	m_levels[5].m_flag->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 7 && m_frame == 60 )	m_levels[6].m_flag->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 8 && m_frame == 65 )	m_levels[7].m_flag->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 9 && m_frame == 70 )	m_levels[8].m_flag->State_SetIndex(1);
			if ( g_game->m_player->m_profile.level > 9 && m_frame == 75 )	m_levels[9].m_flag->State_SetIndex(1);

			if ( m_frame == 50 ) { for ( int i=0; i<3 && i<g_game->m_player->m_profile.stars[0]; i++)	m_levels[0].m_star[i]->State_SetIndex(1); }
			if ( m_frame == 55 ) { for ( int i=0; i<3 && i<g_game->m_player->m_profile.stars[1]; i++)	m_levels[1].m_star[i]->State_SetIndex(1); }
			if ( m_frame == 60 ) { for ( int i=0; i<3 && i<g_game->m_player->m_profile.stars[2]; i++)	m_levels[2].m_star[i]->State_SetIndex(1); }
			if ( m_frame == 65 ) { for ( int i=0; i<3 && i<g_game->m_player->m_profile.stars[3]; i++)	m_levels[3].m_star[i]->State_SetIndex(1); }
			if ( m_frame == 70 ) { for ( int i=0; i<3 && i<g_game->m_player->m_profile.stars[4]; i++)	m_levels[4].m_star[i]->State_SetIndex(1); }
			if ( m_frame == 75 ) { for ( int i=0; i<3 && i<g_game->m_player->m_profile.stars[5]; i++)	m_levels[5].m_star[i]->State_SetIndex(1); }
			if ( m_frame == 80 ) { for ( int i=0; i<3 && i<g_game->m_player->m_profile.stars[6]; i++)	m_levels[6].m_star[i]->State_SetIndex(1); }
			if ( m_frame == 85 ) { for ( int i=0; i<3 && i<g_game->m_player->m_profile.stars[7]; i++)	m_levels[7].m_star[i]->State_SetIndex(1); }
			if ( m_frame == 90 ) { for ( int i=0; i<3 && i<g_game->m_player->m_profile.stars[8]; i++)	m_levels[8].m_star[i]->State_SetIndex(1); }
			if ( m_frame == 95 ) { for ( int i=0; i<3 && i<g_game->m_player->m_profile.stars[9]; i++)	m_levels[9].m_star[i]->State_SetIndex(1); }
		}

	}

	{
		float x, y, z;
		m_diff_scroll->GetElement(1)->Matrix().GetTranslation(x, y, z);
		sx::gui::Panel* panel = (sx::gui::PPanel)m_diff_scroll->GetChild(0);
		panel->Position().Set( x, y, z );
	}

	Menu::Update(elpsTime);
}

void MenuMap::Draw( DWORD flag )
{
	sx_callstack();
	Menu::Draw(flag);
}

void MenuMap::Show( void )
{
	for ( int i=0; i<10; i++ )
	{
		m_levels[i].m_area->State_SetIndex(2);
		
		switch ( g_game->m_player->m_profile.difficulty[i] )
		{
		case 0:	m_levels[i].m_flag->GetElement(0)->SetTextureSrc( L"gui_menu_map_flag_0.txr" );		break;
		case 1:	m_levels[i].m_flag->GetElement(0)->SetTextureSrc( L"gui_menu_map_flag_1.txr" );		break;
		case 2:	m_levels[i].m_flag->GetElement(0)->SetTextureSrc( L"gui_menu_map_flag_2.txr" );		break;
		}
	}

	m_selectedLevel = g_game->m_player->m_profile.level_selected;
	SEGAN_CLAMP( m_selectedLevel, 1, 10 );
	m_chooser->State_SetIndex( m_selectedLevel );

	//	load from player profile
	m_diff_scroll->SetValue( (float)g_game->m_player->m_profile.curDifficulty );

	Menu::Show();
	m_frame = 0;
}

void MenuMap::Hide( void )
{
	if ( IsVisible() )
	{
		//	get updates
		g_game->m_gui->m_upgradePanel->GetData( g_game->m_player->m_profile.upgrades );

		//	apply player to profile
		g_game->m_gui->m_profile->SyncProfileAndPlayer( false );

		//	save profiles
		g_game->m_gui->m_profile->SaveProfile();
	}

	Menu::Hide();
	for ( int i=0; i<10; i++ )
	{
		m_levels[i].m_road->State_SetIndex(0);
		m_levels[i].m_flag->State_SetIndex(0);
		m_levels[i].m_area->State_SetIndex(2);
		m_levels[i].m_star[0]->State_SetIndex(0);
		m_levels[i].m_star[1]->State_SetIndex(0);
		m_levels[i].m_star[2]->State_SetIndex(0);
	}
//	m_upgradePanel->Hide();

	//	prepare to start game
	g_game->m_player->SyncPlayerAndGame( true );
}

void MenuMap::OnClick( sx::gui::PControl sender )
{
	if ( !sender ) return;

	switch ( sender->GetUserTag() )
	{
	case 0:	case 1:	case 2:	case 3:	case 4:	case 5:	case 6:	case 7:	case 8:	case 9:
		{
			m_levels[ m_selectedLevel-1 ].m_area->State_SetIndex(0);

			g_game->m_player->m_profile.level_selected = m_selectedLevel = sender->GetUserTag() + 1;
			m_chooser->State_SetIndex( m_selectedLevel );

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		break;

	case 10:	//	play
		{
			g_game->m_miniGame = false;
			g_game->m_game_nextLevel = m_selectedLevel;

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

			Hide();
			g_game->m_gui->m_main->Hide();
		}
		break;

	case 11:	// play mini game
		{
			if ( g_game->m_player->m_profile.level > m_selectedLevel )
			{
				g_game->m_miniGame = true;
				g_game->m_game_nextLevel = m_selectedLevel;

				msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
				m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

				Hide();
				g_game->m_gui->m_main->Hide();
			}
			else
			{
				msg_SoundPlay msg( true, 0, 0, L"menu_minigame" );
				m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
			}
		}
		break;

	case 12:	// upgrade
		{
// 			Hide();
// 			g_game->m_gui->m_main->Show();
// 			g_game->m_gui->m_main->m_mainBack->State_SetIndex(1);
			
			g_game->m_gui->m_upgradePanel->SetData (
				g_game->m_player->m_profile.level, 
				g_game->m_player->m_profile.GetNumStars(), 
				g_game->m_player->m_profile.upgrades 
				);
			g_game->m_gui->m_upgradePanel->Show();

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		break;

	case 13:	// back
		{
			Hide();
			g_game->m_gui->m_main->Show();
			g_game->m_gui->m_main->m_mainBack->State_SetIndex(1);

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		break;

	}
}

void MenuMap::OnScroll( sx::gui::PControl sender )
{
	if ( !sender ) return;

	switch ( sender->GetUserTag() )
	{
	case 2:	//	right
		m_diff_scroll->SetValue(2);
		break;

	case 3:	//	left
		m_diff_scroll->SetValue(0);
		break;
	}

	g_game->m_player->m_profile.curDifficulty = int( m_diff_scroll->GetValue() + 0.5f );

	switch ( g_game->m_player->m_profile.curDifficulty )
	{
	case 0:
		m_diff_label->SetText( m_diff_norm_text );
		g_game->m_difficultyLevel = 0;
		break;
	case 1:
		m_diff_label->SetText( m_diff_hard_text );
		g_game->m_difficultyLevel = 1;
		break;
	case 2:
		m_diff_label->SetText( m_diff_insane_text );
		g_game->m_difficultyLevel = 2;
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
//	profile menu
//////////////////////////////////////////////////////////////////////////
void MenuProfile::Initialize( void )
{
	Menu::Initialize();
	
	m_back->SetSize( float2( 1024, 1024 ) );
	m_back->GetElement(0)->SetTextureSrc( L"gui_Profile.txr" );
	m_back->State_GetByIndex(0).Position.Set( 160.0f, -76.0f, 0.0f );
	m_back->State_GetByIndex(0).Blender.Set( 0.2f, 0.3f );
	m_back->State_GetByIndex(1).Position.Set( 90.0f, -76.0f, 0.0f );
	m_back->State_GetByIndex(1).Blender.Set( 0.03f, 0.85f );

	//	create text edit
	m_profileName = sx_new( sx::gui::TextEdit );
	m_profileName->SetParent( m_back );
	m_profileName->SetSize( float2( 350, 50 ) );
	m_profileName->Position().Set( -15.0f, 225.0f, 0.0f );
	m_profileName->SetAlign( GTA_LEFT );
	m_profileName->GetElement(0)->Color().a = 0;
	m_profileName->GetElement(1)->Color().a = 0.8f;
	m_profileName->SetFont( L"Font_rob_profile.fnt" );
	m_profileName->SetOnKeyDown( (sx::gui::PForm)this, (GUICallbackEvent)&MenuProfile::OnKey );

	//	create rename button
	m_rename = sx_new( sx::gui::Button );
	m_rename->SetParent( m_back );
	m_rename->SetSize( float2( 32, 32 ) );
	m_rename->Position().Set( 100.0f, 235.0f, 0.0f );
	m_rename->GetElement(0)->SetTextureSrc( L"gui_profile_rename.txr" );
	m_rename->GetElement(0)->Color().a = 0.7f;
	m_rename->GetElement(1)->SetTextureSrc( L"gui_profile_rename.txr" );
	m_rename->GetElement(1)->Color().a = 1.0f;
	m_rename->GetElement(2)->SetTextureSrc( L"gui_profile_rename.txr" );
	m_rename->GetElement(2)->Color().a = 0.6f;
	SEGAN_GUI_SET_ONCLICK( m_rename, MenuProfile::OnClick );

	//	create delete button
	m_delete = sx_new( sx::gui::Button );
	m_delete->SetParent( m_back );
	m_delete->SetSize( float2( 32, 32 ) );
	m_delete->Position().Set( 140.0f, 225.0f, 0.0f );
	m_delete->GetElement(0)->SetTextureSrc( L"gui_profile_delete.txr" );
	m_delete->GetElement(0)->Color().a = 0.7f;
	m_delete->GetElement(1)->SetTextureSrc( L"gui_profile_delete.txr" );
	m_delete->GetElement(1)->Color().a = 1.0f;
	m_delete->GetElement(2)->SetTextureSrc( L"gui_profile_delete.txr" );
	m_delete->GetElement(2)->Color().a = 0.6f;
	SEGAN_GUI_SET_ONCLICK( m_delete, MenuProfile::OnClick );


	//	create profile list
	for ( int i=0; i<4; i++ )
	{
		m_profPanel[i] = sx_new( sx::gui::PanelEx );
		m_profPanel[i]->SetParent( m_back );
		m_profPanel[i]->SetSize( float2( 512, 64) );
		m_profPanel[i]->GetElement(0)->SetTextureSrc( L"gui_profileSelect.txr" );

		switch ( i )
		{
		case 0:		m_profPanel[i]->Position().Set( 49.0f, 184.5f, 0.0 );	break;
		case 1:		m_profPanel[i]->Position().Set( 61.5f, 137.5f, 0.0 );	break;
		case 2:		m_profPanel[i]->Position().Set( 74.5f, 90.5f, 0.0 );	break;
		case 3:		m_profPanel[i]->Position().Set( 87.5f, 43.0f, 0.0 );	break;
		}

		m_profPanel[i]->State_Add();
		m_profPanel[i]->State_Add();

		m_profPanel[i]->State_GetByIndex(0).Color.w = 0.01f;
		m_profPanel[i]->State_GetByIndex(1).Color.w = 0.2f;
		m_profPanel[i]->State_GetByIndex(2).Color.w = 1.0f;

		//	label to show profile name
		sx::gui::Label* lbl = sx_new( sx::gui::Label );
		lbl->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
		lbl->SetParent( m_profPanel[i] );
		lbl->SetSize( float2( 450, 40) );
		lbl->SetAlign( GTA_LEFT );
		lbl->GetElement(0)->Color().a = 0.0f;
		lbl->GetElement(1)->Color().a = 0.8f;
		lbl->SetFont( L"Font_rob_profile.fnt" );
		lbl->Position().y = -5.0f;
		SEGAN_GUI_SET_ONCLICK( lbl, MenuProfile::OnClick );
		SEGAN_GUI_SET_ONENTER( lbl, MenuProfile::OnEnter );
		SEGAN_GUI_SET_ONEXIT( lbl, MenuProfile::OnExit );

		//	label to show total stars
		lbl = sx_new( sx::gui::Label );
		lbl->SetParent( m_profPanel[i] );
		lbl->SetSize( float2( 50, 32) );
		lbl->SetAlign( GTA_LEFT );
		lbl->GetElement(0)->Color().a = 0.0f;
		lbl->GetElement(1)->Color().a = 0.8f;
		lbl->SetFont( L"Font_tob_profileInfo.fnt" );
		lbl->Position().Set( 120.0f, -17.0f, 0.0f );

		//	label to show total people
		lbl = sx_new( sx::gui::Label );
		lbl->SetParent( m_profPanel[i] );
		lbl->SetSize( float2( 100, 32) );
		lbl->SetAlign( GTA_LEFT );
		lbl->GetElement(0)->Color().a = 0.0f;
		lbl->GetElement(1)->Color().a = 0.8f;
		lbl->SetFont( L"Font_tob_profileInfo.fnt" );
		lbl->Position().Set( 230.0f, -17.0f, 0.0f );
	}

	//	create back button
	m_goback = sx_new( sx::gui::Button );
	m_goback->SetParent( m_back );
	m_goback->SetSize( float2( 128, 32 ) );
	m_goback->Position().Set( -188.0f, -92.0f, 0.0f );
	m_goback->GetElement(0)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(0)->Color().a = 0.01f;
	m_goback->GetElement(1)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(1)->Color().a = 1.0f;
	m_goback->GetElement(2)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(2)->Color().a = 0.5f;
	SEGAN_GUI_SET_ONCLICK( m_goback, MenuProfile::OnClick );
	SEGAN_GUI_SET_ONENTER( m_goback, Menu::OnEnter );

	//	prepare profile
	String::Copy( m_profiles[0].name, 32, L"Player1" );
	String::Copy( m_profiles[1].name, 32, L"Player2" );
	String::Copy( m_profiles[2].name, 32, L"Player3" );
	String::Copy( m_profiles[3].name, 32, L"Player4" );
	
	//	load profiles
	String fileName = sx::sys::GetDocumentsFolder();
	fileName.MakePathStyle();
	fileName << L"RoadsOfBattle";
	fileName << L"/profiles.dat";

	sx::sys::FileStream	file;
	if ( file.Open( fileName, FM_OPEN_READ ) )
	{
		int version = 0;
		file.Read( &version, sizeof(version) );

		if ( version == 1 )
		{
			file.Read( &m_profIndex, sizeof(m_profIndex) );

			struct PlayerProfile_v1
			{
				WCHAR			name[32];
				int				achievements[15];
				int				upgrades[44];
				int				stars[10];
				int				level;
				int				people;
			};
			PlayerProfile_v1 profilev1[4];
			file.Read( profilev1, sizeof(profilev1) );

			memcpy( &m_profiles[0], &profilev1[0], sizeof(PlayerProfile_v1) );
			memcpy( &m_profiles[1], &profilev1[1], sizeof(PlayerProfile_v1) );
			memcpy( &m_profiles[2], &profilev1[2], sizeof(PlayerProfile_v1) );
			memcpy( &m_profiles[3], &profilev1[3], sizeof(PlayerProfile_v1) );
			
		}
		else if ( version == 2 )
		{
			file.Read( &m_profIndex, sizeof(m_profIndex) );

			struct PlayerProfile_v2
			{
				//	version 1
				WCHAR			name[32];
				int				achievements[15];
				int				upgrades[44];
				int				stars[10];
				int				level;
				int				people;

				//	version 2
				int				level_selected;
				int				level_played;
			};
			PlayerProfile_v2 profilev1[4];
			file.Read( profilev1, sizeof(profilev1) );

			memcpy( &m_profiles[0], &profilev1[0], sizeof(PlayerProfile_v2) );
			memcpy( &m_profiles[1], &profilev1[1], sizeof(PlayerProfile_v2) );
			memcpy( &m_profiles[2], &profilev1[2], sizeof(PlayerProfile_v2) );
			memcpy( &m_profiles[3], &profilev1[3], sizeof(PlayerProfile_v2) );

		}
		else if ( version == 3 )
		{
			file.Read( &m_profIndex, sizeof(m_profIndex) );
			file.Read( m_profiles, sizeof(m_profiles) );
		}
		else m_profIndex = -1;

	}
	else m_profIndex = -1;

	SyncProfileAndPlayer( true );
}

void MenuProfile::Finalize( void )
{
	Menu::Finalize();
}

void MenuProfile::ProcessInput( bool& inputHandled, float elpsTime )
{
	if ( !m_back->State_GetIndex() ) return;
	if ( inputHandled ) return;
	sx_callstack();

	Menu::ProcessInput( inputHandled, elpsTime );
	if ( !m_back->State_GetIndex() ) return;

	if ( SEGAN_KEYUP( 0, SX_INPUT_KEY_ESCAPE ) )
	{
		m_profileName->SetFocused( false );
		Hide();
		g_game->m_gui->m_main->Show();
	}

	for ( int i=0; i<4; i++ )
	{
		bool ih = false;
		m_profPanel[i]->ProcessInput( ih );
		if ( ih ) inputHandled = true;

		if ( i == m_profIndex )
			m_profPanel[i]->State_SetIndex(2);
	}
}

void MenuProfile::Update( float elpsTime )
{
	Menu::Update( elpsTime );
	if ( g_game->m_game_currentLevel ) return;

	str512 name = m_profiles[m_profIndex].name;
	if ( name == m_profileName->GetText() )
		m_rename->RemProperty( SX_GUI_PROPERTY_VISIBLE );
	else
		m_rename->AddProperty( SX_GUI_PROPERTY_VISIBLE );

	switch ( m_profIndex )
	{
	case 0:		m_delete->Position().Set( 329.0f, 174.5f, 0.0 );	break;
	case 1:		m_delete->Position().Set( 341.0f, 127.5f, 0.0 );	break;
	case 2:		m_delete->Position().Set( 354.0f, 80.5f, 0.0 );		break;
	case 3:		m_delete->Position().Set( 367.0f, 33.0f, 0.0 );		break;
	}
}

void MenuProfile::Show( void )
{
	Menu::Show();
}


void MenuProfile::Hide( void )
{
	if ( m_back->State_GetIndex() )
		SaveProfile();
	Menu::Hide();
}

void MenuProfile::OnClick( sx::gui::PControl sender )
{
	if ( sender == m_goback )
	{
		Hide();
		g_game->m_gui->m_main->Show();
	}
	else if ( sender == m_rename )
	{
		if ( !m_profileName->GetText() )
		{
			switch ( m_profIndex )
			{
			case 0:	m_profileName->SetText( L"Player1" ); break;
			case 1:	m_profileName->SetText( L"Player2" ); break;
			case 2:	m_profileName->SetText( L"Player3" ); break;
			case 3:	m_profileName->SetText( L"Player4" ); break;
			}
		}
		
		String::Copy( m_profiles[m_profIndex].name, 32, m_profileName->GetText() );
	}
	else if ( sender == m_delete )
	{
		m_profiles[m_profIndex].Reset();

		switch ( m_profIndex )
		{
		case 0:	String::Copy( m_profiles[0].name, 32, L"Player1" ); break;
		case 1:	String::Copy( m_profiles[1].name, 32, L"Player2" ); break;
		case 2:	String::Copy( m_profiles[2].name, 32, L"Player3" ); break;
		case 3:	String::Copy( m_profiles[3].name, 32, L"Player4" ); break;
		}
	}

	for ( int i=0; i<4; i++ )
	{
		if ( sender == m_profPanel[i]->GetChild(0) )
		{
			m_profIndex = i;
		}
		else
			m_profPanel[i]->State_SetIndex(0);
	}

	SyncProfileAndPlayer( true );

	msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void MenuProfile::OnEnter( sx::gui::PControl sender )
{
	if ( sender->GetType() == GUI_LABEL )
	{
		sx::gui::PanelEx* pnl = (sx::gui::PanelEx*)sender->GetParent();
		pnl->State_SetIndex(1);

		return;
	}

	msg_SoundPlay msg( true, 0, 0, L"mouseHover" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void MenuProfile::OnExit( sx::gui::PControl sender )
{
	if ( sender->GetType() == GUI_LABEL )
	{
		sx::gui::PanelEx* pnl = (sx::gui::PanelEx*)sender->GetParent();
		pnl->State_SetIndex(0);
	}
}

void MenuProfile::OnKey( sx::gui::PControl sender )
{
	if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_RETURN) )
	{
		OnClick( m_rename );
		return;
	}

	str256 profname = m_profileName->GetText();
	if ( profname.Length() > 14 )
	{
		profname.Delete(14, 200);
		m_profileName->SetText( profname );
	}

	msg_SoundPlay msg( true, 0, 0, L"typist" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void MenuProfile::SyncProfileAndPlayer( bool profileToPlayer )
{
	if ( m_profIndex < 0 )
		m_profIndex = 0;

	//	sync player and profiler
	if ( profileToPlayer )
	{
		g_game->m_player->m_profile = m_profiles[ m_profIndex ];
		g_game->m_player->m_name = g_game->m_player->m_profile.name;
	}
	else
	{
		m_profiles[ m_profIndex ] = g_game->m_player->m_profile;
	}

	for ( int i=0; i<4; i++ )
	{
		int totalStars = 0;
		for ( int j=0; j<10; j++ )
			totalStars += m_profiles[i].stars[j];

		sx::gui::PLabel( m_profPanel[i]->GetChild(0) )->SetText( m_profiles[i].name );

		str64 tmpstr; tmpstr << totalStars;
		sx::gui::PLabel( m_profPanel[i]->GetChild(1) )->SetText( tmpstr );

		tmpstr.Clear(); tmpstr << m_profiles[i].people;
		sx::gui::PLabel( m_profPanel[i]->GetChild(2) )->SetText( tmpstr );
	}

	m_profileName->SetText( m_profiles[ m_profIndex ].name );
}

void MenuProfile::SaveProfile( void )
{
	//	save profiles
	String fileName = sx::sys::GetDocumentsFolder();
	fileName.MakePathStyle();
	fileName << L"RoadsOfBattle";
	sx::sys::MakeFolder( fileName );
	fileName << L"/profiles.dat";

	sx::sys::FileStream	file;
	if ( file.Open( fileName, FM_CREATE ) )
	{
		int version = 3;
		file.Write( &version, sizeof(version) );

		file.Write( &m_profIndex, sizeof(m_profIndex) );
		file.Write( m_profiles, sizeof(m_profiles) );
	}
}

void MenuProfile::MsgProc( UINT recieverID, UINT msg, void* data )
{
	switch ( msg )
	{
	case GMT_GAME_END:
		//SaveProfile();
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
//	achievements menu
//////////////////////////////////////////////////////////////////////////
void MenuAchievements::Initialize( void )
{
	Menu::Initialize();

	m_back->SetSize( float2( 1024, 1024 ) );
	m_back->GetElement(0)->SetTextureSrc( L"gui_achievements.txr" );
	m_back->State_GetByIndex(0).Position.Set( 160.0f, -76.0f, 0.0f );
	m_back->State_GetByIndex(0).Blender.Set( 0.2f, 0.3f );
	m_back->State_GetByIndex(1).Position.Set( 90.0f, -76.0f, 0.0f );
	m_back->State_GetByIndex(1).Blender.Set( 0.03f, 0.85f );

	//	create back button
	m_goback = sx_new( sx::gui::Button );
	m_goback->SetParent( m_back );
	m_goback->SetSize( float2( 128, 32 ) );
	m_goback->Position().Set( -188.0f, -92.0f, 0.0f );
	m_goback->GetElement(0)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(0)->Color().a = 0.01f;
	m_goback->GetElement(1)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(1)->Color().a = 1.0f;
	m_goback->GetElement(2)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(2)->Color().a = 0.5f;
	SEGAN_GUI_SET_ONCLICK( m_goback, MenuAchievements::OnClick );
	SEGAN_GUI_SET_ONENTER( m_goback, Menu::OnEnter );

	//	create panel to show selected achievement
	m_image = sx_new( sx::gui::Panel );
	m_image->SetParent( m_back );
	m_image->SetSize( float2( 64, 64 ) );
	m_image->GetElement(0)->SetTextureSrc( g_game->m_achievements[0].icon );
	m_image->Position().Set( -130.0f, -21.0f, 0.0f );

	//	create label for the name
	m_name = sx_new( sx::gui::Label );
	m_name->SetParent( m_back );
	m_name->GetElement(0)->Color() = 0x00010000;
	m_name->GetElement(1)->Color() = 0xffffff00;
	m_name->SetSize( float2( 270, 30 ) );
	m_name->SetAlign( GTA_RIGHT );
	m_name->SetFont( L"font_achievements_name.fnt" );
	m_name->Position().Set( 50.0f, -5.0f, 0.0f );

	//	create label for the desc
	m_desc = sx_new( sx::gui::Label );
	m_desc->SetParent( m_back );
	m_desc->GetElement(0)->Color() = 0x00010000;
	m_desc->SetSize( float2( 290, 50 ) );
	m_desc->SetAlign( GTA_RIGHT );
	m_desc->SetFont( L"font_achievements_desc.fnt" );
	m_desc->Position().Set( 60.0f, -40.0f, 0.0f );
	m_desc->AddProperty( SX_GUI_PROPERTY_MULTILINE );
	m_desc->AddProperty( SX_GUI_PROPERTY_WORDWRAP );

	//	create achievement icons
	for ( int i=0; i<15; i++ )
	{
		m_icon[i] = sx_new( sx::gui::Panel );
		m_icon[i]->SetParent( m_back );
		m_icon[i]->SetSize( float2( 64, 64 ) );
		m_icon[i]->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
		m_icon[i]->GetElement(0)->SetTextureSrc( g_game->m_achievements[i].icon );
		m_icon[i]->SetUserTag(i);
		SEGAN_GUI_SET_ONENTER(m_icon[i], MenuAchievements::OnMouseEnter);
		SEGAN_GUI_SET_ONEXIT(m_icon[i], MenuAchievements::OnMouseExit);

		switch ( i )
		{
		case 0:	case 1:	case 2:	case 3:	case 4:
			m_icon[i]->Position().Set( -225.0f + i * 85.0f, 245.0f, 0.0f );
			break;

		case 5:	case 6:	case 7:	case 8:	case 9:
			m_icon[i]->Position().Set( -200.0f + (i-5) * 85.0f, 160.0f, 0.0f );
			break;

		case 10: case 11: case 12: case 13: case 14:
			m_icon[i]->Position().Set( -175.0f + (i-10) * 85.0f, 75.0f, 0.0f );
			break;
		}
	}
}

void MenuAchievements::ProcessInput( bool& inputHandled, float elpsTime )
{
	if ( inputHandled ) return;
	sx_callstack();

	Menu::ProcessInput( inputHandled, elpsTime );
	if ( !m_back->State_GetIndex() ) return;
	if ( SEGAN_KEYUP( 0, SX_INPUT_KEY_ESCAPE ) )
	{
		Hide();
		g_game->m_gui->m_main->Show();
	}
}

void MenuAchievements::OnClick( sx::gui::PControl sender )
{
	if ( sender == m_goback )
	{
		Hide();
		g_game->m_gui->m_main->Show();

		msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
		m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
	}
}

void MenuAchievements::OnMouseEnter( sx::gui::Control* sender )
{
	if ( !sender ) return;
	int i = sender->GetUserTag();

	m_name->SetText( g_game->m_achievements[i].name );
	m_desc->SetText( g_game->m_achievements[i].desc );
	m_image->GetElement(0)->SetTexture( m_icon[i]->GetElement(0)->GetTexture() );
	m_image->AddProperty( SX_GUI_PROPERTY_VISIBLE );

	if ( g_game->m_achievements[i].IsUnlocked() )
	{
		m_name->GetElement(1)->Color().a = 1.0f;
		m_desc->GetElement(1)->Color().a = 1.0f;
		m_image->GetElement(0)->Color().a = 1.0f;
	}
	else
	{
		m_name->GetElement(1)->Color().a = 0.2f;
		m_desc->GetElement(1)->Color().a = 0.7f;
		m_image->GetElement(0)->Color().a = 0.2f;
	}

	msg_SoundPlay msg( true, 0, 0, L"mouseHover" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void MenuAchievements::OnMouseExit( sx::gui::Control* sender )
{
	m_name->SetText( NULL );
	m_desc->SetText( NULL );
	m_image->RemProperty( SX_GUI_PROPERTY_VISIBLE );
}

void MenuAchievements::Show( void )
{
	g_game->m_player->SyncPlayerAndGame( true );

	for ( int i=0; i<15; i++ )
	{
		if ( g_game->m_achievements[i].IsUnlocked() )
			m_icon[i]->GetElement(0)->Color().a = 1.0f;
		else
			m_icon[i]->GetElement(0)->Color().a = 0.0f;
	}

	Menu::Show();
}

//////////////////////////////////////////////////////////////////////////
//	setting menu
//////////////////////////////////////////////////////////////////////////
void MenuSettings::Initialize( void )
{
	m_applyChange = true;
	Menu::Initialize();

	m_back->State_Add();
	m_back->SetSize( float2( 1024, 1024 ) );
	m_back->GetElement(0)->SetTextureSrc( L"gui_Settings.txr" );
	m_back->State_GetByIndex(0).Position.Set( 160.0f, -76.0f, 0.0f );
	m_back->State_GetByIndex(0).Blender.Set( 0.2f, 0.3f );
	m_back->State_GetByIndex(1).Position.Set( 90.0f, -76.0f, 0.0f );
	m_back->State_GetByIndex(1).Blender.Set( 0.03f, 0.85f );
	m_back->State_GetByIndex(2).Align.Set( 0.5f, 0.0f );
	m_back->State_GetByIndex(2).Position.Set( -250.0f, -76.0f, 0.0f );
	m_back->State_GetByIndex(2).Blender.Set( 0.03f, 0.85f );
	m_back->State_GetByIndex(2).Color.Set( 1.0f, 1.0f, 1.0f, 1.0f );

	//	create full screen check box 
	m_fullscreen = sx_new( sx::gui::CheckBox );
	m_fullscreen->SetParent( m_back );
	m_fullscreen->SetSize( float2(32.0f, 32.0f) );
	m_fullscreen->Position().Set( -220.0f, 210.0f, 0.0f );
	m_fullscreen->GetElement(0)->Color().a = 0.0f;
	m_fullscreen->GetElement(1)->SetTextureSrc( L"gui_settings_check.txr" );
	SEGAN_GUI_SET_ONCLICK( m_fullscreen, MenuSettings::OnClick );


	//	create scrolls
	for ( int i=0; i<6; i++ )
	{
		sx::gui::TrackBar* scroll = sx_new( sx::gui::TrackBar );
		scroll->SetParent( m_back );
		scroll->SetSize( float2(219, 32) );
		scroll->GetElement(0)->Color().a = 0.0f;
		scroll->GetElement(1)->SetTextureSrc( L"gui_settings_trackbar.txr" );
		SEGAN_GUI_SET_ONSCROLL( scroll, MenuSettings::OnScroll );
		
		switch ( i )
		{
		case 0:
			scroll->SetMax(2);
			scroll->AddProperty( SX_GUI_PROPERTY_AUTOSIZE );
			scroll->Position().Set( -105.0f, 164.0f, 0.0f );
			m_shader = scroll;
			break;

		case 1:
			scroll->SetMax(3);
			scroll->AddProperty( SX_GUI_PROPERTY_AUTOSIZE );
			scroll->Position().Set( -95.0f, 128.0f, 0.0f );
			m_shadow = scroll;
			break;

		case 2:
			scroll->SetMax(4);
			scroll->AddProperty( SX_GUI_PROPERTY_AUTOSIZE );
			scroll->Position().Set( -86.0f, 92.0f, 0.0f );
			m_reflection = scroll;
			break;


		case 3:
			scroll->Position().Set( -76.0f, 57.0f, 0.0f );
			m_music = scroll;
			break;

		case 4:
			scroll->Position().Set( -68.0f, 22.0f, 0.0f );
			m_sound = scroll;
			break;

		case 5:
			scroll->SetMin( 0.5f );
			scroll->SetMax( 2.0f );
			scroll->Position().Set( -57.0f, -12.0f, 0.0f );
			m_mouse = scroll;
			break;

		}
	}


	//	create back button
	m_goback = sx_new( sx::gui::Button );
	m_goback->SetParent( m_back );
	m_goback->SetSize( float2( 128, 32 ) );
	m_goback->Position().Set( -166.0f, -113.0f, 0.0f );
	m_goback->GetElement(0)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(0)->Color().a = 0.01f;
	m_goback->GetElement(1)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(1)->Color().a = 1.0f;
	m_goback->GetElement(2)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(2)->Color().a = 0.5f;
	SEGAN_GUI_SET_ONCLICK( m_goback, MenuSettings::OnClick );
	SEGAN_GUI_SET_ONENTER( m_goback, Menu::OnEnter );
}

void MenuSettings::ProcessInput( bool& inputHandled, float elpsTime )
{
	if ( !m_back->State_GetIndex() ) return;
	if ( inputHandled ) return;
	sx_callstack();

	if ( SEGAN_KEYUP( 0, SX_INPUT_KEY_ESCAPE ) )
	{
		Hide();
		if ( g_game->m_game_currentLevel )
			g_game->m_gui->m_pause->Show();
		else
			g_game->m_gui->m_main->Show();
	}
	Menu::ProcessInput( inputHandled, elpsTime );
	inputHandled = true;
}

void MenuSettings::Show( void )
{
	m_applyChange = false;
	m_fullscreen->Checked() = SEGAN_SET_HAS( Config::GetData()->device_CreationFlag, SX_D3D_FULLSCREEN );
	m_shader->SetValue( 2.0f - (float)Config::GetData()->shaderLevel );
	m_shadow->SetValue( 3.0f - (float)Config::GetData()->shadowLevel );
	m_reflection->SetValue( 4.0f - (float)Config::GetData()->reflectionLevel );
	m_music->SetValue( Config::GetData()->musicVolume );
	m_sound->SetValue( Config::GetData()->soundVolume );
	m_mouse->SetValue( Config::GetData()->mouseSpeed );
	m_applyChange = true;

	if ( g_game->m_game_currentLevel )
		m_back->State_SetIndex(2);
	else
		m_back->State_SetIndex(1);
}

void MenuSettings::OnClick( sx::gui::PControl sender )
{
	if ( sender == m_goback )
	{
		switch ( Config::GetData()->shaderLevel )
		{
		case 0: sx::d3d::Shader::Manager::SetShaderQuality(SQ_HIGH);	break;
		case 1: sx::d3d::Shader::Manager::SetShaderQuality(SQ_MEDIUM);	break;
		case 2: sx::d3d::Shader::Manager::SetShaderQuality(SQ_LOW);		break;
		}

		Config::SaveConfig();
		Hide();
		if ( g_game->m_game_currentLevel )
			g_game->m_gui->m_pause->Show();
		else
			g_game->m_gui->m_main->Show();
	}
	else if ( sender == m_fullscreen && m_applyChange )
	{
		if ( m_fullscreen->Checked() )
		{
			Config::GetData()->display_Size.x = sx::sys::GetDesktopWidth();
			Config::GetData()->display_Size.y = sx::sys::GetDesktopHeight();
			Config::GetData()->device_CreationFlag = SX_D3D_FULLSCREEN | SX_D3D_VSYNC;
		}
		else
		{
			Config::GetData()->display_Size.x = 1024;
			Config::GetData()->display_Size.y = 1024 * sx::sys::GetDesktopHeight() / sx::sys::GetDesktopWidth();
			Config::GetData()->device_CreationFlag = SX_D3D_VSYNC;
		}

		g_game->PostMessage( 0, GMT_SCREEN_RESIZED, NULL );

		Config::SaveConfig();
	}

	msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void MenuSettings::OnScroll( sx::gui::PControl sender )
{
	if ( !m_applyChange ) return;

	if ( sender == m_shader )
	{
		Config::GetData()->shaderLevel = 2 - int( m_shader->GetValue() );
	}
	else if ( sender == m_shadow )
	{
		Config::GetData()->shadowLevel = 3 - int( m_shadow->GetValue() );
	}
	else if ( sender == m_reflection )
	{
		Config::GetData()->reflectionLevel = 4 - int( m_reflection->GetValue() );
	}
	else if ( sender == m_music )
	{
		Config::GetData()->musicVolume = m_music->GetValue();
		sx::snd::Device::SetVolume( Config::GetData()->musicVolume, Config::GetData()->soundVolume );
	}
	else if ( sender == m_sound )
	{
		Config::GetData()->soundVolume = m_sound->GetValue();
		sx::snd::Device::SetVolume( Config::GetData()->musicVolume, Config::GetData()->soundVolume );
	}
	else if ( sender == m_mouse )
	{
		float v = m_mouse->GetValue();
		Config::GetData()->mouseSpeed = v;
		sx::io::Input::SendSignal( 0, IST_SET_SPEED, &v );
	}


	switch ( Config::GetData()->shadowLevel )
	{
	case 0:	case 1:	case 2:
		sx::core::Settings::GetOption_Shadow()->SetLevel( Config::GetData()->shadowLevel );
		sx::core::Settings::GetOption_Shadow()->AddPermission( OPT_BY_USER );
		break;
	default:
		sx::core::Settings::GetOption_Shadow()->RemPermission( OPT_BY_USER );
		break;
	}

	switch ( Config::GetData()->reflectionLevel )
	{
	case 0:	case 1:	case 2: case 3:
		sx::core::Settings::GetOption_Reflection()->SetLevel( Config::GetData()->reflectionLevel );
		sx::core::Settings::GetOption_Reflection()->AddPermission( OPT_BY_USER );
		break;
	default:
		sx::core::Settings::GetOption_Reflection()->RemPermission( OPT_BY_USER );
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
//	credits menu
//////////////////////////////////////////////////////////////////////////
void MenuCredits::Initialize( void )
{
	Menu::Initialize();
}

void MenuCredits::Finalize( void )
{
	Menu::Finalize();
}

//////////////////////////////////////////////////////////////////////////
//	exit confirmation menu
//////////////////////////////////////////////////////////////////////////
void MenuConfirmExit::Initialize( void )
{
	Menu::Initialize();

	m_back->SetSize( float2( 3000, 3000 ) );
	m_back->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_back->RemProperty( SX_GUI_PROPERTY_BLENDCHILDS );
	m_back->State_GetByIndex(0).Color.Set( 0, 0, 0, 0 );
	m_back->State_GetByIndex(1).Color.Set( 0, 0, 0, 0.7f );

	//	create confirmation form
	m_form = sx_new( sx::gui::PanelEx );
	m_form->SetParent( m_back );
	m_form->AddProperty( SX_GUI_PROPERTY_BLENDCHILDS );
	m_form->SetSize( float2( 512, 256 ) );
	m_form->GetElement(0)->SetTextureSrc( L"gui_exitBack.txr" );
	m_form->State_Add();
	m_form->State_GetByIndex(0).Color.Set( 0, 0, 0, 0 );
	m_form->State_GetByIndex(1).Color.Set( 1.0f, 1.0f, 1.0f, 1.0f );

	//	create button yes
	m_yes = sx_new( sx::gui::Button );
	m_yes->SetParent( m_back );
	m_yes->SetSize( float2(128, 32) );
	m_yes->Position().Set( -75.0f, -19.0f, 0.0f );
	m_yes->GetElement(0)->Color().a = 0;
	m_yes->GetElement(1)->SetTextureSrc( L"gui_exitYes.txr" );
	m_yes->GetElement(2)->Color().a = 0;
	SEGAN_GUI_SET_ONCLICK( m_yes, MenuConfirmExit::OnClick );
	SEGAN_GUI_SET_ONENTER( m_yes, Menu::OnEnter );

	//	create button no
	m_no = sx_new( sx::gui::Button );
	m_no->SetParent( m_back );
	m_no->SetSize( float2(128, 32) );
	m_no->Position().Set( 76.0f, -19.0f, 0.0f );
	m_no->GetElement(0)->Color().a = 0;
	m_no->GetElement(1)->SetTextureSrc( L"gui_exitNo.txr" );
	m_no->GetElement(2)->Color().a = 0;
	SEGAN_GUI_SET_ONCLICK( m_no, MenuConfirmExit::OnClick );
	SEGAN_GUI_SET_ONENTER( m_no, Menu::OnEnter );
}

void MenuConfirmExit::Finalize( void )
{
	Menu::Finalize();
}

void MenuConfirmExit::Show( bool cancelPause /*= true */ )
{
	m_yes->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_no->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_form->State_SetIndex(1);
	m_cancelPause = cancelPause;
	Menu::Show();
}

void MenuConfirmExit::Hide( void )
{
	m_yes->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_no->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_form->State_SetIndex(0);
	Menu::Hide();

	if ( g_game->m_game_currentLevel && m_cancelPause )
		g_game->m_game_paused = false;
}

void MenuConfirmExit::OnClick( sx::gui::PControl sender )
{
	if ( sender == m_no )
	{
		Hide();
	}
	else
	{
		if ( g_game->m_game_currentLevel == 0 )
		{
			g_game->m_gui->m_profile->SaveProfile();
		}

		else if ( g_game->m_gui->m_victory->IsVisible() )
		{
			g_game->PostMessage( 0, GMT_GAME_END, NULL );
			g_game->m_gui->m_victory->ApplyChangesToProfile();
		}

		else if ( g_game->m_gui->m_gameOver->IsVisible() )
		{
			g_game->m_player->SyncPlayerAndGame( true );
		}

		g_game->m_app_Closing = true;
		sx::sys::Application::Terminate();
	}

	msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void MenuConfirmExit::ProcessInput( bool& inputHandled, float elpsTime )
{
	sx_callstack();

	Menu::ProcessInput( inputHandled, elpsTime );
	if ( !m_back->State_GetIndex() ) return;
	if ( SEGAN_KEYUP( 0, SX_INPUT_KEY_ESCAPE ) )
	{
		Hide();
	}
}

//////////////////////////////////////////////////////////////////////////
//	status menu
//////////////////////////////////////////////////////////////////////////
void MenuStatus::Initialize( void )
{
	Menu::Initialize();

	BatteryStatus battry;
	sx::sys::GetBatteryStatus( battry );

	//	create status bar
	m_back->SetSize( float2( 256.0f, 64.0f) );
	m_back->State_GetByIndex(0).Align.Set( 0.5f, 0.5f );
	m_back->State_GetByIndex(0).Position.Set( -125.0f, 32.0f, 0.0f );
	m_back->State_GetByIndex(1).Align.Set( 0.5f, 0.5f );
	m_back->State_GetByIndex(1).Position.Set( -125.0f, -32.0f, 0.0f );

	if ( battry.BatteryPresent )
		m_back->GetElement(0)->SetTextureSrc( L"gui_statusBack1.txr" );
	else
		m_back->GetElement(0)->SetTextureSrc( L"gui_statusBack2.txr" );

	//	create player name on status
	m_playerName = sx_new( sx::gui::Label );
	m_playerName->SetParent( m_back );
	m_playerName->SetSize( float2(140, 20) );
	m_playerName->SetAlign( GTA_CENTER );
	m_playerName->Position().Set( -35.0f, 12.0f, 0.0f );
	m_playerName->GetElement(0)->Color().a = 0.0f;
	m_playerName->GetElement(1)->Color().a = 0.6f;
	m_playerName->SetFont( L"Font_rob_status.fnt" );

	//	create battery status
	if ( battry.BatteryPresent )
	{
		m_battery = sx_new( sx::gui::ProgressBar );
		m_battery->SetParent( m_back );
		m_battery->SetSize( float2(16, 6) );
		m_battery->Position().Set( 50.5f, 11.5f, 0.0f );
		m_battery->GetElement(0)->Color() = 0x00000100;
		m_battery->GetElement(1)->Color() = 0xaaaaaaaa;

	}
	else m_battery = NULL;

	//	create exit button
	m_exit = sx_new( sx::gui::Button );
	m_exit->SetParent( m_back );
	m_exit->SetSize( float2(64, 32) );
	m_exit->Position().Set( 94.0f, 13.0f, 0.0f );
	m_exit->GetElement(0)->Color().a = 0;
	m_exit->GetElement(1)->SetTextureSrc( L"gui_statusExit.txr" );
	m_exit->GetElement(2)->SetTextureSrc( L"gui_statusExit.txr" );
	SEGAN_GUI_SET_ONCLICK( m_exit, MenuStatus::OnClick );
	SEGAN_GUI_SET_ONENTER( m_exit, Menu::OnEnter );
}

void MenuStatus::Update( float elpsTime )
{
	Menu::Update( elpsTime );
	if ( g_game->m_game_currentLevel || !m_back->State_GetIndex() ) return;

	m_playerName->SetText( g_game->m_player->m_name );

	if ( !m_battery ) return;
	static float m_time = 10000;
	m_time += elpsTime;
	if ( m_time > 10000 )
	{
		m_time = 0;
		BatteryStatus bt;
		sx::sys::GetBatteryStatus( bt );
		m_battery->SetMax( (float)bt.MaxCapacity );
		m_battery->SetValue( (float)bt.CurCapacity );
	}
}

void MenuStatus::OnClick( sx::gui::PControl sender )
{
	g_game->m_gui->m_confirmExit->Show();
}

//////////////////////////////////////////////////////////////////////////
//	pause menu
//////////////////////////////////////////////////////////////////////////
void MenuPause::Initialize( void )
{
	Menu::Initialize();

	m_back->SetSize( float2( 1024, 1024 ) );
	m_back->GetElement(0)->SetTextureSrc( L"gui_pause.txr" );
	m_back->State_GetByIndex(0).Blender.Set( 0.2f, 0.6f );
	m_back->State_GetByIndex(0).Align.Set( -0.5f, 0.0f );
	m_back->State_GetByIndex(0).Position.Set( -600, 0, 0 );
	m_back->State_GetByIndex(1).Blender.Set( 0.2f, 0.6f );
	m_back->State_GetByIndex(1).Align.Set( -0.5f, 0.0f );
	m_back->State_GetByIndex(1).Position.Set( 100, 0, 0 );


	for ( int i=0; i<5; i++ )
	{
		sx::gui::Button* butn = sx_new( sx::gui::Button );
		butn->SetParent( m_back );
		SEGAN_GUI_SET_ONCLICK( butn, MenuPause::OnClick );
		SEGAN_GUI_SET_ONENTER( butn, Menu::OnEnter );
		butn->SetSize( float2( 256, 64 ) );
		butn->SetUserTag( i );

		switch ( i )
		{
		case 0:
			butn->Position().Set( 95.0f, 141.0f, 0 );
			butn->GetElement(1)->SetTextureSrc( L"gui_pause_resume.txr" );
			butn->GetElement(2)->SetTextureSrc( L"gui_pause_resume.txr" );
			break;
		case 1:
			butn->Position().Set( 110.0f, 80.0f, 0 );
			butn->GetElement(1)->SetTextureSrc( L"gui_pause_reset.txr" );
			butn->GetElement(2)->SetTextureSrc( L"gui_pause_reset.txr" );
			break;
		case 2:
			butn->Position().Set( 124.0f, 16.0f, 0 );
			butn->GetElement(1)->SetTextureSrc( L"gui_pause_settings.txr" );
			butn->GetElement(2)->SetTextureSrc( L"gui_pause_settings.txr" );
			break;
		case 3:
			butn->Position().Set( 146.0f, -46.0f, 0 );
			butn->GetElement(1)->SetTextureSrc( L"gui_pause_menu.txr" );
			butn->GetElement(2)->SetTextureSrc( L"gui_pause_menu.txr" );
			break;
		case 4:
			butn->Position().Set( 165.0f, -112.0f, 0 );
			butn->GetElement(1)->SetTextureSrc( L"gui_pause_quit.txr" );
			butn->GetElement(2)->SetTextureSrc( L"gui_pause_quit.txr" );
			break;
		}

		butn->GetElement(0)->Color().a = 0;
		butn->GetElement(2)->Color().a = 0.5f;
	}
}

void MenuPause::ProcessInput( bool& inputHandled, float elpsTime )
{
	if ( inputHandled ) return;
	sx_callstack();

	if ( g_game->m_mouseMode == MS_Null && g_game->m_game_currentLevel && SEGAN_KEYDOWN(0, SX_INPUT_KEY_ESCAPE) )
	{
		switch ( m_back->State_GetIndex() )
		{
		case 0:
			Show();
			break;

		case 1:
			Hide();
			break;

		case 2:
			Show();
			break;
		}
	}
	if ( !m_back->State_GetIndex() ) return;
	Menu::ProcessInput( inputHandled, elpsTime );
}

void MenuPause::Update( float elpsTime )
{
// 	if ( g_game->m_game_paused )
// 	{
// 		static float gameTime = 0;
// 		gameTime += elpsTime;
// 
// 		float mousex = Config::GetData()->display_Size.x * 0.5f - SEGAN_MOUSE_ABSX(0);
// 		float mousey = Config::GetData()->display_Size.y * 0.5f - SEGAN_MOUSE_ABSY(0);
// 
// 		m_back->Rotation().y = mousex * 0.0005f;
// 		m_back->Rotation().x = mousey * 0.0005f;
// 	}

	Menu::Update( elpsTime );
}

void MenuPause::MsgProc( UINT recieverID, UINT msg, void* data )
{
	sx_callstack_param(MenuPause::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

	Menu::MsgProc( recieverID, msg, data );

	switch ( msg )
	{
	case GMT_GAME_PAUSED:
		Show();
		break;
	}
}

void MenuPause::Show( void )
{
	if ( g_game->m_gui->m_victory->m_back->State_GetIndex() ) return;
	if ( g_game->m_gui->m_gameOver->m_back->State_GetIndex() ) return;
	if ( g_game->m_gui->m_settings->m_back->State_GetIndex() ) return;

	g_game->m_game_paused = true;
	g_game->m_mouseMode = MS_Null;

// 	if ( !g_game->m_game_paused )
// 	{
// 		g_game->PostMessage( 0, GMT_GAME_PAUSED, NULL );
// 	}
	
	Menu::Show();

	msg_SoundPlay msg( true, 0, 0, L"scrollMenu" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

}

void MenuPause::Hide( void )
{
	g_game->m_game_paused = false;
	Menu::Hide();
}

void MenuPause::OnClick( sx::gui::PControl sender )
{
	if ( !sender ) return;

	switch ( sender->GetUserTag() )
	{
	case 0:	//	resume
		{
			Hide();

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		break;

	case 1:	//  restart
		{
			g_game->Reset();
			Hide();

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		break;

	case 2: //	setting
		{
			Hide();
			g_game->m_game_paused = true;
			g_game->m_gui->m_settings->Show();

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		break;

	case 3:	//	exit to menu
		{
			Hide();
			g_game->m_game_nextLevel = 0;

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		break;

	case 4:	//	quit game
		{
			Menu::Hide();
			g_game->m_gui->m_confirmExit->Show();

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
//	victory menu
//////////////////////////////////////////////////////////////////////////
void MenuVictory::Initialize( void )
{
	m_starAdd = 0;
	m_starCount = 0;
	m_goldCounter = 0;
	m_golds = 0;
	m_peoples = 0;
	m_time = 0;
	m_shakeTime = 0;
	m_starTime = 0;
	m_goldTime = 0;
	m_setDataToUpgrade = 0;
	Menu::Initialize();

	m_back->SetSize( float2( 1024, 1024 ) );
	m_back->GetElement(0)->SetTextureSrc( L"gui_victoryBack.txr" );
	m_back->AddProperty( SX_GUI_PROPERTY_ACTIVATE );

	//	create stars
	for ( int i=0; i<3; i++ )
	{
		m_stars[i] = sx_new( sx::gui::PanelEx );
		m_stars[i]->SetParent( m_back );
		m_stars[i]->SetSize( float2( 128, 128) );
		m_stars[i]->GetElement(0)->SetTextureSrc( L"gui_victoryStar.txr" );
		switch ( i )
		{
		case 0:	m_stars[i]->State_GetByIndex(0).Position.Set( 66.0f, 125.5f, 0.0f );	break;
		case 1:	m_stars[i]->State_GetByIndex(0).Position.Set( 6.0f,  125.0f, 0.0f );	break;
		case 2:	m_stars[i]->State_GetByIndex(0).Position.Set( 126.0f,125.0f, 0.0f );	break;
		}
		m_stars[i]->State_Add();
		m_stars[i]->State_GetByIndex(0).Color.Set( 1.0f, 1.0f, 1.0f, 0.0f );
		m_stars[i]->State_GetByIndex(0).Scale.Set( 5.0f, 5.0f, 5.0f );
		m_stars[i]->State_GetByIndex(1).Blender.Set( 0.3f, 0.6f );
	}

	//	create label to show people
	m_peopleLabel = sx_new( sx::gui::Label );
	m_peopleLabel->SetParent( m_back );
	m_peopleLabel->SetSize( float2(100, 50) );
	m_peopleLabel->SetAlign( GTA_CENTER );
	m_peopleLabel->Position().Set( -71.0f, 98.0f, 0.0f );
	m_peopleLabel->GetElement(0)->Color().a = 0.0f;
	m_peopleLabel->GetElement(1)->Color() = 0xffffffaa;
	m_peopleLabel->SetFont( L"Font_victory_people.fnt" );

	//	create label to show golds
	m_goldLabel = (sx::gui::Label*)m_peopleLabel->Clone();
	m_goldLabel->SetParent( m_back );
	m_goldLabel->SetAlign(GTA_RIGHT);
	m_goldLabel->Position().Set( -152.0f, 110.0f, 0.0f );

	//	create buttons
	for ( int i=0; i<5; i++ )
	{		
		sx::gui::Button* btn = sx_new( sx::gui::Button );
		btn->SetParent( m_back );
		btn->SetSize( float2( 256, 64 ) );
		btn->GetElement(0)->Color() = D3DColor( 1.0f, 1.0f, 0.0f, 0.0f );
		btn->GetElement(1)->Color() = D3DColor( 1.0f, 1.0f, 1.0f, 1.0f );
		btn->GetElement(2)->Color() = D3DColor( 1.0f, 1.0f, 1.0f, 0.0f );
		btn->SetUserTag( i );
		switch ( i )
		{
		case 0:
			btn->GetElement(0)->SetTextureSrc( L"gui_victory_next.txr" );
			btn->GetElement(1)->SetTextureSrc( L"gui_victory_next.txr" );
			btn->GetElement(2)->SetTextureSrc( L"gui_victory_next.txr" );
			btn->Position().Set( -15.0f, 41.0f, 0.0f );
			m_nextLevel = btn;
			break;

		case 1:
			btn->GetElement(0)->SetTextureSrc( L"gui_victory_restart.txr" );
			btn->GetElement(1)->SetTextureSrc( L"gui_victory_restart.txr" );
			btn->GetElement(2)->SetTextureSrc( L"gui_victory_restart.txr" );
			btn->Position().Set( -5.0f, -13.0f, 0.0f );
			break;

		case 2:
			btn->GetElement(0)->SetTextureSrc( L"gui_victory_upgrades.txr" );
			btn->GetElement(1)->SetTextureSrc( L"gui_victory_upgrades.txr" );
			btn->GetElement(2)->SetTextureSrc( L"gui_victory_upgrades.txr" );
			btn->Position().Set( 9.0f, -69.0f, 0.0f );
			break;

		case 3:
			btn->GetElement(0)->SetTextureSrc( L"gui_victory_menu.txr" );
			btn->GetElement(1)->SetTextureSrc( L"gui_victory_menu.txr" );
			btn->GetElement(2)->SetTextureSrc( L"gui_victory_menu.txr" );
			btn->Position().Set( 23.0f, -132.0f, 0.0f );
			break;

		case 4:
			btn->GetElement(0)->SetTextureSrc( L"gui_victory_quit.txr" );
			btn->GetElement(1)->SetTextureSrc( L"gui_victory_quit.txr" );
			btn->GetElement(2)->SetTextureSrc( L"gui_victory_quit.txr" );
			btn->Position().Set( 45.0f, -190.0f, 0.0f );
			break;
		}		
		SEGAN_GUI_SET_ONCLICK( btn, MenuVictory::OnClick );
		SEGAN_GUI_SET_ONENTER( btn, Menu::OnEnter );
	}


}

void MenuVictory::Finalize( void )
{
	Hide();
	Menu::Finalize();
}

void MenuVictory::ProcessInput( bool& inputHandled, float elpsTime )
{
	sx_callstack();

	if ( !m_back->State_GetIndex() ) return;
	Menu::ProcessInput( inputHandled, elpsTime );
	inputHandled = true;
}

void MenuVictory::Update( float elpsTime )
{
	//	make a simple delay to start any things
	if ( m_time > 0 )
	{
		m_time -= elpsTime;
		Menu::Update( elpsTime );
		return;
	}

	if ( m_shakeTime > 0 )
	{
		m_shakeTime -= elpsTime;
		if ( m_shakeTime <= 0 )
		{
			m_back->Position().Set( 0.0f, 0.0f, 0.0f );
		}
		else
		{
			m_back->Position().Set( 
					m_shakeTime * ( sx::cmn::Random( 0.15f ) - sx::cmn::Random( 0.15f ) ),
					m_shakeTime * ( sx::cmn::Random( 0.15f ) - sx::cmn::Random( 0.15f ) ),
					m_shakeTime * ( sx::cmn::Random( 0.15f ) - sx::cmn::Random( 0.15f ) )
					);
		}
	}

	//	show stars
	if ( m_starCount < m_starAdd )
	{
		m_starTime += elpsTime;
		if ( m_starTime > 500 )
		{
			m_starTime = 0;
			m_starCount++;

			for ( int i=0; i<3 && i<m_starCount; i++ )
				m_stars[i]->State_SetIndex(1);

			msg_SoundPlay msg( true, 0, 0, L"victory", 1 );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

			m_shakeTime = 200;
		}
	}

	//	show golds
	if ( m_golds > 0 )
	{
		m_goldTime += elpsTime;
		if ( m_goldTime > 15 )
		{
			m_goldTime = 0;

			if ( m_golds > 11 )
			{
				m_golds -= 11;
				m_goldCounter += 11;

				if ( m_goldCounter >= 500 )
				{
					m_goldCounter = 0;

					m_peoples++;
					str64 tmpstr; tmpstr << m_peoples;
					m_peopleLabel->SetText( tmpstr );

					APL* apl = sx_new( APL );
					apl->label = (sx::gui::PLabel)m_peopleLabel->Clone();
					apl->label->SetParent( m_back );
					apl->label->GetElement(1)->Color() = 0xffff5555;
					tmpstr = L"+"; tmpstr << (m_peoples - g_game->m_player->m_people);
					apl->label->SetText( tmpstr );
					apl->label->Position() = m_peopleLabel->Position();
					apl->label->Position().y += 15.0f;
					apl->age = 1000.0f;
					m_apl.PushBack( apl );

					msg_SoundPlay msg( false, 0, 0, L"victory", 2 );
					m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
				}
			}
			else m_golds = 0;

			str64 tmpstr; tmpstr << m_golds;
			m_goldLabel->SetText( tmpstr );
			msg_SoundPlay msg( false, 0, 0, L"victory", 3 );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
	}

	//	update APL array
	for ( int i=0; i<m_apl.Count(); i++ )
	{
		APL* apl = m_apl[i];

		if ( apl->age > 0 )
		{
			apl->age -= elpsTime;
			if ( apl > 0 )
			{
				apl->label->Position().y += apl->age * elpsTime * 0.0001f;
				apl->label->GetElement(1)->Color().a = apl->age * 0.001f;
			}
			else
				apl->label->GetElement(1)->Color().a = 0;
		}
	}

	Menu::Update( elpsTime );

}

void MenuVictory::MsgProc( UINT recieverID, UINT msg, void* data )
{

}

void MenuVictory::Show( void )
{
	Entity::SetSelected( NULL );

	str64 tmpstr;

	//	compute golds
	m_golds = g_game->m_player->m_gold;
	tmpstr << m_golds;
	m_goldLabel->SetText( tmpstr );
	tmpstr.Clear();
	m_goldCounter = 0;

	//	compute people
	m_peoples = g_game->m_player->m_people;
	tmpstr << m_peoples;
	m_peopleLabel->SetText( tmpstr );

	if ( g_game->m_miniGame )
	{
		m_nextLevel->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
		m_starAdd = 0;
		m_starCount = 0;
	}
	else
	{
		m_nextLevel->AddProperty( SX_GUI_PROPERTY_ACTIVATE );

		int deadPeople = g_game->m_player->m_profile.people - g_game->m_player->m_people;
		if ( deadPeople < 3 )
			m_starAdd = 3;
		else if ( deadPeople < 7 )
			m_starAdd = 2;
		else if ( deadPeople < 13 )
			m_starAdd = 1;
		else
			m_starAdd = 0;
		m_starCount = 0;

		if ( m_starAdd )
		{
			switch ( g_game->m_difficultyLevel )
			{
			case 0: if ( m_starAdd > 1 ) m_starAdd = 1; break;
			case 1: if ( m_starAdd > 2 ) m_starAdd = 2; break;
			}
		}
	}

	for ( int i=0; i<3; i++ )
		m_stars[i]->State_SetIndex(0);

	msg_SoundPlay msg( true, 0, 0, L"victory", 0 );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

	m_time = 800;
	Menu::Show();

	m_setDataToUpgrade = true;
}

void MenuVictory::Hide( void )
{
	Menu::Hide();
	
	for ( int i=0; i<3; i++ )
		m_stars[i]->State_SetIndex(0);

	for ( int i=0; i<m_apl.Count(); i++ )
	{
		APL* apl = m_apl[i];
		apl->label->SetParent( NULL );
		sx_delete( apl->label );
		sx_delete( apl );
	}
	m_apl.Clear();
}

void MenuVictory::ApplyChangesToProfile( void )
{
	int curtLevel = g_game->m_game_currentLevel - 1;
	int nextLevel = curtLevel + 2;

	//	add people
	g_game->m_player->m_people += g_game->m_player->m_gold / 500;
	g_game->m_player->m_profile.people = g_game->m_player->m_people;

	//	save difficulty level
	if ( !g_game->m_miniGame && g_game->m_difficultyLevel > g_game->m_player->m_profile.difficulty[ curtLevel ] )
		g_game->m_player->m_profile.difficulty[ curtLevel ] = g_game->m_difficultyLevel;

	//	add stars
	if ( m_starCount > g_game->m_player->m_profile.stars[ curtLevel ] )
		g_game->m_player->m_profile.stars[ curtLevel ] = m_starCount;

	//	unlock next level
	if ( nextLevel > g_game->m_player->m_profile.level )
		g_game->m_player->m_profile.level = nextLevel;

	//	update profile
	g_game->m_gui->m_upgradePanel->GetData( g_game->m_player->m_profile.upgrades );
	g_game->m_player->SyncPlayerAndGame( false );
	g_game->m_gui->m_profile->SyncProfileAndPlayer( false );

	// save profile
	g_game->m_gui->m_profile->SaveProfile();
	g_game->m_player->SyncPlayerAndGame( true );
}

void MenuVictory::OnClick( sx::gui::PControl sender )
{
	if ( !sender ) return;

	int usertag = sender->GetUserTag();
	switch ( usertag )
	{
	case 0:		//	next level
	case 3:		//	main menu
		{
			if ( g_game->m_miniGame && usertag == 0 )
				return;

			//	notify that game has been end
			g_game->PostMessage( 0, GMT_GAME_END, NULL );

			//	apply changes
			ApplyChangesToProfile();

			//	go ahead
			g_game->m_game_nextLevel = usertag ? 0 : g_game->m_game_currentLevel + 1;

			Hide();
			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		break;

	case 1:		//	restart
		{
			Hide();
			g_game->Reset();
		
			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		break;

	case 2:		//	upgrades
		{
			int stars = 0;
			for ( int i=0; i<10; i++ )
			{
				if ( i == g_game->m_game_currentLevel-1 && g_game->m_player->m_profile.stars[i] < m_starCount )
					stars += m_starCount;
				else
					stars += g_game->m_player->m_profile.stars[i];
			}

			if ( m_setDataToUpgrade )
			{
				g_game->m_gui->m_upgradePanel->SetData( g_game->m_game_currentLevel+1, stars, g_game->m_player->m_profile.upgrades );
				m_setDataToUpgrade = false;
			}
			g_game->m_gui->m_upgradePanel->Show();

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

			return;
		}
		break;

	case 4:		//	quit
		{
			g_game->m_gui->m_confirmExit->Show( false );

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
			return;
		}
		break;
	}

	g_game->m_game_paused = false;
}


//////////////////////////////////////////////////////////////////////////
//	GameOver menu
//////////////////////////////////////////////////////////////////////////
void MenuGameOver::Initialize( void )
{
	Menu::Initialize();

	m_back->SetSize( float2( 1024, 1024 ) );
	m_back->GetElement(0)->SetTextureSrc( L"gui_gameoverBack.txr" );
	m_back->AddProperty( SX_GUI_PROPERTY_ACTIVATE );

	for ( int i=0; i<3; i++ )
	{
		sx::gui::Button* btn = sx_new( sx::gui::Button );
		btn->SetParent( m_back );
		btn->SetSize( float2( 256, 64 ) );
		btn->GetElement(0)->Color() = D3DColor( 1.0f, 1.0f, 0.0f, 0.0f );
		btn->GetElement(1)->Color() = D3DColor( 1.0f, 1.0f, 1.0f, 1.0f );
		btn->GetElement(2)->Color() = D3DColor( 1.0f, 1.0f, 1.0f, 0.5f );
		btn->SetUserTag( i );
		switch ( i )
		{
		case 0:
			btn->GetElement(0)->SetTextureSrc( L"gui_gameover_restart.txr" );
			btn->GetElement(1)->SetTextureSrc( L"gui_gameover_restart.txr" );
			btn->GetElement(2)->SetTextureSrc( L"gui_gameover_restart.txr" );
			btn->Position().Set( -16.0f, -31.0f, 0.0f );
			break;

		case 1:
			btn->GetElement(0)->SetTextureSrc( L"gui_gameover_menu.txr" );
			btn->GetElement(1)->SetTextureSrc( L"gui_gameover_menu.txr" );
			btn->GetElement(2)->SetTextureSrc( L"gui_gameover_menu.txr" );
			btn->Position().Set( 5.0f, -99.0f, 0.0f );
			break;

		case 2:
			btn->GetElement(0)->SetTextureSrc( L"gui_gameover_quit.txr" );
			btn->GetElement(1)->SetTextureSrc( L"gui_gameover_quit.txr" );
			btn->GetElement(2)->SetTextureSrc( L"gui_gameover_quit.txr" );
			btn->Position().Set( 28.0f, -167.0f, 0.0f );
			break;
		}	
		SEGAN_GUI_SET_ONCLICK( btn, MenuGameOver::OnClick );
		SEGAN_GUI_SET_ONENTER( btn, Menu::OnEnter );
	}
}

void MenuGameOver::ProcessInput( bool& inputHandled, float elpsTime )
{
	sx_callstack();

	if ( !m_back->State_GetIndex() ) return;

	Menu::ProcessInput( inputHandled, elpsTime );
	inputHandled = true;
}

void MenuGameOver::Show( void )
{
	Menu::Show();
	Entity::SetSelected( NULL );

	msg_SoundPlay msg( true, 0, 0, L"gameOver" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void MenuGameOver::OnClick( sx::gui::PControl sender )
{
	if ( !sender ) return;

	switch ( sender->GetUserTag() )
	{
	case 0:		//	reset level
		{
			g_game->Reset();

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

			Hide();
		}
		break;

	case 1:		//	main menu
		{
			g_game->m_player->SyncPlayerAndGame( true );
			g_game->m_game_nextLevel = 0;

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

			Hide();
		}
		break;

	case 2:		//	quit
		{
			g_game->m_gui->m_confirmExit->Show( false );

			msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

			return;
		}
		break;
	}

	g_game->m_game_paused = false;
}


//////////////////////////////////////////////////////////////////////////
//	Cinematic menu
//////////////////////////////////////////////////////////////////////////
void MenuCinematic::Initialize( void )
{
	Menu::Initialize();

	m_back->SetSize( float2(8000, 2000) );
	m_back->State_GetByIndex(0).Color.Set( 0, 0, 0, 0 );
	m_back->State_GetByIndex(0).Position.Set( 0, 1400.0f, 0 );
	m_back->State_GetByIndex(1).Color.Set( 0, 0, 0, 1.0f );
	m_back->State_GetByIndex(1).Position.Set( 0, 1400.0f, 0 );
	m_back->State_GetByIndex(1).Blender.Set( 0.3f, 0.6f );

	sx::gui::Panel* pnl = sx_new( sx::gui::Panel );
	pnl->SetSize( float2(8000, 2000) );
	pnl->SetParent( m_back );
	pnl->Position().Set( 0, -2800.0f, 0 );
	pnl->GetElement(0)->Color() = D3DColor(0,0,0,1);

}

void MenuCinematic::Update( float elpsTime )
{
	if ( g_game->m_game_paused )
		m_back->State_SetIndex( 1 );
	else
		m_back->State_SetIndex( 0 );
	m_back->Update( elpsTime );
}

//////////////////////////////////////////////////////////////////////////
//	MenuInfo menu
//////////////////////////////////////////////////////////////////////////
void MenuInfo::Initialize( void )
{
	Menu::Initialize();

	m_Index = -1;
	m_time = 0;
	m_delayTime = 0;

	m_back->SetSize( float2( 20000, 2000 ) );
	m_back->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	m_back->State_GetByIndex(0).Color.Set(0,0,0,0);
	m_back->State_GetByIndex(1).Color.Set(0,0,0,0.8f);
	SEGAN_GUI_SET_ONCLICK( m_back, MenuInfo::OnClick );

	m_indicator = sx_new( sx::gui::Label );
	m_indicator->SetParent( m_back );
	m_indicator->GetElement(0)->Color() = 0x00010000;
	m_indicator->SetSize( float2( 512, 64 ) );
	m_indicator->SetFont( L"Font_info_indicator.fnt" );
	m_indicator->SetAlign( GTA_CENTER );
	m_indicator->Position().y = 250.0f;

	m_title = sx_new( sx::gui::Label );
	m_title->SetParent( m_back );
	m_title->GetElement(0)->Color() = 0x00010000;
	m_title->SetSize( float2( 512, 64 ) );
	m_title->SetFont( L"font_info_title.fnt" );
	m_title->SetAlign( GTA_CENTER );
	m_title->Position().y = 230.0f;

	m_desc = sx_new( sx::gui::Label );
	m_desc->SetParent( m_back );
	m_desc->GetElement(0)->Color() = 0x00001000;
	m_desc->SetSize( float2( 1000, 200 ) );
	m_desc->SetFont( L"font_info_desc.fnt" );
	m_desc->SetAlign( GTA_CENTER );
	m_desc->Position().y = -260.0f;
	m_desc->AddProperty( SX_GUI_PROPERTY_MULTILINE );

	m_next = sx_new( sx::gui::Button );
	m_next->SetParent( m_back );
	m_next->SetSize( float2( 128, 128 ) );
	m_next->Position().Set( 400.0f, 80.0f, 0.0f );
	m_next->GetElement(0)->SetTextureSrc( L"gui_Info_next.txr" );
	m_next->GetElement(1)->SetTextureSrc( L"gui_Info_next.txr" );
	m_next->GetElement(2)->SetTextureSrc( L"gui_Info_next.txr" );
	m_next->GetElement(0)->Color().a = 0.7f;
	m_next->GetElement(2)->Color() = 0xffaaaaaa;
	SEGAN_GUI_SET_ONCLICK( m_next, MenuInfo::OnClick );
	SEGAN_GUI_SET_ONENTER( m_next, Menu::OnEnter );

	m_prev = sx_new( sx::gui::Button );
	m_prev->SetParent( m_back );
	m_prev->SetSize( float2( 128, 128 ) );
	m_prev->Position().Set( -400.0f, 80.0f, 0.0f );
	m_prev->GetElement(0)->SetTextureSrc( L"gui_Info_prev.txr" );
	m_prev->GetElement(1)->SetTextureSrc( L"gui_Info_prev.txr" );
	m_prev->GetElement(2)->SetTextureSrc( L"gui_Info_prev.txr" );
	m_prev->GetElement(0)->Color().a = 0.7f;
	m_prev->GetElement(2)->Color() = 0xffaaaaaa;
	SEGAN_GUI_SET_ONCLICK( m_prev, MenuInfo::OnClick );
	SEGAN_GUI_SET_ONENTER( m_prev, Menu::OnEnter );

}

void MenuInfo::Finalize( void )
{
	ClearTutorial();

	Menu::Finalize();
}

void MenuInfo::ProcessInput( bool& inputHandled, float elpsTime )
{
	sx_callstack();

	if (g_game->m_gui->m_pause->IsVisible() || g_game->m_gui->m_settings->IsVisible() || 
		g_game->m_gui->m_victory->IsVisible() || g_game->m_gui->m_gameOver->IsVisible() ) return;

	if ( m_delayTime > 0 )
	{
		inputHandled = true;
		return;
	}

	if ( !m_back->State_GetIndex() ) return;
	if ( inputHandled ) return;
	if ( SEGAN_KEYUP( 0, SX_INPUT_KEY_ESCAPE ) || SEGAN_KEYUP( 0, SX_INPUT_KEY_SPACE ) )
	{
		Hide();
	}
	m_back->ProcessInput( inputHandled );
}

void MenuInfo::MsgProc( UINT recieverID, UINT msg, void* data )
{
	sx_callstack_param(MenuInfo::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

	switch ( msg )
	{
	case GMT_GAME_END:
		if ( g_game->m_player->m_profile.level_played < g_game->m_game_currentLevel )
			g_game->m_player->m_profile.level_played = g_game->m_game_currentLevel;

	case GMT_GAME_RESETING:
	case GMT_LEVEL_CLEAR:
		ClearTutorial();
		m_time = 0;
		break;

	case GMT_GAME_RESET:
	case GMT_GAME_START:
		{
			//	reset button of information
			g_game->m_gui->m_goldPeople->m_info->State_SetIndex(0);

			//  load some level configuration
			String str = Game::GetLevelPath();
			if ( g_game->m_miniGame )
				str << L"config_mini.txt";
			else
				str << L"config.txt";

			Scripter script;
			script.Load( str );

			for (int i=0; i<script.GetObjectCount(); i++)
			{
				str512 tmpStr;
				if ( script.GetString(i, L"Type", tmpStr) )
				{
					if ( tmpStr == L"Tutorial" )
					{
						if ( !script.GetString(i, L"Name", tmpStr) )
							continue;

						str512 title, desc, image;
						if ( !script.GetString( i, L"title", title	) )
							continue;
						if ( !script.GetString( i, L"desc", desc	) )
							continue;
						if ( !script.GetString( i, L"image", image	) )
							continue;

						if ( g_game->m_player->m_profile.level_played < g_game->m_game_currentLevel )
						{
							AddTutorial( title, desc, image, true, false );
						}
						else
						{
							AddTutorial( title, desc, image, false, false );
						}
					}
				}
			}

		}
		break;
	}
}

void MenuInfo::Update( float elpsTime )
{
	if (g_game->m_gui->m_pause->IsVisible() || g_game->m_gui->m_settings->IsVisible() || 
		g_game->m_gui->m_victory->IsVisible() || g_game->m_gui->m_gameOver->IsVisible() ) return;

	if ( m_delayTime > 0 )
	{
		m_delayTime -= elpsTime;
		if ( m_delayTime <= 0 )
		{
			Show();

			msg_SoundPlay msg( true, 0, 0, L"info" );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		m_time = 0;
	}

	if ( m_time > 0 )
	{
		m_time -= elpsTime;
		if ( m_time < 0 )
			g_game->m_gui->m_goldPeople->m_info->State_SetIndex(0);
		else
			g_game->m_gui->m_goldPeople->m_info->State_SetIndex(1);
	}
	
	m_back->Update( elpsTime );
}

void MenuInfo::Draw( DWORD flag )
{
	sx_callstack();

	m_back->Draw( flag );
}

void MenuInfo::Show( void )
{
	Menu::Show();
	g_game->m_game_paused = true;
	m_time = 0;
}

void MenuInfo::Hide( void )
{
	Menu::Hide();
	g_game->m_game_paused = false;
}

void MenuInfo::OnClick( sx::gui::PControl sender )
{
	if ( sender == m_back )
	{
		Hide();

		msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
		m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

		return;
	}

	if ( sender == m_next )
	{
		if ( m_Index < 0 || m_Index >= m_tutorial.Count() - 1 )
		{
			Hide();
			return;
		}

		Tutorial* curr = m_tutorial[m_Index];
		curr->image->RemProperty( SX_GUI_PROPERTY_VISIBLE );

		curr = m_tutorial[++m_Index];
		curr->image->AddProperty( SX_GUI_PROPERTY_VISIBLE );
		m_title->SetText( curr->title );
		m_desc->SetText( curr->desc );

		//	update label
		{
			str512 tmp;
			tmp.Format( L"%d/%d", (m_Index+1), m_tutorial.Count() );
			m_indicator->SetText( tmp );
		}

		msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
		m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

		return;
	}

	if ( sender == m_prev )
	{
		if ( m_Index <= 0 )
		{
			Hide();
			return;
		}

		Tutorial* curr = m_tutorial[m_Index];
		curr->image->RemProperty( SX_GUI_PROPERTY_VISIBLE );

		curr = m_tutorial[--m_Index];
		curr->image->AddProperty( SX_GUI_PROPERTY_VISIBLE );
		m_title->SetText( curr->title );
		m_desc->SetText( curr->desc );

		//	update label
		{
			str512 tmp;
			tmp.Format( L"%d/%d", (m_Index+1), m_tutorial.Count() );
			m_indicator->SetText( tmp );
		}

		msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
		m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

		return;
	}

}


void MenuInfo::AddTutorial( const WCHAR* title, const WCHAR* desc, const WCHAR* image, bool showNow /*= false*/, bool settoCurrent /*= true*/ )
{
	if ( !title || !desc || !image ) return;
	if ( !title[0] || !desc[0] || !image[0] ) return;

	//	search for repetitious tutorial
	for ( int i=0; i<m_tutorial.Count(); i++ )
	{
		Tutorial* t = m_tutorial[i];
		if ( t->title == title && t->desc == desc )
		{
			str512 img = t->image->GetElement(0)->GetTextureSrc();
			if ( img == image )
				return;
		}
	}

	//	create tutorial
	Tutorial* tutor = sx_new( Tutorial );
	tutor->title = title;
	tutor->desc = desc;
	tutor->desc.Replace( L"\\n", L"\n" );
	tutor->image = sx_new( sx::gui::Panel );
	tutor->image->SetParent( m_back );
	tutor->image->SetSize( float2(1024, 1024) );
	tutor->image->GetElement(0)->SetTextureSrc( image );
	tutor->image->Position().y = 64.0f;

	if ( tutor->image->GetElement(0)->GetTexture() )
	{
		sx::d3d::Texture::Manager::LoadInThread() = true;
		tutor->image->GetElement(0)->GetTexture()->Activate();
		sx::d3d::Texture::Manager::LoadInThread() = false;
	}

	//	push to list
	m_tutorial.PushBack( tutor );

	//	set as next
	if ( settoCurrent || m_tutorial.Count() == 1 )
	{
		m_Index = m_tutorial.Count() - 1;

		Tutorial* curr = m_tutorial[m_Index];
		m_title->SetText( curr->title );
		m_desc->SetText( curr->desc );
	}

	//	update images
	//	remove current tutorial
	for ( int i=0; i<m_tutorial.Count(); i++ )
		m_tutorial[i]->image->RemProperty( SX_GUI_PROPERTY_VISIBLE );
	m_tutorial[m_Index]->image->AddProperty( SX_GUI_PROPERTY_VISIBLE );
	m_next->SetParent( NULL );
	m_next->SetParent( m_back );
	m_prev->SetParent( NULL );
	m_prev->SetParent( m_back );
	m_title->SetParent( NULL );
	m_title->SetParent( m_back );
	m_desc->SetParent( NULL );
	m_desc->SetParent( m_back );
	m_indicator->SetParent( NULL );
	m_indicator->SetParent( m_back );

	//	update label
	{
		str512 tmp;
		tmp.Format( L"%d/%d", (m_Index+1), m_tutorial.Count() );
		m_indicator->SetText( tmp );
	}

	if ( showNow )
	{
		m_delayTime = 500;
		m_time = 0;
	}
	else m_time = 60000;

	if ( m_delayTime <= 0 )
	{
		msg_SoundPlay msg( false, 0, 0, L"info" );
		m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
	}
}

void MenuInfo::ClearTutorial( void )
{
	sx_callstack();

	for ( int i=0; i<m_tutorial.Count(); i++ )
	{
		Tutorial* t = m_tutorial[i];
		t->image->SetParent( NULL );
		sx_delete_and_null(t->image);
		sx_delete_and_null(t);
	}
	m_tutorial.Clear();
	m_Index = -1;
}

//////////////////////////////////////////////////////////////////////////
//	upgrade menu 
//////////////////////////////////////////////////////////////////////////
void MenuUpgrade::Initialize( void )
{
	Menu::Initialize();
	m_points = 0;

	m_back->SetSize( float2(4000, 4000) );
	m_back->State_GetByIndex(0).Position.Set( 0.0f, 1024.0f, 0.0f );
	m_back->State_GetByIndex(0).Color.Set(0,0,0,0);
	m_back->State_GetByIndex(1).Color.Set(0,0,0,0.8f);
	m_back->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
	SEGAN_GUI_SET_ONENTER( m_back, MenuUpgrade::OnCheckEnter );

	//	create border of check boxes
	m_border = sx_new( sx::gui::Panel );
	m_border->SetParent( m_back );
	m_border->SetSize( float2( 1024, 1024 ) );
	m_border->GetElement(0)->SetTextureSrc( L"gui_menu_upgrade.txr" );
	m_border->Position().y = 150.0f;

	for ( int i=0; i<44; i++ )
	{
		sx::gui::CheckBox* ch = sx_new( sx::gui::CheckBox );
		ch->SetUserTag( i );
		ch->SetParent( m_border );
		ch->SetSize( float2( 128, 32 ) );
		ch->GetElement(0)->SetTextureSrc( L"gui_menu_upgrade_check0.txr" );
		ch->GetElement(1)->SetTextureSrc( L"gui_menu_upgrade_check1.txr" );
		SEGAN_GUI_SET_ONCLICK( ch, MenuUpgrade::OnClick );
		SEGAN_GUI_SET_ONENTER( ch, MenuUpgrade::OnCheckEnter );
		SEGAN_GUI_SET_ONEXIT( ch, MenuUpgrade::OnCheckExit );
		m_checks[i] = ch;

		sx::gui::Label* lb = sx_new( sx::gui::Label );
		lb->SetUserTag( i );
		lb->SetParent( ch );
		lb->SetSize( float2( 160, 26 ) );
		lb->SetAlign(GTA_RIGHT);
		lb->SetFont( L"font_upgrade_name.fnt" );
		lb->Position().Set( 40.0f, -2.0f, 0.0f );
		lb->GetElement(0)->Color().a = 0.0f;
		lb->GetElement(1)->Color().a = 1.0f;
		m_labels[i] = lb;

		switch ( i )
		{
		case 0: case 1: case 2: case 3: case 4:
			ch->Position().Set( -350.0f, 43 - i * 32.0f, 0.0f );	break;

		case 5: case 6: case 7: case 8: case 9: case 10:
			ch->Position().Set( -172.0f, 43 - (i-5) * 32.0f, 0.0f );	break;

		case 11: case 12: case 13: case 14: case 15: case 16:
			ch->Position().Set( 8.0f, 43 - (i-11) * 32.0f, 0.0f );		break;

		case 17: case 18: case 19: case 20: case 21: case 22:
			ch->Position().Set( 192.0f, 43 - (i-17) * 32.0f, 0.0f );	break;

		case 23: case 24: case 25: case 26: case 27:
			ch->Position().Set( 375.0f, 43 - (i-23) * 32.0f, 0.0f );	break;

		case 28: case 29: case 30: case 31:
			ch->Position().Set( -350.0f, -227.0f - (i-28) * 32.0f, 0.0f );	break;

		case 32: case 33: case 34: case 35:
			ch->Position().Set( -172.0f, -227.0f - (i-32) * 32.0f, 0.0f );	break;

		case 36: case 37: case 38:
			ch->Position().Set( 8.0f, -227.0f - (i-36) * 32.0f, 0.0f );		break;

		case 39: case 40:
			ch->Position().Set( 192.0f, -227.0f - (i-39) * 32.0f, 0.0f );	break;

		case 41: case 42: case 43:
			ch->Position().Set( 375.0f, -227.0f - (i-41) * 32.0f, 0.0f );	break;
		}
	}

	//	create a label to display number of starts
	m_stars = sx_new( sx::gui::Label );
	m_stars->SetParent( m_back );
	m_stars->SetSize( float2(128, 32) );
	m_stars->SetFont( L"font_upgrade_stars.fnt" );
	m_stars->SetAlign( GTA_RIGHT );
	m_stars->GetElement(0)->Color() = 0x00010000;
	m_stars->GetElement(1)->Color() = 0xFFFFFF00;
	m_stars->Position().Set( -130.0f, -256.0f, 0.0f );

	//	create back button
	m_goback = sx_new( sx::gui::Button );
	m_goback->SetParent( m_back );
	m_goback->SetSize( float2( 128, 32 ) );
	m_goback->Position().Set( -358.0f, -256.0f, 0.0f );
	m_goback->GetElement(0)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(0)->Color().a = 0.01f;
	m_goback->GetElement(1)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(1)->Color().a = 1.0f;
	m_goback->GetElement(2)->SetTextureSrc( L"gui_menu_back.txr" );
	m_goback->GetElement(2)->Color().a = 0.5f;
	SEGAN_GUI_SET_ONCLICK( m_goback, MenuUpgrade::OnClick );
	SEGAN_GUI_SET_ONENTER( m_goback, Menu::OnEnter );

	m_desc = sx_new( sx::gui::Label );
	m_desc->SetParent( m_back );
	m_desc->SetSize( float2( 430, 100 ) );
	m_desc->SetAlign( GTA_RIGHT );
	m_desc->SetFont( L"font_upgrade_desc.fnt" );
	m_desc->Position().Set( 220.0f, -250.0f, 0.0f );
	m_desc->GetElement(0)->Color() = 0x00000001;
	m_desc->GetElement(1)->Color() = 0xffffffaa;
	m_desc->AddProperty( SX_GUI_PROPERTY_MULTILINE );
	m_desc->AddProperty( SX_GUI_PROPERTY_WORDWRAP );
}

void MenuUpgrade::Finalize( void )
{
	Menu::Finalize();
}

void MenuUpgrade::ProcessInput( bool& inputHandled, float elpsTime )
{
	sx_callstack();

	if ( !m_back->State_GetIndex() ) return;
	m_back->ProcessInput( inputHandled );
	inputHandled = true;

	if ( SEGAN_KEYUP( 0, SX_INPUT_KEY_ESCAPE ) )
	{
		Hide();
	}
}

void MenuUpgrade::MsgProc( UINT recieverID, UINT msg, void* data )
{

}

void MenuUpgrade::Update( float elpsTime )
{
	m_back->Update( elpsTime );
}

void MenuUpgrade::Show( void )
{
	Menu::Show();

	msg_SoundPlay msg( true, 0, 0, L"upgradeShow" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
}

void MenuUpgrade::Hide( void )
{
	//	if upgrade shown in main menu save upgrades
	if ( g_game->m_game_currentLevel == 0 )
	{
		GetData( g_game->m_player->m_profile.upgrades );
		g_game->m_gui->m_profile->SaveProfile();
	}

	Menu::Hide();
}

void MenuUpgrade::OnClick( sx::gui::PControl sender )
{
	if ( !sender ) return;

	if ( sender == m_goback )
	{
		Hide();

		msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
		m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

		return;
	}

	int index = sender->GetUserTag();
	if ( index < 44 )
	{
		if ( m_checks[index]->Checked() )
		{
			if ( m_points )
			{
				m_points--;

				msg_SoundPlay msg( true, 0, 0, L"upgradePanel", 0 );
				m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
			}
			else
			{
				m_checks[index]->Checked() = false;

				msg_SoundPlay msg( true, 0, 0, L"upgradePanel", 2 );
				m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
			}
		}
		else
		{
			m_points++;

			msg_SoundPlay msg( true, 0, 0, L"upgradePanel", 1 );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
	}

	str64 tmpstr; tmpstr << m_points;
	m_stars->SetText( tmpstr );
}

void MenuUpgrade::OnCheckEnter( sx::gui::PControl sender )
{
	if ( !sender ) return;

	if ( sender == m_back )
	{
		m_desc->RemProperty( SX_GUI_PROPERTY_VISIBLE );
		return;
	}

	int index = sender->GetUserTag();

	m_checks[index]->GetElement(0)->Color() = 0xffffbb00;
	m_checks[index]->GetElement(1)->Color() = 0xffffbb00;

	m_labels[index]->GetElement(1)->Color() = 0xffffffff;

	m_desc->SetText( g_game->m_upgrades.desc[index] );
	m_desc->AddProperty( SX_GUI_PROPERTY_VISIBLE );

	msg_SoundPlay msg( true, 0, 0, L"mouseHover" );
	m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

}

void MenuUpgrade::OnCheckExit( sx::gui::PControl sender )
{
	int index = sender->GetUserTag();

	m_checks[index]->GetElement(0)->Color() = 0xffffffff;
	m_checks[index]->GetElement(1)->Color() = 0xffffffff;

	m_labels[index]->GetElement(1)->Color() = 0xaaffffff;

}

void MenuUpgrade::SetData( const int level, const int points, const int* upgrades )
{
	m_points = points;

// 	m_points = 0;
// 	for ( int i=0; i<10; i++ )
// 		m_points += g_game->m_player->m_profile.stars[i];

	for ( int i=0; i<44; i++ )
	{
		m_labels[i]->SetText( g_game->m_upgrades.name[i] );

		if ( upgrades[i] )
		{
			m_checks[i]->Checked() = true;
			m_points--;
		}
		else m_checks[i]->Checked() = false;

		if ( level < g_game->m_upgrades.unlock[i] )
		{
			m_checks[i]->RemProperty( SX_GUI_PROPERTY_ENABLE );
			m_checks[i]->GetElement(0)->Color() = 0x44ffffff;
			m_labels[i]->GetElement(1)->Color() = 0x44ffffff;
		}
		else
		{
			m_checks[i]->AddProperty( SX_GUI_PROPERTY_ENABLE );
			m_checks[i]->GetElement(0)->Color() = 0xffffffff;
			m_labels[i]->GetElement(1)->Color() = 0xaaffffff;
		}
	}

	str64 tmpstr; tmpstr << m_points;
	m_stars->SetText( tmpstr );

}

void MenuUpgrade::GetData( int* upgrades )
{
	for ( int i=0; i<44; i++ )
	{
		if ( m_checks[i]->Checked() )
		{
			upgrades[i] = 1;
		}
		else upgrades[i] = 0;
	}
}
