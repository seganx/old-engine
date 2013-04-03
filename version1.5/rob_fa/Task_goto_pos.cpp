#include "Task_goto_pos.h"
#include "Entity.h"
#include "PathFinder.h"

Task_goto_pos::Task_goto_pos( void )
: Task()
, m_stunValue(0)
, m_stunTime(0)
, m_posGoal(0,0,0)
{
	sx_callstack();

	m_Type = GTT_GOTO_POSITION;
}

Task_goto_pos::~Task_goto_pos( void )
{
	sx_callstack();
}

void Task_goto_pos::Initialize( void )
{
	sx_callstack();
	Task::Initialize();
}

void Task_goto_pos::Finalize( void )
{
	sx_callstack();
}

void Task_goto_pos::Update( float elpstime, DWORD& status )
{
	if ( m_Status == TS_FAILED || m_owner->m_health.icur<1 ) return;
	
	sx_callstack();

	if ( !m_owner->m_move.moveSpeed || m_posGoal.Distance_sqr( m_owner->GetPosition() ) < 1.0f )
	{
		status = TS_COMPLETED;
		m_Status = TS_COMPLETED;
		return;
	}

	//  we should have some checkups to verify that moving has no problem
	if ( false )
	{
		status = TS_FAILED;		//  rapid reply
		m_Status = TS_FAILED;	//  this will removed in next loop from sub states
		return;
	}

	//  just move entity to target position
	float3 curPos		= m_owner->GetPosition();
	float3 curDir		= m_owner->GetDirection();
	float moveSpeed		= ( m_owner->m_move.moveSpeed / 100.0f );

	//  check the stun value
	if ( m_stunTime > 0 )
	{
		 moveSpeed /= ( m_stunValue + 1 );
		 m_stunTime -= elpstime*0.001f;
	}

	float3 movedir = m_posGoal - curPos;
	movedir.Normalize( movedir );

	curDir += (movedir - curDir) * elpstime * 0.003f;
	curPos += movedir * elpstime * 0.1f * moveSpeed;

	m_owner->SetPosition( curPos );
	m_owner->SetDirection( curDir );

	m_owner->SetState(ES_WALK);

}

FORCEINLINE void Task_goto_pos::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(Task_goto_pos::MsgProc(msg=%d), msg);

	switch (msg)
	{
	case GMT_GOTO:
		if( data )
		{
			Mission* mission = (Mission*)data;
			m_posGoal = mission->pos;
		}
		break;
		
	case GMT_DAMAGE:
		if ( data )
		{
			msgDamage* damage = (msgDamage*)data;
			if ( damage->stunValue > m_stunValue )
				m_stunValue = damage->stunValue;

			if ( damage->stunTime > m_stunTime )
				m_stunTime = damage->stunTime;

			if ( m_stunValue < 0 ) m_stunValue = 0;
			if ( m_stunTime < 0 ) m_stunTime = 0;
		}
		break;
	}
}
