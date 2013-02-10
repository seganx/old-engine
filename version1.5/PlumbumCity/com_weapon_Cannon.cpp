#include "com_weapon_Cannon.h"
#include "Entity.h"
#include "Projectile.h"
#include "ProjectileManager.h"


com_weapon_Cannon::com_weapon_Cannon( void ) : Component()
	, m_target(0)
	, m_nodeWeapon(0)
	, m_nodePipe(0)
	, m_shootCount(0)
	, m_shootTime(0)
	, m_Dir(0,0,1)
	, m_DirOffset(0,0,1)
	, m_Fire(false)
{
	sx_callstack_push(com_weapon_Cannon::com_weapon_Cannon());

	m_name = L"Cannon";
}

com_weapon_Cannon::~com_weapon_Cannon( void )
{
	sx_callstack_push(com_weapon_Cannon::~com_weapon_Cannon());
}

void com_weapon_Cannon::Initialize( void )
{
	sx_callstack_push(com_weapon_Cannon::Initialize());

	m_owner->m_weaponType = GWT_MORTAR;

	//  find weapon node
	if ( m_owner->m_node )
	{
		m_owner->m_node->GetChildByName(L"weapon",	m_nodeWeapon);
		m_owner->m_node->GetChildByName(L"pipe0",	m_nodePipe);
	}

	//  invisible meshes of fire and stop particles
	if ( m_nodeWeapon )
	{
		float3 face(0,0,1);
		m_Dir.Transform_Norm( face, m_nodeWeapon->GetMatrix_world() );
		m_DirOffset = m_Dir;

		if ( m_nodePipe )
		{
			msg_Mesh msgMesh( SX_MESH_INVISIBLE );
			m_nodePipe->MsgProc( MT_MESH, &msgMesh );
			msg_Particle msgPrtcl( 0, SX_PARTICLE_SPRAY, 0, true );
			m_nodePipe->MsgProc( MT_PARTICLE, &msgPrtcl );
		}
	}
}

void com_weapon_Cannon::Finalize( void )
{
	sx_callstack_push(com_weapon_Cannon::Finalize());

	m_nodeWeapon	= NULL;
	m_nodePipe		= NULL;
}

void com_weapon_Cannon::Update( float elpsTime )
{
	if ( !m_owner || !m_nodeWeapon || !m_nodePipe || m_owner->m_health.icur<1) return;
	sx_callstack_push(com_weapon_Cannon::Update());

	m_shootTime += elpsTime;

	//  collect some initial information
	const int			ownerLevel = m_owner->m_level;
	const prpAttack*	attack = &( m_owner->m_curAttack );

	//  invisible meshes of fire
	if ( m_shootTime < 200 && m_shootTime > 40 )
	{
		msg_Mesh msgMesh( SX_MESH_INVISIBLE );
		m_nodePipe->MsgProc( MT_MESH, &msgMesh );
	}

	// verify that this entity can fire
	if ( m_target && m_Fire &&  attack->projectile && m_target->m_health.icur>0 )
	{
		//  compute direction to the target
		BlendDirection(elpsTime * 0.005f);
		m_nodeWeapon->SetDirection_world( m_Dir );

		//  compute max shoot time depend on fire rate
		const float	maxShootTime = (attack->rate > 0.01f) ? (1000.0f / attack->rate) : 0.0f ;
		if ( m_DirOffset.Dot( m_Dir ) > 0.97f && m_shootTime >  maxShootTime )
		{
			m_shootTime = 0;
			m_Fire = false;

			//  rotate and show the pipe
			floatQ q; q.SetRotationXYZ( 0, 0, sx::cmn::Random(6.0f) );
			m_nodePipe->SetQuaternion(q);
			msg_Mesh msgMesh( 0, SX_MESH_INVISIBLE );
			m_nodePipe->MsgProc( MT_MESH, &msgMesh );

			//  play a sound
			msg_SoundPlay msgSound(true, sx::cmn::Random(1000), 0, L"fire", ownerLevel );
			m_owner->m_node->MsgProc( MT_SOUND_PLAY, &msgSound );

			//	spray bullet particle
			str256 bulletShell = L"bullet_shell0";
			msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, bulletShell );
			m_nodeWeapon->MsgProc( MT_PARTICLE, &msgPar );

			m_shootCount++;
			if ( m_shootCount > 7 )
			{
				msg_Particle msgPrtcl(SX_PARTICLE_SPRAY);
				m_nodePipe->MsgProc( MT_PARTICLE, &msgPrtcl);
				m_shootCount = sx::cmn::Random(5);
			}
			
			//  shoot the bomb
			int special = ownerLevel - MAX_LEVEL;
			ShootTheBomp( sx_max_i( 0, special ) );
		}

		if ( m_owner->GetDistance_edge( m_target ) > attack->maxRange )
		{
			m_target = NULL;
		}
	}
	else
	{
		m_Fire = false;

		//  wandering the pipe for moving entity
		if ( m_owner->m_move.moveSpeed > 0 )
		{
			if ( int(m_shootTime) % 1500 < elpsTime*1.1f )
			{
				float x = sx::cmn::Random(1.0f) - sx::cmn::Random(1.0f);
				float z = sx::cmn::Random(1.0f) - sx::cmn::Random(1.0f);

				m_DirOffset.x = m_owner->GetDirection().x + x;
				m_DirOffset.z = m_owner->GetDirection().z + z;
				m_DirOffset.y = m_owner->GetDirection().y;
				m_DirOffset.Normalize( m_DirOffset );
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
				m_DirOffset.Set( m_Dir.x +  x*10.0f, x*y*z, m_Dir.z +  z*10.0f );
				m_DirOffset.Normalize( m_DirOffset );
			}
			BlendDirection( elpsTime * 0.001f );
		}

		//  set weapon direction for wandering state
		m_nodeWeapon->SetDirection_world( m_Dir );

		if ( m_shootTime > 3000 )
			m_shootCount = sx::cmn::Random(5);
	}
}

void com_weapon_Cannon::MsgProc( UINT msg, void* data )
{
	sx_callstack_push(com_weapon_Cannon::MsgProc(msg=%d), msg);

	switch (msg)
	{
	case GMT_FIRE: if ( data && m_nodeWeapon && m_nodePipe && m_owner->m_curAttack.projectile )
				   {
						msgFire* mf = (msgFire*)data;
						mf->fired = true;	//  say to AI that i fired

						m_target = (Entity*)mf->entity;

						float maxRange = m_owner->m_curAttack.maxRange;

						float X = mf->pos.x - m_nodeWeapon->GetPosition_world().x;
						float Z = mf->pos.z - m_nodeWeapon->GetPosition_world().z;
						float Y = /*mf->pos.y*/ - m_nodeWeapon->GetPosition_world().y + 2.0f;

						float X2 = X*X;
						float Z2 = Z*Z;
						float3 pos(X, sqrt(X2+Z2), Z);

						pos.y *= pos.y / (1.0f + maxRange * 1.5f );
						m_DirOffset.Normalize( pos );

						m_Fire = true;
				   }
				   break;

	case GMT_I_DIED: if ( data == m_owner && m_nodeWeapon && m_nodePipe )
					 {
						 //m_nodeWeapon->SetDirection_local( float3(0,0,0.99f) );

						 //  invisible meshes of fire and stop particles
						 msg_Mesh msgMesh( SX_MESH_INVISIBLE );
						 m_nodePipe->MsgProc( MT_MESH, &msgMesh );
					 }
					 else if ( data == m_target )
					 {
						 m_target = NULL;
					 }

					 break;
	}
}

Component* com_weapon_Cannon::Clone( void )
{
	sx_callstack_push(com_weapon_Cannon::Clone());

	com_weapon_Cannon * me	=	sx_new( com_weapon_Cannon );
	me->m_tag					=	m_tag;
	return me;
}

void com_weapon_Cannon::BlendDirection( float elpsTime )
{
	sx_callstack_push(com_weapon_Cannon::BlendDirection());

	if ( m_Dir.Dot( m_DirOffset ) < 0 )
	{
		float2 corrctor(m_DirOffset.x + m_Dir.x, m_DirOffset.z + m_Dir.z);
		corrctor.Normalize( corrctor );

		float dx = corrctor.x - m_Dir.x;
		float dy = m_DirOffset.y - m_Dir.y;
		float dz = corrctor.y - m_Dir.z;

		m_Dir.y += dy * elpsTime;
		m_Dir.x += dx * elpsTime;
		m_Dir.z += dz * elpsTime;
	}
	else
	{
		float dx = m_DirOffset.x - m_Dir.x;
		float dy = m_DirOffset.y - m_Dir.y;
		float dz = m_DirOffset.z - m_Dir.z;

		m_Dir.y += dy * elpsTime;
		m_Dir.x += dx * elpsTime;
		m_Dir.z += dz * elpsTime;
	}

	m_Dir.Normalize( m_Dir );
}

void com_weapon_Cannon::ShootTheBomp( int special )
{
	if ( !m_target ) return;

	sx_callstack_push(com_weapon_Cannon::ShootTheBomp(special=%d), special);

	const prpAttack* attack		= &( m_owner->m_curAttack );
	const prpAttack* ability	= &( m_owner->m_curAbility );

	switch ( special )
	{
	case 1:
	case 2:
	case 3:
		if ( sx::cmn::Random(1.0f) > ability->chance )
			special = 0;
		break;
	}

	switch ( special )
	{
	case 0:
		{
			Projectile* proj	= attack->projectile->Clone();

			proj->m_killParty	= m_owner->m_partyEnemies;
			proj->m_attack		= *attack;
			proj->m_target		= m_target;
			proj->m_sender		= m_owner;

			proj->m_dir.Normalize( m_Dir );
			proj->m_pos		= m_nodePipe->GetPosition_world();
			proj->m_pos		+= m_Dir * proj->m_node->GetSphere_local().r;
			

			ProjectileManager::AddProjectile(proj);
		}
		break;

	case 1:
		{
			int count = ability->actionCount + sx::cmn::Random(1) - sx::cmn::Random(1);
			for ( int c=0; c<count; c++ )
			{
				Projectile* proj	= ability->projectile->Clone();

				proj->m_killParty	= m_owner->m_partyEnemies;
				proj->m_attack		= *ability;
				proj->m_target		= m_target;
				proj->m_sender		= m_owner;

				proj->m_pos = m_nodePipe->GetPosition_world();
				proj->m_pos += m_Dir * proj->m_node->GetSphere_local().r;

				const float range = 0.2f;
				float3 dir(
					m_Dir.x + sx::cmn::Random(range) - sx::cmn::Random(range),
					m_Dir.y + sx::cmn::Random(range) - sx::cmn::Random(range),
					m_Dir.z + sx::cmn::Random(range) - sx::cmn::Random(range)
					);
				proj->m_dir.Normalize( dir );

				ProjectileManager::AddProjectile(proj);
			}
		}
		break;

	case 2:
	case 3:
		{
			Projectile* proj	= ability->projectile->Clone();

			proj->m_killParty	= m_owner->m_partyEnemies;
			proj->m_attack		= *ability;
			proj->m_sender		= m_owner;
			proj->m_target		= m_target;

			proj->m_dir.Normalize( m_Dir );
			proj->m_pos		= m_nodePipe->GetPosition_world();
			proj->m_pos		+= m_Dir * proj->m_node->GetSphere_local().r;

			ProjectileManager::AddProjectile(proj);
		}
		break;
	}
	
}
