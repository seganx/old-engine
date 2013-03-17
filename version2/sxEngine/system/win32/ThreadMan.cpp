#if defined(_WIN32)

#include "../ThreadMan.h"
#include "../OS.h"
#include "../Log.h"
#include "Win6.h"


#define SX_TASKBASE_AUTO_FREE	0x00000100
#define SX_TASKBASE_EXECUTING	0x00000200



DWORD WINAPI threadProc1( __in  LPVOID lpParameter );
DWORD WINAPI threadProc2( __in  LPVOID lpParameter );
DWORD WINAPI threadProc3( __in  LPVOID lpParameter );
DWORD WINAPI threadProc4( __in  LPVOID lpParameter );


/*
here is the main task manager class that will initialized automatically.
the maximum number of threads is 4.
*/
class ThreadMan_internal
{
public:
	void Initialize( void )
	{
		//  fill defined variables
		for ( int i=0; i<5; i++ )
		{
			m_threadID[i] = 0;
			m_thread[i] = null;
			m_event[i] = null;
			m_runing[i] = false,
			m_taskList[i].SetSize( 32 );
		}

		//  get system info to know there are how many processor core exist in this set
		CPUInfo cpuinfo;
		sx_os_get_cpu_info( cpuinfo );
		m_numThread = cpuinfo.processors + 1;
		sx_clamp_i( m_numThread, 2, 5 );

		//  create events to suspend threads when the list is empty
		for ( int i=1; i<m_numThread; i++ )
			m_event[i] = CreateEvent( NULL, true, false, NULL );

		//  now lets start the threads
		if ( m_numThread > 1 ) m_thread[1] = CreateThread( NULL, 0, &threadProc1, this, 0, &m_threadID[1] );
		if ( m_numThread > 2 ) m_thread[2] = CreateThread( NULL, 0, &threadProc2, this, 0, &m_threadID[2] );
		if ( m_numThread > 3 ) m_thread[3] = CreateThread( NULL, 0, &threadProc3, this, 0, &m_threadID[3] );
		if ( m_numThread > 4 ) m_thread[4] = CreateThread( NULL, 0, &threadProc4, this, 0, &m_threadID[4] );

		m_taskIndex = 0;
	}

	void Finalize( void )
	{
		//  close handles of the events and threads
		for ( int i=1; i<m_numThread; i++ )
		{
			if ( m_event[i] )
			{
				CloseHandle( m_event[i] );
				m_event[i] = NULL;
			}

			if ( m_thread[i] )	
			{
				CloseHandle( m_thread[i] );
				m_thread[i]= NULL;
			}
		}
	}

	void Add( ThreadTask* newTask, int destThread )
	{
		if ( !newTask ) return;

		if ( destThread == SEGAN_TASKMAN_AUTO )
		{
			//  select a task automatically
			m_taskIndex++;
			if ( m_taskIndex >= m_numThread ) m_taskIndex = 1;
			sx_1th_byte_of( newTask->m_flag ) = (byte)m_taskIndex;

			sx_os_enter_critical_section();
			m_taskList[m_taskIndex].PushBack( newTask );
			sx_os_leave_critical_section();

			SetEvent( m_event[m_taskIndex] );
		}
		else
		{
			if ( destThread >= m_numThread )
				destThread = m_numThread - 1;

			sx_1th_byte_of( newTask->m_flag ) = (byte)destThread;

			//  push task to the specified thread
			sx_os_enter_critical_section();
			m_taskList[destThread].PushBack( newTask );
			sx_os_leave_critical_section();

			if ( destThread )
				SetEvent( m_event[destThread] );
		}
	}

	void Remove( ThreadTask* pTask )
	{
		if ( !pTask ) return;

		int index = sx_1th_byte_of( pTask->m_flag );

		sx_os_enter_critical_section();
		while( m_taskList[index].Remove( pTask ) );
		sx_os_leave_critical_section();
	}

	int						m_taskIndex;
	int						m_numThread;
	DWORD					m_threadID[5];
	HANDLE					m_thread[5];
	HANDLE					m_event[5];
	bool					m_runing[5];
	Array<ThreadTask*>		m_taskList[5];	//  4 + 1 for main thread

};
ThreadMan_internal*	s_threadMan = null;



//////////////////////////////////////////////////////////////////////////
//		thread manager
//////////////////////////////////////////////////////////////////////////
void sx_thread_initilize( void )
{
	if ( s_threadMan )
	{
		g_logger->Log( L"Warning: calling sx_thread_initialize() failed. Thread Manager has been initialized !" );
		return;
	}
	s_threadMan = sx_new( ThreadMan_internal );
	s_threadMan->Initialize();
}

void sx_thread_finalize( void )
{
	if ( !s_threadMan )
	{
		g_logger->Log( L"Warning! calling sx_thread_finalize() failed. Thread Manager is not initialized !" );
		return;
	}
	s_threadMan->Finalize();
	sx_delete_and_null( s_threadMan );
}

void sx_thread_add_task( ThreadTask* ptask, uint destThread /*= SEGAN_TASKMAN_AUTO */ )
{
	s_threadMan->Add( ptask, destThread );
}

void sx_thread_remove_task( ThreadTask* ptask )
{
	s_threadMan->Remove( ptask );
}

bool sx_thread_isempty( void )
{
	return	s_threadMan->m_taskList[0].IsEmpty() &&
			s_threadMan->m_taskList[1].IsEmpty() &&
			s_threadMan->m_taskList[2].IsEmpty() &&
			s_threadMan->m_taskList[3].IsEmpty() &&
			s_threadMan->m_taskList[4].IsEmpty() &&

			!s_threadMan->m_runing[0] &&
			!s_threadMan->m_runing[1] &&
			!s_threadMan->m_runing[2] &&
			!s_threadMan->m_runing[3] &&
			!s_threadMan->m_runing[4];
}

void sx_thread_clear( void )
{
	sx_os_enter_critical_section();

	s_threadMan->m_taskList[1].Clear();
	s_threadMan->m_taskList[2].Clear();
	s_threadMan->m_taskList[3].Clear();
	s_threadMan->m_taskList[4].Clear();

	sx_os_leave_critical_section();

	while ( !sx_thread_isempty() )
		sx_thread_update( 10 );
}

int sx_thread_count( void )
{
	return	
		s_threadMan->m_taskList[0].Count() +
		s_threadMan->m_taskList[1].Count() +
		s_threadMan->m_taskList[2].Count() +
		s_threadMan->m_taskList[3].Count() +
		s_threadMan->m_taskList[4].Count();
}

void sx_thread_update( float elpsTime )
{
	uint n = s_threadMan->m_taskList[0].Count();

	while ( n )
	{
		n--;

		if ( s_threadMan->m_taskList[0].IsEmpty() ) return;

		s_threadMan->m_runing[0] = true;

		//  pop a task from list
		sx_os_enter_critical_section();
		ThreadTask* task = s_threadMan->m_taskList[0].At( n );
		task->m_flag |= SX_TASKBASE_EXECUTING;
		s_threadMan->m_taskList[0].RemoveByIndex( n );
		sx_os_leave_critical_section();

		//	execute task
		task->Execute();

		//	free task
		if ( task->m_flag & SX_TASKBASE_AUTO_FREE ) {
			sx_delete( task );
		} else {
			sx_set_rem( task->m_flag, SX_TASKBASE_EXECUTING );
		}

		s_threadMan->m_runing[0] = false;
	}
}


//////////////////////////////////////////////////////////////////////////
//	thread task
//////////////////////////////////////////////////////////////////////////
ThreadTask::ThreadTask( void ): m_flag(0)
{

}

ThreadTask::~ThreadTask()
{

}

void ThreadTask::FreeOnTerminate( bool enable )
{
	sx_os_enter_critical_section();

	if ( enable )
		sx_set_add( m_flag, SX_TASKBASE_AUTO_FREE );
	else
		sx_set_rem( m_flag, SX_TASKBASE_AUTO_FREE );

	sx_os_leave_critical_section();
}

bool ThreadTask::Executing( void )
{
	return ( m_flag & SX_TASKBASE_EXECUTING ) != 0;
}



//////////////////////////////////////////////////////////////////////////
//	thread procedure
//////////////////////////////////////////////////////////////////////////

// define 4 thread procedure indicates that the task manager will have 4 thread in maximum uses
DWORD WINAPI threadProc1( __in  LPVOID lpParameter )
{
	ThreadMan_internal* threadMan = (ThreadMan_internal*)lpParameter;

	while( WaitForSingleObject( threadMan->m_event[1], INFINITE ) == WAIT_OBJECT_0 )
	{
		if ( threadMan->m_taskList[1].IsEmpty() )
		{
			ResetEvent( threadMan->m_event[1] );
			continue;
		}

		threadMan->m_runing[1] = true;

		//	pop a task from the list
		sx_os_enter_critical_section();
		sint h = s_threadMan->m_taskList[1].Count() - 1;
		ThreadTask* task = s_threadMan->m_taskList[1].At( h );
		task->m_flag |= SX_TASKBASE_EXECUTING;
		s_threadMan->m_taskList[1].RemoveByIndex( h );
		sx_os_leave_critical_section();

		//  execute the task
		task->Execute();

		//  remove task from the list
		sx_os_enter_critical_section();
		if ( task->m_flag & SX_TASKBASE_AUTO_FREE ) {
			sx_delete(task);
		} else {
			sx_set_rem( task->m_flag, SX_TASKBASE_EXECUTING );
		}
		sx_os_leave_critical_section();

		threadMan->m_runing[1] = false;
	}

	return 0;
}

DWORD WINAPI threadProc2( __in  LPVOID lpParameter )
{
	ThreadMan_internal* threadMan = (ThreadMan_internal*)lpParameter;

	while( WaitForSingleObject( threadMan->m_event[2], INFINITE ) == WAIT_OBJECT_0 )
	{
		if ( threadMan->m_taskList[2].IsEmpty() )
		{
			ResetEvent( threadMan->m_event[2] );
			continue;
		}

		threadMan->m_runing[2] = true;

		//	pop a task from the list
		sx_os_enter_critical_section();
		sint h = s_threadMan->m_taskList[2].Count() - 1;
		ThreadTask* task = s_threadMan->m_taskList[2].At( h );
		task->m_flag |= SX_TASKBASE_EXECUTING;
		s_threadMan->m_taskList[2].RemoveByIndex( h );
		sx_os_leave_critical_section();

		//  execute the task
		task->Execute();

		//  remove task from the list
		sx_os_enter_critical_section();
		if ( task->m_flag & SX_TASKBASE_AUTO_FREE ) {
			sx_delete( task );
		} else {
			sx_set_rem( task->m_flag, SX_TASKBASE_EXECUTING );
		}
		sx_os_leave_critical_section();

		threadMan->m_runing[2] = false;
	}

	return 0;
}

DWORD WINAPI threadProc3( __in  LPVOID lpParameter )
{
	ThreadMan_internal* threadMan = (ThreadMan_internal*)lpParameter;

	while( WaitForSingleObject( threadMan->m_event[3], INFINITE ) == WAIT_OBJECT_0 )
	{
		if ( threadMan->m_taskList[3].IsEmpty() )
		{
			ResetEvent( threadMan->m_event[3] );
			continue;
		}

		threadMan->m_runing[3] = true;

		//	pop a task from the list
		sx_os_enter_critical_section();
		sint h = s_threadMan->m_taskList[3].Count() - 1;
		ThreadTask* task = s_threadMan->m_taskList[3].At( h );
		task->m_flag |= SX_TASKBASE_EXECUTING;
		s_threadMan->m_taskList[3].RemoveByIndex( h );
		sx_os_leave_critical_section();

		//  execute the task
		task->Execute();

		//  remove task from the list
		sx_os_enter_critical_section();
		if ( task->m_flag & SX_TASKBASE_AUTO_FREE ) {
			sx_delete( task );
		} else {
			sx_set_rem( task->m_flag, SX_TASKBASE_EXECUTING );
		}
		sx_os_leave_critical_section();

		threadMan->m_runing[3] = false;
	}

	return 0;
}

DWORD WINAPI threadProc4( __in  LPVOID lpParameter )
{
	ThreadMan_internal* threadMan = (ThreadMan_internal*)lpParameter;

	while( WaitForSingleObject( threadMan->m_event[4], INFINITE ) == WAIT_OBJECT_0 )
	{
		if ( threadMan->m_taskList[4].IsEmpty() )
		{
			ResetEvent( threadMan->m_event[4] );
			continue;
		}

		threadMan->m_runing[4] = true;

		//	pop a task from the list
		sx_os_enter_critical_section();
		sint h = s_threadMan->m_taskList[4].Count() - 1;
		ThreadTask* task = s_threadMan->m_taskList[4].At( h );
		task->m_flag |= SX_TASKBASE_EXECUTING;
		s_threadMan->m_taskList[4].RemoveByIndex( h );
		sx_os_leave_critical_section();

		//  execute the task
		task->Execute();

		//  remove task from the list
		sx_os_enter_critical_section();
		if ( task->m_flag & SX_TASKBASE_AUTO_FREE ) {
			sx_delete( task );
		} else {
			sx_set_rem( task->m_flag, SX_TASKBASE_EXECUTING );
		}
		sx_os_leave_critical_section();

		threadMan->m_runing[4] = false;
	}

	return 0;
}

#endif
