/********************************************************************
	created:	2016/4/10
	filename: 	plugin.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contains a wrapped class as loaded plugins
*********************************************************************/
#ifndef DEFINED_plugin
#define DEFINED_plugin

#include "imports.h"

//! represent loaded plugin as a class
class Plugin
{
public:
	Plugin();
	~Plugin();

	//! load library from specified file to the Plugin object. return less that 1 if failed loading
	int Load(const wchar* filename);

public:	
	//! initialize parameters of plugin
	int(__cdecl * Initialize)(void) = 0;

	//! finalize the loaded plugin
	int(__cdecl * Finalize)(void) = 0;

	//! reset plugin content
	int(__cdecl * Reset)(void) = 0;

	//! handle the request. This function called in a separate thread so it must be thread-safe
	int(__cdecl * HandleRequest)(class RequestObject* request) = 0;

public:
	wchar	m_name[128];		//! name of the plugin
	int		m_priority = 0;		//! plugin priority indicates when this plugin handle request
	handle	m_module = null;	//! module handle
};

#endif // DEFINED_plugin

