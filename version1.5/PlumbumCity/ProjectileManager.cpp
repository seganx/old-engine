#include "ProjectileManager.h"
#include "ImportEngine.h"
#include "Scripter.h"
#include "Projectile.h"
#include "projectile_Bullet.h"
#include "Projectile_BOMB.h"
#include "projectile_ClusterBomb.h"
#include "projectile_Missile.h"
#include "GameConfig.h"

//static sx::mem::MemPool					s_Pool( 5242880 );		//	5MB for all projectiles in the scene
static Array<Projectile*>		s_Projectiles;			//  projectiles in the scene
static Map<UINT, Projectile*>  s_ProjectileTypes;		//  types of projectiles 

void ProjectileManager::ClearTypes( void )
{
	sx_callstack_push(ProjectileManager::ClearTypes());


	for (Map<UINT, Projectile*>::Iterator it = s_ProjectileTypes.First(); !it.IsLast(); it++)
	{
		sx_delete_and_null( *it );
	}
	s_ProjectileTypes.Clear();

}

void ProjectileManager::LoadTypes( void )
{
	sx_callstack_push(ProjectileManager::LoadTypes());


	String str = sx::sys::FileManager::Project_GetDir();
	str << L"projectiles.txt";

	Scripter script;
	script.Load( str );

	for (int i=0; i<script.GetObjectCount(); i++)
	{
		str512 tmpStr, tmpName;
		if ( script.GetString(i, L"Type", tmpStr) )
		{
			if ( !script.GetString(i, L"Name", tmpName) )
				continue;
			Projectile* proj = NULL;

			script.GetString(i, L"Type", tmpStr);
			if ( tmpStr == L"BULLET" )
				proj = sx_new( projectile_Bullet );
			else if ( tmpStr == L"BOMB" )
				proj = sx_new( Projectile_BOMB );
			else if ( tmpStr == L"MISSILE" )
				proj = sx_new( projectile_Missile );
			else if ( tmpStr == L"CLUSTERBOMB" )
			{
				projectile_ClusterBomb* cproj = sx_new( projectile_ClusterBomb );
				script.GetString( i, L"miniBomb", tmpStr );
				String::Copy( cproj->m_miniBombName, 128, tmpStr);
				proj = cproj;
			}
			else continue;

			script.GetFloat(i, L"speed", proj->m_speed);

			if ( script.GetString(i, L"node", tmpStr) )
			{
				PStream pfile = NULL;
				if ( sx::sys::FileManager::File_Open(tmpStr, SEGAN_PACKAGENAME_DRAFT, pfile) )
				{
					proj->m_node = sx_new( sx::core::Node );
					proj->m_node->Load( *pfile );
					proj->m_node->SetUserTag( PARTY_PROJECTILE );

					float f = 0;
					proj->m_node->MsgProc(MT_ACTIVATE, &f);

					sx::sys::FileManager::File_Close(pfile);
				}
			}

			if ( !proj->m_node )
			{
				sxLog::Log( L"Node not found for projectile '%s'!", tmpName.Text() );
				sx_delete_and_null( proj );
				continue;
			}

			if ( s_ProjectileTypes.Insert( sx::cmn::GetCRC32(tmpName), proj ) )
			{
				if ( Config::GetData()->display_Debug == 3 )
				{
					sxLog::Log( L"Projectile registered with type '%s' and name '%s'", tmpStr.Text(), tmpName.Text() );
				}
			}
		}
	}
}

Projectile* ProjectileManager::GetTypeByName( const WCHAR* name )
{
	sx_callstack_push(ProjectileManager::GetTypeByName(name=%s), name);

	if ( !name || !name[0] ) return NULL;

	UINT nameID = sx::cmn::GetCRC32( name );

	Projectile* res = NULL;
	if ( !s_ProjectileTypes.Find( nameID, res ) )
		sxLog::Log( L"ERROR : bullet NOT found with name '%s'", name );

	return res;
}

Projectile* ProjectileManager::CreateProjectileByType( ProjectileType weaponType )
{
	switch (weaponType)
	{
	case GPT_BULLET:
		{
			//projectile_Bullet* p = s_Pool.Alloc<projectile_Bullet*>( sizeof(projectile_Bullet) );
			//p->projectile_Bullet::projectile_Bullet();
			return sx_new( projectile_Bullet );
		}
		break;

	case GPT_BOMB:
		{
			//Projectile_BOMB* p = s_Pool.Alloc<Projectile_BOMB*>( sizeof(Projectile_BOMB) );
			//p->Projectile_BOMB::Projectile_BOMB();
			return  sx_new( Projectile_BOMB );
		}
		break;

	case GPT_CLUSTERBOMB:
		{
			//projectile_ClusterBomb* p = s_Pool.Alloc<projectile_ClusterBomb*>( sizeof(projectile_ClusterBomb) );
			//p->projectile_ClusterBomb::projectile_ClusterBomb();
			return sx_new( projectile_ClusterBomb );
		}
		break;

	case GPT_MISSILE:
		{
			//projectile_Missile* p = s_Pool.Alloc<projectile_Missile*>( sizeof(projectile_Missile) );
			//p->projectile_Missile::projectile_Missile();
			return sx_new( projectile_Missile );
		}
		break;
	}

	sxLog::Log( L" ERROR : ProjectileManager::CreateProjectileByType returned null !" );
	return NULL;
}

Projectile* ProjectileManager::CreateProjectileByTypeName( const WCHAR* name )
{
	sx_callstack_push(ProjectileManager::CreateProjectileByTypeName(name=%s), name);
	sx_assert(name);

	if ( !name ) return NULL;

	UINT nameID = sx::cmn::GetCRC32(name);

	Projectile* res = NULL;

	if ( !s_ProjectileTypes.Find(nameID, res) )
		sxLog::Log(L"ERROR : bullet NOT found with name '%s'", name);

	if (res)
		return res->Clone();
	else
		return NULL;
}

void ProjectileManager::DeleteProjectile( Projectile* &me )
{
	sx_callstack_push(ProjectileManager::DeleteProjectile());

	//me->Projectile::~Projectile();
	//s_Pool.Free(me);
	//me = NULL;
	sx_delete_and_null( me );
}

void ProjectileManager::AddProjectile( Projectile* p )
{
	sx_callstack_push(ProjectileManager::AddProjectile());

	if ( p->m_node )
	{
		p->AddToScene();
		s_Projectiles.PushBack(p);
	}
	else
		DeleteProjectile( p );
}

void ProjectileManager::ClearProjectiles( void )
{
	sx_callstack_push(ProjectileManager::ClearProjectiles());


	for (int i=0; i<s_Projectiles.Count(); i++)
	{
		Projectile* me = s_Projectiles[i];
		DeleteProjectile(me);
	}
	s_Projectiles.Clear();

}

void ProjectileManager::Update( float elpstime )
{
	sx_callstack_push(ProjectileManager::Update());

	for ( int i=0; i<s_Projectiles.Count(); i++ )
	{
		s_Projectiles[i]->Update( elpstime );
	}

	for ( int i=0; i<s_Projectiles.Count(); i++ )
	{
		Projectile* me = s_Projectiles[i];
		if ( me->m_dead )
		{
			DeleteProjectile( me );
			s_Projectiles.RemoveByIndex( i );
			i--;
		}
	}

}

int ProjectileManager::GetProjectileCount( void )
{
	return s_Projectiles.Count();
}

void ProjectileManager::MsgProc( UINT recieverID, UINT msg, void* data )
{
	sx_callstack_push(ProjectileManager::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

	switch ( msg )
	{
	case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_LEVEL_LOAD

	case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
		{						//////////////////////////////////////////////////////////////////////////
			ClearProjectiles();
		}
		break;	//	GMT_LEVEL_CLEAR

	case GMT_GAME_START:		/////////////////////////////////////////////////    START GAME
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_GAME_START

	case GMT_GAME_END:			/////////////////////////////////////////////////    END GAME
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_GAME_END

	case GMT_GAME_RESETING:		/////////////////////////////////////////////////    RESET GAME
		{						//////////////////////////////////////////////////////////////////////////
			ClearProjectiles();
		}
		break;	//	GMT_GAME_RESETING

	case GMT_GAME_RESET:
		{
			if ( Config::GetData()->display_Debug == 3 )
			{
				ClearTypes();
			}

			if ( Config::GetData()->display_Debug == 3 )
			{
				LoadTypes();
			}
		}
		break;	//	GMT_GAME_RESET
	}
}
