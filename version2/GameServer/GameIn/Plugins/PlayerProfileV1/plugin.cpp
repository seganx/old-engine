#include <Windows.h>
#include <stdio.h>

#include "plugin.h"
#include "database.h"


int __stdcall process_msg(int msg, void* data)
{
	switch (msg)
	{
		case GAMEIN_PLUGIN_INITIALIZE:
			return 1;

		case GAMEIN_PLUGIN_FINALIZE:
			return 1;

		case GAMEIN_PLUGIN_NAME:
			strcpy_s((char*)data, 64, "Player Profile V1");
			return 1;

		case GAMEIN_PLUGIN_DESC:
			strcpy_s((char*)data, 256, "Retrieve player profile from database");
			return 1;

		case GAMEIN_PLUGIN_PRIORITY:
			return 10;

		case GAMEIN_PLUGIN_COMMAND:
			if (data)
			{
				char* cmd = (char*)data;
				printf("Command received to profile plugin : %s\n", cmd);
			}
			return 1;

		case GAMEIN_PLUGIN_REQUEST:
			if (data)
			{
				char* msg = "Here is profile plugin!\n";

				Request* req = (Request*)data;
				req->send(req->connection, msg, sx_str_len(msg));
			}
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
