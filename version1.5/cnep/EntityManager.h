/********************************************************************
	created:	2011/07/13
	filename: 	EntityManager.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain EntityManager to manage entities 
				in the game world
*********************************************************************/
#ifndef GUARD_EntityManager_HEADER_FILE
#define GUARD_EntityManager_HEADER_FILE

#include "GameTypes.h"

class Entity;

//////////////////////////////////////////////////////////////////////////
//	MANAGER
//////////////////////////////////////////////////////////////////////////
class EntityManager
{
public:
	
	//! cleanup entity types
	static void ClearTypes(void);

	//! load entity types from file "entities.txt"
	static void LoadTypes( Callback_Draw_Loading drawLoading );

	//! return entity type by specified index. return NULL for invalid index
	static const Entity* GetTypeByIndex(int index);

	//! return entity type by specified name. return NULL for invalid name
	static const Entity* GetTypeByName(const WCHAR* name);

	/*
	create entity by specified name.
	this function use names which loaded by EntityManager::LoadTypes()
	return simple entity if name = null
	return null for invalid name
	*/
	static Entity* CreateEntityByTypeName(const WCHAR* name);

	//! add created entity to the entity manager
	static void AddEntity(Entity* pe);

	//! remove created entity by ID
	static void RemoveEntity(UINT id);

	//! cleanup all entities.
	static void ClearEntities(void);

	//! return number of entities
	static UINT GetEntityCount(void);

	/*!
	return created entity by specified index.
	NOTE: just use this function to traverse entities
	*/
	static Entity* GetEntityByIndex(const int index);

	//! return created entity by specified ID. return NULL if no entity found
	static Entity* GetEntityByID(const UINT id);

	//! update all entities in the game world
	static void Update(float elpsTime);

	//! send message to the entities
	static void MsgProc(UINT RecieverID, UINT msg, void* data);

};

#endif	//	GUARD_EntityManager_HEADER_FILE