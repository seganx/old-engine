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

private:
	
	sx::core::PNode		m_node;
	float				m_time;
	float				m_energy;
	float				m_repair;
	float				m_overActiveTime;
	float				m_time_exp;

	struct EntityExp
	{
		Entity* entity;
		float	experience;

		EntityExp();

		EntityExp(Entity* entity, float	experience);

		bool operator==(const EntityExp& other) const;
	};

	Array<EntityExp>	m_towers;

};

#endif	//	GUARD_com_Supporter_HEADER_FILE