// sxlib.cpp : Defines the entry point for the DLL application.
//

#include "../Lib.h"
#include <Windows.h>


//! initialize internal library
extern void sx_lib_initialize( void );

//! finalize internal library
extern void sx_lib_finalize( void );


WORD defaultGammaArray[3][256];
bool applyGammaArray = false;

//////////////////////////////////////////////////////////////////////////
//	DLL main function
BOOL APIENTRY DllMain ( HMODULE hModule, dword  ul_reason_for_call, LPVOID lpReserved )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		sx_lib_initialize();

		{
			HDC hdc = GetDC(NULL);
			if ( hdc )
			{
				if ( GetDeviceGammaRamp(hdc, defaultGammaArray) == TRUE )
				{
					WORD gammaArray[3][256];
					WORD wBrightness = 128;

					for (int iIndex = 0; iIndex < 256; iIndex++)
					{
						int iArrayValue = iIndex * (wBrightness + 128);

						if (iArrayValue > 65535)
							iArrayValue = 65535;

						gammaArray[0][iIndex] = 
							gammaArray[1][iIndex] = 
							gammaArray[2][iIndex] = (WORD)iArrayValue;

					}

					if ( SetDeviceGammaRamp(hdc, gammaArray) == TRUE )
						applyGammaArray = true;
				}
				ReleaseDC(NULL, hdc);
			}
		}

		break;

	case DLL_PROCESS_DETACH:

		if ( applyGammaArray )
		{
			HDC hdc = GetDC(NULL);
			if ( hdc )
			{
				SetDeviceGammaRamp(hdc, defaultGammaArray);
				ReleaseDC(NULL, hdc);
			}
		}

		sx_lib_finalize();

		break;
	}
	return TRUE;
}

