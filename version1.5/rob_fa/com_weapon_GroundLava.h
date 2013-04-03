/********************************************************************
	created:	2011/11/28
	filename: 	com_weapon_GroundLava.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple GroundLava weapon type
*********************************************************************/
#ifndef GUARD_Weapon_GroundLava_HEADER_FILE
#define GUARD_Weapon_GroundLava_HEADER_FILE


#include "ImportEngine.h"
#include "Component.h"

class Entity;
struct prpAttack;


//! use this abstract class as grandpa of other game components
class com_weapon_GroundLava : public Component
{
public:

	com_weapon_GroundLava(void);
	virtual ~com_weapon_GroundLava(void);

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

	//! search for head shot
	void SearchForHeadshot( float elpsTime );

	//! update the head shot mode
	void UpdateHeadshot( float elpsTime );

public:

	Entity*				m_target;			//  target entity

	sx::core::PNode		m_nodeWeapon;		//  weapon node
	sx::core::PNode		m_nodePipe;			//  pipes of weapon to shoot
	int					m_shootCount;		//  number of shooting
	float				m_shootTime;		//  time of shooting
	float3				m_dir;				//  direction of weapon
	float3				m_dirOffset;		//  offset of direction

	float				m_searchTime;		//	cool time head shot
	bool				m_headShot;			//	weapon is going to head shot

	int					m_fire;				//  fire order for weapon

};

#endif	//	GUARD_Weapon_GroundLava_HEADER_FILE
