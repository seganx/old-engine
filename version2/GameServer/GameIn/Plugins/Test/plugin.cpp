#include <Windows.h>
#include <stdio.h>

#include "plugin.h"
#include "../../request.h"

int gi_get_command(char* command)
{
	int i = 0;
	while (true)
	{
		if ((command[i++] = getchar()) == 10)
		{
			command[--i] = 0;
			return i;
		}
	};
}

int __stdcall process_msg(int msg, void* data)
{
	switch (msg)
	{
		case GAMEIN_PLUGIN_INITIALIZE:
			return 1;

		case GAMEIN_PLUGIN_FINALIZE:
			return 1;

		case GAMEIN_PLUGIN_NAME:
			strcpy_s((char*)data, 64, "First Plugin");
			return 1;

		case GAMEIN_PLUGIN_DESC:
			strcpy_s((char*)data, 256, "This is a test plugin");
			return 1;

		case GAMEIN_PLUGIN_PRIORITY:
			return 2;

		case GAMEIN_PLUGIN_COMMAND:
			if (data)
			{
				char* cmd = (char*)data;
				if (strcmp( cmd, "plugin test" ) == 0)
				{
					printf("enter the command for plugin test: ");
					char command[256] = {0};
					int len = gi_get_command(command);
					printf("Command handled in plugin test: %s\n", command);
				}
				
			}
			return 1;

		case GAMEIN_PLUGIN_REQUEST:
			if (data)
			{
				char* msg = "Here is test plugin!\n";
				Request* req = (Request*)data;
				req->send(req->connection, msg, strlen(msg));
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
