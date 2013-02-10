/********************************************************************
	created:	2011/07/15
	filename: 	Task.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain this is the base class of other tasks
*********************************************************************/
#ifndef GUARD_Task_HEADER_FILE
#define GUARD_Task_HEADER_FILE

#include "ImportEngine.h"

class Entity;


class Task
{
	SEGAN_STERILE_CLASS(Task);

public:
	
	enum{TS_ACTIVE, TS_INACTIVE, TS_COMPLETED, TS_FAILED};

	Task(void);
	virtual ~Task(void);

	//! return status of this state
	UINT GetStatus(void);

	//! set owner of this state
	void SetOwner(Entity* owner);

	//! return owner
	Entity* GetOwner(void);

	//! initialize state 
	virtual void Initialize(void);

	//! finalize the state
	virtual void Finalize(void);

	//! this will called in game loop if state is activated
	virtual void Update(float elpstime, DWORD& status);

	//! handle messages
	virtual void MsgProc(UINT msg, void* data);

	//! return a unique number for each type
	static UINT GetType(Task* task);

protected:

	//! add new sub state to the states list
	virtual void AddSubState(Task* state);

	//! clear all allocated data and sub states in the state list
	virtual void CleanSubStates(void);

protected:

	UINT					m_Type;			//  is a simple unique number used in brain
	UINT					m_Status;		//  status of this state
	Entity*					m_owner;		//  owner of this state
	Stack<Task*>	m_subStates;	//  stake of sub states
};

#endif	//	GUARD_Task_HEADER_FILE