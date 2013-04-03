#include "com_weapon_Luncher.h"
#include "Entity.h"
#include "Projectile.h"
#include "ProjectileManager.h"


com_weapon_Luncher::com_weapon_Luncher( void ) : Component()
	, m_target(0)
	, m_nodeWeapon(0)
	, m_pipeIndex(0)
	, m_shootTime(0)
	, m_Fire(false)
{
	sx_callstack();

	ZeroMemory( m_nodePipe, sizeof(m_nodePipe) );
	m_name = L"Launcher";
}

com_weapon_Luncher::~com_weapon_Luncher( void )
{
	sx_callstack();
}

void com_weapon_Luncher::Initialize( void )
{
	sx_callstack();

	m_owner->m_weaponType = GWT_LAUNCHER;

	//  find weapon node
	if ( m_owner->m_node )
	{
		m_owner->m_node->GetChildByName(L"weapon",	m_nodeWeapon);
		m_owner->m_node->GetChildByName(L"pipe0",	m_nodePipe[0]);
		m_owner->m_node->GetChildByName(L"pipe1",	m_nodePipe[1]);
		m_owner->m_node->GetChildByName(L"pipe2",	m_nodePipe[2]);
		m_owner->m_node->GetChildByName(L"pipe3",	m_nodePipe[3]);
	}

	//  invisible meshes of fire and stop particles
	if ( m_nodeWeapon )
	{
		for ( int i=0; i<4; i++ )
		{
			if ( m_nodePipe[i] )
			{
				msg_Mesh msgMesh( SX_MESH_INVISIBLE );
				m_nodePipe[i]->MsgProc( MT_MESH, &msgMesh );
				msg_Particle msgPrtcl( 0, SX_PARTICLE_SPRAY, 0, true );
				m_nodePipe[i]->MsgProc( MT_PARTICLE, &msgPrtcl );
			}
		}
	}
}

void com_weapon_Luncher::Finalize( void )
{
	sx_callstack();

	m_nodeWeapon	= NULL;
	ZeroMemory( m_nodePipe, sizeof(m_nodePipe) );
}

void com_weapon_Luncher::Update( float elpsTime )
{
	if ( !m_owner || !m_nodeWeapon || !m_nodePipe[0] || m_owner->m_health.icur<1) return;
	sx_callstack();

	m_shootTime += elpsTime;

	//  collect some initial information
	const prpAttack*	attack = &( m_owner->m_curAttack );

	//  invisible meshes of fire
	if ( m_shootTime < 200 && m_shootTime > 40 )
	{
		msg_Mesh msgMesh( SX_MESH_INVISIBLE );
		if ( m_nodePipe[0] ) m_nodePipe[0]->MsgProc( MT_MESH, &msgMesh );
		if ( m_nodePipe[1] ) m_nodePipe[1]->MsgProc( MT_MESH, &msgMesh );
		if ( m_nodePipe[2] ) m_nodePipe[2]->MsgProc( MT_MESH, &msgMesh );
		if ( m_nodePipe[3] ) m_nodePipe[3]->MsgProc( MT_MESH, &msgMesh );
	}

	// verify that this entity can fire
	if ( m_target && m_Fire &&  attack->projectile && m_target->m_health.icur>0 )
	{
		//  compute max shoot time depend on fire rate
		const float	maxShootTime = (attack->rate > 0.01f) ? (1000.0f / attack->rate) : 0.0f ;

		if ( m_shootTime >  maxShootTime )
		{
			m_shootTime = 0;
			m_Fire = false;

			//  rotate and show the pipe
			floatQ q; q.SetRotationXYZ( 0, 0, sx::cmn::Random(6.0f) );
			m_nodePipe[m_pipeIndex]->SetQuaternion(q);
			msg_Mesh msgMesh( 0, SX_MESH_INVISIBLE );
			m_nodePipe[m_pipeIndex]->MsgProc( MT_MESH, &msgMesh );

			//	spray bullet particle
			str256 bulletShell = L"bullet_shell"; bulletShell << m_pipeIndex;
			msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, bulletShell );
			m_nodeWeapon->MsgProc( MT_PARTICLE, &msgPar );

			//  play a sound
			msg_SoundPlay msgSound( true, sx::cmn::Random(1000), 0, L"fire" );
			m_owner->m_node->MsgProc( MT_SOUND_PLAY, &msgSound );

			//  shoot the Missile
			ShootTheMissile();
		}

		if ( m_owner->GetDistance_edge( m_target ) > attack->maxRange )
		{
			m_target = NULL;
		}
	}
}

void com_weapon_Luncher::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(com_weapon_Luncher::MsgProc(msg=%d), msg);

	switch (msg)
	{
	case GMT_FIRE: if ( data && m_nodeWeapon && m_nodePipe[0] && m_owner->m_curAttack.projectile )
				   {
						msgFire* mf = (msgFire*)data;
						mf->fired = true;	//  say to AI that i fired
						m_target = (Entity*)mf->entity;
						m_Fire = true;
				   }
				   break;

	case GMT_I_DIED: if ( data == m_owner && m_nodeWeapon )
					 {
						 //  invisible meshes of fire and stop particles
						 msg_Mesh msgMesh( SX_MESH_INVISIBLE );
						 if ( m_nodePipe[0] ) m_nodePipe[0]->MsgProc( MT_MESH, &msgMesh );
						 if ( m_nodePipe[1] ) m_nodePipe[1]->MsgProc( MT_MESH, &msgMesh );
						 if ( m_nodePipe[2] ) m_nodePipe[2]->MsgProc( MT_MESH, &msgMesh );
						 if ( m_nodePipe[3] ) m_nodePipe[3]->MsgProc( MT_MESH, &msgMesh );
					 }
					 else if ( data == m_target )
					 {
						 m_target = NULL;
					 }

					 break;
	}
}

Component* com_weapon_Luncher::Clone( void )
{
	sx_callstack();

	com_weapon_Luncher * me	=	sx_new( com_weapon_Luncher );
	me->m_tag				=	m_tag;

	return me;
}

void com_weapon_Luncher::ShootTheMissile()
{
	if ( !m_target ) return;

	sx_callstack();

	Projectile* proj	= m_owner->m_curAttack.projectile->Clone();

	proj->m_killParty	= m_owner->m_partyEnemies;
	proj->m_attack		= m_owner->m_curAttack;
	proj->m_target		= m_target;
	proj->m_sender		= m_owner;

	float3 face( 0.0f, 0.0f, 1.0f );
	face.Transform_Norm( face, m_nodePipe[m_pipeIndex]->GetMatrix_world() );
	proj->m_dir.Normalize( face );
	proj->m_pos		= m_nodePipe[m_pipeIndex]->GetPosition_world();
	proj->m_pos		+= face * proj->m_node->GetSphere_local().r;
	
	ProjectileManager::AddProjectile(proj);
	
	//  change pipe index
	switch (m_pipeIndex)
	{
	case 0:	m_pipeIndex = m_nodePipe[1] ? 1 : 0;	break;
	case 1:	m_pipeIndex = m_nodePipe[2] ? 2 : 0;	break;
	case 2:	m_pipeIndex = m_nodePipe[3] ? 3 : 0;	break;
	case 3:	m_pipeIndex = 0; break;
	}	
}
