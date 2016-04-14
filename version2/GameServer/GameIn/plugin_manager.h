/********************************************************************
	created:	2016/4/13
	filename: 	plugin_manager.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the plugins manager which is responsible
	to manage all plugins
	*********************************************************************/
#ifndef DEFINED_plugin_manager
#define DEFINED_plugin_manager

#include "imports.h"

//! forward declaration
class Plugin;

//! Plugin manager is responsible to manage all plugins. The only one function is multi threaded support
class PluginMan
{
public:
	PluginMan();
	~PluginMan();

	//! add a plugin to system
	Plugin* Add(const wchar* filename);

	//! remove a plugin from system
	void Delete(const wchar* name);

	//! remove a plugin from system
	void RemoveAt(const int index);

	//! return plugin specified by index. return null for invalid index 
	Plugin* Get(const int index);

	//! fill out the array of activated plugins. this function is multi threaded safe
	void GetPlugins(Plugin* dest[], const int destSize);

private:
	Mutex			m_mutex;			//	make this object multi threaded safe
	Array<Plugin*>	m_plugins;			//	array of plugins. sorted by priority
	
};

#endif // DEFINED_plugin_manager

