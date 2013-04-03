#include "Task_attack_weapon.h"
#include "Entity.h"

#define ATTACK_MAX_SEARCH_TIME	500.0f		//  every 0.5 second
#define ATTACK_RND_SEARCH_TIME	100.0f		//  randomization search time
#define ATTACK_MAX_FIRE_TIME	50.0f		//  maximum fire time

//////////////////////////////////////////////////////////////////////////
//	TASK ATTACK WEAPON
//////////////////////////////////////////////////////////////////////////
Task_attack_weapon::Task_attack_weapon( void ): Task(), m_Target(NULL), m_targetIndex(0), m_searchTime(0), m_fireTime(0)
{
	sx_callstack();

	m_Type = GTT_ATTACK_WEAPON;
}

Task_attack_weapon::~Task_attack_weapon( void )
{
	sx_callstack();

}

void Task_attack_weapon::Initialize( void )
{
	sx_callstack();

	Task::Initialize();

	m_searchTime = sx::cmn::Random(ATTACK_MAX_SEARCH_TIME);
	m_fireTime = 0;
	m_arrTargets.Clear();
	m_Target = NULL;
	m_targetIndex = 0;
}

void Task_attack_weapon::Finalize( void )
{
	sx_callstack();

	m_searchTime = 0;
	m_fireTime = 0;
	m_arrTargets.Clear();
	m_Target = NULL;
	m_targetIndex = 0;

	Task::Finalize();
}

void Task_attack_weapon::Update( float elpstime, DWORD& status )
{
	if ( m_Status == TS_FAILED || m_owner->m_health.icur < 1 ) return;

	sx_callstack();


	m_searchTime += elpstime;
	if ( m_searchTime > ATTACK_MAX_SEARCH_TIME )
	{
		SearchForEnemy();
		m_searchTime = sx::cmn::Random(ATTACK_RND_SEARCH_TIME);
	}

	if ( m_arrTargets.Count() )
	{
		prpAttack* pAttack = &( m_owner->m_curAttack );

#if 1

		//	validate target
		bool targetDead = m_Target ? ( !m_Target->m_mesh || m_Target->m_health.icur < 1 ) : true;
		bool badMoveTypr = m_Target ? m_Target->m_move.type != GMT_BOTH && pAttack->targetType != GMT_BOTH && pAttack->targetType != m_Target->m_move.type : true;
		float dis = m_Target ? m_owner->m_node->GetPosition_world().Distance( m_Target->m_node->GetPosition_world() ) : FLT_MAX;
		if ( badMoveTypr || dis > pAttack->maxRange )
		{
			m_arrTargets.Remove( m_Target );

			dis = FLT_MAX;
			m_Target = NULL;
			for (int i=0; i<m_arrTargets.Count(); i++)
			{
				float d = m_owner->m_node->GetPosition_world().Distance( m_arrTargets[i]->m_node->GetPosition_world() );
				if ( dis > d )
				{
					dis = d;
					m_Target = m_arrTargets[i];
				}
			}
		}

		if ( m_Target )
		{
			m_fireTime += elpstime;
			if ( m_fireTime > ATTACK_MAX_FIRE_TIME )
			{
				m_fireTime = sx::cmn::Random( ATTACK_MAX_FIRE_TIME );

				msgFire mf( m_Target->GetBoundingSphere().center, m_Target );
				m_owner->MsgProc( GMT_FIRE, &mf );
			}
		}

#else

		//  verify that owner is an stunner or attacker
		float stun = pAttack->stunValue * pAttack->stunTime;
		float damg = pAttack->physicalDamage + pAttack->electricalDamage + pAttack->splashRadius;

		if ( stun > damg && false )	//  owner is an stunner
		{
			if ( m_targetIndex >= m_arrTargets.Count() )
				m_targetIndex = 0;

			m_Target = m_arrTargets[ m_targetIndex ];
			if ( m_Target->m_mesh && m_Target->m_health.icur > 0 )
			{
				msgFire mf( m_Target->GetBoundingSphere().center, m_Target );
				m_owner->MsgProc( GMT_FIRE, &mf );

				if ( mf.fired )
					m_targetIndex++;
			}
			else
			{
				m_arrTargets.Remove( m_Target );
				m_targetIndex++;
			}
		}
		else	//  owner is an attacker
		{
			float dis = m_Target ? m_owner->m_node->GetPosition_world().Distance( m_Target->m_node->GetPosition_world() ) : FLT_MAX;
			if ( dis > pAttack->maxRange || dis < pAttack->minRange )
			{
				m_arrTargets.Remove( m_Target );

				dis = FLT_MAX;
				m_Target = NULL;
				for (int i=0; i<m_arrTargets.Count(); i++)
				{
					float d = m_owner->m_node->GetPosition_world().Distance( m_arrTargets[i]->m_node->GetPosition_world() );
					if ( dis > d )
					{
						dis = d;
						m_Target = m_arrTargets[i];
					}
				}
			}

			if ( m_Target )
			{
				if ( m_Target->m_mesh && m_Target->m_health.icur > 0 )
				{
					msgFire mf( m_Target->GetBoundingSphere().center, m_Target );
					m_owner->MsgProc( GMT_FIRE, &mf );
				}
				else
				{
					m_arrTargets.Remove( m_Target );
					m_searchTime = ATTACK_MAX_SEARCH_TIME;	//  ready to find new target
				}
			}
		}
#endif

	}
	else m_targetIndex = 0;
}

FORCEINLINE void Task_attack_weapon::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(Task_attack_weapon::MsgProc(msg=%d), msg);

	switch (msg)
	{
	case GMT_I_DIED:
		{
			Entity* en = static_cast<Entity*>(data);

			if ( en == m_owner )
			{
				//  do somethings
			}
			else
			{
				m_arrTargets.Remove( en );

				if ( en == m_Target )
					m_Target = NULL;
			}
		}
		break;

	case GMT_LEAVE_ME:
		if ( data )
		{
			Entity* en = static_cast<Entity*>(data);

			if ( en == m_owner )
			{
				//  do somethings
			}
			else
			{
				m_arrTargets.Remove( en );

				if ( en == m_Target )
					m_Target = NULL;
			}
		}
		break;
	}
}

void Task_attack_weapon::SearchForEnemy( void )
{
	sx_callstack();

	prpAttack* pAttack = &( m_owner->m_curAttack );

	//  collect enemies in the scene
	static sx::core::ArrayPNode_inline nodeList(128);	nodeList.Clear();
	sx::core::Scene::GetNodesByArea( m_owner->GetPosition(), pAttack->maxRange, nodeList, NMT_ALL, m_owner->m_partyEnemies );

	//  if number of entities changed
	if ( m_arrTargets.Count() == nodeList.Count() ) return;

	//  fill out target list
	m_arrTargets.Clear();
	for (int i=0; i<nodeList.Count(); i++)
	{
		sx::core::PNode node = nodeList[i];
		if ( !node->GetUserData() ) continue;

		Entity* pe = static_cast<Entity*>( node->GetUserData() );
		if ( pe->m_health.icur < 1 ) continue;
		if ( pe->m_move.type != GMT_BOTH && pAttack->targetType != GMT_BOTH && pAttack->targetType != pe->m_move.type ) continue;

		float dis = m_owner->GetDistance_edge( pe );
		if ( dis < pAttack->maxRange )
		{
			if ( pe->m_move.type == GMT_GROUND || pAttack->targetType == GMT_GROUND )
			{
				if ( dis > pAttack->minRange )
				{
					m_arrTargets.PushBack( pe );
				}
			}
			else m_arrTargets.PushBack( pe );
		}
	}
}

