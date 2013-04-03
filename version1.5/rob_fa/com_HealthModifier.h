/********************************************************************
	created:	2012/07/23
	filename: 	com_HealthModifier.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple component to regenerate the health
*********************************************************************/
#ifndef GUARD_com_HealthModifier_HEADER_FILE
#define GUARD_com_HealthModifier_HEADER_FILE

#include "ImportEngine.h"
#include "Component.h"

//! a simple component to regenerate health
class com_HealthModifier : public Component
{
public:

	com_HealthModifier(void);
	virtual ~com_HealthModifier(void);

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

	str512				m_nodeName;
	int					m_value;
	int					m_count;
	float				m_coolTime;
	float				m_time;

};

#endif	//	GUARD_com_HealthModifier_HEADER_FILE