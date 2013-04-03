/********************************************************************
	created:	2012/07/23
	filename: 	com_GoldenTower.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple component to make golden tower
*********************************************************************/
#ifndef GUARD_com_GoldenTower_HEADER_FILE
#define GUARD_com_GoldenTower_HEADER_FILE

#include "ImportEngine.h"
#include "Component.h"

//! a simple component to regenerate health
class com_GoldenTower : public Component
{
public:

	com_GoldenTower(void);
	virtual ~com_GoldenTower(void);

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
	float		m_time;
};

#endif	//	GUARD_com_GoldenTower_HEADER_FILE
