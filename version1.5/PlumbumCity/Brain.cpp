#include "Brain.h"
#include "Entity.h"
#include "Task.h"
#include "Task_goto_node.h"
#include "Task_goto_pos.h"
#include "Task_attack_weapon.h"
#include "Game.h"
#include "GameGUI.h"

Brain::Brain( void )
: m_Owner(NULL)
, m_missionIndex(0)
, m_missionTime(0)
, m_taskCurrent(0)
, m_taskParallel(0)
{

}

Brain::~Brain( void )
{
	sx_delete_and_null( m_taskCurrent );
	sx_delete_and_null( m_taskParallel );
}

void Brain::Initialize( void )
{

}

void Brain::Finalize( void )
{
	sx_callstack_push(Brain::Finalize());

	SetCurrentTask( NULL );
	SetParallelTask( NULL );
}

void Brain::SetOwner( Entity* owner )
{
	m_Owner = owner;
}

void Brain::AddMission( Mission& m )
{
	m_Missions.PushBack( m );
}

void Brain::ClearMissions( void )
{
	SetCurrentTask( NULL );
	SetParallelTask( NULL );

	m_Missions.Clear();
	m_missionIndex = 0;
}

void Brain::Update( float elpsTime )
{
	if ( m_Missions.IsEmpty() || m_Owner->m_health.icur < 1 ) return;

	//  check mission status
	switch ( m_Missions[m_missionIndex].status )
	{
	case Mission::MS_INACTIVE:
		break;

	case Mission::MS_ACTIVE:	
		break;

	case Mission::MS_COMPLETE:
		{
			SetCurrentMission(m_missionIndex + 1);
		}
		break;

	case Mission::MS_FAILED:
		break;
	}

	//  process current mission
	ProcessMission( elpsTime, m_Missions[m_missionIndex] );
}

void Brain::MsgProc( UINT msg, void* data )
{
	sx_callstack_push(Brain::MsgProc(msg=%d), msg);

	if ( m_taskParallel )	m_taskParallel->MsgProc( msg, data );
	if ( m_taskCurrent )	m_taskCurrent->MsgProc( msg, data );
}

FORCEINLINE void Brain::ProcessMission( float elpsTime, Mission& m )
{
	// check mission status
	if ( m.status == Mission::MS_INACTIVE ) return;
	m_missionTime += elpsTime;


	//////////////////////////////////////////////////////////////////////////
	//	BEGIN THINK
	//////////////////////////////////////////////////////////////////////////

	if ( m.flag == MISSION_SUICIDE  )
	{
		g_game->m_gui->m_goldPeople->Alarm();
		if ( g_game->m_upgrades.general_people_resistance > 0.05f )
		{
			float healthFactor = (float)m_Owner->m_health.icur / (float)m_Owner->m_health.imax;
			if ( healthFactor <= g_game->m_upgrades.general_people_resistance )
			{
				m_Owner->m_curAttack.killPeople = 0;
			}
		}
		m_Owner->m_health.imax = 0;
		m_Owner->m_health.icur = 0;
		return;
	}

	//  set goto node task
	if ( m.flag & MISSION_GOTO_NODE )
	{
		if ( Task::GetType( m_taskCurrent) != GTT_GOTO_NODE )
		{
			Task_goto_node* task = sx_new( Task_goto_node );
			task->MsgProc( GMT_GOTO, &m );
			SetCurrentTask(task);
		}
	}	//  set goto position task
	else if ( m.flag & MISSION_GOTO_POSITION )
	{
		if ( Task::GetType( m_taskCurrent) != GTT_GOTO_POSITION )
		{
			Task_goto_pos* task = sx_new( Task_goto_pos );
			task->MsgProc( GMT_GOTO, &m );
			SetCurrentTask(task);
		}
	}


	//  set attack task
	if ( m.flag & MISSION_KILL_ENEMY )
	{
		if ( Task::GetType( m_taskParallel ) != GTT_ATTACK_WEAPON )
		{
			SetParallelTask( sx_new( Task_attack_weapon ) );
		}
	}

	
	//////////////////////////////////////////////////////////////////////////
	//	END THINK
	//////////////////////////////////////////////////////////////////////////

	//  check the current task status
	if ( m_taskCurrent )
	{
		//if ( Task::GetType( m_taskCurrent ) == GTT_GOTO_NODE )
		//{
			switch ( m_taskCurrent->GetStatus() )
			{
			case Task::TS_COMPLETED:	m.status = Mission::MS_COMPLETE;	break;
			case Task::TS_FAILED:		m.status = Mission::MS_FAILED;		break;
			}
		//}
	}

	//  check mission time
	if ( m.time>0.001f && m_missionTime > m.time )
	{
		m.status = Mission::MS_COMPLETE;
		return;
	}

	//  update tasks
	if ( m_taskCurrent )
	{
		DWORD status = Task::TS_ACTIVE;
		m_taskCurrent->Update(elpsTime, status);
	}
	if ( m_taskParallel )
	{
		DWORD status = Task::TS_ACTIVE;
		m_taskParallel->Update(elpsTime, status);
	}
}

void Brain::SetCurrentMission( UINT index )
{
	UINT n = m_Missions.Count();
	m_missionIndex = index < n ? index : 0;
	if ( n ) m_Missions[m_missionIndex].status = Mission::MS_ACTIVE;
	m_missionTime = 0;

	SetCurrentTask(NULL);
	SetParallelTask(NULL);
}

void Brain::SetCurrentTask( Task* curTask )
{
	sx_callstack_push(SetCurrentTask());

	if ( m_taskCurrent )
		m_taskCurrent->Finalize();

	sx_delete_and_null( m_taskCurrent );

	m_taskCurrent = curTask;

	if (m_taskCurrent)
	{
		m_taskCurrent->SetOwner(m_Owner);
		m_taskCurrent->Initialize();
	}
}

void Brain::SetParallelTask( Task* parTask )
{
	sx_callstack_push(Brain::SetParallelTask());

	if ( m_taskParallel )
		m_taskParallel->Finalize();

	sx_delete_and_null( m_taskParallel );

	m_taskParallel = parTask;

	if (m_taskParallel)
	{
		m_taskParallel->SetOwner(m_Owner);
		m_taskParallel->Initialize();
	}
}


