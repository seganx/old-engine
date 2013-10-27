#include "../sxEngine.h"

#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
//	MAIN ENGINE DLL
//////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain ( HMODULE hModule, dword  ul_reason_for_call, LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		break;
	case DLL_PROCESS_DETACH:

		break;
	}
	return TRUE;
}

