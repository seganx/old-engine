#include "EntityManager.h"
#include "Entity.h"
#include "Scripter.h"
#include "GameConfig.h"
#include "com_weapon_MachineGun.h"
#include "com_weapon_Cannon.h"
#include "com_weapon_Flighter.h"
#include "com_weapon_GroundLava.h"
#include "com_weapon_Snower.h"
#include "com_weapon_Luncher.h"
#include "com_ShowRange.h"
#include "com_HUD.h"
#include "com_GoldenTower.h"
#include "com_HealthModifier.h"
#include "com_ParamModifier.h"
#include "ComponentManager.h"

typedef Map<UINT, Entity*>		sxMapEntity;
typedef Array<Entity*>			sxArrEntity;


static sxMapEntity				s_EntityMap;		//!  use to fast search
static sxArrEntity				s_EntityArr;		//!	 use to fast traverse
static sxArrEntity				s_EntityTypes;		//!  use to hold types of entities

//////////////////////////////////////////////////////////////////////////
//	ENTITY MANAGER
//////////////////////////////////////////////////////////////////////////

void EntityManager::ClearTypes( void )
{
	sx_callstack();


 	for (int i=0; i<s_EntityTypes.Count(); i++)
 	{
		Entity* en = s_EntityTypes[i];

		if ( Config::GetData()->display_Debug == 3 )
		{
			sxLog::Log( L"clear %s", en->m_typeName.Text() );
		}

		sx_delete_and_null( en );
 	}
 	s_EntityTypes.Clear();
}

void EntityManager::LoadTypes( Callback_Draw_Loading drawLoading )
{
	sx_callstack();

	String str = sx::sys::FileManager::Project_GetDir();
	str << L"entities.txt";

	Scripter script;
	script.Load( str );

	for (int i=0; i<script.GetObjectCount(); i++)
	{
		str512 tmpStr;

		if ( !script.GetString(i, L"node", tmpStr) ) continue;
		if ( !sx::sys::FileManager::File_Exist(tmpStr, SEGAN_PACKAGENAME_DRAFT) ) continue;
		if ( !script.GetString(i, L"Name", tmpStr) ) continue;

		if ( drawLoading )
			drawLoading( 0, 0, L"loading entity", tmpStr );

		Entity* pe = sx_new( Entity );
		pe->m_typeName = tmpStr;
		s_EntityTypes.PushBack(pe);
	
		if ( Config::GetData()->display_Debug == 3 )
			sxLog::Log(L"Entity registered with name : %s", tmpStr.Text());

		if ( script.GetString(i, L"node", tmpStr) )
		{
			PStream pfile = NULL;
			if ( sx::sys::FileManager::File_Open(tmpStr, SEGAN_PACKAGENAME_DRAFT, pfile) )
			{
				pe->m_node = sx_new( sx::core::Node );
				pe->m_node->Load(*pfile);
				sx::sys::FileManager::File_Close(pfile);
			}
		}

		if ( !pe->m_node )
		{
			sxLog::Log(L"ERROR : Node not found for entity %s", tmpStr.Text());
		}

		if ( script.GetString(i, L"desc", tmpStr) )
		{
			pe->m_typeDesc = tmpStr;
			pe->m_typeDesc.Replace(L"\\n", L"\n");
		}

		if ( script.GetString(i, L"Type", tmpStr) )
		{
			if ( tmpStr == L"Tower" )
			{
				pe->m_partyCurrent = PARTY_TOWER;
				pe->m_partyEnemies = PARTY_ENEMY;

				script.GetFloat( i, L"onDamageXP", pe->test_onDamageXP );
				script.GetFloat( i, L"onDeadXP", pe->test_onDeadXP );

				if ( script.GetString(i, L"targetType", tmpStr) )
				{
					if ( tmpStr == L"air" )
						pe->m_curAttack.targetType = GMT_AIR;
					else if ( tmpStr == L"ground" )
						pe->m_curAttack.targetType = GMT_GROUND;
				}

				for (int l=0; l<NUM_LEVELS; l++)
				{
					tmpStr.Format(L"%d_cost", l);
					script.GetInteger(i, tmpStr, pe->m_cost[l]);

					tmpStr.Format(L"%d_unlockXP", l);
					script.GetInteger(i, tmpStr, pe->m_costXP[l]);

					tmpStr.Format(L"%d_health", l);
					script.GetInteger(i, tmpStr, pe->m_health.level[l]);

					tmpStr.Format(L"%d_physicalDamage", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].physicalDamage);

					tmpStr.Format(L"%d_physicalArmor", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].physicalArmor);

					tmpStr.Format(L"%d_electricalDamage", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].electricalDamage);

					tmpStr.Format(L"%d_electricalArmor", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].electricalArmor);

					tmpStr.Format(L"%d_splashRadius", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].splashRadius);

					tmpStr.Format(L"%d_stunValue", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].stunValue);

					tmpStr.Format(L"%d_stunTime", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].stunTime);

					tmpStr.Format(L"%d_maxRange", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].maxRange);

					tmpStr.Format(L"%d_minRange", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].minRange);

					tmpStr.Format(L"%d_fireRate", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].rate);

					tmpStr.Format(L"%d_goldenValue", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].goldenValue);

					tmpStr.Format(L"%d_actionCount", l);
					script.GetInteger(i, tmpStr, pe->m_attackLevel[l].actionCount);

					tmpStr.Format(L"%d_actionTime", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].actionTime);

					tmpStr.Format(L"%d_coolTime", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].coolTime);

					tmpStr.Format(L"%d_chance", l);
					script.GetFloat(i, tmpStr, pe->m_attackLevel[l].chance);

					str.Format(L"%d_component", l);
					if ( script.GetString(i, str, tmpStr) )
					{
						pe->Attach( ComponentManager::CreateComponentByTypeName( tmpStr ) );
					}
					
					str.Format(L"%d_bullet", l);
					if ( script.GetString(i, str, tmpStr) )
					{
						String::Copy( pe->m_attackLevel[l].bullet, 64, tmpStr );
					}

					pe->m_attackLevel[l].targetType = pe->m_curAttack.targetType;
				}
				pe->m_curAttack		= pe->m_curAttackLevel = pe->m_attackLevel[0];
				pe->m_health.imax	= pe->m_health.level[0];
				pe->m_health.icur	= pe->m_health.imax;

				pe->Attach( sx_new( com_GoldenTower ) );
				pe->Attach( sx_new( com_ShowRange ) );

			}
			else if ( tmpStr == L"Enemy" )
			{
				pe->m_partyCurrent = PARTY_ENEMY;
				pe->m_partyEnemies = PARTY_TOWER;

				if ( script.GetString(i, L"enemyType", tmpStr) )
				{
					if ( tmpStr == L"both" )
						pe->m_move.type = GMT_BOTH;
					else if ( tmpStr == L"air" )
						pe->m_move.type = GMT_AIR;
					else if ( tmpStr == L"ground" )
						pe->m_move.type = GMT_GROUND;
				}

				int matIndex = -1;
				if ( script.GetInteger(i, L"matIndex", matIndex) && pe->m_node )
				{
					msg_Mesh msgmesh(0, 0, 0, matIndex);
					pe->m_node->MsgProc( MT_MESH, &msgmesh );
				}

				script.GetInteger(i, L"gold", pe->m_cost[0]);
				script.GetFloat(i, L"xp", pe->m_experience);
				script.GetFloat(i, L"moveSpeed", pe->m_move.moveSpeed);
				script.GetFloat(i, L"animSpeed", pe->m_move.animSpeed);
				script.GetInteger(i, L"health", pe->m_health.level[0]);
				script.GetFloat(i, L"damage", pe->m_attackLevel[0].physicalDamage);
				script.GetFloat(i, L"physicalArmor", pe->m_attackLevel[0].physicalArmor);
				script.GetFloat(i, L"electricalArmor", pe->m_attackLevel[0].electricalArmor);
				script.GetFloat(i, L"fireRange", pe->m_attackLevel[0].maxRange);
				script.GetFloat(i, L"fireRate", pe->m_attackLevel[0].rate);
				script.GetFloat(i, L"splashRadius", pe->m_attackLevel[0].splashRadius);
				script.GetInteger(i, L"killPeople", pe->m_attackLevel[0].killPeople);
				if ( script.GetString(i, L"bullet", tmpStr) )
					String::Copy( pe->m_attackLevel[0].bullet, 64, tmpStr );

				pe->m_health.imax	= pe->m_health.level[0];
				pe->m_health.icur	= pe->m_health.imax;
				pe->m_curAttack		= pe->m_curAttackLevel = pe->m_attackLevel[0];

				if ( pe->m_curAttackLevel.maxRange < 1 ) pe->m_curAttackLevel.maxRange = 2;
				pe->Attach( sx_new( com_ShowRange ) );
			}
		}

		if ( script.GetString(i, L"weaponType", tmpStr) )
		{
			if ( tmpStr == L"machineGun" )
				pe->Attach( sx_new( com_weapon_MachineGun ) );
			else if ( tmpStr == L"cannon" )
				pe->Attach( sx_new( com_weapon_Cannon ) );
			else if ( tmpStr == L"snower" )
				pe->Attach( sx_new( com_weapon_Snower ) );
			else if ( tmpStr == L"flighter" )
				pe->Attach( sx_new( com_weapon_Flighter ) );
			else if ( tmpStr == L"groundLava" )
				pe->Attach( sx_new( com_weapon_GroundLava ) );
			else if ( tmpStr == L"luncher" )
				pe->Attach( sx_new( com_weapon_Luncher ) );
		}

		for ( int c = 0; c < 15; c++ )
		{
			str.Format( L"component%d", c );
			if ( script.GetString(i, str, tmpStr) )
			{
				pe->Attach( ComponentManager::CreateComponentByTypeName( tmpStr ) );
			}
		}
		pe->Attach( sx_new(com_HUD) );

		if ( pe->m_node )
		{
			msg_Mesh_Count msgMesh;
			pe->m_node->MsgProc( MT_MESH_COUNT, &msgMesh );
			for ( int m = 0; m<msgMesh.numMeshes; m++ )
			{
				sx::core::Mesh* mesh = (sx::core::Mesh*)msgMesh.meshes[m];

				if ( wcscmp( mesh->GetName(), L"body" )==0  )
					pe->m_mesh = mesh;

// 				sx::d3d::PShader shader = NULL;
// 				if ( sx::d3d::Shader::Manager::Exist( shader, mesh->GetActiveMaterial()->GetShader() ) )
// 				{
// 					for ( int sp=0; sp<shader->GetParamCount(); sp++ )
// 					{
// 						sx::d3d::PShaderParameter shaderParam = shader->GetParam( sp );
// 						if ( shaderParam && strcmp(shaderParam->GetDesc()->uiName , "clip height :")==0 )
// 						{
// 							shaderParam->SetFloat( 5.0f );
// 						}
// 					}
// 				}
			}

			if ( !pe->m_mesh )
				sxLog::Log_( L"    WARNING: no mesh 'body' found in entity %s ", pe->m_typeName.Text() );
		}
		else sxLog::Log_( L"    ERROR: no NODE found in entity %s ", pe->m_typeName.Text() );

	}	//	for (int i=0; i<script.GetObjectCount(); i++)


	if ( Config::GetData()->display_Debug == 3 )
	{
		String log = 
			L"Name" EXCEL_CHAR_SEPERATOR L"Health" EXCEL_CHAR_SEPERATOR L"Damage" EXCEL_CHAR_SEPERATOR 
			L"electricalArmor" EXCEL_CHAR_SEPERATOR L"physicalArmor" EXCEL_CHAR_SEPERATOR L"gold" EXCEL_CHAR_SEPERATOR 
			L"xp" EXCEL_CHAR_SEPERATOR L"moveSpeed" EXCEL_CHAR_SEPERATOR L"killPeople \r\n";
		//log.SetFloatPrecision(2);


		str = sx::sys::FileManager::Project_GetDir();
		str << L"enemies_excel.csv";
		sx::cmn::String_Save( log, str, false );

		log = L"Name_level" EXCEL_CHAR_SEPERATOR L"Health" EXCEL_CHAR_SEPERATOR L"cost" EXCEL_CHAR_SEPERATOR L"unlock XP" EXCEL_CHAR_SEPERATOR 
			L"electricalDamage" EXCEL_CHAR_SEPERATOR L"physicalDamage" EXCEL_CHAR_SEPERATOR L"electricalArmor" EXCEL_CHAR_SEPERATOR 
			L"physicalArmor" EXCEL_CHAR_SEPERATOR L"splashRadius" EXCEL_CHAR_SEPERATOR L"StunValue" EXCEL_CHAR_SEPERATOR 
			L"StunTime" EXCEL_CHAR_SEPERATOR L"range" EXCEL_CHAR_SEPERATOR L"fireRate\r\n";
		for ( int i=0; i < s_EntityTypes.Count(); i++ )
		{
			Entity* en = s_EntityTypes[i];
			if ( en->m_partyCurrent != PARTY_TOWER ) continue;

		}

		str = sx::sys::FileManager::Project_GetDir();
		str << L"towers_excel.csv";
		sx::cmn::String_Save( log, str, false );
	}
}

const Entity* EntityManager::GetTypeByIndex( int index )
{
	sx_callstack();

	if ( index < 0 || index >= s_EntityTypes.Count() ) return NULL;
	return s_EntityTypes[index];
}

const Entity* EntityManager::GetTypeByName( const WCHAR* name )
{
	sx_callstack();

	for (int i=0; i<s_EntityTypes.Count(); i++)
	{
		if ( s_EntityTypes[i]->m_typeName == name )
		{
			return s_EntityTypes[i];
		}
	}

	return NULL;
}

Entity* EntityManager::CreateEntityByTypeName( const WCHAR* name )
{
	sx_callstack_param(EntityManager::CreateEntityByTypeName(name=%s), name);

	for (int i=0; i<s_EntityTypes.Count(); i++)
	{
		if ( s_EntityTypes[i]->m_typeName == name )
		{
			Entity* pe = s_EntityTypes[i]->Clone();
			return pe;
		}
	}

	return NULL;
}

void EntityManager::AddEntity( Entity* pe )
{
	sx_callstack();

	if ( s_EntityMap.Insert(pe->m_ID, pe) )
	{
		s_EntityArr.PushBack(pe);
		pe->Initialize();
	}
}

void EntityManager::RemoveEntity( UINT id )
{
	sx_callstack();

	Entity* pe = NULL;
	if ( s_EntityMap.Find(id, pe) )
	{
		s_EntityMap.Remove(id);
		s_EntityArr.Remove(pe);
		pe->Finalize();
	}
}

void EntityManager::ClearEntities( void )
{
	sx_callstack();


	while ( s_EntityArr.Count() )
	{
		Entity* me = s_EntityArr[0];
		sx_delete_and_null( me );
	}
	s_EntityArr.Clear();
	s_EntityMap.Clear();
}

UINT EntityManager::GetEntityCount( void )
{
	sx_callstack();

	return s_EntityArr.Count();
}

Entity* EntityManager::GetEntityByIndex( const int index )
{
	sx_callstack_param(EntityManager::GetEntityByIndex(index=%d : count=%d), index, s_EntityArr.Count());
	sx_assert(index>=0 && index<s_EntityArr.Count());
	return s_EntityArr[index];
}


Entity* EntityManager::GetEntityByID( const UINT id )
{
	Entity* pe = NULL;
	s_EntityMap.Find(id, pe);
	return pe;
}

void EntityManager::Update( float elpsTime )
{
	sx_callstack();

	int n = s_EntityArr.Count();
	for (int i=0; i<n; i++)
	{
		Entity* pe = s_EntityArr[i];

		if ( pe->m_health.icur < 1 )
		{
			pe->SetState( ES_DIE );

			pe->m_health.deathTime -= elpsTime * 0.001f;
			if ( pe->m_health.deathTime <= 0 )
			{
				if ( pe->m_node )
				{
					msg_Mesh_Count msgMesh;
					pe->m_node->MsgProc( MT_MESH_COUNT, &msgMesh );
					for ( int m = 0; m<msgMesh.numMeshes; m++ )
					{
						sx::core::Mesh* mesh = (sx::core::Mesh*)msgMesh.meshes[m];

						float fclip = mesh->GetActiveMaterial()->GetFloat( 1 );
						fclip += elpsTime * 0.0015f;
						mesh->GetActiveMaterial()->SetFloat( 1, fclip );
					}

					msg_Mesh_Count msgMesh1( L"tarak" );
					pe->m_node->MsgProc( MT_MESH_COUNT, &msgMesh1 );
					for ( int m = 0; m<msgMesh1.numMeshes; m++ )
					{
						sx::core::Mesh* mesh = (sx::core::Mesh*)msgMesh1.meshes[m];

						float fclip = mesh->GetActiveMaterial()->GetFloat( 0 );
						if ( fclip > 0 )
						{
							fclip -= elpsTime * 0.0015f;
							mesh->GetActiveMaterial()->SetFloat( 0, fclip );
						}
					}
				}

				pe->m_health.deleteTime -= elpsTime * 0.001f;
				if ( pe->m_health.deleteTime <= 0 )
				{
// 					if ( Config::GetData()->display_Debug == 3 )
// 						sxLog::Log( L"INFO : entity %s deleted. current entity in the scene : %d", pe->m_typeName.Text(), s_EntityArr.Count()-1 );

					sx_delete_and_null( pe );
					i--;
					n--;
					continue;
				}
			}

			pe->m_health.icur = 0;
		}
		else
		{
			if ( pe->m_partyCurrent == PARTY_TOWER && pe->m_experience < 0.1f )
			{
				msg_Mesh_Count msgMesh( L"tarak" );
				pe->m_node->MsgProc( MT_MESH_COUNT, &msgMesh );
				for ( int m = 0; m<msgMesh.numMeshes; m++ )
				{
					sx::core::Mesh* mesh = (sx::core::Mesh*)msgMesh.meshes[m];

					float fclip = mesh->GetActiveMaterial()->GetFloat( 0 );
					if ( fclip < 1 )
					{
						fclip += elpsTime * 0.0015f;
						mesh->GetActiveMaterial()->SetFloat( 0, fclip );
					}
				}
			}
		}

		pe->Update(elpsTime);
	}
}

void EntityManager::MsgProc( UINT RecieverID, UINT msg, void* data )
{
	sx_callstack_param(EntityManager::MsgProc(RecieverID=%d, msg=%d), RecieverID, msg);

	switch ( msg )
	{
	case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_LEVEL_LOAD

	case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
		{						//////////////////////////////////////////////////////////////////////////
			ClearEntities();
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
			Entity::SetSelected(NULL);
		}
		break;	//	GMT_GAME_RESETING

	case GMT_GAME_RESET:
		{
			ClearEntities();

			// reload types in development mode
			if ( Config::GetData()->display_Debug == 3 )
			{
				ClearTypes();
			}

			// reload types in development mode
			if ( Config::GetData()->display_Debug == 3 )
			{
				LoadTypes( NULL );
			}
		}
		break;
	}

	if ( RecieverID )
	{
		Entity* pe = NULL;
		if ( s_EntityMap.Find(RecieverID, pe) )
			pe->MsgProc(msg, data);
	}
	else
	{
		for (int i=0; i<s_EntityArr.Count(); i++)
			s_EntityArr[i]->MsgProc(msg, data);
	}
}
