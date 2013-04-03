#include "Projectile.h"
#include "Entity.h"

Projectile::Projectile( void )
:	m_dead(false)

,	m_target(0)
,	m_sender(0)
,	m_component(0)
,	m_node(0)
,	m_killParty( PARTY_ENEMY | PARTY_TOWER )
,	m_targetPos(0,0,0)
,	m_pos(0,0,0)
,	m_dir(0,0,0)
,	m_speed(0)
,	m_state(0)
,	m_time(0)
,	m_initSpeed(0)
,	m_initPos(0,0,0)
,	m_radius(0)
,	m_tag(0)
{

}

Projectile::~Projectile( void )
{
	sx::core::Scene::DeleteNode( m_node );
}

void Projectile::AddToScene( void )
{
	if ( m_node )
	{
		m_node->SetPosition( m_pos );
		m_node->SetDirection_local( m_dir );
		m_node->SetUserData( this );
		sx::core::Scene::AddNode( m_node );

		float f = 0;
		m_node->MsgProc( MT_ACTIVATE, &f );

		msg_Particle msgPrtcl(SX_PARTICLE_SPRAY, 0, L"smoke");
		m_node->MsgProc( MT_PARTICLE, &msgPrtcl );

		msg_Mesh_Count meshes;
		m_node->MsgProc( MT_MESH_COUNT, &meshes );
		if ( meshes.numMeshes )
		{
			sx::math::AABox box( sx::math::VEC3_MAX, -sx::math::VEC3_MAX );
			for ( int i=0; i<meshes.numMeshes; i++ )
			{
				sx::core::PMesh mesh = (sx::core::PMesh)meshes.meshes[i];
				mesh->MsgProc( MT_COVERBOX, &box );
			}

			sx::math::Sphere sphere;
			sphere.ComputeByAABox( box );
			m_radius = sphere.r;
		}
	}
}