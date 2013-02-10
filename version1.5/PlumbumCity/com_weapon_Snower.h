/********************************************************************
	created:	2011/11/28
	filename: 	com_weapon_Snower.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple snower weapon type
*********************************************************************/
#ifndef GUARD_Weapon_snower_HEADER_FILE
#define GUARD_Weapon_snower_HEADER_FILE


#include "ImportEngine.h"
#include "com_ParamModifier.h"

class Entity;
class Projectile;
struct prpAttack;


//! use this abstract class as grandpa of other game components
class com_weapon_Snower : public Component
{
public:

	com_weapon_Snower(void);
	virtual ~com_weapon_Snower(void);

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

	//! shoot a bullet and change index of pipe
	void ShootTheBullet( const prpAttack* pAttack, Sphere& targetSphere, bool special );

public:

	com_ParamModifier	m_special;			//	special component
	Entity*				m_target;			//  target entity

	sx::core::PNode		m_nodeWeapon;		//  weapon node
	sx::core::PNode		m_nodePipe[4];		//  pipes of weapon to shoot
	
	int					m_pipeIndex;		//  index of weapon pipeline
	int					m_shootCount;		//  number of shooting
	float				m_shootTime;		//  time of shooting
	float3				m_Dir;				//  direction of weapon
	float3				m_DirOffset;		//  offset of direction
	
	bool				m_Fire;				//  fire order for weapon
};

#endif	//	GUARD_Weapon_snower_HEADER_FILE
