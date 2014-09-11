#include "Entity.h"
#include "EntityManager.h"
#include "ProjectileManager.h"
#include "GameTypes.h"
#include "Game.h"
#include "Player.h"
#include "Task.h"
#include "GameConfig.h"

static Entity*	s_Selected = NULL;

//////////////////////////////////////////////////////////////////////////
//	GAME ENTITY
//////////////////////////////////////////////////////////////////////////
Entity::Entity( void )
: m_initialized(false)
, m_upgradeReady(false)
, m_state(ES_DEAD)
, m_partyCurrent(0)
, m_partyEnemies(0)
, m_ID(0)
, m_node(NULL)
, m_mesh(NULL)
, m_pos(0, 0, 0)
, m_posOffset(0, 0, 0)
, m_face(0, 0, 1)
, m_level(-1)
, m_maxLevel(NUM_LEVELS-1)
, m_weaponType(GWT_NULL)
, m_experience(0)
, m_levelVisual(0,1,2,1)
, m_traveling(1)
, m_travelingGUI(0)
, test_onDamageXP(0)
, test_onDeadXP(0)
{
	sx_callstack();

	m_ID = g_game->GetNewID();
	SetDirection(m_face);
	m_brain.SetOwner(this);
	ZeroMemory( m_cost, sizeof(m_cost) );
	ZeroMemory( m_costXP, sizeof(m_costXP) );
}

Entity::~Entity( void )
{
	sx_callstack_param(Entity::~Entity(%s), m_typeName.Text());

	if ( m_initialized ) Finalize();

	for ( int i=0; i<m_components.Count(); i++ )
	{
		PComponent com = m_components[i];

		if ( com->m_tag == MAKEFOURCC('M','C','T','C') )
		{
			sxLog::Log( L" ERROR : try to remove Mechanic_CreateTower::m_showRange in entity deletation" );
		}
		else
		{
			com->m_owner = NULL;
			sx_delete_and_null( com );
		}
	}
	m_components.Clear();

	sx::core::Scene::DeleteNode( m_node );
}

void Entity::Attach( Component* com )
{
	if ( !com ) return;
	sx_callstack_param(Entity[%s]::Attach(com=%s), m_typeName.Text(), com->m_name.Text());

	if ( com->m_owner && com->m_owner != this )
		com->m_owner->m_components.Remove( com );

	if ( m_components.IndexOf( com ) < 0 )
		m_components.PushBack( com );

	com->m_owner = this;

	if ( m_initialized )
		com->Initialize();
}

void Entity::Detach( Component* com )
{
	if ( !com ) return;

	sx_callstack_param(Entity[%s]::Detach(com=%s), m_typeName.Text(), com->m_name.Text());

	if ( m_components.Remove( com ) )
	{
		if ( m_initialized )
			com->Finalize();
		com->m_owner = NULL;
	}
}

void Entity::Initialize( void )
{
	if ( m_initialized ) return;

	sx_callstack_param(Entity[%s]::Initialize(), m_typeName.Text());

	for ( int i=0; i<NUM_LEVELS; i++ )
		m_attackLevel[i].projectile = ProjectileManager::GetTypeByName( m_attackLevel[i].bullet );
	m_curAttack.projectile = m_curAttackLevel.projectile = m_attackLevel[0].projectile;

	SetState(ES_IDLE);

	g_game->PostMessage(0, GMT_I_INITIALIZED, this);

	if (m_node)
	{
		msg_Particle msgPartcl(0,0,0,true);
		m_node->MsgProc( MT_PARTICLE, &msgPartcl );

		m_node->SetUserData( this );
		m_node->SetUserTag(m_partyCurrent);

		sx::core::Scene::AddNode( m_node );
		m_node->Update(0);
	}
	
	for (int i=0; i<m_components.Count(); i++)
		m_components[i]->Initialize();

	m_brain.Initialize();

	m_initialized = true;
}

void Entity::Finalize( void )
{
	sx_callstack_param(Entity[%s]::Finalize(), m_typeName.Text());

	sx_delete_and_null( m_travelingGUI );

	if ( s_Selected == this )
		s_Selected = NULL;

	if ( !m_initialized ) return;
	m_initialized = false;

	g_game->PostMessage(0, GMT_I_FINALIZED, this);

	m_brain.Finalize();

	for (int i=0; i<m_components.Count(); i++)
		m_components[i]->Finalize();

	EntityManager::RemoveEntity( m_ID );
	sx::core::Scene::DeleteNode( m_node );
}

const float3& Entity::GetPosition() const
{
	sx_callstack();

	return m_pos;
}

inline void Entity::SetPosition( const float3& pos )
{
	sx_callstack();

	m_pos = pos;
	if (m_node)
	{
		float3 npos = m_pos + m_posOffset;
		m_node->SetPosition( npos );
	}
}

inline const float3& Entity::GetDirection( void ) const
{
	sx_callstack();

	return m_face;
}

inline void Entity::SetDirection( const float3& dir )
{
	sx_callstack();

	m_face = dir;
	if (m_node)
		m_node->SetDirection_local(dir);
}

Brain* Entity::GetBrain( void )
{
	sx_callstack();
	return &m_brain;
}

Sphere Entity::GetBoundingSphere( void )
{
	sx_callstack();

	Sphere res( GetPosition(), 0 );
	if ( m_mesh )
		m_mesh->MsgProc( MT_GETSPHERE_WORLD, &res );
	return res;
}

float Entity::GetDistance_edge( Entity* target )
{
	sx_callstack();

	if ( !target ) return FLT_MAX;

	if ( m_partyCurrent == PARTY_TOWER )
	{
		Sphere targetsphere = target->GetBoundingSphere();
		return GetBoundingSphere().center.Distance( targetsphere.center ) - targetsphere.r;
	}
	else
	{
		return GetBoundingSphere().center.Distance( target->GetBoundingSphere().center ) - 2.0f;
	}
}


void Entity::SetLevel( int level )
{
	sx_callstack_param(Entity[%s]::SetLevel(level=%d), m_typeName.Text(), level);

	//  verify that the level is exist in tower
	if ( m_maxLevel > 6 )
	{
		if ( Config::GetData()->display_Debug == 3 )
			sxLog::Log( L"WARNING : maxLevel out of range for entity '%s'", m_typeName.Text() );
		m_maxLevel = 6;
	}
	if ( level <= m_level || level > m_maxLevel ) return;

	//  apply health value
	float healthPers = (float)m_health.icur / (float)m_health.imax;
	m_health.imax = m_health.level[ level ];
	m_health.icur =  int( healthPers * m_health.imax );

	m_level = level;

	if ( m_level < MAX_LEVEL )
		m_curAttackLevel = m_attackLevel[ level ];
	else
		m_curAbility = m_attackLevel[ level ];

	//  apply level to 3d scene node
	if ( m_node && m_partyCurrent == PARTY_TOWER )
	{
		sx::core::PNode node = NULL;
		if ( level==0 && m_node->GetChildByName( L"_oncreate", node ) )
		{
			msg_Particle msgPar( SX_PARTICLE_SPRAY );
			node->MsgProc( MT_PARTICLE, &msgPar );

			msg_SoundPlay msgSnd( true );
			node->MsgProc( MT_SOUND_PLAY, &msgSnd );
		}

		if ( m_node->GetChildByName( L"_onupgrade", node ) )
		{
			msg_Particle msgPar( SX_PARTICLE_SPRAY );
			node->MsgProc( MT_PARTICLE, &msgPar );
		}

		if ( m_level )
		{
			msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, L"displaylevel", false, m_level-1 );
			m_node->MsgProc( MT_PARTICLE, &msgPar );
		}

		if ( level == 2 )
		{
			msg_Animator msgAnim( SX_ANIMATION_PLAY, 0, 0, 4, 1 );
			m_node->MsgProc( MT_ANIMATOR, &msgAnim );
		}

		msg_SoundPlay msgSound(true, 0, 0, L"upgrade", level);
		m_node->MsgProc( MT_SOUND_PLAY, &msgSound );

		if ( level == 6 )
		{
#if USE_STEAM_SDK
			g_game->m_steam.CallAchievement( EAT_Perfect_Battle, ESC_InPlay );
#else
			g_game->m_achievements[EAT_Perfect_Battle].AddValue();
#endif
		}
	}
}

void Entity::SetState( UINT state )
{
	if ( m_state == state ) return;

	sx_callstack_param(Entity::SetState(state=%d), m_typeName.Text(), state);

	m_state = state;

	switch (state)
	{
	case ES_IDLE:
		if ( m_node && m_partyCurrent != PARTY_TOWER )
		{
			msg_Animator msgAnim(SX_ANIMATION_PLAY, 0, 0, sx::cmn::Random(3), 1.0f );
			m_node->MsgProc( MT_ANIMATOR, &msgAnim );
		}
		break;

	case ES_WALK:
		if ( m_node && m_partyCurrent != PARTY_TOWER )
		{
			msg_Animator msgAnim(SX_ANIMATION_PLAY, 0, 0, 4 + sx::cmn::Random(3), m_move.animSpeed, m_move.animScale, 0.0f );
			m_node->MsgProc( MT_ANIMATOR, &msgAnim );

			if ( Config::GetData()->display_Debug == 3 && ( m_move.animSpeed < 1.0f || m_move.animScale < 1.0f ) )
				sxLog::Log( L"WARNING : entity %s has %.2f animSpeed & %.2f animScale!", m_typeName.Text(), m_move.animSpeed, m_move.animScale );
		}
		break;

	case ES_DIE:
		m_move.animScale = 1.0f;
		m_move.animSpeed = 1.0f;
		sx_delete_and_null( m_travelingGUI );

		{	//	stop sounds and particles
			msg_SoundStop msgSnd( false );
			m_node->MsgProc( MT_SOUND_STOP, &msgSnd );

			msg_Particle msgPar( 0, SX_PARTICLE_SPRAY, 0, true );
			m_node->MsgProc( MT_PARTICLE, &msgPar );
		}

		g_game->PostMessage(0, GMT_I_DIED, this);

		if ( m_node )
			m_node->SetUserTag(0);

		//	a tower is going to die
		if ( m_node && m_partyCurrent == PARTY_TOWER )
		{
			sx::core::PNode node = NULL;

			if ( m_health.icur == HEALTH_SELL )
			{
				if ( m_node->GetChildByName( L"_onsell", node ) )
				{
					msg_Particle msgPar( SX_PARTICLE_SPRAY );
					node->MsgProc( MT_PARTICLE, &msgPar );

					msg_SoundPlay msgSnd( false );
					node->MsgProc( MT_SOUND_PLAY, &msgSnd );
				}

				msg_Mesh msgMat(0, 0, 0, 0);
				m_mesh->MsgProc( MT_MESH, &msgMat );
			}
			else
			{
				msg_Animator msgAnim(SX_ANIMATION_PLAY, 0, 0, 5, 1.0f );
				m_node->MsgProc( MT_ANIMATOR, &msgAnim );

				if ( m_node->GetChildByName( L"_ondead", node ) )
				{
					msg_Particle msgPar( SX_PARTICLE_SPRAY );
					node->MsgProc( MT_PARTICLE, &msgPar );

					msg_SoundPlay msgSnd( false );
					node->MsgProc( MT_SOUND_PLAY, &msgSnd );
				}
			}

			if ( m_node->GetChildByName( L"_oninjured", node ) )
			{
				msg_Particle msgPar( 0, SX_PARTICLE_SPRAY );
				node->MsgProc( MT_PARTICLE, &msgPar );
			}

			//  hide display level
			{
				msg_Particle msgPar( 0, SX_PARTICLE_SPRAY, L"displaylevel", true);
				m_node->MsgProc( MT_PARTICLE, &msgPar );
			}			
		}

		//	an enemy is going to die
		else if ( m_node && m_partyCurrent != PARTY_TOWER )
		{
			msg_Animator msgAnim(SX_ANIMATION_PLAY, 0, 0, 8 + sx::cmn::Random(3), 1.0f );
			m_node->MsgProc( MT_ANIMATOR, &msgAnim );

			sx::core::PNode node = NULL;
			if ( m_node->GetChildByName( L"_ondead", node ) )
			{
				msg_Particle msgPar( SX_PARTICLE_SPRAY );
				node->MsgProc( MT_PARTICLE, &msgPar );
			}

			{
				int ecount = EntityManager::GetEntityCount() + 1;
				float chance = 10.0f / ecount;
				if ( sx::cmn::Random(1.0f) < chance )
				{
					msg_SoundPlay msgSound( true, 0, 0, L"dead", sx::cmn::Random(15) );
					m_node->MsgProc( MT_SOUND_PLAY, &msgSound );
				}
			}

#if USE_GAMEUP
			if ( m_typeName.Find( L"boss" )>-1 || m_typeName.Find( L"Boss" )>-1 )
			{
				gameup_add_score( GAME_SCORE_BOSS );
			}
#endif
		}

		//  set maximum dead time
		if ( m_node )
		{
			msg_Animator_Count msgAnim;
			m_node->MsgProc( MT_ANIMATOR_COUNT, &msgAnim );
			if ( msgAnim.numAnimator )
			{
				sx::core::PAnimator pAnim = (sx::core::PAnimator)msgAnim.animators[0];
				m_health.deathTime = pAnim->GetMaxTime();
			}
		}

		break;

	case ES_DEAD:

		break;

	case ES_SPELLING:
		{
			m_move.moveScale = 0.0f;
			msg_Animator msg(0, 0, 0, 12 );
			m_node->MsgProc( MT_ANIMATOR, &msg );
		}
		break;
	}
}

void Entity::Update( float elpsTime )
{
	sx_callstack();

	sx_assert(m_node);

	if ( !m_node )
	{
		sxLog::Log( L"ERROR : Entity '%s' has no node !!", m_typeName.Text() );
		return;
	}

	//	update attack properties
	if ( m_health.icur > 0 )
	{
		if ( m_level > MAX_LEVEL )
			m_curAbility = m_attackLevel[m_level];

		float curRange = m_curAttack.maxRange;
		m_curAttack = m_curAttackLevel;
		float finalRange = m_attackLevel[m_level].maxRange;
		if ( m_partyCurrent == PARTY_TOWER )	//	apply upgrade panel
		{
			switch ( m_weaponType )
			{
			case GWT_MACHINEGUN:	finalRange *= g_game->m_upgrades.machinegun_range;	break;
			case GWT_SNOWER:		finalRange *= g_game->m_upgrades.snower_range;		break;
			case GWT_MORTAR:		finalRange *= g_game->m_upgrades.mortar_range;		break;
			case GWT_GROUNDLAVA:	finalRange *= g_game->m_upgrades.groundlava_range;	break;
			}
		}
		curRange += ( finalRange - curRange ) * elpsTime * 0.005f;
		m_curAttack.maxRange = curRange;

		if ( m_partyCurrent == PARTY_TOWER )
		{
			//	apply upgrade panel
			switch ( m_weaponType )
			{
			case GWT_MACHINEGUN:
				m_curAttack.rate				*= g_game->m_upgrades.machinegun_firerate;
				m_curAttack.physicalDamage		*= g_game->m_upgrades.machinegun_damage;
				m_curAttack.electricalDamage	*= g_game->m_upgrades.machinegun_damage;
				m_curAbility.actionCount		= int( m_curAbility.actionCount * g_game->m_upgrades.machinegun_discharger_count );
				m_curAbility.physicalDamage		*= g_game->m_upgrades.machinegun_discharger_power;
				break;

			case GWT_SNOWER:
				m_curAttack.rate				*= g_game->m_upgrades.snower_firerate;
				m_curAttack.stunTime			*= g_game->m_upgrades.snower_freeze;
				m_curAttack.stunValue			*= g_game->m_upgrades.snower_freeze;
				m_curAbility.chance				*= g_game->m_upgrades.snower_airback_chance;
				m_curAbility.coolTime			*= g_game->m_upgrades.snower_airback_cooltime;
				m_curAbility.stunTime			*= g_game->m_upgrades.snower_airback_time;
				break;

			case GWT_MORTAR:
				m_curAttack.splashRadius		*= g_game->m_upgrades.mortar_splash;
				m_curAttack.physicalDamage		*= g_game->m_upgrades.mortar_damage;
				m_curAttack.electricalDamage	*= g_game->m_upgrades.mortar_damage;
				m_curAbility.chance				*= g_game->m_upgrades.mortar_ability_chance;
				m_curAbility.actionCount		+= int( g_game->m_upgrades.mortar_ability_count );
				m_curAbility.minRange			*= g_game->m_upgrades.mortar_ability_range;				
				break;

			case GWT_GROUNDLAVA:
				m_curAttack.rate				*= g_game->m_upgrades.groundlava_firerate;
				m_curAttack.physicalDamage		*= g_game->m_upgrades.groundlava_damage;
				m_curAttack.electricalDamage	*= g_game->m_upgrades.groundlava_damage;
				m_curAbility.chance				*= g_game->m_upgrades.groundlava_headshot_chance;
				m_curAbility.coolTime			*= g_game->m_upgrades.groundlava_headshot_cooltime;
				m_curAbility.actionTime			*= g_game->m_upgrades.groundlava_headshot_time;
				break;

			case GWT_FLIGHTER:
				m_curAttack.physicalDamage		*= g_game->m_upgrades.flighter_damage;
				m_curAttack.electricalDamage	*= g_game->m_upgrades.flighter_damage;
				m_curAbility.chance				+= g_game->m_upgrades.flighter_crazy_chance;
				m_curAbility.coolTime			*= g_game->m_upgrades.flighter_crazy_cooltime;
				m_curAbility.actionTime			+= g_game->m_upgrades.flighter_crazy_time;
				m_curAbility.splashRadius		*= g_game->m_upgrades.flighter_crazy_range;
				break;
			}

			if ( m_level < m_maxLevel && m_experience >= m_costXP[m_level+1] && g_game->m_player->m_gold >= m_cost[m_level+1] )
			{
				if ( !m_upgradeReady )
				{
					m_upgradeReady = true;
					msg_Particle msgPar( SX_PARTICLE_SPRAY, 0, L"upgradeready" );
					m_node->MsgProc( MT_PARTICLE, &msgPar );
				}
			}
			else
			{
				if ( m_upgradeReady )
				{
					m_upgradeReady = false;
					msg_Particle msgPar( 0, SX_PARTICLE_SPRAY, L"upgradeready" );
					m_node->MsgProc( MT_PARTICLE, &msgPar );
				}
			}
		}		

	}

	for (int i=0; i<m_components.Count(); i++)
		m_components[i]->Update(elpsTime);

	for (int i=0; i<m_components.Count(); i++)
	{
		Component* com = m_components[i];
		if ( com->m_deleteMe )
		{
			m_components.RemoveByIndex(i);
			i--;

			com->Finalize();
			com->m_owner = NULL;			
			sx_delete_and_null( com );
		}
	}

	if ( m_health.icur > 0 )
	{
		m_brain.Update(elpsTime);

		//	play talk sound with a chance
#if 0
		static float soundChance = sx::cmn::Random(5000.0f);
		soundChance += elpsTime;
		if ( soundChance > EntityManager::GetEntityCount() * 5000.0f )
		{
			soundChance = sx::cmn::Random( EntityManager::GetEntityCount() * 3000.0f );
			msg_SoundPlay msgSound(true, 0, 0, L"talk", sx::cmn::Random(20) );
			m_node->MsgProc( MT_SOUND_PLAY, &msgSound );
		}
#endif

	}

	//	apply visualize for towers
	if ( m_partyCurrent == PARTY_TOWER )
	{
		switch ( m_level )
		{
		case 1: case 2: case 3:
			{
				const float ftime = elpsTime * 0.001f;
				m_levelVisual.x += ( 2.0f - m_levelVisual.x ) * ftime;
				m_levelVisual.y += ( 1.0f - m_levelVisual.y ) * ftime;
				m_levelVisual.z += ( 0.0f - m_levelVisual.z ) * ftime;
				m_mesh->GetMaterial(0)->SetFloat4(0, m_levelVisual );
			}
			break;

		case 4: case 5: case 6:
			{
				const float ftime = elpsTime * 0.001f;
				m_levelVisual.x += ( 1.5f - m_levelVisual.x ) * ftime;
				m_levelVisual.y += ( 0.3f - m_levelVisual.y ) * ftime;
				m_levelVisual.z += ( 0.3f - m_levelVisual.z ) * ftime;
				m_mesh->GetMaterial(0)->SetFloat4(0, m_levelVisual );
			}
			break;
		}
	}
}

void Entity::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(Entity[%s]::MsgProc(msg=%d), m_typeName.Text(), msg);

	if ( !m_initialized ) return;

	for (int i=0; i<m_components.Count(); i++)
		m_components[i]->MsgProc(msg, data);

	switch (msg)
	{
	case GMT_DAMAGE:
		msgDamage* damage = (msgDamage*)data;
		if ( damage && m_health.icur>0 )
		{
			m_curAttackLevel.physicalArmor -= damage->physicalArmor;
			if ( m_curAttackLevel.physicalArmor < 0 ) m_curAttackLevel.physicalArmor = 0;

			m_curAttackLevel.electricalArmor -= damage->electricalArmor;
			if ( m_curAttackLevel.electricalArmor < 0 ) m_curAttackLevel.electricalArmor = 0;

			float healthdamage = damage->physicalDamage * ( 1 - m_curAttackLevel.physicalArmor ) + damage->electricalDamage	* ( 1 - m_curAttackLevel.electricalArmor );
			m_health.icur -= int( healthdamage + 0.5f );

			//	compute experience
			if ( damage->sender && damage->sender->m_partyCurrent == PARTY_TOWER )
			{
				float addXP;
				if ( m_health.icur > 0 )
					addXP = damage->sender->test_onDamageXP * m_experience;
				else
					addXP = damage->sender->test_onDeadXP * m_experience;
				damage->sender->m_experience += addXP;

#if USE_STEAM_SDK
				g_game->m_steam.CallStat( EST_Score, ESC_InPlay, addXP );
#endif
			}

			//	compute achievement values
			if ( damage->tag && m_health.icur < 1 )
			{
#if USE_STEAM_SDK
				g_game->m_steam.CallAchievement( damage->tag, ESC_InPlay );
#else
				g_game->m_achievements[ damage->tag ].AddValue();
#endif
			}
			sx::core::PNode node = NULL;
			if ( m_node->GetChildByName( L"_ondamage", node ) )
			{
				msg_Particle msgPar( SX_PARTICLE_SPRAY );
				node->MsgProc( MT_PARTICLE, &msgPar );
			}

			float chance = 5.0f / (EntityManager::GetEntityCount() + 1);
			if ( sx::cmn::Random(1.0f) < chance )
			{
				msg_SoundPlay msgSound(true, 0, 0, L"damage", sx::cmn::Random(5) );
				m_node->MsgProc( MT_SOUND_PLAY, &msgSound );
			}
		}

		//	apply injured effects
		{
			sx::core::PNode node = NULL;
			if ( m_node->GetChildByName( L"_oninjured", node ) )
			{
				if ( m_health.icur * 2 < m_health.imax && m_health.icur>0 )
				{
					msg_Particle msgPar( SX_PARTICLE_SPRAY );
					node->MsgProc( MT_PARTICLE, &msgPar );

					if ( m_partyCurrent == PARTY_TOWER )
					{
						msg_SoundPlay msgSnd( false );
						node->MsgProc( MT_SOUND_PLAY, &msgSnd );
					}
				}
				else
				{
					msg_Particle msgPar( 0, SX_PARTICLE_SPRAY );
					node->MsgProc( MT_PARTICLE, &msgPar );

					if ( m_partyCurrent == PARTY_TOWER )
					{
						msg_SoundStop msgSnd( false );
						node->MsgProc( MT_SOUND_STOP, &msgSnd );
					}
				}	
			}
		}
		break;
	}

	m_brain.MsgProc( msg, data );

}

Entity* Entity::Clone( void )
{
	sx_callstack_param(Entity[%s]::Clone(), m_typeName.Text());

	//  verify that this entity is not initialized
	sx_assert( m_initialized==false );
	if ( m_initialized )
	{
		sxLog::Log( L"Can't clone initialized entity ! typeName : %s", m_typeName );
		return NULL;
	}

	Entity* pe = sx_new( Entity );
	if (pe)
	{
		pe->m_typeName		= m_typeName;
		pe->m_displayName	= m_displayName;

		pe->SetPosition( float3(5000, 5000, 5000) );
		pe->SetDirection( GetDirection() );

		pe->test_onDamageXP = test_onDamageXP;
		pe->test_onDeadXP = test_onDeadXP;

		pe->m_partyCurrent	= m_partyCurrent;
		pe->m_partyEnemies	= m_partyEnemies;
		pe->m_maxLevel		= m_maxLevel;
		pe->m_health		= m_health;
		pe->m_move			= m_move;
		pe->m_curAttack		= m_curAttack;
		pe->m_curAttackLevel= m_curAttackLevel;
		pe->m_experience	= m_experience;
		

		for (int i=0; i<NUM_LEVELS; i++)
		{
			pe->m_cost[i]			= m_cost[i];
			pe->m_costXP[i]			= m_costXP[i];
			pe->m_attackLevel[i]	= m_attackLevel[i];
		}

		if (m_node)
		{
			pe->m_node = m_node->Clone();

			msg_Mesh_Count msgMesh;
			pe->m_node->MsgProc( MT_MESH_COUNT, &msgMesh );
			for ( int m = 0; m<msgMesh.numMeshes; m++ )
			{
				sx::core::Mesh* mesh = (sx::core::Mesh*)msgMesh.meshes[m];
				if ( wcscmp( mesh->GetName(), L"body" )==0 || wcscmp( mesh->GetName(), L"Body" )==0  )
					pe->m_mesh = mesh;
				break;
			}
		}

		for (int i=0; i<m_components.Count(); i++)
		{
			pe->Attach( m_components[i]->Clone() );
		}

	}
	return pe;
}

void Entity::SetSelected( Entity* pEntity )
{
	s_Selected = pEntity;
}

Entity* Entity::GetSelected( void )
{
	return s_Selected;
}



