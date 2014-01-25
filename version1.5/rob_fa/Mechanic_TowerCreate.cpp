#include "Mechanic_TowerCreate.h"
#include "Game.h"
#include "GameGUI.h"
#include "Player.h"
#include "Entity.h"
#include "Scripter.h"
#include "EntityManager.h"
#include "GameConfig.h"


static sx::core::PNode	towerNode = NULL;
static float3			towerPos;

namespace GM
{

	Mechanic_TowerCreate::Mechanic_TowerCreate( void ) : Mechanic()
		,	m_pnlCreate0(0)
		,	m_pnlCreate1(0)
		,	m_pnlArrow(0)
		,	m_tower_distance(7.0f)
		,	m_towerIndex(0)
		,	m_Tower(0)
	{
		sx_callstack();

		for (int i=0; i<5; i++)
			m_btnCreate[i] = 0;

		m_ShowRange.m_tag = MAKEFOURCC('M','C','T','C');
	}

	Mechanic_TowerCreate::~Mechanic_TowerCreate( void )
	{
		sx_callstack();
	}

	void Mechanic_TowerCreate::Initialize( void )
	{
		sx_callstack();

		Finalize();

		//  create a panel for constructing towers
		m_pnlCreate0 = sx_new( sx::gui::PanelEx );
		m_pnlCreate0->SetSize( float2(256, 256) );
		m_pnlCreate0->GetElement(0)->SetTextureSrc( L"gui_pnlTowers0.txr" );
		m_pnlCreate0->State_GetByIndex(0).Rotation.Set( 0, 0, sx::math::DegToRad(5.0f) );
		m_pnlCreate0->State_GetByIndex(0).Blender.Set(0.6f, 0.2f);
		m_pnlCreate0->State_GetByIndex(0).Color.Set( 0, 0, 0, 0 );
		m_pnlCreate0->State_Add();
		m_pnlCreate0->State_GetByIndex(1).Rotation.Set( 0, 0, 0 );
		m_pnlCreate0->State_GetByIndex(1).Blender.Set(0.2f, 0.6f);
		m_pnlCreate0->State_GetByIndex(1).Color.Set( 1, 1, 1, 1 );

		m_pnlCreate1 = sx_new( sx::gui::PanelEx );
		m_pnlCreate1->SetSize( float2(256, 256) );
		m_pnlCreate1->GetElement(0)->SetTextureSrc( L"gui_pnlTowers1.txr" );
		m_pnlCreate1->State_GetByIndex(0).Rotation.Set( 0, 0, -sx::math::DegToRad(5.0f) );
		m_pnlCreate1->State_GetByIndex(0).Blender.Set(0.6f, 0.2f);
		m_pnlCreate1->State_GetByIndex(0).Color.Set( 0, 0, 0, 0 );
		m_pnlCreate1->State_Add();
		m_pnlCreate1->State_GetByIndex(1).Rotation.Set( 0, 0, 0 );
		m_pnlCreate1->State_GetByIndex(1).Blender.Set(0.2f, 0.6f);
		m_pnlCreate1->State_GetByIndex(1).Color.Set( 1, 1, 1, 1 );

		m_pnlArrow = sx_new( sx::gui::PanelEx );
		m_pnlArrow->SetSize( float2(64, 64) );
		m_pnlArrow->GetElement(0)->SetTextureSrc( L"gui_pnlTowersArrow.txr" );
		m_pnlArrow->State_GetByIndex(0).Blender.Set(0.3f, 0.7f);
		m_pnlArrow->State_GetByIndex(0).Center.Set( 0.0f, 0.8f, 0.0f );
		m_pnlArrow->State_Add();
		m_pnlArrow->State_Add();
		m_pnlArrow->State_Add();
		m_pnlArrow->State_Add();
		m_pnlArrow->State_Add();
		m_pnlArrow->State_GetByIndex(0).Color.Set( 1.0f, 1.0f, 1.0f, 0.0f );
		m_pnlArrow->State_GetByIndex(2).Rotation.Set( 0.0f, 0.0f, sx::math::DegToRad(  72.0f ) );
		m_pnlArrow->State_GetByIndex(3).Rotation.Set( 0.0f, 0.0f, sx::math::DegToRad( 144.0f ) );
		m_pnlArrow->State_GetByIndex(4).Rotation.Set( 0.0f, 0.0f, sx::math::DegToRad( 216.0f ) );
		m_pnlArrow->State_GetByIndex(5).Rotation.Set( 0.0f, 0.0f, sx::math::DegToRad( 288.0f ) );

		g_game->m_gui->Add_Front( m_pnlCreate0 );

		m_ShowRange.Initialize();


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

				if ( tmpStr == L"GeneralTips" )
				{
					script.GetString( i, L"towerlocked",	m_tower_locked	);
					m_tower_locked.Replace(L"\\n", L"\n");
					break;
				}
			}
		}
	}

	void Mechanic_TowerCreate::Finalize( void )
	{
		sx_callstack();

		g_game->m_gui->Remove( m_pnlCreate0 );

		sx_delete_and_null( m_pnlCreate0 );

		m_ShowRange.Finalize();

		for (int i=0; i<5; i++)
			m_btnCreate[i] = NULL;

		m_towerIndex = 0;
		m_Tower = NULL;
		towerNode = NULL;
	}

	void Mechanic_TowerCreate::ProcessInput( bool& inputHandled, float elpsTime )
	{
		sx_callstack();

		if ( NotInGame() )
		{
			ReleaseCaughtTower();
			return;
		}

		//  check create mode
		else if ( g_game->m_mouseMode == MS_CreateTower )
		{
			HidePanel();
			
			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_ESCAPE) || SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_RIGHT) )
			{
				ReleaseCaughtTower();
				inputHandled = true;
				return;
			}
			if ( inputHandled ) return;

			//  set tower spirit position
			if ( m_Tower )
			{				
				sx::math::Ray ray = sx::core::Renderer::GetCamera()->GetRay( SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0) );
				msg_IntersectRay msgRay(NMT_MESH, ray, msg_IntersectRay::BOX, NULL);
				sx::core::Scene::GetNodeByRay(msgRay);

				bool doCreate = false;
				for ( int i=0; i<32; i++ )
				{
					if ( msgRay.results[i].node )
					{
						sx::core::PNode node = sx::core::PNode( msgRay.results[i].node );
						String str = node->GetName();
						if ( str == L"zone" )
						{
							//  now test geometry
							msg_IntersectRay meshRay(NMT_MESH, ray, msg_IntersectRay::GEOMETRY, NULL);
							node->MsgProc(MT_INTERSECT_RAY, &meshRay);
							if ( meshRay.results[0].member )
							{
								doCreate = true;
								towerPos = meshRay.results[0].position;
							}
						}
					}
				}

				if ( !doCreate )
				{
					Plane p; p.Make(sx::math::VEC3_ZERO, sx::math::VEC3_Y);
					ray.Intersect_Plane(p, &towerPos);
				}
				else
				{
					//  verify that no tower is in place
					AABox box;
					{
						static sx::core::ArrayPNodeMember memberlist(256); memberlist.Clear();
						m_Tower->m_node->GetMembersByType(NMT_MESH, memberlist, true);
						memberlist[0]->MsgProc(MT_GETBOX_LOCAL, &box);
					}

					//float2 threshold( (box.Max.x - box.Min.x) * 0.5f, (box.Max.z - box.Min.z) * 0.5f );
					//float boxRadius = 7.0f;//threshold.Length();
					static sx::core::ArrayPNode nodelist(256); nodelist.Clear();
					sx::core::Scene::GetNodesByArea(towerPos, m_tower_distance, nodelist, NMT_MESH, PARTY_TOWER);

					for (int i=0; i<nodelist.Count(); i++)
					{
						AABox otherBox;
						static sx::core::ArrayPNodeMember memberlist(256); memberlist.Clear();
						nodelist[i]->GetMembersByType(NMT_MESH, memberlist, true);
						memberlist[0]->MsgProc(MT_GETBOX_LOCAL, &otherBox);

						float2 otherThreshold( (otherBox.Max.x - otherBox.Min.x) * 0.5f, (otherBox.Max.z - otherBox.Min.z) * 0.5f );
						float otherRadius = otherThreshold.Length();						
						
						if ( towerPos.Distance( nodelist[i]->GetPosition_world() ) < m_tower_distance + otherRadius )
						{
							doCreate = false;
							break;
						}
					}
				}

				towerPos.y += 0.5f;
				m_Tower->SetPosition( towerPos );
				if ( doCreate )
				{
					//  set tower direction
					static sx::core::ArrayPNode nodelist(128); nodelist.Clear();
					sx::core::Scene::GetNodesByArea( towerPos, m_Tower->m_attackLevel[MAX_LEVEL].maxRange, nodelist, NMT_PATHNODE );
					if ( nodelist.Count() )
					{
						float3 sumpos(0,0,0);
						for ( int i=0; i<nodelist.Count(); i++ )
							sumpos += nodelist[i]->GetPosition_world();
						sumpos /= (float)nodelist.Count();

						float3 towerDir( sumpos.x - towerPos.x, 0, sumpos.z - towerPos.z );
						towerDir.Normalize( towerDir );
						
						const float3& curDir = m_Tower->GetDirection();
						towerDir.x = ( towerDir.x + curDir.x * 10000.0f ) / 20000.0f;
						towerDir.z = ( towerDir.z + curDir.z * 10000.0f ) / 20000.0f;
						m_Tower->SetDirection( towerDir );
					}

					m_ShowRange.SetColor( D3DColor(1.0f, 1.0f, 1.0f, 1.0f) );
					CreateTower(towerPos);
				}
				else
				{
					m_ShowRange.SetColor( D3DColor(1.0f, 0.0f, 0.0f, 1.0f) );
				}

				inputHandled = true;
			}

		}
		else if ( g_game->m_mouseMode == MS_ShowCreateTower )
		{
			//  check GUI 
			for ( int i=0; i<5; i++ )
			{
				if ( !m_btnCreate[i] ) continue;

				m_btnCreate[i]->MouseUp( -99999, -99999 );
				bool res = false;
				m_btnCreate[i]->ProcessInput( res );

				if ( !m_btnCreate[i]->GetUserData() ) continue;

				if ( sx::gui::Control::GetCapturedControl() == m_btnCreate[i] )
				{
					m_pnlArrow->State_SetIndex(i+1);
					m_btnCreate[i]->State_SetIndex(2);
					inputHandled = true;

					if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT ) )
					{
						CatchTower( i );

						msg_SoundPlay msg( true, 0, 0, L"mouseClick" );
						m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
					}
				}
				else
				{
					m_btnCreate[i]->State_SetIndex( CanBuyTower(i) ? 1 : 3 );
				}
			}

			if ( !inputHandled && (
				SEGAN_KEYDOWN( 0, SX_INPUT_KEY_MOUSE_LEFT ) || 
				SEGAN_KEYDOWN( 0, SX_INPUT_KEY_MOUSE_RIGHT ) || 
				SEGAN_KEYDOWN( 0, SX_INPUT_KEY_ESCAPE ) ) )
			{
				g_game->m_mouseMode = MS_Null;
				inputHandled = true;
				HidePanel();
			}
		}
		else
		{
			//ReleaseCaughtTower();

			if ( !inputHandled && SEGAN_KEYDOWN( 0, SX_INPUT_KEY_MOUSE_RIGHT ) )
			{
				g_game->m_mouseMode = MS_ShowCreateTower;
				inputHandled = true;

				float w = SEGAN_VP_WIDTH / 2;
				float h = SEGAN_VP_HEIGHT / 2;
				float x = SEGAN_MOUSE_ABSX(0) - w;
				float y = h - SEGAN_MOUSE_ABSY(0);
				float d = 130.0f;
				if ( x < d-w ) x = d-w;
				if ( y < d-h ) y = d-h;
				if ( x > w-d ) x = w-d;
				if ( y > h-d ) y = h-d;
				
				m_pnlCreate0->State_GetByIndex(0).Position.Set(x, y, 0);
				m_pnlCreate0->State_GetByIndex(1).Position.Set(x, y, 0);
				m_pnlCreate0->State_GetBlended().Position.Set(x, y, 0);
				ShowPanel();

				msg_SoundPlay msg( true, 0, 0, L"towerPanel" );
				m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
			}
			//else HidePanel();
		}
	}

	void Mechanic_TowerCreate::Update( float elpsTime )
	{
		sx_callstack();

		if ( !g_game->m_game_currentLevel || g_game->m_game_paused )
		{
			HidePanel();
			return;
		}

		if ( g_game->m_app_Loading || g_game->m_mouseMode != MS_ShowCreateTower )
			HidePanel();		

		if ( m_pnlCreate0->State_GetIndex() )
		{
			for ( int i=0; i<5; i++ )
			{
				if ( m_btnCreate[i] )
				{
					Entity* towerType = (Entity*)m_btnCreate[i]->GetUserData();
					if ( towerType )
					{
						str1024 hint, tmp;
						tmp.Format( L" %s \n %s ", towerType->m_displayName.Text(), towerType->m_typeDesc.Text() );
						hint.Format( tmp, towerType->m_cost[0], g_game->m_player->m_gold );
						m_btnCreate[i]->SetHint( hint );
					}
					else
					{
						m_btnCreate[i]->SetHint( m_tower_locked );
					}
				}
			}
		}

		m_ShowRange.Update( elpsTime );
	}

	void Mechanic_TowerCreate::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		sx_callstack_param(Mechanic_TowerCreate::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

		switch ( msg )
		{
		case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
			{
				Entity* towerTypes[5] = {0,0,0,0,0};

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
						if ( tmpStr == L"Player" )
						{
							if ( !script.GetString(i, L"Name", tmpStr) )
								continue;

							if ( tmpStr == L"NORMAL" )
							{
								m_tower_distance = 7.0f;
								script.GetFloat( i, L"tower_distance", m_tower_distance );

								for ( int t=0; t<5; t++ )
								{
									str.Format(L"tower%d", t+1);
									if ( script.GetString( i, str, tmpStr ) )
										towerTypes[t] = (Entity*)EntityManager::GetTypeByName( tmpStr );

									if ( towerTypes[t] )
									{
										towerTypes[t]->m_maxLevel = NUM_LEVELS-1;
										str.Format(L"tower%d_maxLevel", t+1);
										script.GetInteger( i, str, towerTypes[t]->m_maxLevel );
									}
								}
							}
						}
					}
				}

				//  create tower icons
				m_pnlCreate1->SetParent( NULL );
				m_pnlArrow->SetParent(NULL);
				for ( int i=0; i<5; i++ )
				{
					m_btnCreate[i] = sx_new( sx::gui::PanelEx );
					m_btnCreate[i]->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
					m_btnCreate[i]->SetParent( m_pnlCreate0 );
					m_btnCreate[i]->SetSize( float2(64, 64) );
					
					m_btnCreate[i]->State_GetByIndex(0).Center.Set( 0.0f, 1.75f, 0.0f );
					m_btnCreate[i]->State_GetByIndex(0).Scale.Set( 1.0f, 0.5f, 1.0f );
					m_btnCreate[i]->State_GetByIndex(0).Blender.Set(0.6f, 0.4f);
					m_btnCreate[i]->State_GetByIndex(0).Color.Set( 1.0f, 1.0f, 1.0f, 0.0f );

					switch ( i )
					{
					case 0: m_btnCreate[i]->State_GetByIndex(0).Rotation.Set( 0.0f, 0.0f, 0.0f ); break;
					case 1: m_btnCreate[i]->State_GetByIndex(0).Rotation.Set( 0.0f, 0.0f, sx::math::DegToRad( 72.0f) ); break;
					case 2: m_btnCreate[i]->State_GetByIndex(0).Rotation.Set( 0.0f, 0.0f, sx::math::DegToRad( 144.0f) ); break;
					case 3: m_btnCreate[i]->State_GetByIndex(0).Rotation.Set( 0.0f, 0.0f, sx::math::DegToRad(-144.0f) ); break;
					case 4: m_btnCreate[i]->State_GetByIndex(0).Rotation.Set( 0.0f, 0.0f, sx::math::DegToRad(-72.0f) ); break;
					}

					m_btnCreate[i]->State_Add();
					m_btnCreate[i]->State_GetByIndex(1).Scale.Set( 1.0f, 1.0f, 1.0f );
					m_btnCreate[i]->State_GetByIndex(1).Center.Set( 0.0f, 1.36f, 0.0f );
					m_btnCreate[i]->State_GetByIndex(1).Color.Set( 1.0f, 1.0f, 1.0f, 1.0f );

					m_btnCreate[i]->State_Add();
					m_btnCreate[i]->State_GetByIndex(2).Scale.Set( 1.0f, 1.0f, 1.0f );
					m_btnCreate[i]->State_GetByIndex(2).Center.Set( 0.0f, 1.36f, 0.0f );
					m_btnCreate[i]->State_GetByIndex(2).Color.Set(1.0f, 0.7f, 0.0f, 1.0f);

					m_btnCreate[i]->State_Add();
					m_btnCreate[i]->State_GetByIndex(3).Scale.Set( 1.0f, 1.0f, 1.0f );
					m_btnCreate[i]->State_GetByIndex(3).Center.Set( 0.0f, 1.36f, 0.0f );
					m_btnCreate[i]->State_GetByIndex(3).Color.Set(0.4f, 0.3f, 0.3f, 1.0f);


					if ( towerTypes[i] )
					{
						m_btnCreate[i]->SetUserData( towerTypes[i] );
						
						str1024 textureName;
						textureName.Format( L"gui_%s.txr", towerTypes[i]->m_typeName.Text() );
						m_btnCreate[i]->GetElement(0)->SetTextureSrc( textureName );
						
					}
					else m_btnCreate[i]->GetElement(0)->SetTextureSrc( L"gui_tower_locked.txr" );
					
				}
				m_pnlCreate1->SetParent( m_pnlCreate0 );
				m_pnlArrow->SetParent( m_pnlCreate0 );
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

			}
			break;	//	GMT_GAME_END

		case GMT_GAME_RESETING:		/////////////////////////////////////////////////    GOING TO RESET GAME
			{						//////////////////////////////////////////////////////////////////////////
				ReleaseCaughtTower();
			}
			break;	//	GMT_GAME_RESETING

		case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
			{						//////////////////////////////////////////////////////////////////////////
				// reload types in development mode
				if ( Config::GetData()->display_Debug == 3 )
				{
					Finalize();
					Initialize();
					MsgProc( 0, GMT_LEVEL_LOAD, 0 );
				}
			}
			break;	//	GMT_GAME_RESET

		case GMT_GAME_PAUSED:		/////////////////////////////////////////////////	GAME PAUSED
			{
				ReleaseCaughtTower( true );
			}
		}
	}

	void Mechanic_TowerCreate::CatchTower( int index )
	{
		sx_callstack_param(Mechanic_TowerCreate::CatchTower(index=%d), index);

		if ( !m_btnCreate[index] ) return;
		Entity* towerType = (Entity*)m_btnCreate[index]->GetUserData();
		if ( !towerType ) return;

		//  verify that the player can buy this tower
		if ( g_game->m_player->m_gold >= towerType->m_cost[0] )
		{
			HidePanel();

			// place scene node from 3D scene to show spirit tower
			if ( towerType->m_node )
			{
				g_game->m_mouseMode = MS_CreateTower;
				m_Tower = towerType;
				m_towerIndex = index;

				//  set spirit mode to place tower
				towerNode = m_Tower->m_node;
				sx::core::Scene::AddNode( towerNode );

				Entity::SetSelected( m_Tower );
				m_ShowRange.SetOwner( m_Tower );
			}
		}
		else	//  player can not buy this tower. say him by the way
		{

		}
	}

	void Mechanic_TowerCreate::ReleaseCaughtTower( bool force /*= false */ )
	{
		if ( g_game->m_mouseMode != MS_CreateTower && !force ) return;

		sx_callstack();

		g_game->m_mouseMode = MS_Null;
		Entity::SetSelected(NULL);
		sx::core::Scene::RemoveNode( towerNode );
		m_ShowRange.SetOwner( NULL );
		towerNode = NULL;
		m_Tower = NULL;
	}

	void Mechanic_TowerCreate::CreateTower( float3 pos )
	{
		sx_callstack();
		sx_assert(m_Tower);

		//  now try to locate best place depend on mouse position
		if ( !m_Tower ) return;
		
		if ( SEGAN_KEYDOWN( 0, SX_INPUT_KEY_MOUSE_LEFT ) )
		{
			m_ShowRange.SetOwner( NULL );

			Entity* tower = EntityManager::CreateEntityByTypeName( m_Tower->m_typeName.Text() );

			ReleaseCaughtTower();

			if ( tower )
			{
				pos.y -= 0.5f;
				tower->SetPosition( pos );

				Mission m;
				m.status = Mission::MS_ACTIVE;
				m.flag = MISSION_KILL_ENEMY;
				tower->GetBrain()->AddMission(m);

				Entity::SetSelected( tower );
				EntityManager::AddEntity( tower );
				tower->SetLevel(0);

				if ( tower->m_node )
				{
					sx::core::PNode tarak = NULL;
					if ( tower->m_node->GetChildByName( L"tarak", tarak ) )
					{
						tarak->SetRotation( 0, sx::cmn::Random(9.5f), 0 );

						msg_Mesh msgMesh( 0, SX_MESH_INVISIBLE );
						tarak->MsgProc( MT_MESH, &msgMesh );
					}
				}

				//  prevent from tower selection
				char* key = (char*)sx::io::Input::GetKeys(0);
				key[SX_INPUT_KEY_MOUSE_LEFT] = SX_INPUT_STATE_NORMAL;

				//  decrease player gold by cost of this created tower
				g_game->m_player->m_gold -= tower->m_cost[0];

				//	achievement
				g_game->m_achievements[4].AddValue();
				g_game->m_achievements[5].AddValue();
				g_game->m_achievements[6].AddValue();

			}
		}
	}

	bool Mechanic_TowerCreate::CanBuyTower( int index )
	{
		sx_callstack();

		if ( !m_btnCreate[index] ) return false;

		Entity* towerType = (Entity*)m_btnCreate[index]->GetUserData();
		if ( !towerType ) return false;

		return ( g_game->m_player->m_gold >= towerType->m_cost[0] );
	}

	FORCEINLINE void Mechanic_TowerCreate::ShowPanel( void )
	{
		sx_callstack();

		//Entity::SetSelected( NULL );

		m_pnlCreate0->State_SetIndex(1);
		m_pnlCreate1->State_SetIndex(1);
		for ( int i=0; i<5; i++ )
		{
			if ( m_btnCreate[i] )
				m_btnCreate[i]->State_SetIndex( 1 );
		}
	}

	FORCEINLINE void Mechanic_TowerCreate::HidePanel( void )
	{
		sx_callstack();

		m_pnlCreate0->State_SetIndex(0);
		m_pnlCreate1->State_SetIndex(0);
		for ( int i=0; i<5; i++ )
		{
			if ( m_btnCreate[i] )
				m_btnCreate[i]->State_SetIndex( 0 );
		}
		m_pnlArrow->State_SetIndex(0);
	}

} // namespace GM
