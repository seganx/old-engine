#include "projectile_Bullet.h"
#include "ProjectileManager.h"
#include "Entity.h"

projectile_Bullet::projectile_Bullet( void ): Projectile(), m_initPos(0,0,0)
{
	sx_callstack();
}

projectile_Bullet::~projectile_Bullet( void )
{
	sx_callstack();
}

void projectile_Bullet::AddToScene( void )
{
	sx_callstack();

	m_initPos = m_pos;

	Projectile::AddToScene();
}

void projectile_Bullet::Update( float elpsTime )
{
	sx_callstack();

	float speed = m_speed * elpsTime * 0.001f;

	if ( m_node )
	{
		if ( m_target && m_target->m_health.icur > 0 /*&& m_sender && m_sender->m_health.icur > 0 */)
		{
			Sphere meshSphere = m_target->GetBoundingSphere();
			meshSphere.center.y += 1.0f;
			m_dir = meshSphere.center - m_pos;
			m_dir.Normalize( m_dir );
			m_pos += m_dir * speed;
			m_node->SetPosition(m_pos);
			m_node->SetDirection_local(m_dir);
		}
		else
		{
			m_target = NULL;
			//m_sender = NULL;
			m_pos += m_dir * speed;
			m_node->SetPosition(m_pos);
			m_node->SetDirection_local(m_dir);
		}		
	}
	else
	{
		m_dead = true;
		return;
	}

	if ( m_target )
	{
		if ( !m_target->m_node || m_target->m_health.icur < 0 )
		{
			m_target = NULL;
			return;
		}

		if ( m_target->m_move.type != GMT_BOTH && m_attack.targetType != GMT_BOTH && m_attack.targetType != m_target->m_move.type )
		{
			m_target = NULL;
			return;
		}

		//  fast intersection check
		Sphere meshSphere = m_target->GetBoundingSphere();
		float rspeed = speed > 1.0f ? speed : 1.0f;
		float r = ( m_radius * rspeed + meshSphere.r ) * 0.7f;
		if ( m_pos.Distance_sqr( meshSphere.center ) < r*r )
		{
			//  apply splash radius
			if ( m_attack.splashRadius > 0 )
			{
				static sx::core::ArrayPNode enemylist(64);	enemylist.Clear();
				sx::core::Scene::GetNodesByArea( m_pos, m_attack.splashRadius, enemylist, NMT_ALL, m_killParty );

				int n = enemylist.Count();
				for (int i=0; i<n; i++)
				{
					sx::core::PNode node = enemylist[i];
					Entity* pe = static_cast<Entity*>( node->GetUserData() );
					if ( pe ) DamageToTarget( pe );
				}
			}
			else
			{
				DamageToTarget( m_target );
			} // if ( m_splashRaduis > 0 )

			m_dead = true;

		} // if ( m_pos.Distance_sqr( pTarget->m_node->GetPosition_world() ) < r*r )

	} // if ( pTarget )

	if ( m_pos.y > 20.0f || m_pos.y < -10 || m_pos.Distance_sqr( m_initPos ) > 50000 )
		m_dead = true;
}

Projectile* projectile_Bullet::Clone( void )
{
	sx_callstack();

	projectile_Bullet* bullet	= sx_new( projectile_Bullet );
	bullet->m_attack			= m_attack;
	bullet->m_speed				= m_speed;
	bullet->m_node				= m_node ? m_node->Clone() : NULL;

	return bullet;
}

void projectile_Bullet::DamageToTarget( Entity* target )
{
	sx_callstack();

	if ( target && target->m_health.icur > 0 && ( target->m_move.type == GMT_BOTH || m_attack.targetType == GMT_BOTH || m_attack.targetType == target->m_move.type ) )
	{
		msgDamage damage( 
			m_attack.physicalDamage, 
			m_attack.physicalArmor, 
			m_attack.electricalDamage, 
			m_attack.electricalArmor, 
			m_attack.stunValue, 
			m_attack.stunTime, 
			m_sender );
		target->MsgProc( GMT_DAMAGE, &damage );

		if ( m_component )
		{
			if ( m_component->m_tag )
			{
				for ( int i=0; i<target->m_components.Count(); i++ )
				{
					if ( target->m_components[i]->m_tag == m_component->m_tag )
						return;
				}
			}

			Component* com = m_component->Clone();
			target->Attach( com );
			//com->Initialize();
		}
	}
}