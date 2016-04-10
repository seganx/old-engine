#include <Windows.h>
#include "plugin.h"


PLUGINAPI int FUNCCONV get_priority(void)
{
	return 32;
}

PLUGINAPI int FUNCCONV initialize(void)
{
	return 1;
}

PLUGINAPI int FUNCCONV finalize(void)
{
	return 1;
}

PLUGINAPI int FUNCCONV reset(void)
{
	return 1;
}

PLUGINAPI int FUNCCONV handle_request(class RequestObject* request)
{
	return 1;
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
