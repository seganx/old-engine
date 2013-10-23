#include "com_weapon_GroundLava.h"
#include "Entity.h"
#include "Projectile.h"
#include "ProjectileManager.h"


#define GROUNDLAVA_FIRE_START	2


com_weapon_GroundLava::com_weapon_GroundLava( void ): Component()
	, m_target(0)
	, m_nodeWeapon(0)
	, m_nodePipe(0)
	, m_shootCount(0)
	, m_shootTime(0)
	, m_dir(0,0,1)
	, m_dirOffset(0,0,1)
	, m_searchTime(0)
	, m_headShot(false)
	, m_fire(0)
{
	sx_callstack();

	m_name = L"Groundlava";
}

com_weapon_GroundLava::~com_weapon_GroundLava( void )
{
	sx_callstack();
}

void com_weapon_GroundLava::Initialize( void )
{
	sx_callstack();

	m_owner->m_weaponType = GWT_GROUNDLAVA;

	//  find weapon node
	if ( m_owner->m_node )
	{
		m_owner->m_node->GetChildByName(L"weapon", m_nodeWeapon);
		m_owner->m_node->GetChildByName(L"pipe0", m_nodePipe);
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

void com_weapon_GroundLava::Finalize( void )
{
	sx_callstack();

	m_nodeWeapon = NULL;
	m_nodePipe = NULL;
}

void com_weapon_GroundLava::Update( float elpsTime )
{
	if ( !m_owner || !m_nodeWeapon || !m_nodePipe || m_owner->m_health.icur<1 ) return;
	sx_callstack();

	m_shootTime += elpsTime;

	//  invisible meshes of fire
	if ( m_shootTime < 200 && m_shootTime > 40 )
	{
		msg_Mesh msgMesh( SX_MESH_INVISIBLE );
		m_nodePipe->MsgProc( MT_MESH, &msgMesh );
	}

	if ( m_owner->m_level > MAX_LEVEL )
	{
		if ( !m_headShot )
			SearchForHeadshot( elpsTime );

		if ( m_headShot )
		{
			UpdateHeadshot( elpsTime );
			return;
		}
	}

	// verify that this entity can fire
	if ( m_target && m_owner->m_curAttack.projectile && m_owner->m_curAttack.rate>0 && m_target->m_health.icur>0 )
	{
		Sphere			targetSphere	= m_target->GetBoundingSphere();
		prpAttack*		attack			= &(m_owner->m_curAttack);
		
		//  compute direction to the target
		m_dirOffset.Normalize( targetSphere.center - m_nodeWeapon->GetPosition_world() );
		BlendDirection( elpsTime * 0.005f );
		m_nodeWeapon->SetDirection_world( m_dir );

		//  compute max shoot time depend on fire rate
		const float	maxShootTime = ( (attack->rate > 0.01f) ? (1000.0f / attack->rate) : 0.0f ) / GROUNDLAVA_FIRE_START ;
		if (  m_fire &&  m_dirOffset.Dot( m_dir ) > 0.97f && m_shootTime > maxShootTime )
		{
			m_shootTime = 0;

			//  rotate and show the pipe
			msg_Mesh msgMesh( 0, SX_MESH_INVISIBLE );
			m_nodePipe->MsgProc( MT_MESH, &msgMesh );
			m_nodePipe->SetRotation( 0, 0, sx::cmn::Random(6.0f) );

			//  play a sound
			msg_SoundPlay msgSound(true, 0, 0, L"fire", m_owner->m_level>MAX_LEVEL ? MAX_LEVEL : m_owner->m_level );
			m_owner->m_node->MsgProc( MT_SOUND_PLAY, &msgSound );

			//	spray bullet particle
			msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, L"bullet_shell0" );
			m_nodeWeapon->MsgProc( MT_PARTICLE, &msgPar );

			m_shootCount++;
			if ( m_shootCount > 20 )
			{
				msg_Particle msgPrtcl(SX_PARTICLE_SPRAY);
				m_nodePipe->MsgProc( MT_PARTICLE, &msgPrtcl);
				m_shootCount = sx::cmn::Random(5);
			}

			ShootTheBullet( attack, targetSphere );

		}

		if ( m_owner->GetDistance_edge( m_target ) > attack->maxRange )
		{
			m_target = NULL;
		}
	}
	else
	{
		//m_shootCount = sx::cmn::Random(7);
		m_fire = 0;

		//  wandering the pipe for moving entity
		if ( m_owner->m_move.moveSpeed > 0 )
		{
			if ( int(m_shootTime) % 1500 < elpsTime*1.1f )
			{
				float x = sx::cmn::Random(1.0f) - sx::cmn::Random(1.0f);
				float z = sx::cmn::Random(1.0f) - sx::cmn::Random(1.0f);

				m_dirOffset.x = m_owner->GetDirection().x + x;
				m_dirOffset.z = m_owner->GetDirection().z + z;
				m_dirOffset.y = m_owner->GetDirection().y;
				m_dirOffset.Normalize( m_dirOffset );
			}
			BlendDirection( elpsTime * 0.002f );
		}
		else	//  wandering the pipe for standing entity
		{
			if ( int(m_shootTime) % 3000 < elpsTime*1.1f )
			{
				float x = sx::cmn::Random(1.0f) - sx::cmn::Random(1.0f);
				float z = sx::cmn::Random(1.0f) - sx::cmn::Random(1.0f);
				float y = sx::cmn::Random(5.0f) - sx::cmn::Random(5.0f);
				m_dirOffset.Set( m_dir.x +  x*10.0f, x*y*z, m_dir.z +  z*10.0f );
				m_dirOffset.Normalize( m_dirOffset );
			}
			BlendDirection( elpsTime * 0.001f );
		}

		//  set weapon direction for wandering state
		m_nodeWeapon->SetDirection_world( m_dir );

		if ( m_shootTime > 3000 )
			m_shootCount = sx::cmn::Random(5);
	}

	if ( m_owner->m_level >= 3 )
	{
		float scl = m_owner->m_levelVisual.w;
		scl += ( 1.4f - scl ) * elpsTime * 0.0005f;
		Matrix& mat = (Matrix&)m_nodeWeapon->GetMatrix_local();
		Matrix sclmat;
		sclmat.Scale( scl, scl, scl );
		mat.Multiply( mat, sclmat );
		m_owner->m_levelVisual.w = scl;
	}
}

void com_weapon_GroundLava::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(com_weapon_GroundLava::MsgProc(msg=%d), msg);

	switch (msg)
	{
	case GMT_FIRE: if ( data && m_nodeWeapon && m_nodePipe && !m_fire && !m_headShot )
				   {
						msgFire* mf = (msgFire*)data;

						mf->fired = true;	//  say to AI that i fired

						m_target = (Entity*)mf->entity;
						if ( m_target )
						{
							const prpAttack* pAttack = &( m_owner->m_curAttack );
							float dis = m_owner->GetDistance_edge( m_target );
							if ( dis < pAttack->maxRange || dis > pAttack->minRange )
								m_fire = GROUNDLAVA_FIRE_START;
							else 
								m_target = NULL;
						}
				   }
				   break;

	case GMT_I_DIED: if ( data == m_target )
					 {
						 m_target = NULL;
					 }
					 else if ( data == m_owner && m_nodePipe )
					 {
						 //  invisible meshes of fire and stop particles
						 msg_Mesh msgMesh( SX_MESH_INVISIBLE );
						 m_nodePipe->MsgProc( MT_MESH, &msgMesh );
					 }
					 break;
	}
}

Component* com_weapon_GroundLava::Clone( void )
{
	sx_callstack();

	com_weapon_GroundLava * me	=	sx_new( com_weapon_GroundLava );
	me->m_tag					=	m_tag;

	return me;
}

void com_weapon_GroundLava::BlendDirection( float elpsTime )
{
	sx_callstack();

	if ( m_dir.Dot( m_dirOffset ) < 0 )
	{
		float2 corrctor(m_dirOffset.x + m_dir.x, m_dirOffset.z + m_dir.z);
		corrctor.Normalize( corrctor );

		float dx = corrctor.x - m_dir.x;
		float dy = m_dirOffset.y - m_dir.y;
		float dz = corrctor.y - m_dir.z;
		
		m_dir.y += dy * elpsTime;
		m_dir.x += dx * elpsTime;
		m_dir.z += dz * elpsTime;
	}
	else
	{
		float dx = m_dirOffset.x - m_dir.x;
		float dy = m_dirOffset.y - m_dir.y;
		float dz = m_dirOffset.z - m_dir.z;

		m_dir.y += dy * elpsTime;
		m_dir.x += dx * elpsTime;
		m_dir.z += dz * elpsTime;
	}

	m_dir.Normalize( m_dir );
}

void com_weapon_GroundLava::ShootTheBullet( const prpAttack* pAttack, Sphere& targetSphere )
{
	sx_callstack();

	//  shoot the bullet
	Projectile* proj			= pAttack->projectile->Clone();

	proj->m_killParty			= m_owner->m_partyEnemies;
	proj->m_target				= m_target;
	proj->m_component			= NULL;

	if ( m_fire == GROUNDLAVA_FIRE_START )
	{
		proj->m_sender	= m_owner;
		proj->m_attack	= *pAttack;
	}
	proj->m_dir.Normalize( targetSphere.center - m_nodePipe->GetPosition_world() );
	proj->m_pos	= m_nodePipe->GetPosition_world();
	proj->m_pos	+= m_dir * proj->m_node->GetSphere_local().r;

	ProjectileManager::AddProjectile(proj);

	m_fire--;

}

void com_weapon_GroundLava::SearchForHeadshot( float elpsTime )
{
	sx_callstack();

	m_searchTime += elpsTime * 0.001f;
	if ( m_searchTime < m_owner->m_curAbility.coolTime ) return;
	m_searchTime = 0;

	prpAttack* ability = &( m_owner->m_curAbility );

	//  collect enemies in the scene
	static sx::core::ArrayPNode_inline nodeList(256); nodeList.Clear();
	sx::core::Scene::GetNodesByArea( m_nodeWeapon->GetPosition_world(), ability->splashRadius, nodeList, NMT_ALL, m_owner->m_partyEnemies );

	for (int i=0; i<nodeList.Count(); i++)
	{
		sx::core::PNode node = nodeList[i];
		if ( !node->GetUserData() ) continue;

		Entity* pe = static_cast<Entity*>( node->GetUserData() );
		if ( pe->m_health.icur < 1 && pe->m_move.type != GMT_GROUND ) continue;

		sx::core::PNode headshotNode = NULL;
		if ( !pe->m_node || !pe->m_node->GetChildByName( L"_onheadshot", headshotNode ) ) continue;

		float dis = m_owner->GetDistance_edge( pe );
		if ( dis > 5.0f && dis < ability->splashRadius )
		{
			float chance = sx::cmn::Random( 1.0f );
			if ( ability->chance > chance )
			{
				m_headShot = true;
				m_shootTime = 0;
				m_target = pe;

				msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, L"aimed" );
				headshotNode->MsgProc( MT_PARTICLE, &msgPar );
				return;
			}
		}
	}
}

void com_weapon_GroundLava::UpdateHeadshot( float elpsTime )
{
	sx_callstack();

	// verify that this entity can head shot
	if ( !m_target || !m_owner->m_curAbility.projectile )
	{
		m_shootTime = 0;
		m_headShot = false;
		m_fire = 0;
		return;
	}
	
	Sphere		targetSphere	= m_target->GetBoundingSphere();
	prpAttack*	ability			= &(m_owner->m_curAbility);

	//  compute direction to the target
	m_dirOffset.Normalize( targetSphere.center - m_nodeWeapon->GetPosition_world() );
	BlendDirection( elpsTime * 0.005f );
	m_nodeWeapon->SetDirection_world( m_dir );

	if ( m_dirOffset.Dot( m_dir ) < 0.98f || m_shootTime < ability->actionTime*1000.0f ) return;

	m_shootTime = 0;
	m_headShot = false;
	m_fire = 0;

	//  rotate and show the pipe
	msg_Mesh msgMesh( 0, SX_MESH_INVISIBLE );
	m_nodePipe->MsgProc( MT_MESH, &msgMesh );
	m_nodePipe->SetRotation( 0, 0, sx::cmn::Random(6.0f) );

	//  play a sound
	msg_SoundPlay msgSound(true, 0, 0, L"fire", m_owner->m_level );
	m_owner->m_node->MsgProc( MT_SOUND_PLAY, &msgSound );

	{
		msg_Particle msgPrtcl(SX_PARTICLE_SPRAY);
		m_nodePipe->MsgProc( MT_PARTICLE, &msgPrtcl);
		m_shootCount = sx::cmn::Random(5);
	}

	{
		msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, L"bullet_headshot" );
		m_nodeWeapon->MsgProc( MT_PARTICLE, &msgPar );
	}
	
	{
		msg_Particle msgPar( 0, SX_PARTICLE_SPRAY, L"aimed" );
		m_target->m_node->MsgProc( MT_PARTICLE, &msgPar );
	}

	{
		msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, L"headshot" );
		m_target->m_node->MsgProc( MT_PARTICLE, &msgPar );
	}

	msgDamage msgDmg( 
		m_owner->m_curAbility.physicalDamage,
		m_owner->m_curAbility.physicalArmor,
		m_owner->m_curAbility.electricalDamage,
		m_owner->m_curAbility.electricalArmor,
		m_owner->m_curAbility.stunValue,
		m_owner->m_curAbility.stunTime,
		m_owner,
		14
		);
	m_target->MsgProc( GMT_DAMAGE, &msgDmg );
}