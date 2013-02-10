/********************************************************************
	created:	2012/07/24
	filename: 	com_ParamModifier.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a component to change others param
*********************************************************************/
#ifndef GUARD_ParamModifier_HEADER_FILE
#define GUARD_ParamModifier_HEADER_FILE

#include "ImportEngine.h"
#include "Component.h"
#include "GameTypes.h"


//! a simple component to change others param
class com_ParamModifier : public Component
{
public:

	com_ParamModifier(void);
	virtual ~com_ParamModifier(void);

	//! initialize Component will called after the entity initialized
	virtual void Initialize( void );

	//! finalize Component will called before entity finalized
	virtual void Finalize( void );

	//! update component
	virtual void Update( float elpsTime );

	//! handle the incoming message
	virtual void MsgProc( UINT msg, void* data );

	//! clone this component and return new one
	virtual Component* Clone( void );

public:

	str512		m_nodeName;				//	spray particle in this node when spelling
	
	float		m_coolTime;				//	used in emitter mode to emit itself 
	float		m_time;					//	used in emitter mode to count cool time
	float		m_radius;				//	used in emitter mode to find allays
	float		m_moveScale;			//	use move scale to control move speed on spelling
	int			m_animIndex;			//	used in emitter mode to play spelling animation
	float		m_animTime;				//	used in emitter mode to play spelling animation
	
	float		m_modifyTime;			//	the time of applying modifier
	float		m_speed;				//	speed of entity in modified time
	float		m_physicalArmor;
	float		m_electricalArmor;
	int			m_health;
	float		m_offsetY;				//	specific height of entity on the ground ( cm )
	MoveType	m_moveType;
	MoveType	m_mainMoveType;

	bool		m_emitter;

};

#endif	//	GUARD_ParamModifier_HEADER_FILE

