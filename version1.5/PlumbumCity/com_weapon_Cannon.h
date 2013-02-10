/********************************************************************
	created:	2011/12/24
	filename: 	Weapon_Cannon.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple cannon weapon type
*********************************************************************/
#ifndef GUARD_Weapon_Cannon_HEADER_FILE
#define GUARD_Weapon_Cannon_HEADER_FILE

#include "ImportEngine.h"
#include "Component.h"

class Entity;
class Projectile;


//! use this abstract class as grandpa of other game components
class com_weapon_Cannon : public Component
{
public:

	com_weapon_Cannon(void);
	virtual ~com_weapon_Cannon(void);

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

	//! blend weapon direction with Y correction
	void BlendDirection(float elpsTime);

	//! fire a bomb to the scene
	void ShootTheBomp( int special );

	Entity*				m_target;

	sx::core::PNode		m_nodeWeapon;		//  weapon node
	sx::core::PNode		m_nodePipe;			//  pipe of weapon to shoot

	int					m_shootCount;		//  number of shooting
	float				m_shootTime;		//  time of shooting
	float3				m_Dir;				//  direction of weapon
	float3				m_DirOffset;		//  offset of direction

	bool				m_Fire;				//  fire order for weapon

};

#endif	//	GUARD_Weapon_Cannon_HEADER_FILE
