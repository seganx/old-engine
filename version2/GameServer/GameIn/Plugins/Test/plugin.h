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

#define PLUGINAPI			__declspec(dllexport)
#define FUNCCONV			__cdecl
//#define FUNCCONV			__stdcall

extern "C"
{
	//! initialize parameters of plugin
	PLUGINAPI int FUNCCONV get_priority(void);

	//! initialize parameters of plugin
	PLUGINAPI int FUNCCONV initialize(void);

	//! finalize the loaded plugin
	PLUGINAPI int FUNCCONV finalize(void);

	//! reset plugin content
	PLUGINAPI int FUNCCONV reset(void);

	//! handle the request. This function called in a separate thread so it must be thread-safe
	PLUGINAPI int FUNCCONV handle_request(class RequestObject* request);
};


#endif // DEFINED_plugin

