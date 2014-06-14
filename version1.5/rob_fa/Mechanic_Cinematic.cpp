#include "Mechanic_Cinematic.h"
#include "Game.h"
#include "Player.h"
#include "Scripter.h"
#include "Entity.h"
#include "GameGUI.h"
#include "EntityManager.h"
#include "GameStrings.h"



//////////////////////////////////////////////////////////////////////////
//	implementation of presenter
//////////////////////////////////////////////////////////////////////////
FirstPresents::FirstPresents( void ) : m_index(0), m_time(0), m_maxtime(5000.0f), m_soundNode(0), m_soundVolume(0)
{

}

FirstPresents::~FirstPresents()
{
	for ( int i=0; i<m_list.Count(); i++ )
	{
		sx::gui::Destroy( m_list[i] );
	}
}

void FirstPresents::AddPresents( const WCHAR* texture, const float size, const wchar* story, const float x, const float y )
{
	sx::gui::Panel* panel = (sx::gui::Panel*)sx::gui::Create( GUI_PANEL );
	panel->SetSize( float2( size, size ) );
	panel->GetElement(0)->SetTextureSrc( texture );
	panel->GetElement(0)->Color().a = 0.0f;

	if ( story )
		create_label( panel, FONT_25_OUTLINE, GTA_LEFT, story, size/3, size/3, x * size/2, y * size/2, 0 );

	m_list.PushBack( panel );
}

void FirstPresents::Update( float elpstime )
{
	{
		MSG msg;
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	if ( m_index > m_list.Count() ) return;

	sx::io::Input::Update( elpstime );
	if (SEGAN_KEYUP(0, SX_INPUT_KEY_SPACE) || 
		SEGAN_KEYUP(0, SX_INPUT_KEY_ESCAPE) || 
		SEGAN_KEYUP(0, SX_INPUT_KEY_RETURN) || 
		SEGAN_KEYUP(0, SX_INPUT_KEY_MOUSE_LEFT) || 
		SEGAN_KEYUP(0, SX_INPUT_KEY_MOUSE_RIGHT) )
	{
		m_index = -10;
		return;
	}

	if ( m_index < -5 )
	{
		if ( m_index == -10 )
		{
			m_time = 0;
			m_index = -9;
		}
		m_time += elpstime;

		for ( int i=0; i<m_list.Count(); i++ )
		{
			sx::gui::Control* pc = m_list[i];
			pc->GetElement(0)->Color().a -= elpstime * 0.001f;
			if ( pc->GetElement(0)->Color().a < 0.0f )
				pc->GetElement(0)->Color().a = 0.0f;

			if ( pc->GetChildCount() )
				pc->GetChild(0)->GetElement(1)->Color().a = pc->GetElement(0)->Color().a;

			pc->Update( elpstime );
		}

		if ( m_soundNode )
		{
			m_soundVolume -= elpstime * 0.001f;
			msg_SoundPlay sndplayfade( false, 0, 0, L"intro", 0, m_soundVolume );
			m_soundNode->MsgProc( MT_SOUND_PLAY, &sndplayfade );
		}

		if ( m_time > 1000 )
			m_index = m_list.Count() + 1;

		return;
	}

	m_time += elpstime;
	if ( m_time > m_maxtime )
	{
		m_time = 0;
		++m_index;
	}

	if ( m_index == ( m_list.Count() - 1 ) )
	{
		if ( m_time > ( m_maxtime - 1000.0f ) )
			m_index = -10;
	}

	for ( int i=0; i<m_list.Count(); i++ )
	{
		sx::gui::Control* pc = m_list[i];

		if ( m_index == i )
		{
			if ( m_time > 1000.0f )
			{
				pc->GetElement(0)->Color().a += elpstime * 0.001f;
				if ( pc->GetElement(0)->Color().a > 1.0f )
					pc->GetElement(0)->Color().a = 1.0f;
			}
 		}
		else
		{
			pc->GetElement(0)->Color().a -= elpstime * 0.001f;
			if ( pc->GetElement(0)->Color().a < 0.0f )
				pc->GetElement(0)->Color().a = 0.0f;
		}

		if ( pc->GetChildCount() )
			pc->GetChild(0)->GetElement(1)->Color().a = pc->GetElement(0)->Color().a;

		pc->Update( elpstime );
	}


}

void FirstPresents::Draw( void )
{
	if ( !sx::core::Renderer::CanRender() ) return;
	sx::core::Renderer::Begin();

	sx::d3d::UI3D::ReadyToDebug( D3DColor(0,0,0,0) );
	sx::d3d::Device3D::Clear_Target(0);
	sx::d3d::Device3D::RS_Alpha( SX_MATERIAL_ALPHABLEND );

	for ( int i=0; i<m_list.Count(); i++ )
	{
		m_list[i]->Draw(0);
	}

	sx::core::Renderer::End();
}

bool FirstPresents::Presenting( void )
{
	return ( m_index <= m_list.Count() && m_time < m_maxtime );
}


namespace GM
{

	Mechanic_Cinematic::Mechanic_Cinematic( void ) : Mechanic()
	{
	}

	Mechanic_Cinematic::~Mechanic_Cinematic( void )
	{
	}

	void Mechanic_Cinematic::Initialize( void )
	{

	}

	void Mechanic_Cinematic::Finalize( void )
	{

	}

	void Mechanic_Cinematic::ProcessInput( bool& inputHandled, float elpsTime )
	{

	}

	void Mechanic_Cinematic::Update( float elpsTime )
	{

	}

	void Mechanic_Cinematic::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		switch (msg)
		{
		case GMT_LEVEL_LOADED:		/////////////////////////////////////////////////    LOAD LEVEL
			{						//////////////////////////////////////////////////////////////////////////

				if ( g_game->m_game_currentLevel == 1 && g_game->m_player->m_profile.level_played < 1 )
				{
					msg_SoundPlay sndplay( true, 0, 0, L"intro", 0 );
					m_soundNode->MsgProc( MT_SOUND_PLAY, &sndplay );

					FirstPresents *presents = sx_new( FirstPresents );
					presents->AddPresents( L"gui_intro_level1_0.txr", SEGAN_VP_WIDTH, g_game->m_strings->Get(220), -0.6f, 0.05f );
					presents->AddPresents( L"gui_intro_level1_1.txr", SEGAN_VP_WIDTH, g_game->m_strings->Get(221), -0.5f, 0.1f );
					presents->AddPresents( L"gui_intro_level1_2.txr", SEGAN_VP_WIDTH, g_game->m_strings->Get(222),  0.5f, 0.1f );
					presents->m_maxtime = 15000.0f;
					presents->m_soundNode = m_soundNode;
					presents->m_soundVolume = sndplay.volume;

					float initTime = sx::sys::GetSysTime();
					float elpsTime = 0;
					while ( presents->Presenting() )
					{
						// calculate elapsed time
						elpsTime = sx::sys::GetSysTime() - initTime;
						initTime = sx::sys::GetSysTime();
						presents->Update( elpsTime );
						presents->Draw();
					}

					msg_SoundStop sndstop( false, 0, L"intro" );
					m_soundNode->MsgProc( MT_SOUND_STOP, &sndstop );

					sx_delete_and_null( presents );
				}
			}
			break;	//	GMT_LEVEL_LOAD

		case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
			{						//////////////////////////////////////////////////////////////////////////

			}
			break;	//	GMT_LEVEL_CLEAR

		case GMT_GAME_START:		/////////////////////////////////////////////////    START GAME
			{						//////////////////////////////////////////////////////////////////////////

			}
			break;	//	GMT_GAME_START

		case GMT_GAME_END:			/////////////////////////////////////////////////    END GAME
			{						//////////////////////////////////////////////////////////////////////////
#if USE_8_LEVELS
				if ( g_game->m_game_currentLevel == 8 )
#else
				if ( g_game->m_game_currentLevel == 10 )
#endif
				{
					msg_SoundPlay sndplay( true, 0, 0, L"intro", 0 );
					m_soundNode->MsgProc( MT_SOUND_PLAY, &sndplay );

					FirstPresents *presents = sx_new( FirstPresents );
					presents->AddPresents( L"gui_intro_level10_0.txr", SEGAN_VP_WIDTH, g_game->m_strings->Get(223), -0.6f, 0.05f );
					presents->AddPresents( L"gui_intro_level10_1.txr", SEGAN_VP_WIDTH, g_game->m_strings->Get(224),  0.5f, -0.4f );
					presents->m_maxtime = 12000.0f;
					presents->m_soundNode = m_soundNode;
					presents->m_soundVolume = sndplay.volume;

					float initTime = sx::sys::GetSysTime();
					float elpsTime = 0;
					while ( presents->Presenting() )
					{
						// calculate elapsed time
						elpsTime = sx::sys::GetSysTime() - initTime;
						initTime = sx::sys::GetSysTime();
						presents->Update( elpsTime );
						presents->Draw();
					}

					msg_SoundStop sndstop( false, 0, L"intro" );
					m_soundNode->MsgProc( MT_SOUND_STOP, &sndstop );

					sx_delete_and_null( presents );
				}
			}
			break;	//	GMT_GAME_END

		case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
			{						//////////////////////////////////////////////////////////////////////////

			}
			break;	//	GMT_GAME_RESET
		}
	}

} // namespace GM
