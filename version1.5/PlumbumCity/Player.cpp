#include "Player.h"
#include "Entity.h"
#include "Game.h"
#include "GameGUI.h"
#include "GameConfig.h"
#include "Scripter.h"
#include "EntityManager.h"
#include "ProjectileManager.h"
#include "projectile_Bullet.h"
#include "Projectile_BOMB.h"
#include "com_weapon_MachineGun.h"
#include "Mechanic_TowerCreate.h"
#include "Mechanic_TowerEdit.h"
#include "Mechanic_EnemyWave.h"
#include "Mechanic_PA_Stunner.h"
#include "Mechanic_PA_Repair.h"
#include "Mechanic_PA_BomberMan.h"
#include "Mechanic_PA_Predator.h"
#include "Mechanic_PA_Trap.h"
#include "Mechanic_PA_DeathRain.h"
#include "Mechanic_PA_GoldenTowers.h"
#include "Mechanic_MT_Machinegun.h"
#include "Mechanic_MT_Sniper.h"
#include "Mechanic_PA_DeathZone.h"


Player::Player( void ):	m_gold(500), m_people(100), m_energy(100), m_fastCoolDown(1.0f)
{
	sx_callstack();

	m_name = L"Player1";
	m_camera_RTS.m_Activate = true;
	SyncPlayerAndGame( true );
}

Player::~Player( void )
{
	sx_callstack();

	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		sx_delete_and_null( m_Mechanics[i] );
	}
	m_Mechanics.Clear();
}

void Player::Initialize( void )
{
	sx_callstack();

	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		m_Mechanics[i]->Initialize();
	}
}

void Player::Finalize( void )
{
	sx_callstack();

	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		m_Mechanics[i]->Finalize();
	}
}

void Player::ProcessInput( bool& inputHandled, float elpsTime )
{
	sx_callstack();

	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		m_Mechanics[i]->ProcessInput( inputHandled, elpsTime );
	}

	if ( !g_game->m_gamePaused && g_game->m_mouseMode != MS_ManualTower )
	{
		m_camera_Pause.m_Activate = false;
		m_camera_MBL.ProseccInput( inputHandled, elpsTime );

		m_camera_RTS.m_Activate = !m_camera_MBL.m_Activate;
		m_camera_RTS.ProseccInput( inputHandled, elpsTime );

		if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_C) )
		{
			m_camera_MBL.m_Activate = !m_camera_MBL.m_Activate;
		}
	}

	//  TEST
#if 1
	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_R) )
	{
		inputHandled = true;
		g_game->Reset();
	}
#endif

	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_L) )
	{
		inputHandled = true;
		m_profile.level = 10;
		m_profile.curGameMode = 2;
		for ( int i=0; i<10; i++ )
			m_profile.stars[i] = 3;
	}

	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_P) )
	{
		inputHandled = true;
		m_profile.people = 1000;
		m_people = 1000;
	}

	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_INSERT) )
	{
		switch ( Config::GetData()->display_Debug )
		{
		case 0:	Config::GetData()->display_Debug = 1; break;
		case 1:	Config::GetData()->display_Debug = 2; break;
		case 2:	Config::GetData()->display_Debug = 3; break;
		case 3:	Config::GetData()->display_Debug = 0; break;
		}
		 
	}

}

void Player::Update( float elpsTime )
{
	sx_callstack();

	if ( !g_game->m_currentLevel || g_game->m_gamePaused )
	{
		static float gameTime = 0;
		gameTime += elpsTime;

		m_camera_Pause.m_Activate = true;
		m_camera_RTS.m_Activate = false;
		m_camera_MBL.m_Activate = false;

		float mousex = Config::GetData()->display_Size.x * 0.5f - SEGAN_MOUSE_ABSX(0);
		float mousey = Config::GetData()->display_Size.y * 0.5f - SEGAN_MOUSE_ABSY(0);
		m_camera_Pause.m_Phi = g_game->m_player->m_camera_RTS.m_Phi + mousex * 0.0001f + sin( 6.0f * cos ( gameTime * 0.0001f ) ) * 0.1f;
		m_camera_Pause.m_Tht = 0.8f + mousey * 0.0001f + sin( 3.0f * cos ( gameTime * 0.0001f ) ) * 0.1f;

		m_camera_Pause.Update( elpsTime );

		for (int i=0; i<m_Mechanics.Count(); i++)
		{
			m_Mechanics[i]->Update( elpsTime * Config::GetData()->game_speed );
		}

		return;
	}
	

	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		m_Mechanics[i]->Update( elpsTime * Config::GetData()->game_speed );
	}

	if ( g_game->m_mouseMode == MS_ManualTower )
	{
		m_camera_Pause.m_Activate = false;
		m_camera_RTS.m_Activate = false;
		m_camera_MBL.m_Activate = false;
	}
	else
	{
		m_camera_MBL.Attach( Entity::GetSelected() );
		m_camera_MBL.Update(elpsTime);
		m_camera_RTS.Update(elpsTime);
		m_camera_Pause = g_game->m_player->m_camera_RTS;
	}
}

void Player::MsgProc( UINT recieverID, UINT msg, void* data )
{
	sx_callstack_param(Player::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

	//  check message
	switch (msg)
	{
	case GMT_GAME_RESETING:
		{
			m_camera_MBL.Attach(NULL);
			m_fastCoolDown = 1.0f;
		}
		break;

	case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
		{						//////////////////////////////////////////////////////////////////////////
			//  load some level configuration
			String str = Game::GetLevelPath();
			if ( g_game->m_miniGame )
				str << L"config_mini.txt";
			else
			{
				switch ( g_game->m_gameMode )
				{
				case 0 : str << L"config_default.txt"; break;
				case 1 : str << L"config_warrior.txt"; break;
				case 2 : str << L"config_legend.txt"; break;
				}
			}

			Scripter script;
			script.Load( str );

			for (int i=0; i<script.GetObjectCount(); i++)
			{
				str512 tmpStr;
				if ( script.GetString(i, L"Type", tmpStr) )
				{
					if ( tmpStr == L"Player" )
					{
						if ( !script.GetString(i, L"Name", tmpStr) )
							continue;

						if ( tmpStr == L"NORMAL" )
						{
							script.GetInteger( i, L"gold", m_gold );

							script.GetInteger( i, L"energy", m_energy );

							switch ( g_game->m_difficultyMode )
							{
							case 0:	
								g_game->m_difficultyValue = 0.7f;
								script.GetFloat( i, L"difficulty_norm", g_game->m_difficultyValue );
								break;
							case 1:	
								g_game->m_difficultyValue = 0.85f;
								script.GetFloat( i, L"difficulty_hard", g_game->m_difficultyValue );
								break;
							case 2:	
								g_game->m_difficultyValue = 1.0f;
								script.GetFloat( i, L"difficulty_insane", g_game->m_difficultyValue );
								break;
							}
						}
					}
				}
			}
			m_fastCoolDown = 1.0f;
		}
		break;

	case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
		{						//////////////////////////////////////////////////////////////////////////
			if ( g_game->m_currentLevel ) 
				m_profile.level_selected = g_game->m_currentLevel;

			//  clear mechanics
			ClearMechanincs();

			//  load some level configuration
			String str = Game::GetLevelPath();
			if ( g_game->m_miniGame )
				str << L"config_mini.txt";
			else
			{
				switch ( g_game->m_gameMode )
				{
				case 0 : str << L"config_default.txt"; break;
				case 1 : str << L"config_warrior.txt"; break;
				case 2 : str << L"config_legend.txt"; break;
				}
			}

			Scripter script;
			script.Load( str );

			for (int i=0; i<script.GetObjectCount(); i++)
			{
				str512 tmpStr;
				if ( script.GetString(i, L"Type", tmpStr) )
				{
					if ( tmpStr == L"Player" )
					{
						if ( !script.GetString( i, L"Name", tmpStr ) )
							continue;

						if ( tmpStr == L"NORMAL" )
						{
							script.GetInteger( i, L"gold", m_gold );

							script.GetInteger( i, L"energy", m_energy );

							switch ( g_game->m_difficultyMode )
							{
							case 0:	
								g_game->m_difficultyValue = 0.7f;
								script.GetFloat( i, L"difficulty_norm", g_game->m_difficultyValue );
								break;
							case 1:	
								g_game->m_difficultyValue = 0.85f;
								script.GetFloat( i, L"difficulty_hard", g_game->m_difficultyValue );
								break;
							case 2:	
								g_game->m_difficultyValue = 1.0f;
								script.GetFloat( i, L"difficulty_insane", g_game->m_difficultyValue );
								break;
							}
						}
					}
					else if ( tmpStr == L"Camera" )
					{
						if ( !script.GetString(i, L"Name", tmpStr) )
							continue;

						if ( tmpStr == L"DEFAULT" )
						{
							m_camera_RTS.m_minRad = 20.0f;
							m_camera_RTS.m_phiThreshold = 2.0f;
							script.GetFloat(i, L"minX", m_camera_RTS.m_minX);
							script.GetFloat(i, L"minZ", m_camera_RTS.m_minZ);
							script.GetFloat(i, L"maxX", m_camera_RTS.m_maxX);
							script.GetFloat(i, L"maxZ", m_camera_RTS.m_maxZ);
							//script.GetFloat(i, L"minR", m_camera_RTS.m_minRad);
							script.GetFloat(i, L"rotationThreshold", m_camera_RTS.m_phiThreshold);
							m_camera_RTS.Reload();
						}
					}
					else if ( tmpStr == L"PowerAttack" )
					{
						if ( !script.GetString(i, L"Name", tmpStr) )
							continue;

						if ( tmpStr == L"Repair" )
							m_Mechanics.PushBack( sx_new( GM::Mechanic_PA_Repair ) );
						else if ( tmpStr == L"Stunner" )
							m_Mechanics.PushBack( sx_new( GM::Mechanic_PA_Stunner ) );
						else if ( tmpStr == L"DeathRain" )
							m_Mechanics.PushBack( sx_new( GM::Mechanic_PA_DeathRain ) );
						else if ( tmpStr == L"GoldenTowers" )
							m_Mechanics.PushBack( sx_new( GM::Mechanic_PA_GoldenTowers ) );
						else if ( tmpStr == L"BomberMan" )
							m_Mechanics.PushBack( sx_new( GM::Mechanic_PA_BomberMan ) );
						else if ( tmpStr == L"LandMine" )
							m_Mechanics.PushBack( sx_new( GM::Mechanic_PA_Trap ) );
						else if ( tmpStr == L"DeathZone" )
							m_Mechanics.PushBack( sx_new( GM::Mechanic_PA_DeathZone ) );
						else if ( tmpStr == L"Predator" )
							m_Mechanics.PushBack( sx_new( GM::Mechanic_PA_Predator ) );
					}
					else if ( tmpStr == L"ManualTower" )
					{
						if ( !script.GetString(i, L"Name", tmpStr) )
							continue;

						if ( tmpStr == L"Machinegun" )
							m_Mechanics.Insert( 0, sx_new( GM::Mechanic_MT_Machinegun ) );
						else if ( tmpStr == L"Sniper" )
							m_Mechanics.Insert( 0, sx_new( GM::Mechanic_MT_Sniper ) );
					}
				}
			}

			m_Mechanics.PushBack( sx_new( GM::Mechanic_TowerCreate ) );
			m_Mechanics.PushBack( sx_new( GM::Mechanic_TowerEdit ) );

			for ( int i=0; i<m_Mechanics.Count(); i++ )
				m_Mechanics[i]->Initialize();

			m_fastCoolDown = 1.0f;
		}
		break;	//	GMT_LEVEL_LOAD

	case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
		{						//////////////////////////////////////////////////////////////////////////
			//  send message to mechanics
			for (int i=0; i<m_Mechanics.Count(); i++)
				m_Mechanics[i]->MsgProc( recieverID, msg, data );

			//  clear mechanics
			ClearMechanincs();
			m_fastCoolDown = 1.0f;
		}
		break;	//	GMT_LEVEL_CLEAR

	case GMT_GAME_START:		/////////////////////////////////////////////////    START GAME
		{						//////////////////////////////////////////////////////////////////////////
			SyncPlayerAndGame( true );	
		}
		break;	//	GMT_GAME_START

	case GMT_GAME_END:			/////////////////////////////////////////////////    END GAME
		{						//////////////////////////////////////////////////////////////////////////
			ClearMechanincs();
			m_fastCoolDown = 1.0f;
		}
		break;	//	GMT_GAME_END

	case GMT_I_DIED:
		if ( data )
		{
			Entity* pEntity = (Entity*)data;
			if ( pEntity->m_partyCurrent == PARTY_ENEMY )
			{
				if ( pEntity->m_health.imax > 0 )
				{
					m_gold += pEntity->m_cost[0] + int( g_game->m_upgrades.general_gold_income * (float)pEntity->m_cost[0] );

					g_game->m_achievements[0].AddValue();
					g_game->m_achievements[1].AddValue();
					g_game->m_achievements[2].AddValue();
					g_game->m_achievements[3].AddValue();

				}
				else
				{
					if ( pEntity->m_curAttack.killPeople > 0 )
					{
						m_people -= pEntity->m_curAttack.killPeople;

						//	play sound of people murdered
						msg_SoundPlay msg( true, 0, 0, L"people", 0 );
						g_game->m_gui->m_main->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
					}
					else
					{
						//	play sound of crowd celebrating
						msg_SoundPlay msg( true, 0, 0, L"people", 1 );
						g_game->m_gui->m_main->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
					}
				}

				if ( m_people < 1 )
				{
					m_people = 0;
					g_game->m_gamePaused = true;
					g_game->m_gui->m_gameOver->Show();
				}
			}
			else
			{

			}
		}
		break;
	}

	//  send message to mechanics
	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		m_Mechanics[i]->MsgProc( recieverID, msg, data );
	}
}

void Player::SyncPlayerAndGame( bool playerToGame )
{
	//	sync player statistics
	if ( playerToGame )
	{
		m_people = m_profile.people;

		m_name = m_profile.name[0] ? m_profile.name : L"Player1";

		for ( int i=0; i<15; i++ )
			g_game->m_achievements[i].value = m_profile.achievements[i];

		g_game->m_upgrades.Reset();
		for ( int i=0; i<44; i++ )
		{
			if ( m_profile.upgrades[i] )
				g_game->m_upgrades.value[i] = g_game->m_upgrades.defaults[i];
		}
	}
	else
	{
		m_profile.people = m_people;

		String::Copy(	m_profile.name, 32, m_name );

		for ( int i=0; i<15; i++ )
			m_profile.achievements[i] = g_game->m_achievements[i].value;
	}

	if ( m_profile.level <  1 ) m_profile.level = 1;
	if ( m_profile.level > 10 ) m_profile.level = 10;

}

void Player::ClearMechanincs( void )
{
	sx_callstack();
	

	for (int i=0; i<m_Mechanics.Count(); i++)
		m_Mechanics[i]->Finalize();
	for (int i=0; i<m_Mechanics.Count(); i++)
		sx_delete_and_null( m_Mechanics[i] );
	m_Mechanics.Clear();

}

