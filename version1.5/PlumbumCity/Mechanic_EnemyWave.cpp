#include "Mechanic_EnemyWave.h"
#include "Game.h"
#include "Player.h"
#include "GameGUI.h"
#include "Scripter.h"
#include "Entity.h"
#include "EntityManager.h"
#include "GameConfig.h"

#define		MAX_ENEMIES		32


void CopyString( WCHAR* dest, int destSize, const WCHAR* src )
{
	if ( !dest || !src ) return;

	int h = destSize-1;
	for ( int i=0; i<h; i++ )
	{
		dest[i] = src[i];
		if ( src[i] == 0 ) return;
	}
	dest[h] = 0;
}

namespace GM
{

	Mechanic_EnemyWaves::Mechanic_EnemyWaves( void ) 
		: Mechanic()
		, m_waveIndex(-1)
		, m_waveTime(0)
		, m_enemyCount(0)
		, m_back(0)
		, m_nextWave(0)
		, m_startProgr(0)
	{
	}

	Mechanic_EnemyWaves::~Mechanic_EnemyWaves( void )
	{
		MsgProc(0, GMT_LEVEL_CLEAR, 0);
	}

	void Mechanic_EnemyWaves::Initialize( void )
	{
		//  create first background panel
		m_back = sx_new( sx::gui::PanelEx );
		m_back->GetElement(0)->SetTextureSrc( L"gui_waveBack.txr" );
		m_back->SetSize( float2( 256, 256 ) );
		m_back->State_GetByIndex(0).Position.Set( -12.0f, 60.0f, 0 );
		m_back->State_Add();
		m_back->State_GetByIndex(1).Position.Set( -12.0f, -38.0f, 0 );

		m_nextWave = sx_new( sx::gui::PanelEx );
		m_nextWave->AddProperty( SX_GUI_PROPERTY_BLENDCHILDS );
		m_nextWave->SetParent( m_back );
		m_nextWave->SetSize( float2( 64, 64) );
		m_nextWave->Position().Set( -50.0f, -42.0f, 0.0f );
		m_nextWave->State_Add();
		m_nextWave->State_GetByIndex(0).Color.w = 0.0f;
		m_nextWave->State_GetByIndex(1).Blender.Set( 0.1f, 0.9f );

		m_startProgr = sx_new( sx::gui::ProgressBar );
		m_startProgr->AddProperty( SX_GUI_PROPERTY_PROGRESSCIRCLE );
		m_startProgr->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
		m_startProgr->SetParent( m_nextWave );
		m_startProgr->SetSize( float2( 128, 128) );
		m_startProgr->GetElement(0)->Color().a = 0.0f;
		m_startProgr->GetElement(1)->SetTextureSrc( L"gui_waveProg.txr" );
		SEGAN_GUI_SET_ONCLICK( m_startProgr, Mechanic_EnemyWaves::OnClick );

		m_label = sx_new( sx::gui::Label );
		m_label->SetParent( m_back );
		m_label->SetFont( L"Font_rob_twedit_info.fnt" );
		m_label->SetSize( float2( 100.0f, 20.0f ) );
		m_label->Position().Set( -45.0f, -103.0f, 0.0f );
		m_label->SetAlign( GTA_LEFT );
		m_label->GetElement(0)->Color() = D3DColor( 0, 0, 0, 0 );
		m_label->GetElement(1)->Color() = 0xaaaaaaaa;

		// add panel to the game
		m_back->SetParent( g_game->m_gui->m_goldPeople->m_back );

		int i = g_game->m_gui->m_goldPeople->m_back->m_Child.IndexOf( m_back );
		while ( i )
		{
			g_game->m_gui->m_goldPeople->m_back->m_Child.Swap( i, i-1 );
			i--;
		}
	}

	void Mechanic_EnemyWaves::Finalize( void )
	{
		m_back->SetParent( NULL );
		sx_delete_and_null( m_back );
	}

	void Mechanic_EnemyWaves::ProcessInput( bool& inputHandled, float elpsTime )
	{
		if ( NotInGame() /*|| g_game->m_mouseMode == MS_ManualTower*/ ) return;

		if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_SPACE ) )
		{
			OnClick( m_startProgr );
			inputHandled = true;
		}
		m_startProgr->ProcessInput( inputHandled );
	}

	void Mechanic_EnemyWaves::Update( float elpsTime )
	{
		if ( m_wavesSrc.IsEmpty() || g_game->m_gamePaused || !g_game->m_currentLevel || m_waveIndex >= m_wavesSrc.Count()+1 )
		{
			m_back->State_SetIndex(0);
			return;
		}

// 		if ( g_game->m_mouseMode == MS_Null )
// 		{
// 			m_back->State_SetIndex(1);
// 		}
// 		else m_back->State_SetIndex(0);

		if ( m_waveIndex < 0 )
		{
			m_back->State_SetIndex(1);
			m_nextWave->State_SetIndex(1);
			m_startProgr->SetMax( 0 );
			m_startProgr->SetValue( 0 );
			return;
		}

		if ( !m_enemyCount && m_waveIndex >= m_wavesSrc.Count() )
		{
			//  say to game that it's over
			m_back->State_SetIndex(0);
			m_nextWave->State_SetIndex(0);
			m_waveIndex++;
			g_game->PostMessage( 0, GMT_WAVE_FINISHED, NULL );
			g_game->m_gamePaused = true;
			g_game->m_gui->m_victory->Show();
			return;
		}

		if ( m_waveIndex >= m_wavesSrc.Count() )
		{
			m_back->State_SetIndex(0);
			return;
		}

		m_waveTime += elpsTime * 0.001f;
		EnemyWave* pWave = m_wavesSrc[ m_waveIndex ];
		
		if ( !pWave->enemyCounts )
		{
			if ( m_waveIndex >= m_wavesSrc.Count()-1 )
			{
				m_waveIndex++;
				return;
			}

			if ( pWave->nextWaveTime>0 )
			{
				pWave->nextWaveTime -= elpsTime*0.001f;

				//  show tips of end wave
				g_game->m_gui->ShowTips( pWave->tipsEnd, 0xffffa947, pWave->tipsEndIcon );
				pWave->tipsEnd[0] = 0;

				m_startProgr->SetValue( pWave->nextWaveTime );
				m_back->State_SetIndex(1);
				m_nextWave->State_SetIndex(1);
			}
			else
			{
				StartWave();
			}

			return;
		}

		//  send enemies to the scene
		for (int i=0; i<WAVE_ENEMY_MAXTYPE; i++)
		{
			EnemyWave::SubWave* pSubWave = &( pWave->subWave[i] );
			if ( !pSubWave->type ) pSubWave->count = 0;
			if ( !pSubWave->count || m_waveTime < pSubWave->startTime ) continue;

			//  its time to create an enemy ?
			pSubWave->elapsTime += elpsTime*0.001f;
			if ( pSubWave->elapsTime < pSubWave->timeStep ) continue;
			pSubWave->elapsTime = 0;
			pSubWave->count--;

			//  create enemy
			Entity* entity = pSubWave->type->Clone();
			if ( !entity )
			{
				sxLog::Log( L" ERROR : can't clone entity : level %d, wave %d , subwave %d , type %s !", 
					g_game->m_currentLevel, m_waveIndex+1, i+1, pSubWave->type->m_typeName.Text() );
			}

			entity->SetLevel(0);

			entity->m_cost[0]			+= pWave->addGold		+ pSubWave->addGold;
			entity->m_health.level[0]	+= pWave->addHealth		+ pSubWave->addHealth;
			//entity->m_health.level[0]	= pWave->addHealth		+ pSubWave->addHealth + 15 + m_waveIndex * 8;
			entity->m_health.imax		= entity->m_health.level[0];
			entity->m_health.icur		= entity->m_health.level[0];
			entity->m_move.moveSpeed	+= pWave->addMoveSpeed	+ pSubWave->addMoveSpeed;
			entity->m_move.animSpeed	+= pWave->addAnimSpeed	+ pSubWave->addAnimSpeed;
			entity->m_experience		+= pWave->addExperience	+ pSubWave->addExperience;

			if ( Config::GetData()->display_Debug == 3 )
			{
				if ( entity->m_cost[0] < 1 )
					sxLog::Log( L"WARNING : level %d, wave %d , subwave %d , entity %s has %d gold !", g_game->m_currentLevel, m_waveIndex+1, i+1, entity->m_typeName.Text(), entity->m_cost[0] );

				if ( entity->m_health.icur < 1 )
					sxLog::Log( L"WARNING : level %d, wave %d , subwave %d , entity %s has %d health !", g_game->m_currentLevel, m_waveIndex+1, i+1, entity->m_typeName.Text(), entity->m_health.icur );

				if ( entity->m_move.animSpeed < 1.0f )
					sxLog::Log( L"WARNING : level %d, wave %d , subwave %d , entity %s has %.2f animSpeed !", g_game->m_currentLevel, m_waveIndex+1, i+1, entity->m_typeName.Text(), entity->m_move.animSpeed );
			}

			entity->m_attackLevel[0].rate				+= pWave->addFireRate			+ pSubWave->addFireRate;
			entity->m_attackLevel[0].physicalDamage		+= pWave->addPhysicalDamage		+ pSubWave->addPhysicalDamage;
			entity->m_attackLevel[0].electricalDamage	+= pWave->addElectricalDamage	+ pSubWave->addElectricalDamage;
			entity->m_attackLevel[0].physicalArmor		+= pWave->addPhysicalArmor		+ pSubWave->addPhysicalArmor;
			entity->m_attackLevel[0].electricalArmor	+= pWave->addElectricalArmor	+ pSubWave->addElectricalArmor;

			entity->m_curAttack = entity->m_curAttackLevel = entity->m_attackLevel[0];

			//	apply difficulty value
			float maxHelath = g_game->m_difficultyValue * (float)entity->m_health.icur;
			entity->m_health.SetMax( int( maxHelath + 0.5f) );

			//  set enemy material
			if ( entity->m_mesh )
			{
				msg_Mesh msgMat( 0, 0, 0, pSubWave->matIndex );
				entity->m_mesh->MsgProc( MT_MESH, &msgMat );
				entity->m_mesh->GetActiveMaterial()->SetFloat( 1, -2.0f );
			}
			else sxLog::Log( L"ERORR : no body found in entity %s", entity->m_typeName.Text() );


			//  set enemy position
			if ( pSubWave->startNode )
				entity->SetPosition( pSubWave->startNode->GetPosition_world() );

			//  set enemy mission
			Mission m;
			m.status = Mission::MS_ACTIVE;
		
			if ( entity->m_curAttack.maxRange<0.1f || entity->m_curAttack.rate<0.001 || !entity->m_curAttack.bullet[0] )
				m.flag = MISSION_GOTO_NODE;
			else
				m.flag = MISSION_GOTO_NODE | MISSION_KILL_ENEMY;

			m.data0 = pSubWave->startNode;
			m.data1 = pSubWave->baseNode ? pSubWave->baseNode : pWave->baseNode;
			m.pos.x = i + 0.1f;
			entity->GetBrain()->AddMission(m);

			m.flag = MISSION_SUICIDE;
			entity->GetBrain()->AddMission(m);

			//  add entity to game
			EntityManager::AddEntity( entity );

			m_enemyCount++;
			pWave->enemyCounts--;

			//	add info to gui
			if ( g_game->m_player->m_profile.level_played < g_game->m_currentLevel )
				g_game->m_gui->m_info->AddTutorial( pSubWave->infoTitle, pSubWave->infoDesc, pSubWave->infoImage, pSubWave->infoShowNow > 0 );
			else
				g_game->m_gui->m_info->AddTutorial( pSubWave->infoTitle, pSubWave->infoDesc, pSubWave->infoImage, false );

			//	ignore adding one tutorial twice
			pSubWave->infoTitle[0] = 0;
		}

	}

	void Mechanic_EnemyWaves::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		switch (msg)
		{
		case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
			{						//////////////////////////////////////////////////////////////////////////
				LoadWaves();
			}
			break;	//	GMT_LEVEL_LOAD

		case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
			{						//////////////////////////////////////////////////////////////////////////
				ClearWaves();
			}
			break;	//	GMT_LEVEL_CLEAR

		case GMT_GAME_START:		/////////////////////////////////////////////////    START GAME
			{						//////////////////////////////////////////////////////////////////////////
				m_waveIndex = -1;
				m_waveTime = 0;
				m_enemyCount = 0;
				m_back->State_SetIndex(1);
				m_nextWave->State_SetIndex(1);
				SetNextWaveImage();
			}
			break;	//	GMT_GAME_START

		case GMT_GAME_END:			/////////////////////////////////////////////////    END GAME
			{						//////////////////////////////////////////////////////////////////////////

			}
			break;	//	GMT_GAME_END

		case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
			{						//////////////////////////////////////////////////////////////////////////
				//! just reload waves :)
				ClearWaves();
				LoadWaves();
				m_waveIndex = -1;
				m_waveTime = 0;
				m_enemyCount = 0;
				m_back->State_SetIndex(1);
				m_nextWave->State_SetIndex(1);
				SetNextWaveImage();
			}
			break;	//	GMT_GAME_RESET

		case GMT_I_FINALIZED:
			if ( data )
			{
				Entity* pEntity = (Entity*)data;
				if ( pEntity->m_partyCurrent == PARTY_ENEMY )
					m_enemyCount--;
			}
			break;
		}
	}

	void Mechanic_EnemyWaves::SetNextWaveImage( void )
	{
		int windex = m_waveIndex + 1;
		if ( windex>=0 && windex<m_wavesSrc.Count() )
		{
			m_nextWave->GetElement(0)->SetTextureSrc( m_wavesSrc[windex]->tipsStartIcon );
			m_nextWave->State_SetIndex(1);
		}
		else m_nextWave->State_SetIndex(0);
	}

	void Mechanic_EnemyWaves::StartWave( void )
	{
		if ( !m_back->State_GetIndex() ) return;

		m_waveTime = 0;
		m_waveIndex++;

		if ( m_waveIndex < m_wavesSrc.Count() )
		{
			if (m_waveIndex == 0)
			{
				g_game->m_wavesComming = true;
				EntityManager::MsgProc(0, GMT_WAVE_STARTED, NULL);
			}

			EnemyWave* pWave = m_wavesSrc[ m_waveIndex ];

			//  show tips of start wave
			g_game->m_gui->ShowTips( pWave->tipsStart, 0xffffa947, pWave->tipsStartIcon );

			//  play particle/sound of start wave
			if ( pWave->tipsStartNode )
			{
				msg_SoundPlay msgSound(true);
				pWave->tipsStartNode->MsgProc( MT_SOUND_PLAY, &msgSound );

				msg_Particle msgPrtcl(SX_PARTICLE_SPRAY);
				pWave->tipsStartNode->MsgProc( MT_PARTICLE, &msgPrtcl );
			}

			String sre;
			sre.Format( L"%d/%d", m_waveIndex+1, m_wavesSrc.Count() );
			m_label->SetText( sre );

			m_back->State_SetIndex(0);
			m_nextWave->State_SetIndex(0);

			int addGold = int( m_startProgr->GetValue() * pWave->goldPerSecond );
			if ( addGold )
			{
				//	play a sound
				g_game->m_player->m_gold += addGold;

				g_game->m_achievements[12].AddValue();

				msg_SoundPlay msg( true, 0, 0, L"waveCall" );
				m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
			}
			m_startProgr->SetMax( pWave->nextWaveTime );
			m_startProgr->SetValue( pWave->nextWaveTime );

			SetNextWaveImage();
		}
	}

	void Mechanic_EnemyWaves::OnClick( sx::gui::PControl sender )
	{
		StartWave();
	}

	void Mechanic_EnemyWaves::ClearWaves( void )
	{
		if ( Config::GetData()->display_Debug == 3 )
		{
			sxLog::Log( L"Clearing waves !" );
		}

		for (int i=0; i<m_wavesSrc.Count(); i++)
			sx_mem_free( m_wavesSrc[i] );
		m_wavesSrc.Clear();

		if ( Config::GetData()->display_Debug == 3 )
		{
			sxLog::Log( L"waves cleared !" );
		}
	}

	void Mechanic_EnemyWaves::LoadWaves( void )
	{
		//ClearWaves();

		if ( Config::GetData()->display_Debug == 3 )
		{
			sxLog::Log( L"Loading waves !" );
		}

		String str = Game::GetLevelPath();
		if ( g_game->m_miniGame )
			str << L"waves_mini.txt";
		else
		{
			switch ( g_game->m_gameMode )
			{
			case 0 : str << L"waves_default.txt"; break;
			case 1 : str << L"waves_warrior.txt"; break;
			case 2 : str << L"waves_legend.txt"; break;
			}
		}

		Scripter script;
		script.Load( str );

		float goldPerSecond = 2.0f;
		for (int i=0; i<script.GetObjectCount(); i++)
		{
			str512 tmpStr;
			if ( script.GetString(i, L"Type", tmpStr) )
			{
				if ( tmpStr == L"Wave" )
				{
					if ( !script.GetString(i, L"Name", tmpStr) )
						continue;

					//	add new enemy wave
					EnemyWave* ew = (EnemyWave*)sx_mem_alloc( sizeof(EnemyWave) );
					ZeroMemory( ew, sizeof(EnemyWave) );
					m_wavesSrc.PushBack(ew);

					CopyString( ew->name, 64, tmpStr );

					if ( script.GetString( i, L"tipsStart", tmpStr ) )
						CopyString( ew->tipsStart, 512, tmpStr );

					if ( script.GetString( i, L"tipsStartIcon", tmpStr ) )
						CopyString( ew->tipsStartIcon, 64, tmpStr );
					
					if ( script.GetString( i, L"tipsEnd", tmpStr ) )
						CopyString( ew->tipsEnd, 512, tmpStr );
					
					if ( script.GetString( i, L"tipsEndIcon", tmpStr ) )
						CopyString( ew->tipsEndIcon, 64, tmpStr );

					if ( script.GetString(i, L"baseNode", tmpStr) )
					{
						sx::core::ArrayPNode nodelist;
						sx::core::Scene::GetNodesByName(tmpStr, nodelist);
						if ( nodelist.Count()>0 )
							ew->baseNode = nodelist[0];
					}

					if ( script.GetString(i, L"tipsStartNode", tmpStr) )
					{
						sx::core::ArrayPNode nodelist;
						sx::core::Scene::GetNodesByName(tmpStr, nodelist);
						if ( nodelist.Count()>0 )
							ew->tipsStartNode = nodelist[0];
					}

					script.GetInteger(i, L"addGold",		ew->addGold);
					script.GetInteger(i, L"addHealth",		ew->addHealth);
					script.GetFloat(i, L"addFireRate",		ew->addFireRate);
					script.GetFloat(i, L"addMoveSpeed",		ew->addMoveSpeed);
					script.GetFloat(i, L"addAnimSpeed",		ew->addAnimSpeed);
					script.GetFloat(i, L"addExperience",	ew->addExperience);

					script.GetFloat(i, L"addDamage",			ew->addPhysicalDamage	);
					script.GetFloat(i, L"addPhysicalDamage",	ew->addPhysicalDamage	);
					script.GetFloat(i, L"addElectricalDamage",	ew->addElectricalDamage );
					script.GetFloat(i, L"addPhysicalArmor",		ew->addPhysicalArmor	);
					script.GetFloat(i, L"addElectricalArmor",	ew->addElectricalArmor	);

					script.GetFloat(i, L"nextWaveTime",	ew->nextWaveTime);


					script.GetFloat(i, L"goldPerSecond", goldPerSecond);
					ew->goldPerSecond = goldPerSecond;

					str512 entityType;
					for (int j=0; j<WAVE_ENEMY_MAXTYPE; j++)
					{
						entityType.Clear();
						tmpStr.Format(L"%d_entityType", j);
						if ( script.GetString(i, tmpStr, entityType) )
						{
							ew->subWave[j].type = (Entity*)EntityManager::GetTypeByName(entityType);
							if ( !ew->subWave[j].type )
								sxLog::Log( L"WARNING : level %d, wave %d , subwave %d entity type %s not found !", g_game->m_currentLevel, ( m_wavesSrc.Count()-1), j, entityType.Text() );
						}

						tmpStr.Format(L"%d_startNode", j);
						if ( script.GetString(i, tmpStr, tmpStr) )
						{
							sx::core::ArrayPNode nodelist;
							sx::core::Scene::GetNodesByName(tmpStr, nodelist);
							if ( nodelist.Count()>0 )
								ew->subWave[j].startNode = nodelist[0];
						}

						tmpStr.Format(L"%d_baseNode", j);
						if ( script.GetString(i, tmpStr, tmpStr) )
						{
							sx::core::ArrayPNode nodelist;
							sx::core::Scene::GetNodesByName(tmpStr, nodelist);
							if ( nodelist.Count()>0 )
								ew->subWave[j].baseNode = nodelist[0];
						}

						tmpStr.Format(L"%d_timeStep", j);
						script.GetFloat(i, tmpStr, ew->subWave[j].timeStep);
						ew->subWave[j].elapsTime = ew->subWave[j].timeStep + 1;

						tmpStr.Format(L"%d_startTime", j);
						script.GetFloat(i, tmpStr, ew->subWave[j].startTime);

						tmpStr.Format(L"%d_count", j);
						script.GetInteger(i, tmpStr, ew->subWave[j].count);

						tmpStr.Format(L"%d_matIndex", j);
						if ( !script.GetInteger(i, tmpStr, ew->subWave[j].matIndex) )
							ew->subWave[j].matIndex = -1;

						tmpStr.Format(L"%d_addGold", j);
						script.GetInteger(i, tmpStr, ew->subWave[j].addGold);

						tmpStr.Format(L"%d_addHealth", j);
						script.GetInteger(i, tmpStr, ew->subWave[j].addHealth);

						tmpStr.Format(L"%d_addDamage", j);
						script.GetFloat( i, tmpStr, ew->subWave[j].addPhysicalDamage	);

						tmpStr.Format(L"%d_addPhysicalDamage", j);
						script.GetFloat( i, tmpStr, ew->subWave[j].addPhysicalDamage	);

						tmpStr.Format(L"%d_addElectricalDamage", j);
						script.GetFloat( i, tmpStr, ew->subWave[j].addElectricalDamage	);

						tmpStr.Format(L"%d_addPhysicalArmor", j);
						script.GetFloat( i, tmpStr, ew->subWave[j].addPhysicalArmor		);

						tmpStr.Format(L"%d_addElectricalArmor", j);
						script.GetFloat( i, tmpStr, ew->subWave[j].addElectricalArmor	);

						tmpStr.Format(L"%d_addFireRate", j);
						script.GetFloat(i, tmpStr, ew->subWave[j].addFireRate);

						tmpStr.Format(L"%d_addMoveSpeed", j);
						script.GetFloat(i, tmpStr, ew->subWave[j].addMoveSpeed);

						tmpStr.Format( L"%d_addAnimSpeed", j );
						script.GetFloat( i, tmpStr, ew->subWave[j].addAnimSpeed );

						tmpStr.Format( L"%d_addExperience", j );
						script.GetFloat( i, tmpStr, ew->subWave[j].addExperience );

						//	add info
						{
							str512 title, desc, image;

							tmpStr.Format( L"%d_infoTitle", j );
							script.GetString( i, tmpStr, title );

							tmpStr.Format( L"%d_infoDesc", j );
							script.GetString( i, tmpStr, desc );

							tmpStr.Format( L"%d_infoImage", j );
							script.GetString( i, tmpStr, image );

							//g_game->m_gui->m_info->AddTutorial( title, desc, image );
							if ( title.Text() && desc.Text() && image.Text() )
							{

								CopyString( ew->subWave[j].infoTitle, 128, title );
								CopyString( ew->subWave[j].infoDesc,  512, desc  );
								CopyString( ew->subWave[j].infoImage, 64,  image );

								tmpStr.Format( L"%d_infoShowNow", j );
								script.GetInteger( i, tmpStr, ew->subWave[j].infoShowNow );
							}
						}

					}
					//	for (int j=0; j<WAVE_ENEMY_MAXTYPE; j++)

					//  check the remain enemy counts
					ew->enemyCounts = 0;
					for ( int i=0; i<WAVE_ENEMY_MAXTYPE; i++ )
						ew->enemyCounts += ew->subWave[i].count;
				}
			}
		}	//	for (int i=0; i<script.GetObjectCount(); i++)

		//	export excel friendly info in development mode
		if ( Config::GetData()->display_Debug == 3 )
		{
			String log = L" Wave" EXCEL_CHAR_SEPERATOR L"Enemies" EXCEL_CHAR_SEPERATOR L"Golds" EXCEL_CHAR_SEPERATOR L"Health" EXCEL_CHAR_SEPERATOR 
				L"XP" EXCEL_CHAR_SEPERATOR L"Damage" EXCEL_CHAR_SEPERATOR L"Armor" EXCEL_CHAR_SEPERATOR L"Entities \r\n";
			//log.SetFloatPrecision(2);

			for ( int i=0; i<m_wavesSrc.Count(); i++ )
			{
				EnemyWave* ew = m_wavesSrc[i];

				int	sumenemy = 0;
				int	sumgold = 0;
				int	sumhealth = 0;
				float sumexperience = 0;
				float sumdamage = 0;
				float sumarmor = 0;
				str1024 names;

				for ( int j=0; j<WAVE_ENEMY_MAXTYPE; j++ )
				{
					if ( !ew->subWave[j].type ) continue;

					int c = ew->subWave[j].count;
					sumenemy += c;
					sumgold += c * ( ew->subWave[j].type->m_cost[0] + ew->subWave[j].addGold + ew->addGold );
					sumhealth += c * ( ew->subWave[j].type->m_health.level[0] + ew->subWave[j].addHealth + ew->addHealth );
					sumdamage += c * ( ew->subWave[j].type->m_attackLevel[0].physicalDamage + ew->subWave[j].addPhysicalDamage + ew->addPhysicalDamage );
					sumdamage += c * ( ew->subWave[j].type->m_attackLevel[0].electricalDamage + ew->subWave[j].addElectricalDamage + ew->addElectricalDamage );
					sumarmor += c * ( ew->subWave[j].type->m_attackLevel[0].physicalArmor + ew->subWave[j].addPhysicalArmor + ew->addPhysicalArmor );
					sumarmor += c * ( ew->subWave[j].type->m_attackLevel[0].electricalArmor + ew->subWave[j].addElectricalArmor + ew->addElectricalArmor );
					sumexperience += c * ( ew->subWave[j].type->m_experience + + ew->subWave[j].addExperience + ew->addExperience );

					names << ew->subWave[j].type->m_typeName.Text() << L" * " << c << L"  ";
				}

				//log << i+1 << EXCEL_CHAR_SEPERATOR << sumenemy << EXCEL_CHAR_SEPERATOR << sumgold << EXCEL_CHAR_SEPERATOR << sumhealth << EXCEL_CHAR_SEPERATOR << sumexperience << EXCEL_CHAR_SEPERATOR << sumdamage << EXCEL_CHAR_SEPERATOR << sumarmor << EXCEL_CHAR_SEPERATOR << names << L"\r\n";
			}

			struct Enemies {
				int				count;
				const WCHAR*	name;
			} enemies[100];
			ZeroMemory(enemies, sizeof(enemies));

			for ( int i=0; i<m_wavesSrc.Count(); i++ )
			{
				EnemyWave* ew = m_wavesSrc[i];
				for ( int j=0; j<WAVE_ENEMY_MAXTYPE; j++ )
				{
					if ( !ew->subWave[j].type ) continue;

					const WCHAR* name = ew->subWave[j].type->m_typeName;
					for ( int k=0; k<100; k++ )
					{
						if ( !enemies[k].name || enemies[k].name == name )
						{
							enemies[k].name = name;
							enemies[k].count += ew->subWave[j].count;
							break;
						}
					}
				}
			}

			log << L"\r\n\r\nEnemy type" EXCEL_CHAR_SEPERATOR L"Count \r\n";
			for ( int i=0; i<100 && enemies[i].name; i++ )
			{
				log << enemies[i].name << EXCEL_CHAR_SEPERATOR << enemies[i].count << L"\r\n";
			}

			str = Game::GetLevelPath();
			str << L"waves_excel.csv";
			sx::cmn::String_Save( log, str, false );
		}


		str.Format( L"0/%d", m_wavesSrc.Count() );
		m_label->SetText( str );

		SetNextWaveImage();
	}


} // namespace GM
