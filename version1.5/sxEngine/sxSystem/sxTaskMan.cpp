#include "sxTaskMan.h"
#include "sxSys.h"
#include "sxLog.h"

typedef sx::sys::TaskBase*	PTaskBase;


#define SX_TASKBASE_AUTO_FREE	0x00000100
#define SX_TASKBASE_EXECUTING	0x00000200
/*
here is the main task manager class that will initialized automatically.
the maximum number of threads is 4.
*/
class TaskMan_internal
{
public:
	static int Initialize(void){
		//  fill defined variables
		for (int i=0; i<5; dThreadID[i]=0, hThread[i]=NULL, hEvent[i]=NULL, bRuning[i]=false, i++);

		//  get system info to know there are how many processor core exist in this set
		SystemInfo sinfo;
		sx::sys::GetSystemInfo(sinfo);
		numThread = sinfo.CPU.ProcessorCount + 1;
		SEGAN_CLAMP(numThread, 2, 5);

		//  create events to suspend threads when the list is empty
		for (int i=1; i<numThread; i++)
			hEvent[i] = CreateEvent(NULL, true, false, NULL);

		//  now lets start the threads
		if (numThread>1) hThread[1] = CreateThread(NULL, 0, &threadProc1, NULL, 0, &dThreadID[1]);
		if (numThread>2) hThread[2] = CreateThread(NULL, 0, &threadProc2, NULL, 0, &dThreadID[2]);
		if (numThread>3) hThread[3] = CreateThread(NULL, 0, &threadProc3, NULL, 0, &dThreadID[3]);
		if (numThread>4) hThread[4] = CreateThread(NULL, 0, &threadProc4, NULL, 0, &dThreadID[4]);

		return -1;
	}

	static void Finalize(void)
	{
		//  close handles of the events and threads
		for (int i=1; i<numThread; i++)
		{
			if (hEvent[i])	
			{
				CloseHandle(hEvent[i]);
				hEvent[i] = NULL;
			}

			if (hThread[i])	
			{
				CloseHandle(hThread[i]);
				hThread[i]= NULL;
			}
		}		

	}

	static void Add(PTaskBase newTask, int destThread){
		if (!newTask) return;

		if (destThread == SEGAN_TASKMAN_AUTO)
		{
			//  select a task automatically
			taskIndex++;
			if (taskIndex>=numThread) taskIndex = 1;
			SEGAN_1TH_BYTEOF(newTask->m_flag) = (BYTE)taskIndex;

			qCS.Enter();
			taskList[taskIndex].PushBack(newTask);
			qCS.Enter();

			SetEvent(hEvent[taskIndex]);
		}
		else
		{
			if (destThread >= numThread) destThread = numThread - 1;

			SEGAN_1TH_BYTEOF(newTask->m_flag) = (BYTE)destThread;

			//  push task to the specified thread
			qCS.Enter();
			taskList[destThread].PushBack(newTask);
			qCS.Leave();

			if (destThread)	SetEvent(hEvent[destThread]);
		}
	}

	static void Remove(PTaskBase pTask){
		if (!pTask) return;

		int index = SEGAN_1TH_BYTEOF(pTask->m_flag);

		qCS.Enter();
		while( taskList[index].Remove(pTask) );
		qCS.Leave();
	}

	// define 4 thread procedure indicates that the task manager will have 4 thread in maximum uses
	static DWORD WINAPI threadProc1( __in  LPVOID lpParameter )
	{
		const int id = 1;
		while( WaitForSingleObject( hEvent[id], INFINITE ) == WAIT_OBJECT_0 )
		{
			if ( taskList[id].IsEmpty() )
			{
				ResetEvent( hEvent[id] );
				continue;
			}
			bRuning[id] = true;

			qCS.Enter();
			sx::sys::TaskBase* task = taskList[id].At( 0 );
			if (task)
			{
				SEGAN_SET_ADD(task->m_flag, SX_TASKBASE_EXECUTING);
			}
			taskList[id].RemoveByIndex( 0 );
			qCS.Leave();

			if (task && task->m_flag < 70000)
			{
				task->Execute();

				qCS.Enter();
				if (SEGAN_SET_HAS(task->m_flag, SX_TASKBASE_AUTO_FREE))
				{
					sx_delete_and_null(task);
				}
				else
				{
					SEGAN_SET_REM(task->m_flag, SX_TASKBASE_EXECUTING);
				}
				qCS.Leave();
			}

			bRuning[id] = false;
		}
		return 0;
	}

	static DWORD WINAPI threadProc2( __in  LPVOID lpParameter )
	{
		const int id = 2;
		while( WaitForSingleObject( hEvent[id], INFINITE ) == WAIT_OBJECT_0 )
		{
			if ( taskList[id].IsEmpty() )
			{
				ResetEvent( hEvent[id] );
				continue;
			}
			bRuning[id] = true;

			qCS.Enter();
			sx::sys::TaskBase* task = taskList[id].At( 0 );
			if (task)
			{
				SEGAN_SET_ADD(task->m_flag, SX_TASKBASE_EXECUTING);
			}
			taskList[id].RemoveByIndex( 0 );
			qCS.Leave();

			if (task && task->m_flag < 70000)
			{
				task->Execute();

				qCS.Enter();
				if (SEGAN_SET_HAS(task->m_flag, SX_TASKBASE_AUTO_FREE))
				{
					sx_delete_and_null(task);
				}
				else
				{
					SEGAN_SET_REM(task->m_flag, SX_TASKBASE_EXECUTING);
				}
				qCS.Leave();
			}

			bRuning[id] = false;
		}
		return 0;
	}

	static DWORD WINAPI threadProc3( __in  LPVOID lpParameter )
	{
		const int id = 3;
		while( WaitForSingleObject( hEvent[id], INFINITE ) == WAIT_OBJECT_0 )
		{
			if ( taskList[id].IsEmpty() )
			{
				ResetEvent( hEvent[id] );
				continue;
			}
			bRuning[id] = true;

			qCS.Enter();
			sx::sys::TaskBase* task = taskList[id].At( 0 );
			if (task)
			{
				SEGAN_SET_ADD(task->m_flag, SX_TASKBASE_EXECUTING);
			}
			taskList[id].RemoveByIndex( 0 );
			qCS.Leave();

			if (task && task->m_flag < 70000)
			{
				task->Execute();

				qCS.Enter();
				if (SEGAN_SET_HAS(task->m_flag, SX_TASKBASE_AUTO_FREE))
				{
					sx_delete_and_null(task);
				}
				else
				{
					SEGAN_SET_REM(task->m_flag, SX_TASKBASE_EXECUTING);
				}
				qCS.Leave();
			}

			bRuning[id] = false;
		}
		return 0;
	}

	static DWORD WINAPI threadProc4( __in  LPVOID lpParameter )
	{
		const int id = 4;
		while( WaitForSingleObject( hEvent[id], INFINITE ) == WAIT_OBJECT_0 )
		{
			if ( taskList[id].IsEmpty() )
			{
				ResetEvent( hEvent[id] );
				continue;
			}
			bRuning[id] = true;

			qCS.Enter();
			sx::sys::TaskBase* task = taskList[id].At( 0 );
			if (task)
			{
				SEGAN_SET_ADD(task->m_flag, SX_TASKBASE_EXECUTING);
			}
			taskList[id].RemoveByIndex( 0 );
			qCS.Leave();

			if (task && task->m_flag < 70000)
			{
				task->Execute();

				qCS.Enter();
				if (SEGAN_SET_HAS(task->m_flag, SX_TASKBASE_AUTO_FREE))
				{
					sx_delete_and_null(task);
				}
				else
				{
					SEGAN_SET_REM(task->m_flag, SX_TASKBASE_EXECUTING);
				}
				qCS.Leave();
			}

			bRuning[id] = false;
		}
		return 0;
	}

	static int		taskIndex;
	static int		numThread;
	static DWORD	dThreadID[5];
	static HANDLE	hThread[5];
	static HANDLE	hEvent[5];
	static bool		bRuning[5];

	//  queue of tasks
	static Array<PTaskBase>		taskList[5];	//  4 + 1 for main thread

	//  use for push/pop item in the queue
	static CriticalSection		qCS;
};
int										TaskMan_internal::numThread;
DWORD									TaskMan_internal::dThreadID[5];
HANDLE									TaskMan_internal::hThread[5];
HANDLE									TaskMan_internal::hEvent[5];
bool									TaskMan_internal::bRuning[5];
Array<PTaskBase>						TaskMan_internal::taskList[5];	//  4 + 1 for main thread
CriticalSection							TaskMan_internal::qCS;
int	TaskMan_internal::taskIndex =				TaskMan_internal::Initialize();

namespace sx { namespace sys
{
	//////////////////////////////////////////////////////////////////////////
	//		TASK MANAGER
	//////////////////////////////////////////////////////////////////////////
	void TaskManager::AddTask( TaskBase* newTask, UINT destThread /*= SEGAN_TASKMAN_AUTO*/ )
	{
		TaskMan_internal::Add(newTask, destThread);
	}

	void TaskManager::RemoveTask( TaskBase* pTask )
	{
		TaskMan_internal::Remove(pTask);
	}

	bool TaskManager::IsEmpty( void )
	{
		return	TaskMan_internal::taskList[0].IsEmpty() &&
				TaskMan_internal::taskList[1].IsEmpty() &&
				TaskMan_internal::taskList[2].IsEmpty() &&
				TaskMan_internal::taskList[3].IsEmpty() &&
				TaskMan_internal::taskList[4].IsEmpty() &&

				!TaskMan_internal::bRuning[0] &&
				!TaskMan_internal::bRuning[1] &&
				!TaskMan_internal::bRuning[2] &&
				!TaskMan_internal::bRuning[3] &&
				!TaskMan_internal::bRuning[4];
	}

	void TaskManager::Clear( void )
	{
		TaskMan_internal::qCS.Enter();

		TaskMan_internal::taskList[1].Clear();
		TaskMan_internal::taskList[2].Clear();
		TaskMan_internal::taskList[3].Clear();
		TaskMan_internal::taskList[4].Clear();

		TaskMan_internal::qCS.Leave();

		while (!IsEmpty())
			Update(0);
	}

	int TaskManager::Count( void )
	{
		return	
			TaskMan_internal::taskList[0].Count() +
			TaskMan_internal::taskList[1].Count() +
			TaskMan_internal::taskList[2].Count() +
			TaskMan_internal::taskList[3].Count() +
			TaskMan_internal::taskList[4].Count();
	}

	void TaskManager::Update( float elpsTime )
	{
		sx_callstack_push(TaskManager::Update());

		for (int i=0; i<4; i++)
		{
			if (TaskMan_internal::taskList[0].IsEmpty()) return;

			TaskMan_internal::bRuning[0] = true;

			TaskMan_internal::qCS.Enter();
			TaskBase* task = TaskMan_internal::taskList[0].At( 0 );
			TaskMan_internal::taskList[0].RemoveByIndex( 0 );
			TaskMan_internal::qCS.Leave();

			if (task)
			{
				UINT flag = task->m_flag;

				SEGAN_SET_ADD(flag, SX_TASKBASE_EXECUTING);

				task->Execute();

				if (SEGAN_SET_HAS(flag, SX_TASKBASE_AUTO_FREE))
				{
					sx_delete_and_null(task);
				}
				else
				{
					SEGAN_SET_REM(flag, SX_TASKBASE_EXECUTING);
				}
			}

			TaskMan_internal::bRuning[0] = false;
		}
	}

	void TaskManager::Finalize( void )
	{
		TaskMan_internal::Finalize();
	}


	//////////////////////////////////////////////////////////////////////////
	//	TASK BASE
	//////////////////////////////////////////////////////////////////////////
	TaskBase::TaskBase( bool FreeOnTerminate ): m_flag(0)
	{
		if (FreeOnTerminate)
			m_flag = SX_TASKBASE_AUTO_FREE;
		else
			m_flag = 0;
	}

	TaskBase::~TaskBase()
	{

	}

	bool TaskBase::Executing( void )
	{
		return SEGAN_SET_HAS(m_flag, SX_TASKBASE_EXECUTING) != 0;
	}
} } // namespace sx { namespace sys
