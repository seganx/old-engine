/********************************************************************
	created:	2012/07/26
	filename: 	com_Turbo.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple component to make entity turbo
*********************************************************************/
#ifndef GUARD_com_Turbo_HEADER_FILE
#define GUARD_com_Turbo_HEADER_FILE


#include "ImportEngine.h"
#include "Component.h"


//! a simple component to regenerate health
class com_Turbo : public Component
{
public:

	com_Turbo(void);
	virtual ~com_Turbo(void);

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
	sx::core::PNode		m_node;
	int					m_turboCount;
	float				m_speed;
	float				m_speedTime;
	float				m_time;

};

#endif	//	GUARD_com_Turbo_HEADER_FILE