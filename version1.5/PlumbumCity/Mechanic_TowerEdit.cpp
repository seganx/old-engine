#include "Mechanic_TowerEdit.h"
#include "Game.h"
#include "GameGUI.h"
#include "GameConfig.h"
#include "Player.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Scripter.h"


static str512 s_te_upgrade;
static str512 s_te_repair;
static str512 s_te_sell;
static str512 s_te_fulupgrade;
static str512 s_te_xp;
static str512 s_te_fast;
static str512 s_te_medium;
static str512 s_te_low;
static str512 s_te_none;


namespace GM
{

	Mechanic_TowerEdit::Mechanic_TowerEdit( void )
		:	Mechanic()
		,	m_back(0)
		,	m_pnlEditor1(0)
		,	m_pnlSell(0)
		,	m_lblHealth(0)
		,	m_lblXP(0)
		,	m_lblDamage(0)
		,	m_lblFireRate(0)
		,	m_mode(0)
	{
		sx_callstack_push(Mechanic_TowerEdit::Mechanic_TowerEdit());

		ZeroMemory( m_pnlUpdate,	sizeof(m_pnlUpdate)	);
		ZeroMemory( m_pnlLamp,		sizeof(m_pnlLamp)	);
		ZeroMemory( m_pnlButton,	sizeof(m_pnlButton)	);
	}

	Mechanic_TowerEdit::~Mechanic_TowerEdit( void )
	{
		sx_callstack_push(Mechanic_TowerEdit::~Mechanic_TowerEdit());

	}

	void Mechanic_TowerEdit::Initialize( void )
	{
		sx_callstack_push(Mechanic_TowerEdit::Initialize());

		// create first background
		m_back = sx_new( sx::gui::PanelEx );
		m_back->SetSize( float2(1024, 128) );
		m_back->GetElement(0)->SetTextureSrc( L"gui_pnlTowerEdit0.txr" );
		m_back->State_GetByIndex(0).Blender.Set( 0.4f, 0.5f );
		m_back->State_GetByIndex(0).Align.Set( 0.5f, -0.5f );
		m_back->State_GetByIndex(0).Position.Set( 530.0f, 63.0f, 0.0f );
		m_back->State_Add();
		m_back->State_GetByIndex(1).Position.Set( -163.0f, 63.0f, 0.0f );
		m_back->State_Add();
		m_back->State_GetByIndex(2).Position.Set( -24.0f, 63.0f, 0.0f );

		//	create upgrade background
		m_pnlEditor1 = sx_new( sx::gui::PanelEx );
		m_pnlEditor1->SetSize( float2(1024, 128) );
		m_pnlEditor1->GetElement(0)->SetTextureSrc( L"gui_pnlTowerEdit1.txr" );

		//  create upgrade panels
		String textureName;
		for ( int i=0; i<4; i++ )
		{
			m_pnlUpdate[i] = sx_new( sx::gui::PanelEx );
			m_pnlUpdate[i]->SetSize( float2(128, 128) );

			textureName.Format( L"gui_pnlUpdateLevel%d.txr", i );
			m_pnlUpdate[i]->GetElement(0)->SetTextureSrc( textureName );

			if ( i )
			{
				m_pnlUpdate[i]->State_Add();
				m_pnlUpdate[i]->State_GetByIndex(0).Color.w = 0.0f;
				m_pnlUpdate[i]->SetParent( m_pnlUpdate[0] );
			}
			else m_pnlUpdate[i]->SetParent( m_back );
		}
		m_pnlUpdate[0]->State_GetByIndex(0).Position.Set( 87.0f, -100, 0 );
		m_pnlUpdate[0]->State_Add();
		m_pnlUpdate[0]->State_GetByIndex(1).Position.Set( 87.0f, 5.0f, 0 );

		//	create neon lamps
		for ( int i=0; i<3; i++ )
		{
			m_pnlLamp[i] = sx_new( sx::gui::PanelEx );
			m_pnlLamp[i]->SetParent( m_pnlUpdate[0] );
			m_pnlLamp[i]->SetSize( float2(32, 32) );
			m_pnlLamp[i]->GetElement(0)->SetTextureSrc( L"gui_pnlUpdateLevel4.txr" );
			m_pnlLamp[i]->State_GetByIndex(0).Blender.Set( 0.1f, 0.73f );
			m_pnlLamp[i]->State_GetByIndex(0).Color.w = 0.0f;
			m_pnlLamp[i]->State_Add();
			m_pnlLamp[i]->State_GetByIndex(1).Color.w = 1.0f;

			switch ( i )
			{
			case 0:
				m_pnlLamp[i]->State_GetByIndex(0).Position.Set( 1.5f, 34.5f, 0 );
				m_pnlLamp[i]->State_GetByIndex(1).Position.Set( 1.5f, 34.5f, 0 );
				break;
			case 1:
				m_pnlLamp[i]->State_GetByIndex(0).Position.Set( -35.5f, -29.5f, 0 );
				m_pnlLamp[i]->State_GetByIndex(0).Rotation.z = sx::math::DegToRad( 120.0f );
				m_pnlLamp[i]->State_GetByIndex(1).Position.Set( -35.5f, -29.5f, 0 );
				m_pnlLamp[i]->State_GetByIndex(1).Rotation.z = sx::math::DegToRad( 120.0f );
				break;
			case 2:
				m_pnlLamp[i]->State_GetByIndex(0).Position.Set( 37.5f, -29.5f, 0 );
				m_pnlLamp[i]->State_GetByIndex(0).Rotation.z = sx::math::DegToRad( -120.0f );
				m_pnlLamp[i]->State_GetByIndex(1).Position.Set( 37.5f, -29.5f, 0 );
				m_pnlLamp[i]->State_GetByIndex(1).Rotation.z = sx::math::DegToRad( -120.0f );
				break;
			}

		}

		//	create sell panel
		m_pnlSell = sx_new( sx::gui::PanelEx );
		m_pnlSell->SetSize( float2(128, 128) );
		m_pnlSell->SetParent( m_back );
		m_pnlSell->State_GetByIndex(0).Position.Set( -413.0f, -80.0f, 0 );
		m_pnlSell->State_Add();
		m_pnlSell->State_GetByIndex(1).Position.Set( -413.0f, -13.0f, 0 );
		m_pnlSell->GetElement(0)->SetTextureSrc( L"gui_pnlSell.txr" );

		//  create buttons
		for (int i=0; i<3; i++)
		{
			m_pnlButton[i] = sx_new( sx::gui::PanelEx );
			m_pnlButton[i]->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
			m_pnlButton[i]->SetParent( m_pnlUpdate[0] );
			m_pnlButton[i]->SetSize( float2(64, 64) );

			if ( i==0 || i==1 )
				m_pnlButton[i]->State_GetByIndex(0).Position.Set( 1.5f, -7.0f, 0 );

			m_pnlButton[i]->State_GetByIndex(0).Scale.Set( 1, 1, 1 );
			m_pnlButton[i]->State_GetByIndex(0).Blender.Set(0.6f, 0.7f);
			m_pnlButton[i]->State_Add();
			m_pnlButton[i]->State_GetByIndex(1).Scale.Set( 1.2f, 1.2f, 1 );
			m_pnlButton[i]->State_GetByIndex(1).Blender.Set(0.6f, 0.7f);

			String textureName;
			textureName.Format( L"gui_pnlTowerEdit_btn%d.txr", i );
			m_pnlButton[i]->GetElement(0)->SetTextureSrc( textureName );

			SEGAN_GUI_SET_ONCLICK( m_pnlButton[i], Mechanic_TowerEdit::OnButtonClick );
		}
		m_pnlButton[2]->SetParent( m_pnlSell );
		m_pnlEditor1->SetParent( m_back );


		//	create labels
		m_lblHealth = sx_new( sx::gui::Label );
		m_lblHealth->SetParent( m_back );
		m_lblHealth->SetSize( float2(95, 30) );
		m_lblHealth->Position().Set( -60.0f, -10.0f, 0.0f );
		m_lblHealth->GetElement(0)->Color().a = 0.0f;
		m_lblHealth->GetElement(1)->Color().a = 0.85f;
		m_lblHealth->SetFont( L"Font_rob_twedit_health.fnt" );

		m_lblXP = sx_new( sx::gui::Label );
		m_lblXP->SetParent( m_back );
		m_lblXP->SetSize( float2(75, 25) );
		m_lblXP->Position().Set( -265.0f, -45.0f, 0.0f );
		m_lblXP->GetElement(0)->Color().a = 0.0f;
		m_lblXP->GetElement(1)->Color().a = 0.85f;
		m_lblXP->SetFont( L"Font_rob_twedit_info.fnt" );

		m_lblDamage = sx_new( sx::gui::Label );
		m_lblDamage->SetParent( m_back );
		m_lblDamage->SetSize( float2(70, 25) );
		m_lblDamage->Position().Set( -153.0f, -45.0f, 0.0f );
		m_lblDamage->GetElement(0)->Color().a = 0.0f;
		m_lblDamage->GetElement(1)->Color().a = 0.85f;
		m_lblDamage->SetFont( L"Font_rob_twedit_info.fnt" );

		m_lblFireRate = sx_new( sx::gui::Label );
		m_lblFireRate->SetParent( m_back );
		m_lblFireRate->SetSize( float2(70, 25) );
		m_lblFireRate->Position().Set( -52.0f, -45.0f, 0.0f );
		m_lblFireRate->GetElement(0)->Color().a = 0.0f;
		m_lblFireRate->GetElement(1)->Color().a = 0.85f;
		m_lblFireRate->SetFont( L"Font_rob_twedit_info.fnt" );


		//	create images for labels
		sx::gui::Panel* pnl = sx_new( sx::gui::Panel );
		pnl->SetParent( m_lblHealth );
		pnl->SetSize( float2(32, 32) );
		pnl->GetElement(0)->SetTextureSrc( L"gui_iconHeart.txr" );
		//pnl->GetElement(0)->Color() = 0xFFF2C795;
		pnl->Position().Set( -60.0f, 2.0f, 0.0f );

		pnl = sx_new( sx::gui::Panel );
		pnl->SetParent( m_lblXP );
		pnl->SetSize( float2(32, 32) );
		pnl->GetElement(0)->SetTextureSrc( L"gui_iconXP.txr" );
		//pnl->GetElement(0)->Color() = 0xFFF2C795;
		pnl->Position().Set( -50.0f, 2.0f, 0.0f );

		pnl = sx_new( sx::gui::Panel );
		pnl->SetParent( m_lblDamage );
		pnl->SetSize( float2(32, 32) );
		pnl->GetElement(0)->SetTextureSrc( L"gui_iconDamage.txr" );
		//pnl->GetElement(0)->Color() = 0xFFF2C795;
		pnl->Position().Set( -50.0f, 2.0f, 0.0f );

		pnl = sx_new( sx::gui::Panel );
		pnl->SetParent( m_lblFireRate );
		pnl->SetSize( float2(32, 32) );
		pnl->GetElement(0)->SetTextureSrc( L"gui_iconFireRate.txr" );
		//pnl->GetElement(0)->Color() = 0xFFF2C795;
		pnl->Position().Set( -50.0f, 2.0f, 0.0f );

		// add editor panel to the game
		g_game->m_gui->Add_Front( m_back );
	}

	void Mechanic_TowerEdit::Finalize( void )
	{
		sx_callstack_push(Mechanic_TowerEdit::Finalize());

		g_game->m_gui->Remove( m_back );

		sx_delete_and_null( m_back );
	}

	void Mechanic_TowerEdit::ProcessInput( bool& inputHandled, float elpsTime )
	{
		if ( NotInGame() || g_game->m_mouseMode == MS_CreateTower )
			return;

		sx_callstack_push(Mechanic_TowerEdit::ProcessInput());

		if ( inputHandled )
		{
			for (int i=0; i<3; i++)
				m_pnlButton[i]->State_SetIndex(0);
			return;
		}

		Entity* selectedTower = Entity::GetSelected();
		if ( selectedTower && SEGAN_KEYDOWN(0, SX_INPUT_KEY_ESCAPE) || SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_RIGHT) )
		{
			Entity::SetSelected(NULL);
			inputHandled = true;
			return;
		}

		if ( selectedTower && selectedTower->m_partyCurrent == PARTY_TOWER )
		{
			if ( SEGAN_KEYUP(0, SX_INPUT_KEY_R) )
			{
				if ( selectedTower->m_health.icur < selectedTower->m_health.imax )
					OnButtonClick( m_pnlButton[1] );	//	repair the tower
				else
					OnButtonClick( m_pnlButton[0] );	//	upgrade the tower
			}
			else if ( SEGAN_KEYUP(0, SX_INPUT_KEY_Y) )
			{
				OnButtonClick( m_pnlButton[2] );		//	sell the tower
			}
		}

		//  handle gui input
		for (int i=0; i<3; i++)
		{
			bool res = false;
			m_pnlButton[i]->ProcessInput( res );

			if ( res )
			{
				inputHandled = true;
				m_pnlButton[i]->State_SetIndex(1);
			}
			else m_pnlButton[i]->State_SetIndex(0);
		}
		if ( inputHandled ) return;

		if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) )		//  pick tower
		{
			sx::math::Ray ray = sx::core::Renderer::GetCamera()->GetRay( SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0) );
			msg_IntersectRay msgRay( NMT_MESH, ray, msg_IntersectRay::BOX, NULL );
			sx::core::Scene::GetNodeByRay(msgRay);

			Entity::SetSelected(NULL);
			for ( int i=0; i<32; i++ )
			{
				if ( msgRay.results[i].node )
				{
					sx::core::PNode node = sx::core::PNode( msgRay.results[i].node );
					if ( node->GetUserData() && (node->GetUserTag() == PARTY_TOWER || node->GetUserTag() == PARTY_ENEMY) )
					{
						Entity* pe = (Entity*)node->GetUserData();
						if ( pe->m_health.icur )
						{
							Entity::SetSelected( pe );
							break;
						}

					}
				}
			}
		}
	}

	void Mechanic_TowerEdit::Update( float elpsTime )
	{
		sx_callstack_push(Mechanic_TowerEdit::Update());

		if ( !g_game->m_game_currentLevel || g_game->m_game_paused )
		{
			m_back->State_SetIndex(0);
			m_pnlSell->State_SetIndex(0);
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		//  show edit panel
		Entity* selectedEntity = g_game->m_mouseMode != MS_Null ? NULL : Entity::GetSelected();

		if ( selectedEntity && selectedEntity->m_initialized && selectedEntity->m_health.icur )
		{
			int maxLevel = selectedEntity->m_maxLevel;
			int curLevel = selectedEntity->m_level;

			prpAttack* pAttack = &(selectedEntity->m_curAttack);
			prpAttack* pMaxAttack = &(selectedEntity->m_attackLevel[maxLevel]);

			//int lampLevel = curLevel - 1;
			m_pnlUpdate[1]->State_SetIndex( curLevel == 1 );
			m_pnlUpdate[2]->State_SetIndex( curLevel == 2 );
			m_pnlUpdate[3]->State_SetIndex( curLevel >= 3 );
			m_pnlLamp[0]->State_SetIndex( curLevel >= 4 );
			m_pnlLamp[1]->State_SetIndex( curLevel >= 5 );
			m_pnlLamp[2]->State_SetIndex( curLevel >= 6 );
			
			//  verify that the selected entity is a tower
			if ( selectedEntity->m_partyCurrent == PARTY_TOWER )
			{
				m_back->State_SetIndex(1);
				m_pnlUpdate[0]->State_SetIndex(1);
				m_pnlSell->State_SetIndex(1);

				m_pnlButton[0]->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
				m_pnlButton[1]->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
				m_pnlButton[2]->AddProperty( SX_GUI_PROPERTY_ACTIVATE );

				//  verify that this tower need repair
				if ( selectedEntity->m_health.icur < selectedEntity->m_health.imax )
				{
					m_pnlButton[1]->AddProperty( SX_GUI_PROPERTY_VISIBLE );
					m_pnlButton[0]->RemProperty( SX_GUI_PROPERTY_VISIBLE );

					//  update hint of buttons
					str1024 strHint;
					int d = selectedEntity->m_health.imax - selectedEntity->m_health.icur;		
					strHint.Format( s_te_repair, d, g_game->m_player->m_gold);
					m_pnlButton[1]->SetHint( strHint );
				}
				else
				{
					m_pnlButton[0]->AddProperty( SX_GUI_PROPERTY_VISIBLE );
					m_pnlButton[1]->RemProperty( SX_GUI_PROPERTY_VISIBLE );

					//  update hint of buttons
					if ( curLevel < maxLevel  )
					{
						str1024 strHint;
						strHint.Format( s_te_upgrade, g_game->m_player->m_gold, selectedEntity->m_cost[curLevel+1], int(selectedEntity->m_experience), selectedEntity->m_costXP[curLevel+1] );
						m_pnlButton[0]->SetHint( strHint );
					}
					else m_pnlButton[0]->SetHint( s_te_fulupgrade );
				}

				//  update hint of buttons
				int cost = 0;
				for (int i=0; i<=curLevel; i++)
					cost += selectedEntity->m_cost[ i ];

				float sellPercent = 0.7f + g_game->m_upgrades.general_sell_income;
				cost = int( sx::cmn::Round( cost * sellPercent ) );

				str1024 strHint;
				strHint.Format( s_te_sell, cost );
				m_pnlButton[2]->SetHint( strHint );


				//	update info
				strHint.Format( L"%d/%d", selectedEntity->m_health.icur, selectedEntity->m_health.imax );
				m_lblHealth->SetText( strHint );

				if ( curLevel < maxLevel )
					strHint.Format( L"%d/%d", int(selectedEntity->m_experience), selectedEntity->m_costXP[curLevel+1] );
				else
					strHint = s_te_xp;
				m_lblXP->SetText( strHint );

				int av = int( pAttack->physicalDamage + pAttack->electricalDamage );
				strHint.Format( L"%d-%d", av-1, av+1 );
				m_lblDamage->SetText( strHint );

				if ( pAttack->rate >= 3 )
					m_lblFireRate->SetText( s_te_fast );
				else if ( pAttack->rate > 1 )
					m_lblFireRate->SetText( s_te_medium );
				else
					m_lblFireRate->SetText(s_te_low );

			}
			else
			{
				m_back->State_SetIndex(2);
				m_pnlUpdate[0]->State_SetIndex(0);
				m_pnlSell->State_SetIndex(0);

				m_pnlButton[0]->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
				m_pnlButton[1]->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
				m_pnlButton[2]->RemProperty( SX_GUI_PROPERTY_ACTIVATE );

				//	update info
				str1024 str;
				str.Format( L"%d/%d", selectedEntity->m_health.icur, selectedEntity->m_health.imax );
				m_lblHealth->SetText( str );

				str.Format( L"%d", int(selectedEntity->m_experience) );
				m_lblXP->SetText( str );

				int av = int( pAttack->physicalDamage + pAttack->electricalDamage );
				if ( av )
				{
					str.Format( L"%d-%d", av-1, av+1 );
					m_lblDamage->SetText( str );

					if ( pAttack->rate > 4 )
						m_lblFireRate->SetText( s_te_fast );
					else if ( pAttack->rate > 1 )
						m_lblFireRate->SetText( s_te_medium );
					else
						m_lblFireRate->SetText( s_te_low );
				}
				else
				{
					m_lblDamage->SetText( s_te_none );
					m_lblFireRate->SetText( s_te_none );
				}

			}
		}
		else
		{
			m_back->State_SetIndex(0);
			m_pnlUpdate[0]->State_SetIndex(0);
			m_pnlSell->State_SetIndex(0);

			m_pnlButton[0]->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
			m_pnlButton[1]->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
			m_pnlButton[2]->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
		}

	}

	void Mechanic_TowerEdit::OnButtonClick( sx::gui::PControl Sender )
	{
		sx_callstack_push(Mechanic_TowerEdit::OnButtonClick());

		Entity* selectedTower = Entity::GetSelected();
		if ( !selectedTower || !selectedTower->m_initialized || !selectedTower->m_health.icur || selectedTower->m_partyCurrent != PARTY_TOWER )
			return;
		

		if ( Sender == m_pnlButton[1] )/////////////////////////////////////////	repair tower
		{
			int neededBlood = selectedTower->m_health.imax - selectedTower->m_health.icur;
			int currentBlood = g_game->m_player->m_gold > neededBlood ? neededBlood : g_game->m_player->m_gold;

			selectedTower->m_health.icur += currentBlood;
			selectedTower->MsgProc( GMT_DAMAGE, NULL );
			g_game->m_player->m_gold -= currentBlood / 2;

			if ( selectedTower->m_node )
			{
				msg_SoundPlay msgSnd( false, 0, 0, L"repair" );
				selectedTower->m_node->MsgProc( MT_SOUND_PLAY, &msgSnd );
			}
		}
		
		else if ( Sender == m_pnlButton[0] )////////////////////////////////////	upgrade tower
		{
			int level = selectedTower->m_level + 1;

			//  verify that the level is exist in tower
			if ( !selectedTower->m_cost[level] ) return;

			if ( level > selectedTower->m_maxLevel )
			{
				//	say to player by the way
				msg_SoundPlay msg( true, 0, 0, L"towerEdit", 0 );
				g_game->m_gui->m_main->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
				return;
			}

			if ( g_game->m_player->m_gold < selectedTower->m_cost[level]  || selectedTower->m_experience < selectedTower->m_costXP[level] )
			{
				//  say to player by the way
				msg_SoundPlay msg( true, 0, 0, L"towerEdit", 1 );
				g_game->m_gui->m_main->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );
				return;
			}
			else g_game->m_player->m_gold -= selectedTower->m_cost[level];

			selectedTower->SetLevel( level );
			
		}
		
		else if ( Sender == m_pnlButton[2] )/////////////////////////////////////	sell tower
		{
			int level = selectedTower->m_level;

			int cost  = 0;
			for (int i=0; i<=level; i++)
				cost += selectedTower->m_cost[ i ];

			//  delete tower
			selectedTower->m_health.icur = HEALTH_SELL;

			float sellPercent = 0.7f + g_game->m_upgrades.general_sell_income;

			g_game->m_player->m_gold += int( sx::cmn::Round( (float)cost * sellPercent ) );
			g_game->m_achievements[7].AddValue();
			g_game->m_achievements[8].AddValue();


			Entity::SetSelected( NULL );
		}
	}

	void Mechanic_TowerEdit::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		sx_callstack_push(Mechanic_TowerEdit::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

		switch ( msg )
		{
		case GMT_GAME_RESETING:
		case GMT_GAME_START:
			if ( g_game->m_achievements[7].value < g_game->m_achievements[7].range )
				g_game->m_achievements[7].value = 0;
		case GMT_GAME_PAUSED:
			break;

		case GMT_GAME_RESET:
		case GMT_LEVEL_LOAD:
			{
				str1024 str = sx::sys::FileManager::Project_GetDir();
				str << L"strings.txt";

				Scripter script;
				script.Load( str );

				for (int i=0; i<script.GetObjectCount(); i++)
				{
					str512 tmpStr;
					if ( script.GetString(i, L"Type", tmpStr) )
					{
						if ( tmpStr == L"Strings" )
						{
							if ( !script.GetString(i, L"Name", tmpStr) )
								continue;

							if ( tmpStr == L"TowerEdit" )
							{
								script.GetString( i, L"upgrade", s_te_upgrade );			s_te_upgrade.Replace( L"\\n", L"\n" );
								script.GetString( i, L"repair", s_te_repair );				s_te_repair.Replace( L"\\n", L"\n" );
								script.GetString( i, L"sell", s_te_sell );					s_te_sell.Replace( L"\\n", L"\n" );
								script.GetString( i, L"fulupgrade", s_te_fulupgrade );		s_te_fulupgrade.Replace( L"\\n", L"\n" );
								script.GetString( i, L"xp", s_te_xp );						s_te_xp.Replace( L"\\n", L"\n" );
								script.GetString( i, L"fast", s_te_fast );					s_te_fast.Replace( L"\\n", L"\n" );
								script.GetString( i, L"medium", s_te_medium );				s_te_medium.Replace( L"\\n", L"\n" );
								script.GetString( i, L"low", s_te_low );					s_te_low.Replace( L"\\n", L"\n" );
								script.GetString( i, L"none", s_te_none );					s_te_none.Replace( L"\\n", L"\n" );

								break;
							}
						}
					}
				}
			}
			break;
		}
	}
} // namespace GM
