/********************************************************************
	created:	2011/07/15
	filename: 	Brain.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of the brain. 
				this class make exist on any entity. if the entity is
				under control, the brain use player commands, defined 
				missions and ... otherwise the brain use State_think to 
				make a perfect decision
*********************************************************************/
#ifndef GUARD_Brain_HEADER_FILE
#define GUARD_Brain_HEADER_FILE

#include "ImportEngine.h"
#include "GameTypes.h"

class Entity;
class Task;

class Brain
{
	SEGAN_STERILE_CLASS(Brain);

public:
	Brain(void);
	~Brain(void);
	void Initialize(void);
	void Finalize(void);

	//! set brain's owner
	void SetOwner(Entity* owner);

	//! add new mission to brain
	void AddMission(Mission& m);

	//! remove all brain
	void ClearMissions(void);

	//! this should call frequently in entity update
	void Update(float elpsTime);

	//! use to communicate with brain
	void MsgProc(UINT msg, void* data);

	//! process the current mission
	void ProcessMission( float elpsTime, Mission& m );

private:
	
	//! set current mission to process
	void SetCurrentMission(UINT index);

	//! delete current task and set new task
	void SetCurrentTask(Task* curTask);

	//! delete parallel task and set new task
	void SetParallelTask(Task* parTask);

private:

	Array<Mission>		m_Missions;			//  array of missions
	Entity	*			m_Owner;			//  owner of this brain
	int					m_missionIndex;		//  index of mission
	float				m_missionTime;		//  time of current mission

	Task	*			m_taskCurrent;		//  current task of entity
	Task	*			m_taskParallel;		//  parallel task of entity
};
#endif	//	GUARD_Brain_HEADER_FILE