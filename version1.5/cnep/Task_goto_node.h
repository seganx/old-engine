/********************************************************************
	created:	2011/07/15
	filename: 	Task_goto_node.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the state which move entity to the
				specified node position. for this propose this state 
				should find the path by pathfinder and traverse throw 
				path nodes
*********************************************************************/
#ifndef GUARD_Task_goto_node_HEADER_FILE
#define GUARD_Task_goto_node_HEADER_FILE

#include "Task.h"

class Task_goto_node: public Task
{
	SEGAN_STERILE_CLASS(Task_goto_node);

public:

	Task_goto_node(void);
	virtual ~Task_goto_node(void);

	//! initialize state. this will try to find the path
	virtual void Initialize(void);

	//! finalize the state
	virtual void Finalize(void);

	//! this will called in game loop if state is activated
	virtual void Update(float elpstime, DWORD& status);

	//! handle messages
	virtual void MsgProc(UINT msg, void* data);

private:

	float						m_stunValue;			//  value of speed decrement
	float						m_stunTime;				//  time of stun
	float						m_animScale;			//	speed changed ?

	sx::core::PNode				m_startNode;			//  start node in the scene
	sx::core::PNode				m_endNode;				//  start node in the scene

	sx::core::ArrayPPathNode	m_Path;					//  array of path nodes
	int							m_targetNode;			//  index of target node in the path

	UINT						m_group;
	int							m_matIndex;				//	index of current material

};

#endif	//	GUARD_Task_goto_node_HEADER_FILE