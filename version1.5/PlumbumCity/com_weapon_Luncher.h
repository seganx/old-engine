/********************************************************************
	created:	2012/11/20
	filename: 	Weapon_Luncher.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple Luncher weapon type
*********************************************************************/
#ifndef GUARD_Weapon_Luncher_HEADER_FILE
#define GUARD_Weapon_Luncher_HEADER_FILE

#include "ImportEngine.h"
#include "Component.h"

class Entity;
class Projectile;


//! use this abstract class as grandpa of other game components
class com_weapon_Luncher : public Component
{
public:

	com_weapon_Luncher(void);
	virtual ~com_weapon_Luncher(void);

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

	//! fire a missile to the scene
	void ShootTheMissile( void );

	Entity*				m_target;
	sx::core::PNode		m_nodeWeapon;		//  weapon node
	sx::core::PNode		m_nodePipe[4];		//  pipe of weapon to shoot
	int					m_pipeIndex;
	float				m_shootTime;		//  time of shooting
	bool				m_Fire;				//  fire order for weapon

};

#endif	//	GUARD_Weapon_com_Luncher_HEADER_FILE
