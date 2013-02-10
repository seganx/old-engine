#include "projectile_ClusterBomb.h"
#include "ProjectileManager.h"
#include "Entity.h"

#define PROJ_STATE_MOVING		0
#define PROJ_STATE_EXPLODING	1


projectile_ClusterBomb::projectile_ClusterBomb( void )
:	Projectile()
,	m_miniBomb(0)
,	m_state(0)
,	m_time(0)
,	m_turnDir(0,0,0)
{
	sx_callstack_push(projectile_ClusterBomb::projectile_ClusterBomb());
}

projectile_ClusterBomb::~projectile_ClusterBomb( void )
{
	sx_callstack_push(projectile_ClusterBomb::~projectile_ClusterBomb());
}

void projectile_ClusterBomb::Update( float elpsTime )
{
	sx_callstack_push(projectile_ClusterBomb::Update());
	sx_assert(m_node);

	if ( !m_node ) return;

	if ( m_target && m_target->m_health.deleteTime > 1 )
	{
		m_targetPos = m_target->GetPosition();
		m_targetPos.y = 15.0f;
	}
	else
	{
		m_target = NULL;
		//m_targetPos.Set( m_pos.x, 15.0f, m_pos.z );
		m_targetPos.y = 15.0f;
	}

	if ( m_targetPos.y - m_pos.y > 0 )
	{
		m_dir.Normalize( m_targetPos - m_pos );

		float timespeed = m_speed * elpsTime * 0.0016f;
		m_time += timespeed;

		float phase = m_time * 0.05f;
		if ( phase > PI ) phase = PI;

		float turnSpeed = ( sx::math::PIMUL2 + sx::math::PIDIV2 ) * sin( phase );
		m_turnDir.x = cosf( turnSpeed ) * m_dir.x;
		m_turnDir.y = sinf( turnSpeed ) * m_dir.y;
		m_turnDir.z = cosf( turnSpeed ) * m_dir.z;
		m_turnDir.Normalize( m_turnDir );

		float3 dir;
		float w = phase / PI;
		dir.x = m_turnDir.x * ( 1.0f - w ) + m_dir.x * w;
		dir.y = m_turnDir.y * ( 1.0f - w ) + m_dir.y * w;
		dir.z = m_turnDir.z * ( 1.0f - w ) + m_dir.z * w;

		m_pos.x += dir.x * timespeed;
		m_pos.y += dir.y * timespeed;
		m_pos.z += dir.z * timespeed;

		dir = m_pos - m_node->GetPosition_world();
		m_node->SetDirection_local( dir );
		m_node->SetPosition( m_pos );
	}
	else
	{
		if ( m_state == PROJ_STATE_EXPLODING )
		{
			m_time += elpsTime;

			if ( m_time > 10000 )
				m_dead = true;

		}
		else if ( m_state == PROJ_STATE_MOVING )
		{
			m_state = PROJ_STATE_EXPLODING;
			m_time = 0;

			int n = m_attack.actionCount + sx::cmn::Random(1) - sx::cmn::Random(1);
			for (int i=0; i<n; i++)
			{
				if ( !m_miniBomb )
				{
					sxLog::Log( L"ERROR : no mini bomb exist in cluster !" );
					break;
				}

				Projectile* proj = m_miniBomb->Clone();
				if ( proj )
				{
					proj->m_targetPos.Set( m_pos.x + 1.0f, 0.0f, m_pos.z + 1.0f );

					static sx::core::ArrayPNode_inline nodes(512); nodes.Clear();
					sx::core::Scene::GetNodesByArea( proj->m_targetPos, m_attack.minRange, nodes, NMT_PATHNODE );
					if ( nodes.Count() )
						proj->m_targetPos = nodes[ sx::cmn::Random( nodes.Count()-1 ) ]->GetPosition_world();

					nodes.Clear();
					sx::core::Scene::GetNodesByArea( proj->m_targetPos, 10.0, nodes, NMT_PATHNODE );
					if ( nodes.Count() )
					{
						proj->m_targetPos = sx::math::VEC3_ZERO;
						for (int n=0; n<nodes.Count(); n++ )
							proj->m_targetPos += nodes[n]->GetPosition_world();
						proj->m_targetPos /= (float)nodes.Count();
					}

					proj->m_killParty	= PARTY_ENEMY;
					proj->m_target		= NULL;
					proj->m_speed		= 0.5f;

					proj->m_attack		= m_attack;
					proj->m_attack.targetType = GMT_BOTH;

					proj->m_pos			= m_pos;
					proj->m_dir.x		= proj->m_targetPos.x - m_pos.x;
					proj->m_dir.z		= proj->m_targetPos.z - m_pos.z;
					proj->m_dir.y		= 0.0f;
					proj->m_dir.Normalize( proj->m_dir );
					proj->m_dir.y		= 1.0f;
					proj->m_dir.Normalize( proj->m_dir );

					ProjectileManager::AddProjectile( proj );
				}
			}

			//  reset node transformation
			m_node->SetRotation( 0, 0, 0 );

			//  hide main mesh
			msg_Mesh msgMesh( SX_MESH_INVISIBLE );
			m_node->MsgProc( MT_MESH, &msgMesh );

			msg_Particle msgPar( 0, SX_PARTICLE_SPRAY );
			m_node->MsgProc( MT_PARTICLE, &msgPar );

			//  spray particles and play sounds
			sx::core::PNode node = NULL;
			if ( m_node->GetChildByName( L"_onexplode", node ) )
			{
				msg_Particle msgPar( SX_PARTICLE_SPRAY );
				node->MsgProc( MT_PARTICLE, &msgPar );

				msg_SoundPlay msgSnd( true );
				node->MsgProc( MT_SOUND_PLAY, &msgSnd );
			}
		}

	}
}

Projectile* projectile_ClusterBomb::Clone( void )
{
	sx_callstack_push(projectile_ClusterBomb::Clone());

	projectile_ClusterBomb* bomb	= sx_new( projectile_ClusterBomb );
	bomb->m_attack					= m_attack;
	bomb->m_speed					= m_speed;
	bomb->m_node					= m_node ? m_node->Clone() : NULL;
	
	String::Copy( bomb->m_miniBombName, 128, m_miniBombName );
	bomb->m_miniBomb				= ProjectileManager::GetTypeByName( m_miniBombName );

	return bomb;
}

