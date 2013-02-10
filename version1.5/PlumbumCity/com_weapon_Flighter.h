/********************************************************************
	created:	2012/07/29
	filename: 	com_weapon_Flighter.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a full pipe crazy weapon
*********************************************************************/
#ifndef GUARD_com_weapon_Flighter_HEADER_FILE
#define GUARD_com_weapon_Flighter_HEADER_FILE


#include "ImportEngine.h"
#include "Component.h"

class Entity;
class Projectile;
struct prpAttack;


//! use this abstract class as grandpa of other game components
class com_weapon_Flighter : public Component
{
public:

	com_weapon_Flighter(void);
	virtual ~com_weapon_Flighter(void);

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
	void ShootTheBullet( const prpAttack* pAttack, Sphere& targetSphere );

public:

	Entity*				m_target;			//  target entity

	sx::core::PNode		m_nodeWeapon;		//  weapon node
	sx::core::PNode		m_nodePipe[4];		//  pipes of weapon to shoot
	
	int					m_pipeIndex;		//  index of weapon pipeline
	int					m_shootCount;		//  number of shooting
	float				m_shootTime;		//  time of shooting
	float3				m_Dir;				//  direction of weapon
	float3				m_DirOffset;		//  offset of direction
	
	float				m_time;				//	the crazy cool time

	bool				m_Fire;				//  fire order for weapon
};

#endif	//	GUARD_com_weapon_Flighter_HEADER_FILE
