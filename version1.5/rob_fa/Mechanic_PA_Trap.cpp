#include "Mechanic_PA_Trap.h"
#include "Game.h"
#include "Player.h"
#include "Scripter.h"
#include "Entity.h"
#include "GameGUI.h"
#include "EntityManager.h"
#include "GameStrings.h"

//////////////////////////////////////////////////////////////////////////
//  external variables
//////////////////////////////////////////////////////////////////////////
extern int	powerAttack_count;



namespace GM
{
	struct Trap
	{
		int					count;			//	count of attack
		sx::core::Node*		node;			//	node in the scene
		float3				pos;			//	position of trap
		float				coolTime;		//	coll time after each attack
		float				deadTime;		//	the time to delete trap
		float				time;

		Trap( void ): count(0), node(0), pos(0,0,0), coolTime(3), deadTime(1), time(9999) {}

		~Trap(void)
		{
			sx::core::Scene::DeleteNode(node);
		}
	};

	Mechanic_PA_Trap::Mechanic_PA_Trap( void ) 
		: Mechanic()
		, m_cost(100)
		, m_time(0)
		, m_coolTime(10)
		, m_index( powerAttack_count++ )
		, m_node(0)
		, m_pos(0,0,0)
	{
		sx_callstack();

		m_attack.actionCount = 1;
	}

	Mechanic_PA_Trap::~Mechanic_PA_Trap( void )
	{
		sx_callstack();

		powerAttack_count--;
	}

	void Mechanic_PA_Trap::Initialize( void )
	{
		sx_callstack();

		m_panelEx = sx_new( sx::gui::PanelEx );
		m_panelEx->SetSize( float2(64,64) );
		m_panelEx->SetParent( g_game->m_gui->m_powerAttaks );
		m_panelEx->State_Add();
		m_panelEx->State_GetByIndex(1).Blender.y = 1.0f;
		m_panelEx->State_GetByIndex(1).Scale.Set(0.95f, 0.95f, 1);
		m_panelEx->GetElement(0)->SetTextureSrc( L"gui_pa_trap.txr" );

		m_panelEx->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
		SEGAN_GUI_SET_ONCLICK( m_panelEx, Mechanic_PA_Trap::OnGUIClick );

		m_progBar = sx_new( sx::gui::ProgressBar );
		m_progBar->SetSize( float2(64,64) );
		m_progBar->SetParent( m_panelEx );
		m_progBar->AddProperty(SX_GUI_PROPERTY_PROGRESSCIRCLE);
		m_progBar->GetElement(0)->Color().a = 0;
		m_progBar->SetMax(1.0f);
		m_progBar->GetElement(1)->SetTextureSrc( L"gui_pa_ring.txr" );
	}

	void Mechanic_PA_Trap::Finalize( void )
	{
		sx_callstack();

		// gui will deleted by their parents
		sx_delete_and_null( m_panelEx );

		//  hot nodes in the scene will deleted by scene manager
		ClearTraps();

		sx_delete_and_null( m_node );
	}

	void Mechanic_PA_Trap::ProcessInput( bool& inputHandled, float elpsTime )
	{
		if ( NotInGame() || g_game->m_mouseMode == MS_CreateTower )
			return;

		sx_callstack();

		if ( m_time < m_coolTime )
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
		if ( g_game->m_mouseMode == MS_CreateTrap )
		{
			Entity::SetSelected(NULL);

			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_ESCAPE) || SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_RIGHT) )
			{
				g_game->m_mouseMode = MS_Null;
				sx::core::Scene::RemoveNode( m_node );
				inputHandled = true;
				return;
			}
			if ( inputHandled ) return;

			//  compute position
			sx::math::Ray ray = sx::core::Renderer::GetCamera()->GetRay( SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0) );
			Plane p; p.Make(sx::math::VEC3_ZERO, sx::math::VEC3_Y);
			ray.Intersect_Plane( p, &m_pos );

			CreateTrap();
			inputHandled = true;
		}
		else
		{
			sx::core::Scene::RemoveNode( m_node );
			int key = SX_INPUT_KEY_1 + m_index;
			if ( SEGAN_KEYDOWN(0, key) || SEGAN_KEYHOLD(0, key) )
			{
				OnGUIClick( m_progBar );
				inputHandled = true;
			}
		}

		m_panelEx->ProcessInput( inputHandled, 0 );

	}

	void Mechanic_PA_Trap::Update( float elpsTime )
	{
		if ( NotInGame() )	return;

		sx_callstack();

		if ( m_time < m_coolTime )
		{
			m_time += elpsTime * 0.001f;
			m_progBar->SetValue( m_time/m_coolTime );
		}
		else m_progBar->SetValue( 1 );

		//  update hint of buttons
		{
			GameString* title = g_game->m_strings->Get( 571 );
			GameString* desc = g_game->m_strings->Get( 572 );
			if ( title && desc )
			{
				swprintf_s( title->text, 512, title->base, (m_index + 1) );
				swprintf_s( desc->text, 512, desc->base, m_cost, g_game->m_player->m_gold );

				m_panelEx->SetHint( L"571\n572" );
				m_progBar->SetHint( L"571\n572" );
			}
		}

		if ( g_game->m_player->m_gold >= m_cost )
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

		//  update all traps
		for (int i=0; i<m_traps.Count(); i++)
		{
			Trap* trap = m_traps[i];

			//	verify the dead of trap
			if ( trap->count < 1 )
			{
				trap->deadTime -= elpsTime * 0.001f;
				if ( trap->deadTime < 0 )
				{
					sx_delete_and_null( trap );
					m_traps.RemoveByIndex(i);
					i--;
					continue;
				}

				continue;
			}

			//	verify the cool time
			trap->time += elpsTime * 0.001f;
			if ( trap->time < trap->coolTime ) continue;

			//  find an enemy near the trap
			float minDis = FLT_MAX;
			Entity* enemy = NULL;
			static sx::core::ArrayPNode enemies(128);	enemies.Clear();
			sx::core::Scene::GetNodesByArea( trap->pos, 0.5f, enemies, NMT_ALL, PARTY_ENEMY );
			for (int j=0; j<enemies.Count(); j++)
			{
				sx::core::PNode enNode = enemies[j];
				if ( enNode->GetUserData() )
				{
					Entity* en = static_cast<Entity*>( enNode->GetUserData() );
					if ( en->m_health.icur > 0 && en->m_move.type == GMT_GROUND )
					{
						float dis = trap->pos.Distance_sqr( en->m_pos );
						if ( dis < 5.0f && dis < minDis )
						{
							enemy = en;
							minDis = dis;
						}
					}
				}
			}

			//	damage to the enemy
			if ( enemy )
			{
				msgDamage damage( 
					m_attack.physicalDamage, 
					m_attack.physicalArmor, 
					m_attack.electricalDamage, 
					m_attack.electricalArmor, 
					m_attack.stunValue, 
					m_attack.stunTime, 
					NULL,
					EAT_Death_Trap
					);
				enemy->MsgProc( GMT_DAMAGE, &damage );

				msg_Animator msgAnim( SX_ANIMATOR_PLAY, SX_ANIMATOR_LOOP, 0, -1, -1, -1, 0 );
				trap->node->MsgProc( MT_ANIMATOR, &msgAnim );

				sx::core::Node* fireNode = NULL;
				if ( trap->node->GetChildByName( L"_onfire", fireNode ) )
				{
					msg_Particle msgPar( SX_PARTICLE_SPRAY, SX_PARTICLE_LOOP );
					fireNode->MsgProc( MT_PARTICLE, &msgPar );

					msg_SoundPlay msgSnd( false );
					fireNode->MsgProc( MT_SOUND_PLAY, &msgSnd );
				}

				trap->time = 0;
				trap->count--;
			}
		}
	}

	void Mechanic_PA_Trap::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		sx_callstack_param(Mechanic_PA_Trap::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

		switch (msg)
		{
		case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
			{						//////////////////////////////////////////////////////////////////////////
				sx_delete_and_null( m_node );

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

						if ( tmpStr == L"Trap" )
						{
							script.GetInt(i, L"cost", m_cost);
							script.GetFloat(i, L"coolTime", m_coolTime);
							m_time = m_coolTime;

							if ( script.GetString(i, L"node", tmpStr) )
							{
								PStream pfile = NULL;
								if ( sx::sys::FileManager::File_Open(tmpStr, SEGAN_PACKAGENAME_DRAFT, pfile) )
								{
									m_node = sx_new( sx::core::Node );
									m_node->Load(*pfile);
									sx::sys::FileManager::File_Close(pfile);

									float f = 0;
									m_node->MsgProc( MT_ACTIVATE, &f );
									m_node->RemProperty( SX_NODE_SELECTABLE );
								}
							}

							script.GetFloat(i, L"physicalDamage",	m_attack.physicalDamage );
							script.GetFloat(i, L"electricalDamage", m_attack.electricalDamage );
							script.GetFloat(i, L"stunTime",			m_attack.stunTime );
							script.GetFloat(i, L"stunValue",		m_attack.stunValue );
							script.GetFloat(i, L"actionTime",		m_attack.actionTime );
							script.GetInt(i, L"damageCount",	m_attack.actionCount );
							script.GetInt(i, L"actionCount",	m_attack.actionCount );

							m_coolTime					-= g_game->m_upgrades.trap_cooltime;
							m_attack.actionCount		= m_attack.actionCount + int(g_game->m_upgrades.trap_count+0.5f);
							m_attack.physicalDamage		+= g_game->m_upgrades.trap_damage;
							m_attack.electricalDamage	+= g_game->m_upgrades.trap_damage;

						}	//	if ( tmpStr == L"Trap" )
					}
				}
			}
			break;	//	GMT_LEVEL_LOAD

		case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
			{						//////////////////////////////////////////////////////////////////////////
				ClearTraps();
			}
			break;	//	GMT_LEVEL_CLEAR

		case GMT_GAME_START:		/////////////////////////////////////////////////    START GAME
			{						//////////////////////////////////////////////////////////////////////////
				m_time = m_coolTime;

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
			}
			break;	//	GMT_GAME_START

		case GMT_GAME_END:			/////////////////////////////////////////////////    END GAME
			{						//////////////////////////////////////////////////////////////////////////
				ClearTraps();
			}
			break;	//	GMT_GAME_END

		case GMT_GAME_RESETING:
			{
				ClearTraps();
			}
			break;

		case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
			{						//////////////////////////////////////////////////////////////////////////
				m_time = 0;
				sx::core::Scene::DeleteNode( m_node );
				MsgProc(0, GMT_LEVEL_LOAD, 0);
			}
			break;	//	GMT_GAME_RESET
		}
	}

	void Mechanic_PA_Trap::OnGUIClick( sx::gui::PControl Sender )
	{
		if ( !m_node ) return;

		static float lasttime = 0;
		const float  newtime = sx::sys::GetSysTime();
		if ( newtime > lasttime && newtime - lasttime < 1000 ) return;
		lasttime = newtime;

		sx_callstack();

		if ( m_time >= m_coolTime && g_game->m_player->m_gold >= m_cost )
		{
			g_game->m_mouseMode = MS_CreateTrap;
			m_node->SetRotation( 0, sx::cmn::Random(6.12f), 0 );

			msg_SoundPlay msg( false, 0, 0, L"powerAttack", 1 );
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

	void Mechanic_PA_Trap::CreateTrap( void )
	{
		sx_callstack();

		static sx::core::ArrayPNode_inline nodes(256); nodes.Clear();
		sx::core::Scene::GetNodesByArea( m_pos, 2.0f, nodes, NMT_PATHNODE );

		m_pos.y = 0;
		float maxY = -9999.0f;
		int n = nodes.Count();
		for ( int i=0; i<n; i++ )
		{
			float y = nodes[i]->GetPosition_world().y;
			if ( maxY < y )
			{
				maxY = y;
				m_pos.y = maxY;
			}
		}
		m_node->SetPosition( m_pos );
		sx::core::Scene::AddNode( m_node );

		if ( !nodes.Count() ) return;

		if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) )
		{
			bool cancelCreation = false;

			// verify trap collision
			for (int i=0; i<m_traps.Count(); i++)
			{
				Trap* trap = m_traps[i];

				if ( m_pos.Distance_sqr( trap->node->GetPosition_world() ) < 4 )
				{
					cancelCreation = true;
					break;
				}
			}
			if ( cancelCreation ) return;

			// extract correct position
			sx::math::Ray ray( float3( m_pos.x, m_pos.y + 5.0f, m_pos.z), float3( 0.001f, -0.999f, 0.001f ) );
			msg_IntersectRay msgray( NMT_MESH, ray, msg_IntersectRay::GEOMETRY, NULL );
			sx::core::Scene::GetNodeByRay( msgray );
			for ( int i=0; i<32; i++ )
			{
				if ( msgray.results[i].member )
				{
					str512 memberName = sx::core::PNodeMember(msgray.results[i].member)->GetName();
					if ( memberName == L"cameraGuard" ) continue;
				}

				sx::core::Node* node = sx::core::PNode(msgray.results[i].node);
				while ( node->GetParent() ) node = node->GetParent();
				if ( node && !node->GetUserData() && !node->GetUserTag() )
				{
					str512 nodeName = node->GetName();
					if ( nodeName == L"cameraGuard" ) continue;

					cancelCreation = msgray.results[i].position.y < - 5.0f;
					if ( cancelCreation ) return;


					Matrix matR, matL, mat;
					matR.RotationX( sx::math::PIDIV2  );

					float3 d = msgray.results[i].normal;
					if ( d.x == 0.0f )	d.x = 0.001f;
					if ( d.z == 0.0f )	d.z = 0.001f;
					matL.LookAtRH( sx::math::VEC3_ZERO, -d, sx::math::VEC3_Y );
					matL.Inverse( matL );
					mat.Multiply( matR, matL );

					floatQ q;
					q.SetRotationMatrix( mat );
					m_node->SetTransformQ( msgray.results[i].position, q );
					break;
				}
			}
			if ( cancelCreation ) return;

			sx::core::Scene::RemoveNode( m_node );
			m_time = 0;
			g_game->m_player->m_gold -= m_cost;

			Trap* trap = sx_new( Trap );
			trap->coolTime	= m_attack.actionTime;
			trap->count		= m_attack.actionCount;
			trap->pos		= m_pos;
			trap->node		= m_node->Clone();
			trap->node->SetPosition( m_node->GetPosition_world() );
			trap->node->SetUserData( this );
 			sx::core::Scene::AddNode( trap->node );
			m_traps.PushBack( trap );

			sx::core::Node* createNode = NULL;
			if ( trap->node && trap->node->GetChildByName( L"_oncreate", createNode ) )
			{
				msg_Particle msgPar( SX_PARTICLE_SPRAY, SX_PARTICLE_LOOP );
				createNode->MsgProc( MT_PARTICLE, &msgPar );

				msg_SoundPlay msgSnd( false );
				createNode->MsgProc( MT_SOUND_PLAY, &msgSnd );
			}

			g_game->m_mouseMode = MS_Null;
		}
	}

	void Mechanic_PA_Trap::ClearTraps( void )
	{
		sx_callstack();

		for ( int i=0; i<m_traps.Count(); i++ )
		{
			Trap* trap =  m_traps[i];
			sx_delete_and_null( trap );
		}
		m_traps.Clear();
	}

} // namespace GM
