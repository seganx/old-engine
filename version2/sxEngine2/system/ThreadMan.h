/********************************************************************
	created:	2012/04/30
	filename: 	TaskMan.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the base class of any task and 
				an static class of task manager to execute tasks
				on other thread.
*********************************************************************/
#ifndef GUARD_TaskMan_HEADER_FILE
#define GUARD_TaskMan_HEADER_FILE


#include "System_def.h"


// select destination thread automatically
#define SEGAN_TASKMAN_AUTO			0xFF



//! ThreadTask is the base of any task that will execute by task manager.
class SEGAN_ENG_API ThreadTask
{
	SEGAN_STERILE_CLASS(ThreadTask);

public:

	ThreadTask( void );
	virtual ~ThreadTask( void );

	//! task will be deleted after execution done
	void FreeOnTerminate( bool enable );

	//! return true if this task is executing right now.
	bool Executing( void );

	//! execute task. this function should be overridden by user and will call in task manager.
	virtual void Execute( void ) = 0;

public:

	dword		m_flag;		//	control the behavior of the task

};


//! initialize thread manager
SEGAN_ENG_API void sx_thread_initilize( void );

//! finalize the thread manager
SEGAN_ENG_API void sx_thread_finalize(void);

/*! add a task to the thread manager. use destThread to specify the index of a thread
use 0 for select main thread or leave it automatically. */
SEGAN_ENG_API void sx_thread_add_task( ThreadTask* ptask, uint destThread = SEGAN_TASKMAN_AUTO );

//! remove the task from the queue
SEGAN_ENG_API void sx_thread_remove_task( ThreadTask* ptask );

//! return true if there is no task in task manager
SEGAN_ENG_API bool sx_thread_isempty( void );

//! just clear queue of tasks. NOTE: call is function in main thread
SEGAN_ENG_API void sx_thread_clear( void );

//! return number of tasks in the task manager
SEGAN_ENG_API int sx_thread_count( void );

//! it's an update function for internal use
SEGAN_ENG_API void sx_thread_update( float elpsTime );


#endif	//	GUARD_TaskMan_HEADER_FILE

