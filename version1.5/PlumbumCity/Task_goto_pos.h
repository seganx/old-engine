/********************************************************************
	created:	2012/02/28
	filename: 	Task_goto_pos.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the state which moves entity to the
				specified position directly
*********************************************************************/
#ifndef GUARD_Task_goto_pos_HEADER_FILE
#define GUARD_Task_goto_pos_HEADER_FILE


#include "Task.h"

class Task_goto_pos: public Task
{
	SEGAN_STERILE_CLASS(Task_goto_pos);

public:

	Task_goto_pos(void);
	virtual ~Task_goto_pos(void);

	//! initialize state. this will try to find the path
	virtual void Initialize(void);

	//! finalize the state
	virtual void Finalize(void);

	//! this will called in game loop if state is activated
	virtual void Update(float elpstime, DWORD& status);

	//! handle messages
	virtual void MsgProc(UINT msg, void* data);

private:

	float		m_stunValue;		//  value of speed decrement
	float		m_stunTime;			//  time of stun

	float3		m_posGoal;			//  goal position in the scene

};

#endif	//	GUARD_Task_goto_pos_HEADER_FILE
