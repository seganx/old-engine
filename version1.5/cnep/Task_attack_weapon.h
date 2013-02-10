/********************************************************************
	created:	2011/07/30
	filename: 	Task_attack_weapon.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a task to attack to target entity
				with weapon
*********************************************************************/
#ifndef GUARD_Task_attack_weapon_HEADER_FILE
#define GUARD_Task_attack_weapon_HEADER_FILE

#include "Task.h"

class Task_attack_weapon: public Task
{
	SEGAN_STERILE_CLASS(Task_attack_weapon);

public:

	Task_attack_weapon(void);
	virtual ~Task_attack_weapon(void);

	//! initialize state.
	virtual void Initialize(void);

	//! finalize the state
	virtual void Finalize(void);

	//! this will called in game loop if state is activated
	virtual void Update(float elpstime, DWORD& status);

	//! handle messages
	virtual void MsgProc(UINT msg, void* data);

private:

	//! search for the enemies in the scene
	void SearchForEnemy();

private:
	
	float						m_searchTime;		//  time of search for enemies
	float						m_fireTime;			//	fire time
	Array_fix<Entity*, 128>		m_arrTargets;		//  array of target nodes
	Entity*						m_Target;			//  attack target
	int							m_targetIndex;		//  index of target for stunner


};

#endif	//	GUARD_Task_attack_weapon_HEADER_FILE
