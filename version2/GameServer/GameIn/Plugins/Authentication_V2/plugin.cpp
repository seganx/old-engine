#include <windows.h>
#include <stdio.h>

#include "../plugin.h"
#include "../../request.h"
#include "../../imports.h"

#include "authenticator.h"

int __stdcall process_msg(int msg, void* data)
{
	switch (msg)
	{
		case GAMEIN_PLUGIN_INITIALIZE:
			return 1;

		case GAMEIN_PLUGIN_FINALIZE:
			return 1;

		case GAMEIN_PLUGIN_NAME:
			strcpy_s((char*)data, 64, "Authentication v2");
			return 1;

		case GAMEIN_PLUGIN_DESC:
			strcpy_s((char*)data, 256, "Authenticate any request comes from client.");
			return 1;

		case GAMEIN_PLUGIN_PRIORITY:
			return 0;

		case GAMEIN_PLUGIN_COMMAND:
			return 1;

		case GAMEIN_PLUGIN_REQUEST:
			if (data)
			{
				Request* req = (Request*)data;
				if (sx_str_cmp(req->uri, "/authen") == 0)
				{
					uint userdata = sx_str_get_value_uint(req->data, "user_data", 0);
					if ( userdata )
						return authen_handle_request(req, userdata);
					else
						return 1;
				}
				else return authen_handle_access(req);
			}
			return 0;

	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//	DLL main function
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			authen_load_config("authen_v2.config");
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;
	}
	return TRUE;
}
