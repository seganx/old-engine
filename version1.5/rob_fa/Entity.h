/********************************************************************
	created:	2011/07/12
	filename: 	GameEntity.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a base class of any entity in the game
*********************************************************************/
#ifndef GUARD_GameEntity_HEADER_FILE
#define GUARD_GameEntity_HEADER_FILE

//#include "cnep.h"
#include "GameTypes.h"
#include "Brain.h"
#include "Component.h"


#define NUM_LEVELS		7	//	number of attack array
#define MAX_LEVEL		3	//	maximum levels


class Task;

//  this is the base class of entities in the game
class Entity
{

	SEGAN_STERILE_CLASS(Entity);

public:

	Entity(void);
	~Entity(void);

	//! add a new component to the entity
	void Attach(Component* com);

	//! remove a component to the entity
	void Detach(Component* com);

	//! init properties, enter node to scene, ...
	void Initialize(void);

	//! free allocated memory, leave node from scene, ...
	void Finalize(void);

	//! return position of this entity
	const float3& GetPosition() const;

	//! set new position for entity
	inline void SetPosition(const float3& pos);

	//! return direction of this entity
	inline const float3& GetDirection(void) const;

	//! set head direction of the entity
	inline void SetDirection(const float3& dir);

	//! return pointer to the brain
	Brain* GetBrain(void);

	//! return bounding sphere of the mesh
	Sphere GetBoundingSphere( void );

	//! return distance of entities
	float GetDistance_edge( Entity* target );

	//! set level of entity
	void SetLevel(int level);

	//! set unit state
	void SetState(UINT state);

	//! update entity in the game world
	void Update(float elpsTime);

	//! handle the incoming message
	void MsgProc(UINT msg, void* data);

	//! clone this entity
	Entity* Clone(void);

	//! set selected entity
	static void SetSelected(Entity* pEntity);

	//! return selected entity
	static Entity* GetSelected(void);

public:

	bool					m_initialized;						//  verify that this entity has been initialized
	bool					m_upgradeReady;						//	upgrade is ready
	UINT					m_ID;								//  unique ID
	String					m_typeName;							//  sub type name of the entity
	String					m_typeDesc;							//  sub type name of the entity
	String					m_displayName;						//  name will display in hints
	UINT					m_partyCurrent;						//  describe entity party
	UINT					m_partyEnemies;						//  this entity can kill some body in some party
	UINT					m_state;							//  state of this entity
	int						m_level;							//  level of the entity
	int						m_maxLevel;							//  level of the entity
	int						m_cost[NUM_LEVELS];					//  cost of his entity
	int						m_costXP[NUM_LEVELS];				//  experience cost of his entity
	prpMove					m_move;
	prpHealth				m_health;
	prpAttack				m_attackLevel[NUM_LEVELS];			//  attack levels optional
	prpAttack				m_curAttackLevel;
	prpAttack				m_curAttack;
	prpAttack				m_curAbility;
	arrayPComponent			m_components;						//  array of components	
	Brain					m_brain;							//  the brain can think
	float3					m_pos;
	float3					m_posOffset;
	float3					m_face;								//  face direction of the entity
	sx::core::PNode			m_node;								//  scene node in the engine
	sx::core::PMesh			m_mesh;								//	mesh in the scene

	WeaponType				m_weaponType;						//	weapon type
	float					m_experience;						//	experience

	//	TEST
	float					test_onDamageXP;
	float					test_onDeadXP;
};

#endif	//	GUARD_GameEntity_HEADER_FILE