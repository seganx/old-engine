#include "Mechanic_MT_Machinegun.h"
#include "Game.h"
#include "Player.h"
#include "Scripter.h"
#include "Entity.h"
#include "GameGUI.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Projectile.h"
#include "ProjectileManager.h"

#define MANUAL_MACHINEGUN_FIRE	1


namespace GM
{

	Mechanic_MT_Machinegun::Mechanic_MT_Machinegun( void ) : Mechanic()
		, m_node(null)
		, m_nodeCamera(null)
		, m_nodeWeapon(null)
		, m_pipeIndex(0)
		, m_shootTime(0)
		, m_Rot(0,0,0)
		, m_RotOffset(0,0,0)
		, m_RotMax(0.5f, 1.0f, 0)
		, m_shakeMagnitude(1.0f)
		, m_forceFeedback(0.01f)
		, m_fire(0)
		, m_selected(false)
		, m_energyPerBullet(1)
		, m_maxTemperature(0.0f)
		, m_curTemperature(0.0f)
		, m_warmingRate(0.0f)
		, m_coldingRate(0.0f)
		, m_temperatureBar(null)
		, m_bulletIndicator(null)
	{
		sx_callstack();

		ZeroMemory( m_nodePipe, sizeof(m_nodePipe) );
	}

	Mechanic_MT_Machinegun::~Mechanic_MT_Machinegun( void )
	{
		sx_callstack();

	}

	void Mechanic_MT_Machinegun::Initialize( void )
	{
		sx_callstack();

		m_temperatureBar = sx_new( sx::gui::ProgressBar );
		m_temperatureBar->SetSize( float2(128, 16) );
		m_temperatureBar->Rotation().Set( 0.0f, 0.0f, PI / 2.0f );
		m_temperatureBar->AddProperty( SX_GUI_PROPERTY_PROGRESSUV );
		m_temperatureBar->AddProperty( SX_GUI_PROPERTY_BILLBOARD );
		m_temperatureBar->AddProperty( SX_GUI_PROPERTY_3DSPACE );

		m_temperatureBar->GetElement(0)->SetTextureSrc( L"gui_healthBar0.txr" );
		m_temperatureBar->GetElement(1)->SetTextureSrc( L"gui_healthBar1.txr" );

		m_bulletIndicator = sx_new( sx::gui::Label );
		m_bulletIndicator->SetSize( float2(70, 25) );
		m_bulletIndicator->Position().Set( -450.0f, 240.0f, 0.0f );
		m_bulletIndicator->GetElement(0)->Color().a = 0.0f;
		m_bulletIndicator->GetElement(1)->Color().a = 0.85f;
		m_bulletIndicator->SetFont( L"Font_rob_twedit_info.fnt" );

		g_game->m_gui->Add_Back( m_temperatureBar );
		g_game->m_gui->Add_Back( m_bulletIndicator );
	}

	void Mechanic_MT_Machinegun::Finalize( void )
	{
		sx_callstack();

		g_game->m_gui->Remove( m_temperatureBar );
		g_game->m_gui->Remove( m_bulletIndicator );	

		sx_delete_and_null( m_temperatureBar );
		sx_delete_and_null( m_bulletIndicator );
	}

	void Mechanic_MT_Machinegun::ProcessInput( bool& inputHandled, float elpsTime )
	{
		if ( NotInGame() || inputHandled ) return;
		sx_callstack();

		if ( g_game->m_mouseMode == MS_Null )
		{
			//	it's in normal mode. here we can verify that any tower has been selected
			//	if a manual tower has been selected then we can display the GUI or set on the manual mode

			if ( SEGAN_KEYUP(0, SX_INPUT_KEY_MOUSE_LEFT) )
			{
				//	send a ray to the scene to select the node of the manual tower
				sx::math::Ray ray = sx::core::Renderer::GetCamera()->GetRay( SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0) );
				msg_IntersectRay msgRay( NMT_MESH, ray, msg_IntersectRay::BOX, NULL );
				sx::core::Scene::GetNodeByRay(msgRay);

				sx::core::PNode selectedNode = null;
				for ( int i=0; i<32; i++ )
				{
					if ( msgRay.results[i].node )
					{
						sx::core::PNode node = sx::core::PNode( msgRay.results[i].node );
						if ( node == m_node )
						{
							Entity::SetSelected( null );
							selectedNode = node;
							break;
						}
					}
				}

				//	if the tower has been selected and player clicked on that again we should set on manual mode
				if ( selectedNode )
				{
					if ( m_selected )
					{
						EnterToManual();
					}
					else
					{
						//	show the gui for tower
					}
				}
				else
				{
					//	hide the gui
				}
				m_selected = ( selectedNode != null );
			}
		}
		
		if ( g_game->m_mouseMode == MS_ManualTower )	//	player set on a manual tower
		{
			//	verify that this mechanic is focused
			if ( !IsFocused() ) return;

			//	just check the number of bullets and player does fire
			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_LEFT) )
			{
				if ( (!m_fire) && (g_game->m_player->m_energy >= m_energyPerBullet) && (m_curTemperature <= m_maxTemperature) )
				{
					m_fire = MANUAL_MACHINEGUN_FIRE;
				}
			}

			m_RotOffset.x += SEGAN_MOUSE_RLY(0) * 0.002f;
			m_RotOffset.y += SEGAN_MOUSE_RLX(0) * 0.002f;
			if ( m_RotOffset.x > m_RotMax.x ) m_RotOffset.x = m_RotMax.x;
			if ( m_RotOffset.y > m_RotMax.y ) m_RotOffset.y = m_RotMax.y;
			if ( m_RotOffset.x < -m_RotMax.x ) m_RotOffset.x = -m_RotMax.x;
			if ( m_RotOffset.y < -m_RotMax.y ) m_RotOffset.y = -m_RotMax.y;
			SEGAN_MOUSE_ABSX(0) = SEGAN_VP_WIDTH/2;
			SEGAN_MOUSE_ABSY(0) = SEGAN_VP_HEIGHT/2;
			inputHandled = true;

			if ( SEGAN_KEYUP(0, SX_INPUT_KEY_ESCAPE) || SEGAN_KEYUP(0, SX_INPUT_KEY_MOUSE_RIGHT) )
			{
				LeaveManual();
			}
		}
	}

	void Mechanic_MT_Machinegun::Update( float elpsTime )
	{
		m_curTemperature = m_fire ?
			(m_curTemperature + (m_warmingRate * elpsTime * 0.001f)) :
			(m_curTemperature - (m_coldingRate * elpsTime * 0.001f));

		if ( m_curTemperature < 0.0f )
		{
			m_curTemperature = 0.0f;
		}
		else if ( m_curTemperature > m_maxTemperature )
		{
			m_curTemperature = m_maxTemperature;
			m_fire = 0;

			msg_Particle msgPrtcl(SX_PARTICLE_SPRAY);
			m_nodePipe[ m_pipeIndex ]->MsgProc( MT_PARTICLE, &msgPrtcl);
		}

		m_temperatureBar->SetValue( m_curTemperature );
		
		//

		if ( NotInGame() || !m_nodeWeapon || !m_nodePipe[0] ) { return; }

		sx_callstack();

		{
			const int availableBullets = g_game->m_player->m_energy / m_energyPerBullet;

			str128 str;

			if ( availableBullets )
			{
				m_bulletIndicator->GetElement(1)->Color() = D3DColor(1.0f, 1.0f, 1.0f, 0.85f);
				str.Format( L"%d", availableBullets );
			}
			else
			{
				m_bulletIndicator->GetElement(1)->Color() = D3DColor(1.0f, 0.0f, 0.0f, 0.85f);
				str.SetText( "0" );
			}

			m_bulletIndicator->SetText( str );
		}

		m_shootTime += elpsTime;

		if ( g_game->m_mouseMode == MS_ManualTower && IsFocused() )
		{
			UpdateCamera( elpsTime );
		}

		//  invisible meshes of fire
		if ( m_shootTime < 200 && m_shootTime > 40 )
		{
			msg_Mesh msgMesh( SX_MESH_INVISIBLE );

			for (int i=0; i<4; i++)
			{
				if ( m_nodePipe[i] )
				{
					m_nodePipe[i]->MsgProc( MT_MESH, &msgMesh );
				}
			}
		}

		//	Blend Direction
		{
			const float dx = m_RotOffset.x - m_Rot.x;
			const float dy = m_RotOffset.y - m_Rot.y;
			const float dz = m_RotOffset.z - m_Rot.z;

			const float blendTime = elpsTime * 0.005f;
			m_Rot.y += dy * blendTime;
			m_Rot.x += dx * blendTime;
			m_Rot.z += dz * blendTime;
			m_nodeWeapon->SetRotation( m_Rot.x, m_Rot.y, m_Rot.z );
		}

		//  compute max shoot time depend on fire rate
		const float	maxShootTime = ( (m_attack.rate > 0.01f) ? (1000.0f / m_attack.rate) : 500.0f ) / MANUAL_MACHINEGUN_FIRE;
		if ( m_fire && (m_shootTime > maxShootTime) && (g_game->m_player->m_energy >= m_energyPerBullet) && (m_curTemperature <= m_maxTemperature) )
		{
			m_shootTime = 0;

			//  rotate and show the pipe
			msg_Mesh msgMesh( 0, SX_MESH_INVISIBLE );
			m_nodePipe[ m_pipeIndex ]->MsgProc( MT_MESH, &msgMesh );
			m_nodePipe[ m_pipeIndex ]->SetRotation( 0, 0, sx::cmn::Random(6.0f) );

			//  play a sound
			msg_SoundPlay msgSound(true, sx::cmn::Random(1000), 0, L"fire" );
			m_node->MsgProc( MT_SOUND_PLAY, &msgSound );

			//	spray bullet particle
			str256 bulletShell = L"bullet_shell"; bulletShell << m_pipeIndex;
			msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, bulletShell );
			m_nodeWeapon->MsgProc( MT_PARTICLE, &msgPar );

			ShootTheBullet( &m_attack, false );
		}
	}

	void Mechanic_MT_Machinegun::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		static int lastIndex = 0;

		switch (msg)
		{
		case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
			{
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

				for (int i=lastIndex; i<script.GetObjectCount(); i++)
				{
					str512 tmpStr;
					if ( script.GetString(i, L"Type", tmpStr) && tmpStr == L"ManualTower" )
					{
						if ( !script.GetString(i, L"Name", tmpStr) )
						{
							continue;
						}

						if ( tmpStr == L"Machinegun" )
						{
							if ( script.GetString(i, L"node", tmpStr) )
							{
								sx::core::ArrayPNode nodes(16);
								sx::core::Scene::GetNodesByName( tmpStr, nodes );
								m_node = nodes.Count() ? nodes[0] : null;
							}

							script.GetFloat(i, L"stunValue", m_attack.stunValue);
							script.GetFloat(i, L"stunTime", m_attack.stunTime);
							script.GetFloat(i, L"physicalDamage", m_attack.physicalDamage);
							script.GetFloat(i, L"electricalDamage", m_attack.electricalDamage);
							script.GetFloat(i, L"fireRate", m_attack.rate);
							script.GetFloat(i, L"maxPhi", m_RotMax.y);
							script.GetFloat(i, L"maxTheta", m_RotMax.x);
							script.GetFloat(i, L"shakeMagnitude", m_shakeMagnitude);
							script.GetFloat(i, L"forceFeedback", m_forceFeedback);
							script.GetInteger(i, L"energyPerBullet", m_energyPerBullet);
							script.GetFloat(i, L"maxTemperature", m_maxTemperature);
							script.GetFloat(i, L"warmingRate", m_warmingRate);
							script.GetFloat(i, L"coldingRate", m_coldingRate);

							if ( script.GetString(i, L"bullet", tmpStr) )
							{
								String::Copy( m_attack.bullet, 64, tmpStr );
								m_attack.projectile = ProjectileManager::GetTypeByName( m_attack.bullet );
							}

							lastIndex = i+1;
							break;
						}	//	if ( tmpStr == L"machinegun" )
					}
				}
			}
			break;	//	GMT_LEVEL_LOAD

		case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
			{						//////////////////////////////////////////////////////////////////////////
				m_selected = false;
				m_node = null;
				LeaveManual();
				lastIndex = 0;
			}
			break;	//	GMT_LEVEL_CLEAR

		case GMT_GAME_RESETING:
			m_attack.projectile = 0;
			lastIndex = 0;
			break;
		case GMT_GAME_RESET:
			MsgProc( 0, GMT_LEVEL_LOAD, 0 );
		case GMT_GAME_START:
		case GMT_GAME_END:
			{
				if ( m_node )
				{
					m_node->GetChildByName(L"weapon", m_nodeWeapon);
					m_node->GetChildByName(L"pipe0", m_nodePipe[0]);
					m_node->GetChildByName(L"pipe1", m_nodePipe[1]);
					m_node->GetChildByName(L"pipe2", m_nodePipe[2]);
					m_node->GetChildByName(L"pipe3", m_nodePipe[3]);

					//  stop particle spray
					msg_Particle msgPrtcl(0, SX_PARTICLE_SPRAY, 0, true );
					m_node->MsgProc( MT_PARTICLE, &msgPrtcl );
				}

				//  invisible meshes of fire and stop particles
				if ( m_nodeWeapon )
				{
					m_nodeWeapon->SetRotation( 0, 0, 0 );
					for (int i=0; i<4; i++)
					{
						if ( m_nodePipe[i] )
						{
							msg_Mesh msgMesh( SX_MESH_INVISIBLE );
							m_nodePipe[i]->MsgProc( MT_MESH, &msgMesh );
						}
					}
				}

				m_curTemperature = 0.0f;

				const float3 addPos( 0, m_node->GetBox_local().Max.y + 0.5f, 0 );
				m_temperatureBar->SetMax( m_maxTemperature );
				m_temperatureBar->SetValue( m_curTemperature );
				m_temperatureBar->Position() = m_node->GetPosition_world() + addPos;

				const int availableBullets = g_game->m_player->m_energy / m_energyPerBullet;
				str128 str;
				if ( availableBullets )
				{
					str.Format( L"%d", availableBullets );
					m_bulletIndicator->GetElement(1)->Color() = D3DColor(1.0f, 1.0f, 1.0f, 0.85f);
				}
				else
				{
					str.SetText("0");
					m_bulletIndicator->GetElement(1)->Color() = D3DColor(1.0f, 0.0f, 0.0f, 0.85f);
				}
				m_bulletIndicator->SetText( str );
			}
			//break;
		case GMT_GAME_PAUSED:
		case GMT_WAVE_FINISHED:
			{	
				LeaveManual();
				m_selected = false;
			}
			break;	//	GMT_GAME_RESET
		}
	}

	void Mechanic_MT_Machinegun::EnterToManual( void )
	{
		//  find weapon node
		if ( m_node )
		{
			m_node->GetChildByName(L"camera", m_nodeCamera);
			m_node->GetChildByName(L"weapon", m_nodeWeapon);
			m_node->GetChildByName(L"pipe0", m_nodePipe[0]);
			m_node->GetChildByName(L"pipe1", m_nodePipe[1]);
			m_node->GetChildByName(L"pipe2", m_nodePipe[2]);
			m_node->GetChildByName(L"pipe3", m_nodePipe[3]);
		}

		//  invisible meshes of fire and stop particles
		if ( m_nodeWeapon )
		{
			for (int i=0; i<4; i++)
			{
				if ( m_nodePipe[i] )
				{
					msg_Mesh msgMesh( SX_MESH_INVISIBLE );
					m_nodePipe[i]->MsgProc( MT_MESH, &msgMesh );

					//  stop particle spray
					msg_Particle msgPrtcl(0, SX_PARTICLE_SPRAY, 0, true );
					m_nodePipe[i]->MsgProc( MT_PARTICLE, &msgPrtcl );
				}
			}
		}

		if ( m_nodeWeapon && m_nodePipe[0] && m_nodeCamera )
		{
			SetFocused( true );
			g_game->m_mouseMode = MS_ManualTower;
		}
		else
		{
			SetFocused( false );
			g_game->m_mouseMode = MS_Null;
		}

		m_shootTime = 0.0f;
		m_fire = 0;

		m_temperatureBar->AddProperty( SX_GUI_PROPERTY_VISIBLE );
		m_bulletIndicator->AddProperty( SX_GUI_PROPERTY_VISIBLE );
	}

	void Mechanic_MT_Machinegun::LeaveManual( void )
	{
		if ( g_game->m_mouseMode == MS_ManualTower )
		{
			g_game->m_mouseMode = MS_Null;
		}

		m_nodeCamera = null;
		m_nodeWeapon = null;
		ZeroMemory( m_nodePipe, sizeof(m_nodePipe) );

		m_temperatureBar->RemProperty( SX_GUI_PROPERTY_VISIBLE );
		m_bulletIndicator->RemProperty( SX_GUI_PROPERTY_VISIBLE );
	}

	void Mechanic_MT_Machinegun::UpdateCamera( float elpsTime )
	{
		sx::core::Camera camera;
		camera.Far = CAMERA_FAR;
		camera.Aspect = SEGAN_VP_WIDTH / SEGAN_VP_HEIGHT;
		camera.FOV = PI / (1.7f + camera.Aspect);
		camera.Eye = m_nodeCamera->GetPosition_world();

		float3 dir(0, 0, 0.1f);
		if ( m_fire )
		{
			const float extent_1 = 0.0005f * m_shakeMagnitude;
			const float extent_2 = 0.002f * m_shakeMagnitude;
			dir.x += sx_random_f_limit(-extent_1, extent_1);
			dir.y += sx_random_f_limit(-extent_1, extent_1);
			dir.z += sx_random_f_limit(-extent_2, extent_2);
		}
		dir.Transform_Norm( dir, m_nodeCamera->GetMatrix_world() );
		camera.Eye += dir;
		camera.At = camera.Eye + dir * 10.0f;
		camera.Up.Set(0, 1, 0);

		sx::core::PCamera pCam = sx::core::Renderer::GetCamera();

		pCam->Far = camera.Far;
		pCam->FOV += (camera.FOV - pCam->FOV) * 0.01f * elpsTime;
		pCam->Aspect = camera.Aspect;

#if 1
		float bspeed = m_fire ? 0.04f : 0.03f;
		pCam->At.x += (camera.At.x - pCam->At.x) * bspeed * elpsTime;
		pCam->At.y += (camera.At.y - pCam->At.y) * bspeed * elpsTime;
		pCam->At.z += (camera.At.z - pCam->At.z) * bspeed * elpsTime;

		bspeed = m_fire ? 0.02f : 0.025f;
		pCam->Eye.x += (camera.Eye.x - pCam->Eye.x) * bspeed * elpsTime;
		pCam->Eye.y += (camera.Eye.y - pCam->Eye.y) * bspeed * elpsTime;
		pCam->Eye.z += (camera.Eye.z - pCam->Eye.z) * bspeed * elpsTime;
#else
		//	TEST	
		pCam->Eye = camera.Eye;
		pCam->At = camera.At;
#endif

		sx::core::Renderer::SetCamera( pCam );
	}

	void Mechanic_MT_Machinegun::ShootTheBullet( const prpAttack* attack, const bool special )
	{
		sx_callstack();
		sx_assert( attack->projectile );
		
		//	compute position and direction of bullet
		float3 face(0,0,1), dir, pos;
		dir.Transform_Norm( face, m_nodeWeapon->GetMatrix_world() );
		pos	= m_nodePipe[ m_pipeIndex ]->GetPosition_world();
		pos	+= dir * 3.0f;

		//	search for a target to damage to
		Entity* target = null;
		if ( m_fire == MANUAL_MACHINEGUN_FIRE )
		{
			//sx::math::Ray ray( pos, dir );
			sx::math::Ray ray = sx::core::Renderer::GetCamera()->GetRay( SEGAN_VP_WIDTH / 2, SEGAN_VP_HEIGHT / 2 );
			msg_IntersectRay msgRay( NMT_MESH, ray, msg_IntersectRay::BOX, NULL );
			sx::core::Scene::GetNodeByRay(msgRay);

			for ( int i=0; i<32; i++ )
			{
				if ( msgRay.results[i].node )
				{
					sx::core::PNode node = sx::core::PNode( msgRay.results[i].node );
					if ( node->GetUserData() && node->GetUserTag() == PARTY_ENEMY )
					{
						Entity* pe = (Entity*)node->GetUserData();
						if ( pe->m_health.icur )
						{
							target = pe;
							break;
						}
					}
				}
			}
		}

		//  shoot the bullet
		{
			Projectile* proj			= attack->projectile->Clone();
			
			proj->m_killParty			= PARTY_ENEMY;
			proj->m_target				= target;

			if ( m_fire == MANUAL_MACHINEGUN_FIRE )
			{
				proj->m_sender				= null;
				proj->m_attack				= *attack;
				proj->m_component			= 0;
			}

			float3 face(0,0,1);
			proj->m_dir.Transform_Norm( face, m_nodeWeapon->GetMatrix_world() );
			proj->m_pos	= m_nodePipe[ m_pipeIndex ]->GetPosition_world();
			proj->m_pos	+= proj->m_dir * proj->m_node->GetSphere_local().r;

			ProjectileManager::AddProjectile(proj);
		}

		//  change pipe index
		switch (m_pipeIndex)
		{
		case 0:	m_pipeIndex = m_nodePipe[1] ? 1 : 0;	break;
		case 1:	m_pipeIndex = m_nodePipe[2] ? 2 : 0;	break;
		case 2:	m_pipeIndex = m_nodePipe[3] ? 3 : 0;	break;
		case 3:	m_pipeIndex = 0; break;
		}

		--m_fire;
		m_RotOffset.x -= m_forceFeedback;

		g_game->m_player->m_energy -= m_energyPerBullet;
		const int availableBullets = g_game->m_player->m_energy / m_energyPerBullet;

		str128 str;

		if ( availableBullets )
		{
			str.Format( L"%d", availableBullets );
		}
		else
		{
			m_bulletIndicator->GetElement(1)->Color() = D3DColor(1.0f, 0.0f, 0.0f, 0.85f);
			str.SetText("0");
		}

		m_bulletIndicator->SetText(str);
		
	}

} // namespace GM
