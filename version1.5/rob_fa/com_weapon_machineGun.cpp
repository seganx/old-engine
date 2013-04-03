#include "com_weapon_MachineGun.h"
#include "Entity.h"
#include "Projectile.h"
#include "ProjectileManager.h"


#define MACHINEGUN_FIRE_START	4

com_weapon_MachineGun::com_weapon_MachineGun( void ): Component()
	, m_target(0)
	, m_nodeWeapon(0)
	, m_pipeIndex(0)
	, m_shootCount(0)
	, m_shootTime(0)
	, m_Dir(0,0,1)
	, m_DirOffset(0,0,1)
	, m_fire(0)
{
	sx_callstack();

	ZeroMemory( m_nodePipe, sizeof(m_nodePipe) );
	m_name = L"Machinegun";
	m_special.m_nodeName = L"_ondischarge";
	m_special.m_tag = MAKEFOURCC('W','M','G','N');
}

com_weapon_MachineGun::~com_weapon_MachineGun( void )
{
	sx_callstack();
}

void com_weapon_MachineGun::Initialize( void )
{
	sx_callstack();

	m_owner->m_weaponType = GWT_MACHINEGUN;

	//  find weapon node
	if ( m_owner->m_node )
	{
		m_owner->m_node->GetChildByName(L"weapon", m_nodeWeapon);
		m_owner->m_node->GetChildByName(L"pipe0", m_nodePipe[0]);
		m_owner->m_node->GetChildByName(L"pipe1", m_nodePipe[1]);
		m_owner->m_node->GetChildByName(L"pipe2", m_nodePipe[2]);
		m_owner->m_node->GetChildByName(L"pipe3", m_nodePipe[3]);
	}

	//  invisible meshes of fire and stop particles
	if ( m_nodeWeapon )
	{
		float3 face(0,0,1);
		m_Dir.Transform_Norm( face, m_nodeWeapon->GetMatrix_world() );
		m_DirOffset = m_Dir;

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
}

void com_weapon_MachineGun::Finalize( void )
{
	sx_callstack();

	m_nodeWeapon = NULL;
	ZeroMemory( m_nodePipe, sizeof(m_nodePipe) );
}

void com_weapon_MachineGun::Update( float elpsTime )
{
	if ( !m_owner || !m_nodeWeapon || !m_nodePipe[0] || m_owner->m_health.icur<1 ) return;
	sx_callstack();

	m_shootTime += elpsTime;

	//  invisible meshes of fire
	if ( m_shootTime < 200 && m_shootTime > 40 )
	{
		msg_Mesh msgMesh( SX_MESH_INVISIBLE );
		for (int i=0; i<4; i++)
		{
			if ( m_nodePipe[i] )
				m_nodePipe[i]->MsgProc( MT_MESH, &msgMesh );
		}
	}

	// verify that this entity can fire
	if ( m_target && m_owner->m_curAttack.projectile && m_owner->m_curAttack.rate>0 && m_target->m_health.icur>0 )
	{
		Sphere			targetSphere	= m_target->GetBoundingSphere();
		prpAttack*		attack			= &(m_owner->m_curAttack);
		
		//  compute direction to the target
		m_DirOffset.Normalize( targetSphere.center - m_nodeWeapon->GetPosition_world() );
		BlendDirection( elpsTime * 0.005f );
		m_nodeWeapon->SetDirection_world( m_Dir );

		//  compute max shoot time depend on fire rate
		const float	maxShootTime = ( (attack->rate > 0.01f) ? (1000.0f / attack->rate) : 0.0f ) / MACHINEGUN_FIRE_START;
		if (  m_fire &&  m_DirOffset.Dot( m_Dir ) > 0.97f && m_shootTime > maxShootTime )
		{
			m_shootTime = 0;

			//  rotate and show the pipe
			msg_Mesh msgMesh( 0, SX_MESH_INVISIBLE );
			m_nodePipe[ m_pipeIndex ]->MsgProc( MT_MESH, &msgMesh );
			m_nodePipe[ m_pipeIndex ]->SetRotation( 0, 0, sx::cmn::Random(6.0f) );

			//  play a sound
			msg_SoundPlay msgSound(true, sx::cmn::Random(1000), 0, L"fire", m_owner->m_level );
			m_owner->m_node->MsgProc( MT_SOUND_PLAY, &msgSound );

			//	spray bullet particle
			str256 bulletShell = L"bullet_shell"; bulletShell << m_pipeIndex;
			msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, bulletShell );
			m_nodeWeapon->MsgProc( MT_PARTICLE, &msgPar );

			m_shootCount++;
			if ( m_shootCount > 15 )
			{
				msg_Particle msgPrtcl(SX_PARTICLE_SPRAY);
				m_nodePipe[ m_pipeIndex ]->MsgProc( MT_PARTICLE, &msgPrtcl);
				m_shootCount = sx::cmn::Random(5);
			}
			

			if ( m_owner->m_level > MAX_LEVEL )
			{
				m_special.m_coolTime = m_owner->m_curAbility.actionTime;
				m_special.m_count	 = m_owner->m_curAbility.actionCount;
				m_special.m_value	 = - int( m_owner->m_curAbility.physicalDamage + 0.5f );
				ShootTheBullet( attack, targetSphere, true );
			}
			else ShootTheBullet( attack, targetSphere, false );

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

void com_weapon_MachineGun::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(com_weapon_MachineGun::MsgProc(msg=%d), msg);

	switch (msg)
	{
	case GMT_FIRE: if ( data && m_nodeWeapon )
				   {
						msgFire* mf = (msgFire*)data;
						if ( m_fire == 0 )
						{
							mf->fired = true;	//  say to AI that i fired

							m_target = (Entity*)mf->entity;
							if ( m_target )
							{
								const prpAttack* pAttack = &( m_owner->m_curAttack );
								float dis = m_owner->GetDistance_edge( m_target );
								if ( dis < pAttack->maxRange || dis > pAttack->minRange )
									m_fire = MACHINEGUN_FIRE_START;
								else 
									m_target = NULL;
							}
						}
				   }
				   break;

	case GMT_I_DIED: if ( data == m_target )
					 {
						 m_target = NULL;
					 }
					 else if ( data == m_owner && m_nodeWeapon )
					 {
						 //m_nodeWeapon->SetDirection_local( float3(0,0,0.99f) );
						 
						 //  invisible meshes of fire and stop particles
						 msg_Mesh msgMesh( SX_MESH_INVISIBLE );
						 for (int i=0; i<4 && m_nodeWeapon; i++)
						 {
							 if ( m_nodePipe[i] )
								 m_nodePipe[i]->MsgProc( MT_MESH, &msgMesh );
						 }
					 }
					 break;
	}
}

Component* com_weapon_MachineGun::Clone( void )
{
	sx_callstack();

	com_weapon_MachineGun * me	=	sx_new( com_weapon_MachineGun );
	me->m_tag					=	m_tag;

	return me;
}

void com_weapon_MachineGun::BlendDirection( float elpsTime )
{
	sx_callstack();

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

void com_weapon_MachineGun::ShootTheBullet( const prpAttack* pAttack, Sphere& targetSphere, bool special )
{
	sx_callstack_param(com_weapon_MachineGun::ShootTheBullet(special=%d), byte(special));

	//  shoot the bullet
	Projectile* proj			= pAttack->projectile->Clone();
	proj->m_killParty			= m_owner->m_partyEnemies;
	proj->m_target				= m_target;

	if ( m_fire == MACHINEGUN_FIRE_START )
	{
		proj->m_sender				= m_owner;
		proj->m_attack				= *pAttack;
		proj->m_component			= special ? &m_special : 0;
	}

	proj->m_dir.Normalize( targetSphere.center - m_nodePipe[m_pipeIndex]->GetPosition_world() );
	proj->m_pos	= m_nodePipe[ m_pipeIndex ]->GetPosition_world();
	proj->m_pos	+= m_Dir * proj->m_node->GetSphere_local().r;

	ProjectileManager::AddProjectile(proj);

	//  change pipe index
	switch (m_pipeIndex)
	{
	case 0:	m_pipeIndex = m_nodePipe[1] ? 1 : 0;	break;
	case 1:	m_pipeIndex = m_nodePipe[2] ? 2 : 0;	break;
	case 2:	m_pipeIndex = m_nodePipe[3] ? 3 : 0;	break;
	case 3:	m_pipeIndex = 0; break;
	}

	m_fire--;
}
