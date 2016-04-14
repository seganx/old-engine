#include <Windows.h>
#include "plugin.h"


int __stdcall process_msg(int msg, void* data)
{
	switch (msg)
	{
		case GAMEIN_PLUGIN_INITIALIZE: 
			return 1;

		case GAMEIN_PLUGIN_FINALIZE: 
			return 1;

		case GAMEIN_PLUGIN_NAME:
			strcpy((char*)data, "First Plugin");
			return 1;

		case GAMEIN_PLUGIN_DESC:
			strcpy((char*)data, "This is a test plugin");
			return 1;

		case GAMEIN_PLUGIN_PRIORITY: 
			return 2;

		case GAMEIN_PLUGIN_RESRT: 
			return 1;
			
		case GAMEIN_PLUGIN_REQUEST: 
			return 1;

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
