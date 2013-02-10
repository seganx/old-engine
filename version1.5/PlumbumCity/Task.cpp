#include "Task.h"

Task::Task( void ): m_owner(NULL), m_Status(TS_INACTIVE), m_Type(0)
{

}

Task::~Task( void )
{
	Finalize();
}

UINT Task::GetStatus( void )
{
	return m_Status;
}

void Task::SetOwner( Entity* owner )
{
	m_owner = owner;
}

FORCEINLINE Entity* Task::GetOwner( void )
{
	return m_owner;
}

FORCEINLINE void Task::Initialize( void )
{
	m_Status = TS_ACTIVE;
}

FORCEINLINE void Task::Finalize( void )
{

}

void Task::Update( float elpstime, DWORD& status )
{
	if ( m_Status == TS_INACTIVE )
		Initialize();

	//  at first clean completed/failed sub states
	while ( !m_subStates.IsEmpty() && (m_subStates.Top()->m_Status==TS_COMPLETED || m_subStates.Top()->m_Status==TS_FAILED) )
	{
		sx_delete_and_null( m_subStates.Top() );
		m_subStates.Pop();
	}

	if ( !m_subStates.IsEmpty() )
	{ 
		m_subStates.Top()->Update(elpstime, status);

		//  we want to force parent to keep processing sub states
		if ( status == TS_COMPLETED && m_subStates.Count() > 1 )
			status = TS_ACTIVE;

		return;
	}
}

void Task::MsgProc( UINT msg, void* data )
{
	if ( !m_subStates.IsEmpty() )
		m_subStates.Top()->MsgProc(msg, data);
}

void Task::AddSubState( Task* state )
{
	sx_assert(state && "Task::AddSubState try to push NULL");
	m_subStates.Push(state);
}

void Task::CleanSubStates( void )
{
	while ( !m_subStates.IsEmpty() )
	{
		sx_delete_and_null( m_subStates.Top() );
		m_subStates.Pop();
	}
}

UINT Task::GetType( Task* task )
{
	if (task)
		return task->m_Type;
	else
		return 0;
}