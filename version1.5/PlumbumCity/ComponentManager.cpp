#include "ComponentManager.h"
#include "Component.h"
#include "Scripter.h"
#include "com_HealthModifier.h"
#include "com_ParamModifier.h"
#include "com_Turbo.h"
#include "GameConfig.h"


//////////////////////////////////////////////////////////////////////////
//
arrayPComponent		s_componentTypes;

void ComponentManager::ClearTypes( void )
{
	sx_callstack_push(ComponentManager::ClearTypes());

	for ( int i=0; i<s_componentTypes.Count(); i++ )
	{
		Component* com = s_componentTypes[i];
		sx_delete_and_null( com );
	}
	s_componentTypes.Clear();

}

void ComponentManager::LoadTypes( void )
{
	sx_callstack_push(ComponentManager::LoadTypes());


	String str = sx::sys::FileManager::Project_GetDir();
	str << L"components.txt";

	Scripter script;
	script.Load( str );

	for (int i=0; i<script.GetObjectCount(); i++)
	{
		str512 tmpStr;
		str512 typeName;

		if ( !script.GetString(i, L"Type", typeName) ) continue;
		if ( !script.GetString(i, L"Name", tmpStr) ) continue;

		Component* com = NULL;
		if ( typeName == L"HealthModifier" )
		{
			com_HealthModifier* hcom = sx_new( com_HealthModifier );
			hcom->m_name = tmpStr.Text();
			
			script.GetInteger( i, L"value", hcom->m_value );
			script.GetInteger( i, L"count", hcom->m_count );
			script.GetFloat( i, L"coolTime", hcom->m_coolTime );
			script.GetString( i, L"node", hcom->m_nodeName );		

			com = hcom;
		}
		else if ( typeName == L"ParamModifier" )
		{
			com_ParamModifier* pcom = sx_new( com_ParamModifier );
			pcom->m_name = tmpStr.Text();

			script.GetString(	i, L"node",				pcom->m_nodeName		);
			script.GetFloat(	i, L"coolTime",			pcom->m_coolTime		);
			script.GetFloat(	i, L"radius",			pcom->m_radius			);
			script.GetFloat(	i, L"moveScale",		pcom->m_moveScale		);
			script.GetInteger(	i, L"animIndex",		pcom->m_animIndex		);
			script.GetFloat(	i, L"modifyTime",		pcom->m_modifyTime		);
			script.GetFloat(	i, L"speed",			pcom->m_speed			);
			script.GetFloat(	i, L"physicalArmor",	pcom->m_physicalArmor	);
			script.GetFloat(	i, L"electricalArmor",	pcom->m_electricalArmor	);
			script.GetInteger(	i, L"health",			pcom->m_health			);
			script.GetFloat(	i, L"height",			pcom->m_offsetY			);

			com = pcom;
		}
		else if ( typeName == L"Turbo" )
		{
			com_Turbo* pcom = sx_new( com_Turbo );
			pcom->m_name = tmpStr.Text();

			script.GetString(	i, L"node",			pcom->m_nodeName	);
			script.GetFloat(	i, L"speed",		pcom->m_speed		);
			script.GetFloat(	i, L"speedTime",	pcom->m_speedTime	);
			script.GetInteger(	i, L"turboCount",	pcom->m_turboCount	);

			com = pcom;
		}
		else
		{
			sxLog::Log( L"ERROR : Invalid component type : %s ", typeName.Text() );
			continue;
		}

		s_componentTypes.PushBack( com );

		if ( Config::GetData()->display_Debug == 3 )
			sxLog::Log( L"Component registered with type '%s' and name '%s'", typeName.Text(), com->m_name.Text() );
	
	}
}

Component* ComponentManager::CreateComponentByTypeName( const WCHAR* name )
{
	sx_callstack_push(ComponentManager::CreateComponentByTypeName(name=%s), name);

	for (int i=0; i<s_componentTypes.Count(); i++)
	{
		if ( s_componentTypes[i]->m_name == name )
		{
			return s_componentTypes[i]->Clone();
		}
	}

	sxLog::Log( L"WARNING : no component found in manager with name '%s'", name );

	return NULL;
}

void ComponentManager::MsgProc( UINT msg, void* data )
{
	sx_callstack_push(ComponentManager::MsgProc(msg=%d), msg);

	switch ( msg )
	{
	case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_LEVEL_LOAD

	case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
		{						//////////////////////////////////////////////////////////////////////////
			
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

		}
		break;	//	GMT_GAME_RESETING

	case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
		{						//////////////////////////////////////////////////////////////////////////
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


