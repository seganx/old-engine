/********************************************************************
	created:	2011/07/18
	filename: 	Projectile.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a base class of projectile objects
				each projectile should inherit from this class.
				Projectile::Update() should be called in game
				loop to manage projectiles
*********************************************************************/
#ifndef GUARD_Projectile_HEADER_FILE
#define GUARD_Projectile_HEADER_FILE

#include "ImportEngine.h"
#include "GameTypes.h"

//! base class of any ammo in the game world
class Projectile
{
	friend class ProjectileManager;

	SEGAN_STERILE_CLASS(Projectile);

public:

	Projectile(void);
	virtual ~Projectile(void);

	//! initial projectile and add to the 3D scene
	virtual void AddToScene( void );

	//! this will called in game loop
	virtual void Update(float elpstime) = 0;

	//! clone and return new projectile
	virtual Projectile* Clone(void) = 0;

protected:

	bool				m_dead;					//  set to true to notify manager to delete this in the next loop

public:

	prpAttack			m_attack;				//	>:D

	class Entity*		m_target;				//  target entity
	class Entity*		m_sender;				//  sender entity
	class Component*	m_component;			//	special component will attached to the target enemy
	sx::core::Node*		m_node;					//  scene node to draw bullet

	UINT				m_killParty;			//  which kind of party should kill with this ?
	float3				m_targetPos;			//	current position of bullet
	float3				m_pos;					//	current position of bullet
	float3				m_dir;					//  direction of bullet
	float				m_speed;				//  speed of bullet
	int					m_state;				//  state of projectile
	float				m_time;					//  time of movement
	float				m_initSpeed;			//	speed of initial time
	float3				m_initPos;				//  initial position of bomb used to compute curve
	float				m_radius;				//	radius of this bullet
	int					m_tag;
};

#endif	//	GUARD_Projectile_HEADER_FILE