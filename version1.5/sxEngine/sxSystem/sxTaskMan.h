/********************************************************************
	created:	2010/09/05
	filename: 	sxTaskMan.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the base class of any task and 
				an static class of task manager to execute tasks
				on other thread.
*********************************************************************/
#ifndef GUARD_sxTaskMan_HEADER_FILE
#define GUARD_sxTaskMan_HEADER_FILE


#include "sxSystem_def.h"

// select destination thread automatically
#define SEGAN_TASKMAN_AUTO			0xFF

//  some forward declaration
class TaskMan_internal;

namespace sx { namespace sys
{

/*
TaskBase is the base of any task that will execute by task manager.
*/
class SEGAN_API TaskBase
{
SEGAN_STERILE_CLASS(TaskBase);

public:
	TaskBase(bool FreeOnTerminate);
	virtual ~TaskBase();

	//! return true if this task is executing right now.
	bool Executing(void);

	//! execute task. this function should be overridden by user and will call in task manager.
	virtual void Execute(void) = 0;

protected:
	DWORD	m_flag;		//	control the behavior of the task

private:
	friend class TaskMan_internal;
	friend class TaskManager;
};

/*
Task manager is responsible to execute coming tasks and delete specified tasks after execution.
*/
class SEGAN_API TaskManager
{
public:
	/*! add a task to the thread manager. use destThread to specify the index of thread.
	use 0 for select main thread or leave it automatically. */
	static void AddTask(TaskBase* newTask, UINT destThread = SEGAN_TASKMAN_AUTO);

	//! remove the task from queue. this function will not remove the task which currently executing.
	static void RemoveTask(TaskBase* pTask);

	//! return true if there is no task in task manager.
	static bool IsEmpty(void);

	//! just clear queue of tasks. NOTE: call is function in main thread.
	static void Clear(void);

	//! return number of tasks in the task manager
	static int Count(void);

	//! It's an update function for internal use.
	static void Update(float elpsTime);

	/*
	static manager will initialize automatically. use this function to finalize the task manager.
	this function will call automatically at the end of the application
	*/
	static void Finalize(void);
};

} } // namespace sx { namespace sys

#endif  //  GUARD_sxTaskMan_HEADER_FILE
