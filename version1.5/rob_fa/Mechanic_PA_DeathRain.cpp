#include "Mechanic_PA_DeathRain.h"
#include "Game.h"
#include "Player.h"
#include "Scripter.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Projectile.h"
#include "GameGUI.h"
#include "ProjectileManager.h"

//////////////////////////////////////////////////////////////////////////
//  external variables
//////////////////////////////////////////////////////////////////////////
extern int	powerAttack_count;


struct HotZone
{
	int					bombs;			// number of bombs;
	float				ratetime;		// times
	sx::core::Node*		node;			// node in the scene

	HotZone(void): bombs(0), ratetime(0), node(null) {}

	~HotZone(void)
	{
		sx::core::Scene::DeleteNode(node);
		node = NULL;
	}
};
Array<HotZone*>		arrayBombRain;



namespace GM
{

	Mechanic_PA_DeathRain::Mechanic_PA_DeathRain( void ) 
		: Mechanic()
		, m_Cost(100)
		, m_Time(0)
		, m_coolTime(10)
		, m_index( powerAttack_count++ )
		, m_hotNode(0)
	{
		sx_callstack();

	}

	Mechanic_PA_DeathRain::~Mechanic_PA_DeathRain( void )
	{
		sx_callstack();

		powerAttack_count--;
	}

	void Mechanic_PA_DeathRain::Initialize( void )
	{
		sx_callstack();

		m_panelEx = sx_new( sx::gui::PanelEx );
		m_panelEx->SetSize( float2(64,64) );
		m_panelEx->SetParent( g_game->m_gui->m_powerAttaks );
		m_panelEx->State_Add();
		m_panelEx->State_GetByIndex(1).Blender.y = 1.0f;
		m_panelEx->State_GetByIndex(1).Scale.Set(0.95f, 0.95f, 1);
		m_panelEx->GetElement(0)->SetTextureSrc( L"gui_pa_deathrain.txr" );

		m_panelEx->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
		SEGAN_GUI_SET_ONCLICK( m_panelEx, Mechanic_PA_DeathRain::OnGUIClick );

		m_progBar = sx_new( sx::gui::ProgressBar );
		m_progBar->SetSize( float2(64,64) );
		m_progBar->SetParent( m_panelEx );
		m_progBar->AddProperty(SX_GUI_PROPERTY_PROGRESSCIRCLE);
		m_progBar->GetElement(0)->Color().a = 0;
		m_progBar->SetMax(1.0f);
		m_progBar->GetElement(1)->SetTextureSrc( L"gui_pa_ring.txr" );
	}

	void Mechanic_PA_DeathRain::Finalize( void )
	{
		sx_callstack();

		// gui will deleted by their parents
		sx_delete_and_null( m_panelEx );

		//  hot nodes in the scene will deleted by scene manager
		for ( int i=0; i<arrayBombRain.Count(); i++ )
			sx_delete( arrayBombRain[i] );
		arrayBombRain.Clear();

		sx_delete_and_null( m_hotNode );

	}

	void Mechanic_PA_DeathRain::ProcessInput( bool& inputHandled, float elpsTime )
	{
		if ( NotInGame() || g_game->m_mouseMode == MS_CreateTower )
			return;
		sx_callstack();

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

		//  check create mode
		if ( g_game->m_mouseMode == MS_CreateHotzone )
		{
			Entity::SetSelected(NULL);

			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_ESCAPE) || SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_RIGHT) )
			{
				g_game->m_mouseMode = MS_Null;
				sx::core::Scene::RemoveNode( m_hotNode );
				inputHandled = true;
				return;
			}
			if ( inputHandled ) return;

			//  compute position
			float3 hotPos;
			sx::math::Ray ray = sx::core::Renderer::GetCamera()->GetRay( SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0) );
			Plane p; p.Make(sx::math::VEC3_ZERO, sx::math::VEC3_Y);
			ray.Intersect_Plane(p, &hotPos);
			
			sx::core::PMesh mesh = (sx::core::PMesh)m_hotNode->GetMemberByIndex(0);
			if ( mesh )
				mesh->GetActiveMaterial()->SetFloat( 0 , m_Attack.maxRange );

			m_hotNode->SetPosition(hotPos);
			sx::core::Scene::AddNode( m_hotNode );
			CreateBombZone(hotPos);
			inputHandled = true;
		}
		else
		{
			sx::core::Scene::RemoveNode( m_hotNode );
			int key = SX_INPUT_KEY_1 + m_index;
			if ( SEGAN_KEYDOWN(0, key) || SEGAN_KEYHOLD(0, key) )
			{
				OnGUIClick( m_progBar );
				inputHandled = true;
			}
		}

		m_panelEx->ProcessInput( inputHandled, 0 );
	}

	void Mechanic_PA_DeathRain::Update( float elpsTime )
	{
		if ( NotInGame() )	return;

		sx_callstack();

		if ( m_Time < m_coolTime )
		{
			m_Time += elpsTime * 0.001f;
			m_progBar->SetValue( m_Time/m_coolTime );
		}
		else m_progBar->SetValue( 1 );

		//  update hint of buttons
		str1024 strHint;
		if ( m_Hint.Text() )
			strHint.Format(m_Hint.Text(), (m_index+1), m_Cost, g_game->m_player->m_gold);
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

		//  set mesh properties for all hot zone
		for (int i=0; i<arrayBombRain.Count(); i++)
		{
			HotZone* hotzone = arrayBombRain[i];

			//	spill bomb on enemies
			float rateTime = hotzone->ratetime;
			rateTime -= elpsTime;
			if ( rateTime < 0 )
			{
				hotzone->ratetime = 1000.0f / m_Attack.rate;
				
				//  spill bomb
 				Projectile* proj = ProjectileManager::CreateProjectileByTypeName( m_Attack.bullet );
 				if ( proj )
  				{
					proj->m_targetPos = hotzone->node->GetPosition_world();

					static sx::core::ArrayPNode_inline nodes(512); nodes.Clear();
					sx::core::Scene::GetNodesByArea( proj->m_targetPos, m_Attack.maxRange + 5.0f, nodes, NMT_PATHNODE );
					if ( nodes.Count() )
						proj->m_targetPos = nodes[ sx::cmn::Random( nodes.Count()-1 ) ]->GetPosition_world();

					nodes.Clear();
					sx::core::Scene::GetNodesByArea( proj->m_targetPos, 5.0f, nodes, NMT_PATHNODE );
					if ( nodes.Count() )
					{
						proj->m_targetPos = sx::math::VEC3_ZERO;
						for (int n=0; n<nodes.Count(); n++ )
							proj->m_targetPos += nodes[n]->GetPosition_world();
						proj->m_targetPos /= (float)nodes.Count();
					}

					float randomvalue = 1.0f;
					proj->m_targetPos.x	+= sx::cmn::Random(randomvalue) - sx::cmn::Random(randomvalue);
					proj->m_targetPos.z	+= sx::cmn::Random(randomvalue) - sx::cmn::Random(randomvalue);

					proj->m_killParty	= PARTY_ENEMY;
					proj->m_target		= NULL;
					proj->m_speed		= 0.5f;
					proj->m_tag			= EAT_Apocalypto;

					proj->m_attack		= m_Attack;
					proj->m_attack.targetType = GMT_BOTH;

					proj->m_pos		= proj->m_targetPos;
					proj->m_pos.y	= 60.0f;

  					proj->m_dir.Set( 0.0f, -1.0f, 0.0f );
   					ProjectileManager::AddProjectile(proj);
  				}

				//  update bomb count
 				hotzone->bombs--;
 				if ( hotzone->bombs < 1 )
 				{
					sx_delete( arrayBombRain[i] );
 					arrayBombRain.RemoveByIndex(i);
 					i--;
 					continue;
 				}

			}
			else hotzone->ratetime = rateTime;

		}

	}

	void Mechanic_PA_DeathRain::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		sx_callstack_param(Mechanic_PA_DeathRain::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

		switch (msg)
		{
		case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
			{						//////////////////////////////////////////////////////////////////////////
				sx_delete_and_null( m_hotNode );

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
					if ( script.GetString(i, L"Type", tmpStr) && tmpStr == L"PowerAttack" )
					{
						if ( !script.GetString(i, L"Name", tmpStr) )
							continue;

						if ( tmpStr == L"DeathRain" )
						{
							script.GetInteger(i, L"cost", m_Cost);
							script.GetFloat(i, L"coolTime", m_coolTime);
							m_Time = m_coolTime;

							if ( script.GetString(i, L"hint", tmpStr) )
								m_Hint = tmpStr.Text();
							else
								m_Hint.Clear();
							m_Hint.Replace(L"\\n", L"\n");

							m_Attack.targetType = GMT_BOTH;
							script.GetFloat(i, L"physicalDamage",		m_Attack.physicalDamage);
							script.GetFloat(i, L"physicalArmor",		m_Attack.physicalArmor);
							script.GetFloat(i, L"electricalDamage",		m_Attack.electricalDamage);
							script.GetFloat(i, L"electricalArmor",		m_Attack.electricalArmor);
							script.GetFloat(i, L"splashRadius",			m_Attack.splashRadius);
							script.GetFloat(i, L"stunValue",			m_Attack.stunValue);
							script.GetFloat(i, L"stunTime",				m_Attack.stunTime);
							script.GetFloat(i, L"fireRate",				m_Attack.rate);
							script.GetFloat(i, L"bombRadius",			m_Attack.maxRange);
							script.GetFloat(i, L"bombCount",			m_Attack.minRange);

							if ( script.GetString(i, L"bullet", tmpStr) )
								String::Copy( m_Attack.bullet, 64, tmpStr );

 							if ( script.GetString(i, L"node", tmpStr) )
 							{
 								PStream pfile = NULL;
 								if ( sx::sys::FileManager::File_Open(tmpStr, SEGAN_PACKAGENAME_DRAFT, pfile) )
 								{
 									m_hotNode = sx_new( sx::core::Node );
 									m_hotNode->Load(*pfile);
 									sx::sys::FileManager::File_Close(pfile);
 
 									float f = 0;
 									m_hotNode->MsgProc( MT_ACTIVATE, &f );
 									m_hotNode->RemProperty(SX_NODE_SELECTABLE);
 
 									sx::core::PMesh mesh = (sx::core::PMesh)m_hotNode->GetMemberByIndex(0);
 									mesh->SetActiveMaterial(1);
 									mesh->GetActiveMaterial()->SetFloat(0, m_Attack.maxRange/*m_Attack.splashRaduis*/);
 								}
 							}

							m_coolTime				-= g_game->m_upgrades.deathrain_cooltime;
							m_Attack.minRange		+= g_game->m_upgrades.deathrain_count;
							m_Attack.maxRange		+= g_game->m_upgrades.deathrain_range;
							m_Attack.splashRadius	+= g_game->m_upgrades.deathrain_splash;


						}	//if ( tmpStr == L"DeathRain" )
					}
				}
			}
			break;	//	GMT_LEVEL_LOAD

		case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
			{						//////////////////////////////////////////////////////////////////////////
				for ( int i=0; i<arrayBombRain.Count(); i++ )
					sx_delete( arrayBombRain[i] );
				arrayBombRain.Clear();
			}
			break;	//	GMT_LEVEL_CLEAR

		case GMT_GAME_START:		/////////////////////////////////////////////////    START GAME
			{						//////////////////////////////////////////////////////////////////////////
				m_Time = m_coolTime;

				float left, top = -11.0f;
				switch ( m_index )
				{
				case 0:		left = -30.0f;	break;
				case 1:		left = 27.0f;	break;
				case 2:		left = 84.0f;	break;
				case 3:		left = 140.0f;	break;
				case 4:		left = 198.0f;	break;
				default:	left = -30 + m_index * 80.0f;
				}
				m_panelEx->State_GetByIndex(0).Position.Set( left, top, 0.0f );
				m_panelEx->State_GetByIndex(1).Position.Set( left, top, 0.0f );
				m_panelEx->State_GetBlended().Position.Set( left, top, 0.0f );
				m_panelEx->State_SetIndex( 0 );

				if ( m_Attack.rate < 0.001f )
					m_Attack.rate = 1;

#if USE_STEAM_SDK
				g_game->m_steam.CallAchievement( EAT_Wrath_Of_Battle, ESC_OnStart );
#else
				if ( g_game->m_achievements[EAT_Wrath_Of_Battle].value < g_game->m_achievements[EAT_Wrath_Of_Battle].range )
					g_game->m_achievements[EAT_Wrath_Of_Battle].value = 0;
#endif
			}
			break;	//	GMT_GAME_START

		case GMT_GAME_RESETING:
			{
				for ( int i=0; i<arrayBombRain.Count(); i++ )
					sx_delete( arrayBombRain[i] );
				arrayBombRain.Clear();
			}
			break;

		case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
			{						//////////////////////////////////////////////////////////////////////////
				m_Time = 0;
				sx_delete_and_null( m_hotNode );
				MsgProc(0, GMT_LEVEL_LOAD, 0);
			}
			break;	//	GMT_GAME_RESET
		}
	}

	void Mechanic_PA_DeathRain::OnGUIClick( sx::gui::PControl Sender )
	{
		if ( !m_hotNode || !m_Attack.bullet[0] || !m_Attack.minRange ) return;

		static float lasttime = 0;
		const float  newtime = sx::sys::GetSysTime();
		if ( newtime > lasttime && newtime - lasttime < 1000 ) return;
		lasttime = newtime;

		sx_callstack();

		if ( m_Time >= m_coolTime && g_game->m_player->m_gold >= m_Cost )
		{
			g_game->m_mouseMode = MS_CreateHotzone;

			msg_SoundPlay msg( false, 0, 0, L"powerAttack", 4 );
			g_game->m_gui->m_main->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
		else
		{
			// TODO :
			// play a sound to say that spell is not ready
			msg_SoundPlay msg( false, 0, 0, L"powerAttack", 0 );
			g_game->m_gui->m_main->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
		}
	}

	void Mechanic_PA_DeathRain::CreateBombZone( const float3& pos )
	{
		sx_callstack();

		if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) )
		{
			static sx::core::ArrayPNode_inline nodes(512); nodes.Clear();
			sx::core::Scene::GetNodesByArea( pos, m_Attack.maxRange, nodes, NMT_PATHNODE );
			if ( !nodes.Count() )
			{
				//	say to player by a sound
				msg_SoundPlay msg( false, 0, 0, L"powerAttack", 0 );
				g_game->m_gui->m_main->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
				return;
			}

			sx::core::Scene::RemoveNode( m_hotNode );

			m_Time = 0;
			g_game->m_player->m_gold -= m_Cost;

			HotZone* hotzone = sx_new( HotZone );
			hotzone->node = m_hotNode->Clone();
  			hotzone->node->SetPosition( pos );
			hotzone->ratetime = 0;
			hotzone->bombs = int( m_Attack.minRange );
 			arrayBombRain.PushBack( hotzone );
			sx::core::Scene::AddNode( hotzone->node );

			g_game->m_mouseMode = MS_Null;

#if USE_STEAM_SDK
			g_game->m_steam.CallAchievement( EAT_Wrath_Of_Battle, ESC_InPlay );
#else
			g_game->m_achievements[EAT_Wrath_Of_Battle].AddValue();
#endif
		}
	}


} // namespace GM
