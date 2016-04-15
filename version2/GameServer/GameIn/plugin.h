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

#define GAMEIN_PLUGIN_
#define GAMEIN_PLUGIN_INITIALIZE	1	//! initialize parameters of plugin
#define GAMEIN_PLUGIN_FINALIZE		2	//! finalize the loaded plugin
#define GAMEIN_PLUGIN_NAME			3	//! get name of the plugin. data is pointer to char string as destination
#define GAMEIN_PLUGIN_DESC			4	//! get description of the plugin. data is pointer to char string as destination
#define GAMEIN_PLUGIN_PRIORITY		5	//! return priority of plugin. data is null
#define GAMEIN_PLUGIN_COMMAND		6	//! user command from console. data is pointer to char string
#define GAMEIN_PLUGIN_REQUEST		7	//! handle the request. data is pointer to RequestObject. NOTE: This function called in a separate thread so it must be thread-safe


typedef int(_stdcall * pfunc)(int msg, void* data);

//! represent loaded plugin as a class
class Plugin
{
public:
	Plugin();
	~Plugin();

	//! load library from specified file to the Plugin object. return less that 1 if failed loading
	int load(const wchar* filename);

	//!	process the message
	int process_msg(GAMEIN_PLUGIN_ int msg, void* data);
	
public:
	char	m_name[64];		//! name of the plugin
	char	m_desc[256];	//! description of the module
	bool	m_active;		//! status of the plugin. Deactivated plugin will not call to process messages
	int		m_priority;		//! plugin priority indicates when this plugin handle request
	handle	m_module;		//! module handle
	pfunc	m_func;			//! pointer to the main function is plugin
};

#endif // DEFINED_plugin

