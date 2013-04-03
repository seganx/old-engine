#include "Projectile_BOMB.h"
#include "ProjectileManager.h"
#include "Entity.h"

#define PROJ_STATE_MOVING		0
#define PROJ_STATE_EXPLODING	1


Projectile_BOMB::Projectile_BOMB( void ):	Projectile()
{
	sx_callstack();
}

Projectile_BOMB::~Projectile_BOMB( void )
{
	sx_callstack();
}

void Projectile_BOMB::AddToScene( void )
{
	sx_callstack();

	if ( m_target )
		m_targetPos = m_target->GetPosition();

	m_initPos = m_pos;

	float X = m_targetPos.x		- m_initPos.x;
	float Z = m_targetPos.z		- m_initPos.z;
	float Y = /*m_targetPos.y*/	- m_initPos.y + 2.0f;

	float X2 = X*X;
	float Z2 = Z*Z;
	float R = sqrt(X2 + Z2);
	float R2 = X2+Z2;
	float Dy = m_dir.y;
	float Dx2z2 = m_dir.x*m_dir.x + m_dir.z*m_dir.z;

	if ( Dx2z2 > EPSILON )
	{
		m_initSpeed = ( -10.0f / ( Y - ( (R*Dy) / sqrt(Dx2z2) ) ) ) * ( R2 / Dx2z2 ) ;
		m_initSpeed = sqrt( m_initSpeed );
	}
	else m_initSpeed = m_speed;
	
	Projectile::AddToScene();
}

void Projectile_BOMB::Update( float elpsTime )
{
	sx_callstack();
	sx_assert(m_node);

	if ( !m_node ) return;
	
	if ( m_pos.y > 1.0f )
	{
		m_time += elpsTime * 0.0016f * m_speed;

		m_pos.x = m_dir.x * m_initSpeed * m_time + m_initPos.x;
		m_pos.y = -20.0f * m_time * m_time + m_dir.y * m_initSpeed * m_time + m_initPos.y;
		m_pos.z = m_dir.z * m_initSpeed * m_time + m_initPos.z;

		float3 dir = m_pos - m_node->GetPosition_world();
		dir.Normalize( dir );
		m_node->SetDirection_local( dir );
		m_node->SetPosition( m_pos );
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

			static Array<sx::core::Node*> enemylist(64);	enemylist.Clear();
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
				if ( pe && pe->m_health.icur > 0 && ( pe->m_move.type == GMT_BOTH || m_attack.targetType == GMT_BOTH || m_attack.targetType == pe->m_move.type ) )
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

// 			msg_Particle_Count msgPar( L"smoke" );
// 			m_node->MsgProc( MT_PARTICLE_COUNT, &msgPar );
// 			for ( int i=0; i<msgPar.numParticles; i++ )
// 			{
// 				sx::core::Particle* par = (sx::core::Particle*)msgPar.particles[i];
// 				par->RemOption( SX_PARTICLE_SPRAY );
// 				ParticleDesc desc = *par->GetDesc();
// 				desc.moveDependency = 1;
// 				par->SetDesc( &desc );
// 			}

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

Projectile* Projectile_BOMB::Clone( void )
{
	sx_callstack();

	Projectile_BOMB* bomb	= sx_new( Projectile_BOMB );
	bomb->m_attack			= m_attack;
	bomb->m_speed			= m_speed;
	bomb->m_node			= m_node ? m_node->Clone() : NULL;

	return bomb;
}

