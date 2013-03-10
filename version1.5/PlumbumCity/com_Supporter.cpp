#include "com_Supporter.h"
#include "Entity.h"
#include "Game.h"
#include "Player.h"


com_Supporter::com_Supporter( void ): Component()
	,	m_node(null)
	,	m_time(0.0f)
	,	m_energy(0.0f)
	,	m_repair(0.0f)
	,	m_overActiveTime(0.0f)
	,	m_time_exp(0.0f)
	,	m_towers(64)
{
	sx_callstack();

	m_name = L"Supporter";
	m_tag = MAKEFOURCC('C','M','S','P');
}

com_Supporter::~com_Supporter( void )
{
	sx_callstack();	
}

void com_Supporter::Initialize( void )
{
	sx_callstack();

	static sx::core::ArrayPNode_inline nodes(64);
	nodes.Clear();
	sx::core::Scene::GetNodesByArea( m_owner->GetPosition(), m_owner->m_curAttack.maxRange, nodes, NMT_ALL, PARTY_TOWER );

	m_towers.Clear();

	for ( int i = 0; i < nodes.Count(); ++i )
	{
		sx::core::PNode node = nodes[i];
		Entity* entity = static_cast<Entity*>(node->GetUserData());

		if ( (!entity) || (entity->m_health.icur < 1) )
		{
			continue;
		}

		const float distance_squared = m_owner->GetPosition().Distance_sqr( entity->GetPosition() );
		if ( distance_squared > m_owner->m_curAttack.maxRange * m_owner->m_curAttack.maxRange )
		{
			continue;
		}

		bool notSupporter = true;

		for ( int j = 0; j < entity->m_components.Count(); ++j )
		{
			if ( entity->m_components[j]->m_tag == m_tag )
			{
				notSupporter = false;
				break;
			}
		}
		
		if (notSupporter)
		{
			m_towers.PushBack(EntityExp(entity, entity->m_experience));
		}
	}

	m_time = 0.0f;
	m_energy = 0.0f;
	m_repair = 0.0f;
	m_overActiveTime = 0.0f;
	m_time_exp = 0.0f;
	m_owner->m_experience = 0.0f;	
}

void com_Supporter::Finalize( void )
{
	sx_callstack();
}

void com_Supporter::Update( float elpsTime )
{
	sx_callstack();

	if ( (!g_game->m_wavesComming) || (!m_owner) || (m_owner->m_health.icur < 1) || (!m_owner->m_node) )
	{
		return;
	}

	const float delta = elpsTime * 0.001f;

	if ( m_owner->m_level > MAX_LEVEL )
	{
		if ( m_overActiveTime <= 0.0f )
		{
			const float chance = sx::cmn::Random( 1.0f );

			if ( chance <= m_owner->m_curAbility.chance )
			{
				m_overActiveTime = m_owner->m_curAbility.actionTime;
				g_game->m_player->m_fastCoolDown += m_owner->m_curAbility.minRange;
			}
		}
		else
		{
			m_overActiveTime -= delta;

			if ( m_overActiveTime <= 0.0f )
			{
				m_overActiveTime = 0.0f;
				g_game->m_player->m_fastCoolDown -= m_owner->m_curAbility.minRange;
			}
		}
	}
	
	m_energy += m_owner->m_curAttack.stunValue * delta * ((m_overActiveTime > 0.0f) ? m_owner->m_curAbility.stunTime : 1.0f);
	m_repair += m_owner->m_curAttack.physicalDamage * delta;

	m_time += elpsTime;

	if ( m_time > 250.0f )
	{
		m_time = 0.0f;
		
		const int addEnergy = static_cast<int>(m_energy);
		m_energy -= addEnergy;
		g_game->m_player->m_energy += addEnergy;

		const int addHealth = static_cast<int>(m_repair);
		
		if ( addHealth > 0 )
		{
			for ( int i = 0; i < m_towers.Count(); ++i )
			{
				if ( m_towers[i].entity && m_towers[i].entity->m_health.icur > 0 )
				{
					m_towers[i].entity->m_health.icur +=
						m_towers[i].entity->m_health.icur + addHealth > m_towers[i].entity->m_health.imax ? 0 : addHealth;
				}
			}

			m_repair -= addHealth;
		}
	}

	m_owner->m_experience += m_owner->test_onDamageXP * delta;
	m_time_exp += elpsTime;

	if ( m_time_exp > 1000.0f )
	{
		m_time_exp = 0.0f;

		for ( int i = 0; i < m_towers.Count(); ++i )
		{
			if ( m_towers[i].entity && m_towers[i].entity->m_health.icur > 0 )
			{
				const float expDif = m_towers[i].entity->m_experience - m_towers[i].experience;
				m_towers[i].experience = m_towers[i].entity->m_experience;

				m_owner->m_experience += expDif;
			}
		}
	}
}

void com_Supporter::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(com_Supporter::MsgProc(msg=%d), msg);

	if ( !m_owner || m_owner->m_health.icur < 1 || !m_owner->m_node )
	{
		return;
	}

	switch ( msg )
	{
	case GMT_I_INITIALIZED:
		if ( data )
		{
			Entity* entity = static_cast<Entity*>(data);
			if ( entity->m_partyCurrent == PARTY_TOWER )
			{
				for ( int j = 0; j < entity->m_components.Count(); ++j )
				{
					if ( entity->m_components[j]->m_tag == m_tag )
					{				
						return; // 
					}
				}

				const float distance_squared = m_owner->GetPosition().Distance_sqr( entity->GetPosition() );
				if ( distance_squared <= m_owner->m_curAttack.maxRange * m_owner->m_curAttack.maxRange )
				{
					m_towers.PushBack(EntityExp(entity, entity->m_experience));
				}				
			}
		}
		break;

	case GMT_I_FINALIZED:
		if ( data )
		{
			Entity* entity = static_cast<Entity*>(data);
			if ( entity->m_partyCurrent == PARTY_TOWER )
			{
				m_towers.Remove(EntityExp(entity, entity->m_experience));
			}
		}
		break;

	case GMT_I_UPGRADED:
		{
			static sx::core::ArrayPNode_inline nodes(64);
			nodes.Clear();
			sx::core::Scene::GetNodesByArea( m_owner->GetPosition(), m_owner->m_curAttack.maxRange, nodes, NMT_ALL, PARTY_TOWER );

			for ( int i = 0; i < nodes.Count(); ++i )
			{
				sx::core::PNode node = nodes[i];
				Entity* entity = static_cast<Entity*>(node->GetUserData());

				if ( (!entity) || (entity->m_health.icur < 1) )
				{
					continue;
				}

				const float distance_squared = m_owner->GetPosition().Distance_sqr( entity->GetPosition() );
				if ( distance_squared > m_owner->m_curAttack.maxRange * m_owner->m_curAttack.maxRange )
				{
					continue;
				}

				bool can_not_add = false;

				for ( int j = 0; j < entity->m_components.Count(); ++j )
				{
					if ( entity->m_components[j]->m_tag == m_tag )
					{
						can_not_add = true;
						break;
					}
				}

				if ( can_not_add )
				{
					continue;
				}

				for ( int i = 0; i < m_towers.Count(); ++i )
				{
					if ( m_towers[i].entity == entity )
					{
						can_not_add = true;
						break;
					}
				}

				if ( can_not_add )
				{
					continue;
				}

				m_towers.PushBack(EntityExp(entity, entity->m_experience));
			}
		}
		break;
	}
}

Component* com_Supporter::Clone( void )
{
	sx_callstack();

	com_Supporter* me = sx_new( com_Supporter );

	me->m_name = m_name;
	me->m_tag  = m_tag;

	return me;
}


com_Supporter::EntityExp::EntityExp()
	: entity(null), experience(0.0f)
{

}

com_Supporter::EntityExp::EntityExp(Entity* entity, float experience)
	: entity(entity), experience(experience)
{

}

bool com_Supporter::EntityExp::operator==(const EntityExp& other) const
{
	return this->entity == other.entity;
}

