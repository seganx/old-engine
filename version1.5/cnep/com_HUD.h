/********************************************************************
	created:	2012/02/18
	filename: 	com_HUD.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple component to show com_HUD
*********************************************************************/
#ifndef GUARD_com_HUD_HEADER_FILE
#define GUARD_com_HUD_HEADER_FILE

#include "ImportEngine.h"
#include "Component.h"

class Entity;


//! a simple component to show com_HUD
class com_HUD : public Component
{
public:

	com_HUD(void);
	virtual ~com_HUD(void);

	//! initialize Component will called after the entity initialized
	virtual void Initialize(void);

	//! finalize Component will called before entity finalized
	virtual void Finalize(void);

	//! update component
	virtual void Update(float elpsTime);

	//! handle the incoming message
	virtual void MsgProc(UINT msg, void* data);

	//! clone this component and return new one
	virtual Component* Clone(void);

private:

	float3					m_addPos;			//  additional position

	//float					m_healthBar_Time;	//  time of display
	sx::gui::PProgressBar	m_healthBar;		//  health bar of entities

public:

	//! reference to integer value which describe health bar display mode. 0=hide 1=always show 2=on damage show
	static int& Display_HealthBar(void);

};

#endif	//	GUARD_com_HUD_HEADER_FILE
