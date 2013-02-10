/********************************************************************
	created:	2011/07/18
	filename: 	ProjectileManager.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain ProjectileManager to manage projectiles
				in the game world.
				NOET : this manager use memory pool to create/delete any 
				projectiles. thus use ProjectileManager::DeleteProjectile
				to delete individual projectile;

*********************************************************************/
#ifndef GUARD_ProjectileManager_HEADER_FILE
#define GUARD_ProjectileManager_HEADER_FILE

#include "ImportEngine.h"
#include "GameTypes.h"

class Projectile;

/*
use projectile manager to create/destroy/update any projectile in the scene
NOET : this manager use memory pool to create/delete any 
projectiles. thus use ProjectileManager::DeleteProjectile
to delete individual projectile;
*/
class ProjectileManager
{
public:
	//! fully cleanup manager. delete all objects and types
	static void ClearTypes(void);

	//! load projectile types from file "projectiles.txt"
	static void LoadTypes(void);

	//! return projectile type by specified name. return NULL for invalid name
	static Projectile* GetTypeByName(const WCHAR* name);

	//! just allocate new projectile by type. this function DO NOT add this new projectile to game world
	static Projectile* CreateProjectileByType(ProjectileType weaponType);

	/*
	create entity by specified name.
	this function use names which loaded by ProjectileManager::LoadTypeNames()
	return null for invalid name
	NOTE: this function is too slow. just use it in weapon creation to get an
	instance of projectile and the use projectile->clone() to create new one
	*/
	static Projectile* CreateProjectileByTypeName(const WCHAR* name);

	//! delete projectile which created by this manager
	static void DeleteProjectile(Projectile* &me);

	//! add a projectile to manager
	static void AddProjectile(Projectile* p);

	//! cleanup all projectiles.
	static void ClearProjectiles(void);

	//! update all projectile in the manager
	static void Update(float elpstime);

	//! return number of projectiles in the scene
	static int GetProjectileCount(void);

	//! handle messages
	static void MsgProc(UINT recieverID, UINT msg, void* data);

};

#endif	//	GUARD_ProjectileManager_HEADER_FILE