/********************************************************************
	created:	2012/07/22
	filename: 	ComponentManager.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain component manager to manage components
*********************************************************************/
#ifndef GUARD_ComponentManager_HEADER_FILE
#define GUARD_ComponentManager_HEADER_FILE

#include "GameTypes.h"

class Component;

//////////////////////////////////////////////////////////////////////////
//	MANAGER
//////////////////////////////////////////////////////////////////////////
class ComponentManager
{
public:

	//! cleanup types
	static void ClearTypes( void );

	//! load types from file "components.txt"
	static void LoadTypes( void );

	/*
	create component by specified name.
	this function use names which loaded by ComponentManager::LoadTypes()
	return simple component if name = null
	return null for invalid name
	*/
	static Component* CreateComponentByTypeName( const WCHAR* name );

	//! process incoming message
	static void MsgProc( UINT msg, void* data );

};

#endif	//	GUARD_ComponentManager_HEADER_FILE