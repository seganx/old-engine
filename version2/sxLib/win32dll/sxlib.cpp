// sxlib.cpp : Defines the entry point for the DLL application.
//

#include "../Lib.h"
#include <Windows.h>


//! initialize internal library
extern void sx_lib_initialize( void );

//! finalize internal library
extern void sx_lib_finalize( void );



//////////////////////////////////////////////////////////////////////////
//	DLL main function
BOOL APIENTRY DllMain ( HMODULE hModule, dword  ul_reason_for_call, LPVOID lpReserved )
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		sx_lib_initialize();

		break;

	case DLL_PROCESS_DETACH:

		sx_lib_finalize();

		break;
	}
	return TRUE;
}

