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
		, m_tower(null)
		, m_nodeWeapon(null)
		, m_nodePipe(null)
		, m_shootCount(0)
		, m_shootTime(0)
		, m_dir(0,0,1)
		, m_dirOffset(0,0,1)
		, m_fire(0)
		, m_magazineCap(0)
		, m_bullets(0)
		, m_reloadTime(0)
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

		//  find weapon node
		if ( m_tower->m_node )
		{
			m_tower->m_node->GetChildByName(L"weapon", m_nodeWeapon);
			m_tower->m_node->GetChildByName(L"pipe0", m_nodePipe);
		}

		//  invisible meshes of fire and stop particles
		if ( m_nodeWeapon )
		{
			float3 face(0,0,1);
			m_dir.Transform_Norm( face, m_nodeWeapon->GetMatrix_world() );
			m_dirOffset = m_dir;

			if ( m_nodePipe )
			{
				msg_Mesh msgMesh( SX_MESH_INVISIBLE );
				m_nodePipe->MsgProc( MT_MESH, &msgMesh );

				msg_Particle msgPrtcl(0, SX_PARTICLE_SPRAY, 0, true );
				m_nodePipe->MsgProc( MT_PARTICLE, &msgPrtcl );
			}
		}
	}

	void Mechanic_MT_Sniper::Finalize( void )
	{
		sx_callstack();

		m_nodeWeapon = NULL;
		m_nodePipe = NULL;
	}

	void Mechanic_MT_Sniper::ProcessInput( bool& inputHandled, float elpsTime )
	{
		if ( !NotInGame() ) return;
		if ( g_game->m_mouseMode != MS_Null && g_game->m_mouseMode != MS_ManualTower ) return;
		if ( g_game->m_mouseMode == MS_ManualTower && !m_tower ) return;

		sx_callstack();

		if ( g_game->m_mouseMode == MS_ManualTower )
		{

		}


	}

	void Mechanic_MT_Sniper::Update( float elpsTime )
	{
		sx_callstack();

	}

	void Mechanic_MT_Sniper::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		switch (msg)
		{
		case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
			{						//////////////////////////////////////////////////////////////////////////

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

		case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
			{						//////////////////////////////////////////////////////////////////////////

			}
			break;	//	GMT_GAME_RESET
		}
	}

	void Mechanic_MT_Sniper::EnterToManual( const Entity* tower )
	{

	}

	void Mechanic_MT_Sniper::LeaveManual( void )
	{

	}

	void Mechanic_MT_Sniper::ShootTheBullet( const prpAttack* pAttack, const float3& dir )
	{
		const Ray ray( m_tower->GetPosition(), dir );
		msg_IntersectRay intersectRay( 0, ray, msg_IntersectRay::SPHERE, NULL );
		sx::core::Scene::GetNodeByRay( intersectRay );

		sx::core::Node* node = static_cast<sx::core::Node*>(intersectRay.results[0].node);
		Entity* target = NULL;
		bool headShot = false;

		if ( node )
		{
			target = static_cast<Entity*>(node->GetUserData());

			if ( wcsncmp(node->GetName(), L"head", 5) == 0 )
			{
				headShot = true;
			}
		}

		if ( !target )
		{
			return;
		}

		Sphere targetSphere = target->GetBoundingSphere();

		//  shoot the bullet
		Projectile* proj			= pAttack->projectile->Clone();
		proj->m_killParty			= m_tower->m_partyEnemies;
		proj->m_target				= target;

		proj->m_sender				= m_tower;
		proj->m_attack				= *pAttack;
		proj->m_component			= NULL;

		proj->m_dir.Normalize( targetSphere.center - m_nodePipe->GetPosition_world() );
		proj->m_pos	= m_nodePipe->GetPosition_world();
		proj->m_pos	+= m_dir * proj->m_node->GetSphere_local().r;

		ProjectileManager::AddProjectile(proj);

		--m_fire;
	}

} // namespace GM
