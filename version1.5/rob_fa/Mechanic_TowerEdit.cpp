#include "Mechanic_TowerEdit.h"
#include "Game.h"
#include "GameGUI.h"
#include "GameConfig.h"
#include "Player.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Scripter.h"
#include "GameStrings.h"



sx::gui::Panel* te_create_icon(sx::gui::Control* parent, float width, float height, float x, float y, const wchar* texture)
{
	sx::gui::Panel* res = sx_new( sx::gui::Panel );
	res->SetParent( parent );
	res->SetSize( float2(width, height) );
	res->GetElement(0)->SetTextureSrc( texture );
	res->Position().Set( x, y, 0.0f );
	return res;
}

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
		sx_callstack();

		ZeroMemory( m_pnlUpdate,	sizeof(m_pnlUpdate)	);
		ZeroMemory( m_pnlLamp,		sizeof(m_pnlLamp)	);
		ZeroMemory( m_pnlButton,	sizeof(m_pnlButton)	);
	}

	Mechanic_TowerEdit::~Mechanic_TowerEdit( void )
	{
		sx_callstack();

	}

	void Mechanic_TowerEdit::Initialize( void )
	{
		sx_callstack();

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
		m_lblHealth = create_label( m_back, 187, 120, 50, -60, -22, 0 );

		m_lblXP = create_label( m_back, 189, 75, 40, -265, -48, 0 );
		m_lblXP->GetElement(1)->Color().a = 0.85f;

		m_lblDamage = create_label( m_back, 189, 70, 40, -153, -48, 0 );
		m_lblDamage->GetElement(1)->Color().a = 0.85f;

		m_lblFireRate = create_label( m_back, 189, 70, 40, -52, -48, 0 );
		m_lblFireRate->GetElement(1)->Color().a = 0.85f;

		//	create images for labels
		te_create_icon( m_back, 32, 32, -124, -10, L"gui_iconHeart.txr" );
		te_create_icon( m_back, 32, 32, -315, -35, L"gui_iconXP.txr" );
		te_create_icon( m_back, 32, 32, -200, -35, L"gui_iconDamage.txr" );
		te_create_icon( m_back, 32, 32, -100, -35, L"gui_iconFireRate.txr" );

		m_guide = sx_new( GameGuide );
		m_guide->m_back->SetParent( m_pnlUpdate[0] );

		// add editor panel to the game
		g_game->m_gui->Add_Front( m_back );
	}

	void Mechanic_TowerEdit::Finalize( void )
	{
		sx_callstack();

		sx_delete_and_null(m_guide);

		g_game->m_gui->Remove( m_back );
		sx_delete_and_null( m_back );
	}

	void Mechanic_TowerEdit::ProcessInput( bool& inputHandled, float elpsTime )
	{
		if ( NotInGame() || g_game->m_mouseMode == MS_CreateTower )
			return;

		sx_callstack();

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
		sx_callstack();

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

				if ( g_game->m_player->m_profile.level_played == 0 && selectedEntity->m_upgradeReady && g_game->m_guides[GUIDE_UPGRADE]->m_fresh )
				{
					m_guide->SetText( g_game->m_guides[GUIDE_UPGRADE]->Use() );
					m_guide->Show( GameGuide::BOTTOMRIGHT, -20.0f, 20.0f, 120 );
				}

				m_pnlButton[0]->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
				m_pnlButton[1]->AddProperty( SX_GUI_PROPERTY_ACTIVATE );
				m_pnlButton[2]->AddProperty( SX_GUI_PROPERTY_ACTIVATE );

				//  verify that this tower need repair
				if ( selectedEntity->m_health.icur < selectedEntity->m_health.imax )
				{
					m_pnlButton[1]->AddProperty( SX_GUI_PROPERTY_VISIBLE );
					m_pnlButton[0]->RemProperty( SX_GUI_PROPERTY_VISIBLE );

					//  update hint of buttons
					GameString* desc = g_game->m_strings->Get( 173 );
					if ( desc )
					{
						int d = selectedEntity->m_health.imax - selectedEntity->m_health.icur;	
						swprintf_s( desc->text, 512, desc->base, d, g_game->m_player->m_gold );

						m_pnlButton[1]->SetHint( L"172\n173" );
					}					
				}
				else
				{
					m_pnlButton[0]->AddProperty( SX_GUI_PROPERTY_VISIBLE );
					m_pnlButton[1]->RemProperty( SX_GUI_PROPERTY_VISIBLE );

					//  update hint of buttons
					if ( curLevel < maxLevel  )
					{
						GameString* desc = g_game->m_strings->Get( 171 );
						if ( desc )
						{
							swprintf_s( desc->text, 512, desc->base, g_game->m_player->m_gold, selectedEntity->m_cost[curLevel+1], int(selectedEntity->m_experience), selectedEntity->m_costXP[curLevel+1] );
							m_pnlButton[0]->SetHint( L"170\n171" );
						}	
					}
					else m_pnlButton[0]->SetHint( L"170\n176" );
				}

				//  update hint of buttons
				GameString* desc = g_game->m_strings->Get( 175 );
				if ( desc )
				{
					int cost = 0;
					for (int i=0; i<=curLevel; i++)
						cost += selectedEntity->m_cost[ i ];

					float sellPercent = 0.7f + g_game->m_upgrades.general_sell_income;
					cost = int( sx::cmn::Round( cost * sellPercent ) );

					swprintf_s( desc->text, 512, desc->base, cost );
					m_pnlButton[2]->SetHint( L"174\n175" );
				}	

				//	update info
				str64 strHint;
				strHint.Format( L"%d/%d", selectedEntity->m_health.icur, selectedEntity->m_health.imax );
				m_lblHealth->SetText( strHint );

				if ( curLevel < maxLevel )
				{
					strHint.Format( L"%d/%d", int(selectedEntity->m_experience), selectedEntity->m_costXP[curLevel+1] );
					m_lblXP->SetText( strHint );
				}
				else m_lblXP->SetText( g_game->m_strings->Get(177)->text );

				int av = int( pAttack->physicalDamage + pAttack->electricalDamage );
				strHint.Format( L"%d-%d", av - 1, av + 1 );
				m_lblDamage->SetText( strHint );

				if ( pAttack->rate >= 3 )
					update_label( m_lblFireRate, 178 );
				else if ( pAttack->rate > 1 )
					update_label( m_lblFireRate, 179 );
				else
					update_label( m_lblFireRate, 180 );
			}
			else
			{
				m_back->State_SetIndex(2);
				m_pnlUpdate[0]->State_SetIndex(0);
				m_pnlSell->State_SetIndex(0);
				m_guide->Hide();

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
					str.Format( L"%d-%d", av - 1, av + 1 );
					m_lblDamage->SetText( str );

					if ( pAttack->rate > 4 )
						update_label( m_lblFireRate, 178 );
					else if ( pAttack->rate > 1 )
						update_label( m_lblFireRate, 179 );
					else
						update_label( m_lblFireRate, 180 );
				}
				else
				{
					update_label( m_lblDamage, 181 );
					update_label( m_lblFireRate, 181 );
				}
			}
		}
		else
		{
			m_back->State_SetIndex(0);
			m_pnlUpdate[0]->State_SetIndex(0);
			m_pnlSell->State_SetIndex(0);
			m_guide->Hide();

			m_pnlButton[0]->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
			m_pnlButton[1]->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
			m_pnlButton[2]->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
		}

		m_guide->Update(elpsTime);
	}

	void Mechanic_TowerEdit::OnButtonClick( sx::gui::PControl Sender )
	{
		sx_callstack();

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

			float repairParam = (float)neededBlood / (float)selectedTower->m_health.imax;
			if ( repairParam < 0.3f )
			{
				OnButtonClick( m_pnlButton[0] );
			}
		}
		
		else if ( Sender == m_pnlButton[0] )////////////////////////////////////	upgrade tower
		{
			m_guide->Hide();

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

			int addGold = int( sx::cmn::Round( (float)cost * sellPercent ) );
			g_game->m_player->m_gold += addGold;

#if USE_STEAM_SDK
			g_game->m_steam.CallStat( EST_Gaining, ESC_InPlay, (float)addGold + 0.5f );

			g_game->m_steam.CallAchievement( EAT_Tower_Dealer, ESC_InPlay );
			g_game->m_steam.CallAchievement( EAT_Real_Estate, ESC_InPlay );
			g_game->m_steam.CallAchievement( EAT_Clever_Dealer, ESC_InPlay );
#else
			g_game->m_achievements[EAT_Tower_Dealer].AddValue();
			g_game->m_achievements[EAT_Real_Estate].AddValue();
#endif
			Entity::SetSelected( NULL );
		}
	}

	void Mechanic_TowerEdit::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		sx_callstack_param(Mechanic_TowerEdit::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

		switch ( msg )
		{
		case GMT_GAME_RESETING:
		case GMT_GAME_START:

#if USE_STEAM_SDK
			g_game->m_steam.CallAchievement( EAT_Tower_Dealer, ESC_OnStart );
#else
			if ( g_game->m_achievements[EAT_Tower_Dealer].value < g_game->m_achievements[EAT_Tower_Dealer].range )
				g_game->m_achievements[EAT_Tower_Dealer].value = 0;
#endif

		case GMT_GAME_PAUSED:
			break;

		case GMT_GAME_RESET:
		case GMT_LEVEL_LOAD:
			break;
		}
	}
} // namespace GM
