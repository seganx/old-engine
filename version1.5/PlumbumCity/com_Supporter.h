/********************************************************************
	created:	2012/07/26
	filename: 	com_Supporter.h
	Author:		Nader Golbaz
	eMail:		ngolbaz@gmail.com
	Site:		www.?.com
	Desc:		This file contain a simple component to make ...
*********************************************************************/
#ifndef GUARD_com_Supporter_HEADER_FILE
#define GUARD_com_Supporter_HEADER_FILE


#include "ImportEngine.h"
#include "Component.h"


//! a simple component to ...
class com_Supporter : public Component
{
public:

	com_Supporter(void);
	virtual ~com_Supporter(void);

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
	
	sx::core::PNode		m_node;
	float				m_time;
	float				m_energy;
	float				m_repair;
	bool				m_started;

	Array<Entity*>		m_towers;

};

#endif	//	GUARD_com_Supporter_HEADER_FILE