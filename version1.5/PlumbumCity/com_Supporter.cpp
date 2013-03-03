#include "com_Supporter.h"
#include "Entity.h"
#include "Game.h"
#include "Player.h"

com_Supporter::com_Supporter( void ): Component()
	,	m_node(0)
	,	m_time(0)
	,	m_energy(0)
	,	m_repair(0)
	,	m_towers(512)
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

	sx::core::ArrayPNode_inline	nodes(512);
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
			m_towers.PushBack(entity);
		}
	}

	m_time		= 0.0f;
	m_energy	= 0.0f;
	m_repair	= 0.0f;
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

	m_owner->m_experience += m_owner->test_onDamageXP * delta;

	const bool not_over_active = true;
	
	m_energy += m_owner->m_curAttack.stunValue * delta * (not_over_active ? 1.0f : m_owner->m_curAttack.stunTime);
	m_repair += m_owner->m_curAttack.physicalDamage * delta;

	m_time += elpsTime;

	if ( m_time > 250.0f )
	{
		m_time = 0.0f;
		
		const int addEnergy = static_cast<int>(m_energy);
		m_energy -= addEnergy;
		g_game->m_player->m_energy += addEnergy;

		for ( int i = 0; i < m_towers.Count(); ++i )
		{
			if ( m_towers[i] && m_towers[i]->m_health.icur > 0 )
			{
				const int addHealth = static_cast<int>(m_repair);
				m_repair -= addHealth;
				m_towers[i]->m_health.icur +=
					m_towers[i]->m_health.icur + addHealth > m_towers[i]->m_health.imax ? 0 : addHealth;
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
					const float distance = m_owner->GetDistance_edge( entity );
					if ( distance <= m_owner->m_curAttack.maxRange )
					{
						m_towers.PushBack(entity);
					}				
				}
			}
		}
	case GMT_I_FINALIZED:
		if ( data )
		{
			Entity* entity = static_cast<Entity*>(data);
			if ( entity->m_partyCurrent == PARTY_TOWER )
			{
				m_towers.Remove(entity);
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
