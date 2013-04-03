#include "projectile_Missile.h"
#include "ProjectileManager.h"
#include "Entity.h"

#define PROJ_STATE_MOVING		0
#define PROJ_STATE_EXPLODING	1


projectile_Missile::projectile_Missile( void )
:	Projectile()
,	m_state(PROJ_STATE_MOVING)
,	m_time(0)
,	m_turnDir(0,0,0)
{
	sx_callstack();
}

projectile_Missile::~projectile_Missile( void )
{
	sx_callstack();
}

void projectile_Missile::Update( float elpsTime )
{
	sx_callstack();
	sx_assert(m_node);

	if ( !m_node ) return;

	if ( m_target && m_target->m_health.deleteTime > 1 )
	{
		m_targetPos = m_target->GetPosition();
	}
	else
	{
		m_target = NULL;
		//m_targetPos.Set( m_pos.x, 15.0f, m_pos.z );
		m_targetPos.y = 0.0f;
	}

	if ( m_pos.y > 0.0f ) //m_pos.Distance_sqr( m_targetPos ) > 1.0f )
	{
		float timespeed = m_speed * elpsTime * 0.0016f;
		m_time += timespeed;

		m_targetPos. y = ( m_time < 15.0f ) ? 15.0f : 0.0f;

		float3 dir( m_targetPos - m_pos );
		dir.Normalize( dir );
		m_dir += ( dir - m_dir ) * timespeed * m_time * 0.005f;
		m_dir.Normalize( m_dir );
		m_pos.x += m_dir.x * timespeed;
		m_pos.y += m_dir.y * timespeed;
		m_pos.z += m_dir.z * timespeed;
		m_node->SetDirection_local( m_dir );
		m_node->SetPosition( m_pos );

// 		float3 dir;
// 		if ( m_time > 7 )
// 		{
// 			m_targetPos.y = 0.0f;
// 			dir = m_targetPos - m_pos;
// 			m_dir += (dir - m_dir) * timespeed * 0.1f;
// 			m_dir.Normalize( m_dir );
// 		}
// 		else
// 		{
// 			m_targetPos.y = 15.0f;
// 			dir = m_targetPos - m_pos;
// 			m_dir += (dir - m_dir) * timespeed * 0.01f;
// 			m_dir.Normalize( m_dir );
// 		}
// 
// 		float phase = m_time * 0.05f;
// 		if ( phase > PI ) phase = PI;
// 
// 		float turnSpeed = ( sx::math::PIMUL2 + sx::math::PIDIV2 ) * sin( phase );
// 		m_turnDir.x = cosf( turnSpeed ) * m_dir.x;
// 		m_turnDir.y = sinf( turnSpeed ) * m_dir.y;
// 		m_turnDir.z = cosf( turnSpeed ) * m_dir.z;
// 		m_turnDir.Normalize( m_turnDir );
// 
// 		float w = phase / PI;
// 		dir.x = m_turnDir.x * ( 1.0f - w ) + m_dir.x * w;
// 		dir.y = m_turnDir.y * ( 1.0f - w ) + m_dir.y * w;
// 		dir.z = m_turnDir.z * ( 1.0f - w ) + m_dir.z * w;
// 
// 		m_pos.x += dir.x * timespeed;
// 		m_pos.y += dir.y * timespeed;
// 		m_pos.z += dir.z * timespeed;
// 
// 		dir = m_pos - m_node->GetPosition_world();
// 		m_node->SetDirection_local( dir );
// 		m_node->SetPosition( m_pos );
	}
	else
	{
		if ( m_state == PROJ_STATE_EXPLODING )
		{
			m_time += elpsTime;

			if ( m_time > 5000 )
				m_dead = true;

		}
		else if ( m_state == PROJ_STATE_MOVING )
		{
			m_state = PROJ_STATE_EXPLODING;
			m_time = 0;

			static Array<sx::core::PNode> enemylist(128);	enemylist.Clear();
			sx::core::Scene::GetNodesByArea( m_pos, m_attack.splashRadius, enemylist, NMT_ALL, m_killParty );

			int n = enemylist.Count();
			for (int i=0; i<n; i++)
			{
				sx::core::PNode node = enemylist[i];
				if( !node->GetUserData() || node->GetUserData() == this )
				{
					//sxLog::Log(L"detected wrong node party !");
					continue;
				}

				Entity* pe = static_cast<Entity*>( node->GetUserData() );
				if ( pe->m_health.icur > 0 )
				{
					float dis = m_pos.Distance( node->GetPosition_world() );
					if ( dis < m_attack.splashRadius )
					{
						float val = (1.0f - dis/m_attack.splashRadius);
						msgDamage damage(
							m_attack.physicalDamage * val,
							m_attack.physicalArmor * val,
							m_attack.electricalDamage * val,
							m_attack.electricalArmor * val,
							m_attack.stunValue * val,
							m_attack.stunTime * val,
							m_sender
							);
						pe->MsgProc( GMT_DAMAGE, &damage );
					}					
				}
			}

			//  reset node transformation
			m_node->SetRotation( 0, 0, 0 );
			m_pos.y = 0;
			m_node->SetPosition( m_pos );

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

Projectile* projectile_Missile::Clone( void )
{
	sx_callstack();

	projectile_Missile* missile	= sx_new( projectile_Missile );
	missile->m_attack			= m_attack;
	missile->m_speed			= m_speed;
	missile->m_node				= m_node ? m_node->Clone() : NULL;

	return missile;
}

