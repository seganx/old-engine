#include "Mechanic_PA_BomberMan.h"
#include "Game.h"
#include "Player.h"
#include "Scripter.h"
#include "Entity.h"
#include "GameGUI.h"
#include "EntityManager.h"


//////////////////////////////////////////////////////////////////////////
//  external variables
//////////////////////////////////////////////////////////////////////////
extern int	powerAttack_count;


namespace GM
{

	Mechanic_PA_BomberMan::Mechanic_PA_BomberMan( void ) 
		: Mechanic()
		, m_Cost(100)
		, m_Time(0)
		, m_coolTime(10)
		, m_Index( powerAttack_count++ )
		, m_Bomber(0)
	{
	}

	Mechanic_PA_BomberMan::~Mechanic_PA_BomberMan( void )
	{
		
	}

	void Mechanic_PA_BomberMan::Initialize( void )
	{
		m_panelEx = sx_new( sx::gui::PanelEx );
		m_panelEx->SetSize( float2(64,64) );
		m_panelEx->SetParent( g_game->m_gui->m_powerAttaks );
		m_panelEx->State_GetByIndex(0).Color.w = 0.5f;
		m_panelEx->State_Add();
		m_panelEx->State_GetByIndex(1).Blender.y = 1.0f;
		m_panelEx->State_GetByIndex(1).Scale.Set(0.95f, 0.95f, 1);

		m_panelEx->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
		SEGAN_GUI_SET_ONCLICK( m_panelEx, Mechanic_PA_BomberMan::OnGUIClick );

		m_progBar = sx_new( sx::gui::ProgressBar );
		m_progBar->SetSize( float2(64,64) );
		m_progBar->SetParent( m_panelEx );
		m_progBar->AddProperty( SX_GUI_PROPERTY_PROGRESSCIRCLE );
		m_progBar->GetElement(0)->Color().a = 0;
		m_progBar->SetMax(1.0f);
	}

	void Mechanic_PA_BomberMan::Finalize( void )
	{
		// gui will deleted by their parents
		sx_delete_and_null( m_panelEx );

	}

	void Mechanic_PA_BomberMan::ProcessInput( bool& inputHandled, float elpsTime )
	{
		if ( !g_game->m_game_currentLevel || g_game->m_game_paused )
			return;

		if ( m_Time < m_coolTime )
		{
			m_panelEx->State_SetIndex(1);
			m_progBar->AddProperty( SX_GUI_PROPERTY_VISIBLE );
		}
		else
		{
			m_panelEx->State_SetIndex(0);
			m_progBar->RemProperty( SX_GUI_PROPERTY_VISIBLE );
		}


		int key = SX_INPUT_KEY_1 + m_Index;
		if ( SEGAN_KEYDOWN(0, key) || SEGAN_KEYHOLD(0, key) )
		{
			OnGUIClick( m_progBar );
			inputHandled = true;
		}

		m_panelEx->ProcessInput( inputHandled, 0 );


	}

	void Mechanic_PA_BomberMan::Update( float elpsTime )
	{
		if ( !g_game->m_game_currentLevel || g_game->m_game_paused )
			return;

		if ( m_Time < m_coolTime )
		{
			m_Time += elpsTime * 0.001f;
			m_progBar->SetValue( m_Time/m_coolTime );
		}
		else m_progBar->SetValue( 1 );

		//  update hint of buttons
		str1024 strHint;
		if ( m_Hint.Text() )
			strHint.Format(m_Hint.Text(), m_Cost, g_game->m_player->m_gold);
		m_panelEx->SetHint( strHint );
		m_progBar->SetHint( strHint );		

		if ( g_game->m_player->m_gold >= m_Cost )
		{
			m_panelEx->State_GetCurrent().Color.y = 1.0f;
			m_panelEx->State_GetCurrent().Color.z = 1.0f;
			m_progBar->GetElement(1)->Color().g = 1.0f;
			m_progBar->GetElement(1)->Color().b = 1.0f;
		}
		else
		{
			m_panelEx->State_GetCurrent().Color.y = 0.0f;
			m_panelEx->State_GetCurrent().Color.z = 0.0f;
			m_progBar->GetElement(1)->Color().g = 0.0f;
			m_progBar->GetElement(1)->Color().b = 0.0f;
		}
	}

	void Mechanic_PA_BomberMan::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		switch (msg)
		{
		case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
			{						//////////////////////////////////////////////////////////////////////////
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
						if ( tmpStr == L"PowerAttack" )
						{
							if ( !script.GetString(i, L"Name", tmpStr) )
								continue;

							if ( tmpStr == L"BomberMan" )
							{
								script.GetInt(i, L"cost", m_Cost);
								script.GetFloat(i, L"coolTime", m_coolTime);
								m_Time = m_coolTime;

								if ( script.GetString(i, L"hint", tmpStr) )
									m_Hint = tmpStr.Text();
								else
									m_Hint.Clear();
								m_Hint.Replace(L"\\n", L"\n");

								if ( script.GetString(i, L"entityType", tmpStr) )
								{
									m_Bomber = (Entity*)EntityManager::GetTypeByName(tmpStr);

									if ( script.GetString(i, L"startNode", tmpStr) )
									{
										sx::core::ArrayPNode nodelist;
										sx::core::Scene::GetNodesByName(tmpStr, nodelist);
										if ( nodelist.Count()>0 )
											m_startNode = nodelist[0];
									}

									if ( script.GetString(i, L"endNode", tmpStr) )
									{
										sx::core::ArrayPNode nodelist;
										sx::core::Scene::GetNodesByName(tmpStr, nodelist);
										if ( nodelist.Count()>0 )
											m_endNode = nodelist[0];
									}
								}
							}
						}
					}
				}
			}
			break;	//	GMT_LEVEL_LOAD

		case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
			{						//////////////////////////////////////////////////////////////////////////

			}
			break;	//	GMT_LEVEL_CLEAR

		case GMT_GAME_START:		/////////////////////////////////////////////////    START GAME
			{						//////////////////////////////////////////////////////////////////////////
				m_Time = m_coolTime;

				float left, top = -11.0f;
				switch ( m_Index )
				{
				case 0:		left = -30.0f;	break;
				case 1:		left = 27.0f;	break;
				case 2:		left = 84.0f;	break;
				case 3:		left = 140.0f;	break;
				case 4:		left = 198.0f;	break;
				default:	left = -30 + m_Index * 80.0f;
				}
				m_panelEx->State_GetByIndex(0).Position.Set( left, top, 0.0f );
				m_panelEx->State_GetByIndex(1).Position.Set( left, top, 0.0f );
				m_panelEx->State_GetBlended().Position.Set( left, top, 0.0f );
				m_panelEx->State_SetIndex( 0 );

			}
			break;	//	GMT_GAME_START

		case GMT_GAME_END:			/////////////////////////////////////////////////    END GAME
			{						//////////////////////////////////////////////////////////////////////////

			}
			break;	//	GMT_GAME_END

		case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
			{						//////////////////////////////////////////////////////////////////////////
				m_Time = 0;
				m_Bomber = NULL;
				MsgProc(0, GMT_LEVEL_LOAD, 0);
			}
			break;	//	GMT_GAME_RESET
		}
	}

	void Mechanic_PA_BomberMan::OnGUIClick( sx::gui::PControl Sender )
	{
		if ( !m_Bomber ) return;

		if ( m_Time >= m_coolTime && g_game->m_player->m_gold >= m_Cost )
		{
			m_Time = 0;
			g_game->m_player->m_gold -= m_Cost;

			//  create enemy
			Entity* entity = m_Bomber->Clone();

			entity->SetLevel(0);

			//  set position
			if ( m_startNode )
				entity->SetPosition( m_startNode->GetPosition_world() );

			//  set enemy mission
			Mission m;
			m.status = Mission::MS_ACTIVE;

			if ( entity->m_curAttackLevel.maxRange<0.1f || entity->m_curAttackLevel.rate<0.001 || !entity->m_curAttackLevel.bullet[0] )
				m.flag = MISSION_GOTO_NODE;
			else
				m.flag = MISSION_GOTO_NODE | MISSION_KILL_ENEMY;

			m.data0 = m_startNode;
			m.data1 = m_endNode;
			entity->GetBrain()->AddMission(m);

			m.flag = MISSION_SUICIDE;
			entity->GetBrain()->AddMission(m);

			//  add entity to game
			EntityManager::AddEntity( entity );

		}
		else
		{
			// TODO :
			// play a sound to say that spell is not ready
			msg_SoundPlay msg( true, 0, 0, L"powerAttack", 0 );
			g_game->m_gui->m_main->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
	}

} // namespace GM
