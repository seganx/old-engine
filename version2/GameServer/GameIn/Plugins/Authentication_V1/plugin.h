/********************************************************************
	created:	2016/4/10
	filename: 	plugin.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		Base of all plugins interface
*********************************************************************/
#ifndef DEFINED_plugin
#define DEFINED_plugin

#define GAMEIN_PLUGIN_INITIALIZE	1	//! initialize parameters of plugin
#define GAMEIN_PLUGIN_FINALIZE		2	//! finalize the loaded plugin
#define GAMEIN_PLUGIN_NAME			3	//! get name of the plugin. data is pointer to char string
#define GAMEIN_PLUGIN_DESC			4	//! get description of the plugin. data is pointer to char string
#define GAMEIN_PLUGIN_PRIORITY		5	//! get description of the plugin. data is pointer to int
#define GAMEIN_PLUGIN_COMMAND		6	//! user command from console. data is pointer to char string
#define GAMEIN_PLUGIN_REQUEST		7	//! handle the request. data is pointer to RequestObject. NOTE: This function called in a separate thread so it must be thread-safe

//!	process the message
extern "C"
{
	__declspec(dllexport) int __stdcall process_msg(int msg, void* data);
}

#endif // DEFINED_plugin

