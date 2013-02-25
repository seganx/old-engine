#include "Mechanic_MT_Sniper.h"
#include "Game.h"
#include "Player.h"
#include "Scripter.h"
#include "Entity.h"
#include "GameGUI.h"
#include "EntityManager.h"
#include "Projectile.h"
#include "ProjectileManager.h"


namespace GM
{

	Mechanic_MT_Sniper::Mechanic_MT_Sniper( void ) : Mechanic()
		, m_node(null)
		, m_nodeCamera(null)
		, m_nodeWeapon(null)
		, m_nodePipe(null)
		, m_shootCount(0)
		, m_shootTime(0)
		, m_rot(0,0,0)
		, m_rotOffset(0,0,0)
		, m_rotMax(0.5f, 1.0f, 0)
		, m_fire(0)
		, m_bullets(0)
		, m_firedCount(0)
		, m_selected(false)
		, m_lblBullet(null)
	{
		sx_callstack();
	}

	Mechanic_MT_Sniper::~Mechanic_MT_Sniper( void )
	{
		sx_callstack();

	}

	void Mechanic_MT_Sniper::Initialize( void )
	{
		sx_callstack();

		m_lblBullet = sx_new( sx::gui::Label );
		m_lblBullet->SetSize( float2(70, 25) );
		m_lblBullet->Position().Set( -450.0f, 240.0f, 0.0f );
		m_lblBullet->GetElement(0)->Color().a = 0.0f;
		m_lblBullet->GetElement(1)->Color().a = 0.85f;
		m_lblBullet->SetFont( L"Font_rob_twedit_info.fnt" );

		g_game->m_gui->Add_Back( m_lblBullet );
	}

	void Mechanic_MT_Sniper::Finalize( void )
	{
		sx_callstack();

		g_game->m_gui->Remove( m_lblBullet );
		sx_delete_and_null( m_lblBullet );
	}

	void Mechanic_MT_Sniper::ProcessInput( bool& inputHandled, float elpsTime )
	{
		if ( NotInGame() || inputHandled ) { return; }

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
			if ( !IsFocused() ) { return; }

			//	just check the number of bullets and player does fire
			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) )
			{
				if ( (!m_fire) && (m_firedCount < m_bullets) )
				{
					m_fire = 1;
				}
			}

			m_rotOffset.x += SEGAN_MOUSE_RLY(0) * 0.002f;
			m_rotOffset.y += SEGAN_MOUSE_RLX(0) * 0.002f;
			if ( m_rotOffset.x > m_rotMax.x )  { m_rotOffset.x =  m_rotMax.x; }
			if ( m_rotOffset.y > m_rotMax.y )  { m_rotOffset.y =  m_rotMax.y; }
			if ( m_rotOffset.x < -m_rotMax.x ) { m_rotOffset.x = -m_rotMax.x; }
			if ( m_rotOffset.y < -m_rotMax.y ) { m_rotOffset.y = -m_rotMax.y; }
			SEGAN_MOUSE_ABSX(0) = SEGAN_VP_WIDTH/2;
			SEGAN_MOUSE_ABSY(0) = SEGAN_VP_HEIGHT/2;
			inputHandled = true;

			if ( SEGAN_KEYUP(0, SX_INPUT_KEY_ESCAPE) || SEGAN_KEYUP(0, SX_INPUT_KEY_MOUSE_RIGHT) )
			{
				LeaveManual();
			}
		}
	}

	void Mechanic_MT_Sniper::Update( float elpsTime )
	{
		if ( NotInGame() || !m_nodeWeapon || !m_nodePipe ) { return; }

		sx_callstack();

		m_shootTime += elpsTime;

		if ( g_game->m_mouseMode == MS_ManualTower && IsFocused() )
		{
			UpdateCamera( elpsTime );
		}

		//  invisible meshes of fire
		if ( m_shootTime < 200 && m_shootTime > 40 )
		{
			msg_Mesh msgMesh( SX_MESH_INVISIBLE );

			if ( m_nodePipe )
			{
				m_nodePipe->MsgProc( MT_MESH, &msgMesh );
			}
		}

		//	Blend Direction
		{
			const float dx = m_rotOffset.x - m_rot.x;
			const float dy = m_rotOffset.y - m_rot.y;
			const float dz = m_rotOffset.z - m_rot.z;

			const float blendTime = elpsTime * 0.005f;
			m_rot.y += dy * blendTime;
			m_rot.x += dx * blendTime;
			m_rot.z += dz * blendTime;
			m_nodeWeapon->SetRotation( m_rot.x, m_rot.y, m_rot.z );
		}

		//  compute max shoot time depend on fire rate
		const float	maxShootTime = (m_attack.rate > 0.01f) ? (1000.0f / m_attack.rate) : 500.0f;
		if ( m_fire && (m_shootTime > maxShootTime) && (m_firedCount < m_bullets) )
		{
			m_shootTime = 0.0f;

			//  rotate and show the pipe
			msg_Mesh msgMesh( 0, SX_MESH_INVISIBLE );
			m_nodePipe->MsgProc( MT_MESH, &msgMesh );
			m_nodePipe->SetRotation( 0, 0, sx::cmn::Random(6.0f) );

			//  play a sound
			msg_SoundPlay msgSound(true, sx::cmn::Random(1000), 0, L"fire" );
			m_node->MsgProc( MT_SOUND_PLAY, &msgSound );

			//	spray bullet particle
			str256 bulletShell = L"bullet_shell0";
			msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, bulletShell );
			m_nodeWeapon->MsgProc( MT_PARTICLE, &msgPar );

			++m_shootCount;

			if ( m_shootCount > 15 )
			{
				msg_Particle msgPrtcl(SX_PARTICLE_SPRAY);
				m_nodePipe->MsgProc( MT_PARTICLE, &msgPrtcl);
				m_shootCount = sx::cmn::Random(5);
			}

			ShootTheBullet();
		}
	}

	void Mechanic_MT_Sniper::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		static int lastIndex = 0;

		switch (msg)
		{
		case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
			{						//////////////////////////////////////////////////////////////////////////
				//  load some level configuration
				String str = Game::GetLevelPath();
				if ( g_game->m_miniGame )
				{
					str << L"config_mini.txt";
				}
				else
				{
					str << L"config.txt";
				}

				Scripter script;
				script.Load( str );

				for ( int i=lastIndex; i<script.GetObjectCount(); i++ )
				{
					str512 tmpStr;
					if ( script.GetString(i, L"Type", tmpStr) && tmpStr == L"ManualTower" )
					{
						if ( !script.GetString(i, L"Name", tmpStr) )
						{
							continue;
						}

						if ( tmpStr == L"Sniper" )
						{
							if ( script.GetString(i, L"node", tmpStr) )
							{
								sx::core::ArrayPNode nodes(16);
								sx::core::Scene::GetNodesByName( tmpStr, nodes );
								m_node = nodes.Count() ? nodes[0] : NULL;
							}

							script.GetFloat(i, L"stunValue", m_attack.stunValue);
							script.GetFloat(i, L"stunTime", m_attack.stunTime);
							script.GetFloat(i, L"physicalDamage", m_attack.physicalDamage);
							script.GetFloat(i, L"electricalDamage", m_attack.electricalDamage);
							script.GetFloat(i, L"fireRate", m_attack.rate);
							script.GetFloat(i, L"maxPhi", m_rotMax.y);
							script.GetFloat(i, L"maxTheta", m_rotMax.x);
							script.GetInteger(i, L"bulletsCount", m_bullets);

							if ( script.GetString(i, L"bullet", tmpStr) )
							{
								String::Copy( m_attack.bullet, 64, tmpStr );
								m_attack.projectile = ProjectileManager::GetTypeByName( m_attack.bullet );
							}

							lastIndex = i + 1;
							break;
						}	//	if ( tmpStr == L"Sniper" )
					}
				}
			}
			break;	//	GMT_LEVEL_LOAD

		case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
			{						//////////////////////////////////////////////////////////////////////////
				m_selected = false;
				m_node = NULL;
				LeaveManual();
				lastIndex = 0;
			}
			break;	//	GMT_LEVEL_CLEAR

		case GMT_GAME_START:
		case GMT_GAME_END:
		case GMT_GAME_RESETING:
		case GMT_GAME_RESET:
			{
				if ( m_node )
				{
					m_node->GetChildByName(L"weapon", m_nodeWeapon);
					m_node->GetChildByName(L"pipe0", m_nodePipe);

					//  stop particle spray
					msg_Particle msgPrtcl(0, SX_PARTICLE_SPRAY, 0, true );
					m_node->MsgProc( MT_PARTICLE, &msgPrtcl );
				}

				//  invisible meshes of fire and stop particles
				if ( m_nodeWeapon )
				{
					m_nodeWeapon->SetRotation(0,0,0);
					if ( m_nodePipe )
					{
						msg_Mesh msgMesh( SX_MESH_INVISIBLE );
						m_nodePipe->MsgProc( MT_MESH, &msgMesh );
					}
				}

				m_firedCount = 0;
				m_shootTime = 0.0f;
			}
			//break;
		case GMT_WAVE_FINISHED:
			{
				LeaveManual();
				m_selected = false;
			}
			break;
		}
	}

	void Mechanic_MT_Sniper::EnterToManual( void )
	{
		sx_callstack();

		//  find weapon node
		if ( m_node )
		{
			m_node->GetChildByName(L"camera", m_nodeCamera);
			m_node->GetChildByName(L"weapon", m_nodeWeapon);
			m_node->GetChildByName(L"pipe0", m_nodePipe);
		}

		//  invisible meshes of fire and stop particles
		if ( m_nodeWeapon )
		{
			if ( m_nodePipe )
			{
				msg_Mesh msgMesh( SX_MESH_INVISIBLE );
				m_nodePipe->MsgProc( MT_MESH, &msgMesh );

				msg_Particle msgPrtcl(0, SX_PARTICLE_SPRAY, 0, true );
				m_nodePipe->MsgProc( MT_PARTICLE, &msgPrtcl );
			}
		}

		if ( m_nodeWeapon && m_nodePipe && m_nodeCamera )
		{
			SetFocused( true );
			g_game->m_mouseMode = MS_ManualTower;
		}
		else
		{
			SetFocused( false );
			g_game->m_mouseMode = MS_Null;
		}

		m_shootCount = 0;
		m_fire = 0;

		m_lblBullet->AddProperty( SX_GUI_PROPERTY_VISIBLE );
	}

	void Mechanic_MT_Sniper::LeaveManual( void )
	{
		sx_callstack();

		if ( g_game->m_mouseMode == MS_ManualTower )
		{
			g_game->m_mouseMode = MS_Null;
		}
		
		m_nodeCamera = NULL;
		m_nodeWeapon = NULL;
		m_nodePipe = NULL;

		m_lblBullet->RemProperty( SX_GUI_PROPERTY_VISIBLE );
	}

	void Mechanic_MT_Sniper::UpdateCamera( float elpsTime )
	{
		sx_callstack();

		sx::core::Camera camera;
		camera.Far = CAMERA_FAR;
		camera.Aspect = SEGAN_VP_WIDTH / SEGAN_VP_HEIGHT;
		camera.FOV = PI / 8.0f;//(1.7f + camera.Aspect);
		camera.Eye = m_nodeCamera->GetPosition_world();

		float3 dir(0, 0, 0.1f);
		dir.Transform_Norm( dir, m_nodeCamera->GetMatrix_world() );
		camera.Eye += dir;
		camera.At = camera.Eye + dir * 10;
		camera.Up.Set(0, 1, 0);

		sx::core::PCamera pCam = sx::core::Renderer::GetCamera();

		pCam->Far = camera.Far;
		pCam->FOV += (camera.FOV - pCam->FOV) * 0.01f * elpsTime;
		pCam->Aspect = camera.Aspect;

#if 1
		float bspeed = m_fire ? 0.05f : 0.04f;
		pCam->At.x += (camera.At.x - pCam->At.x) * bspeed * elpsTime;
		pCam->At.y += (camera.At.y - pCam->At.y) * bspeed * elpsTime;
		pCam->At.z += (camera.At.z - pCam->At.z) * bspeed * elpsTime;

		bspeed = m_fire ? 0.01f : 0.015f;
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

	void Mechanic_MT_Sniper::ShootTheBullet( void )
	{
		sx_callstack();
		sx_assert( m_attack.projectile );

		const sx::math::Ray ray = sx::core::Renderer::GetCamera()->GetRay( SEGAN_VP_WIDTH / 2, SEGAN_VP_HEIGHT / 2 );
		msg_IntersectRay intersectRay( NMT_MESH, ray, msg_IntersectRay::BOX, NULL );
		sx::core::Scene::GetNodeByRay( intersectRay );

		Entity* target = NULL;
		bool headShot = false;

		for ( int i=0; i<32; i++ )
		{
			sx::core::Node* node = static_cast<sx::core::Node*>(intersectRay.results[i].node);
			if ( node )
			{
				if ( node->GetUserData() && node->GetUserTag() == PARTY_ENEMY )
				{
					Entity* en = static_cast<Entity*>(node->GetUserData());
					if ( en->m_health.icur )
					{
						target = en;

						if ( intersectRay.results[i].member )
						{
							sx::core::NodeMember* nodeMember = static_cast<sx::core::NodeMember*>(intersectRay.results[i].member);
							if ( wcsncmp(nodeMember->GetName(), L"head", 5) == 0 )
							{
								headShot = true;
							}
						}

						break;
					}
				}
			}
		}

		//  shoot the bullet
		Projectile* proj			= m_attack.projectile->Clone();
		proj->m_killParty			= PARTY_ENEMY;
		proj->m_target				= target;

		proj->m_sender				= NULL;
		proj->m_attack				= m_attack;
		proj->m_component			= NULL;

		if ( headShot )
		{
			proj->m_attack.physicalDamage *= 2.0f;
			proj->m_attack.electricalDamage *= 2.0f;
		}

		float3 face(0,0,1);
		proj->m_dir.Transform_Norm( face, m_nodeWeapon->GetMatrix_world() );
		proj->m_pos	= m_nodePipe->GetPosition_world();
		proj->m_pos	+= proj->m_dir * proj->m_node->GetSphere_local().r;

		ProjectileManager::AddProjectile(proj);

		--m_fire;
		m_rotOffset.x -= 0.03f;
		++m_firedCount;

		str128 str;
		str.Format( L"%d/%d", m_firedCount, m_bullets );
		m_lblBullet->SetText(str);
	}

} // namespace GM
