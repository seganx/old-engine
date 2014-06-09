// sxEngine.cpp : Defines the entry point for the DLL application.
//

#include "sxEngine.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

void InitLogger(void)
{
	String logfile = sx::sys::GetAppFolder();
	logfile.MakePathStyle();
	logfile << L"sxLog.txt";

	sxLog::SetFileName(logfile);
	sxLog::SetMode(LM_FILE | LM_WINDOW);

	String sysString = L"SeganX Engine 0.1.0.1 \r\n\r\n";

	SystemInfo sysinfo;
	sx::sys::GetSystemInfo(sysinfo);
	if (sysinfo.LidPresent)		sysString << L"------- Running on Laptop PC --------\r\n\r\n";
	else						sysString << L"------- Running on Desktop PC -------\r\n\r\n";

	OSInfo osinfo;
	sx::sys::GetOSInfo(osinfo);	
// 	sysString << osinfo.Description	<< L"\r\n\r\n"					<<
// 		L"CPU information:\r\n"		<<
// 		L"\tBrand :\t\t"			<< sysinfo.CPU.BrandName		<< L"\r\n"		<<
// 		L"\tClock :\t\t\t"			<< sysinfo.CPU.Clock			<< L" MHz\r\n"	<<
// 		L"\tCache :\t\t\t"			<< sysinfo.CPU.Cache			<< L" KB\r\n"	<<
// 		L"\tNumber of core(s):\t"	<< sysinfo.CPU.ProcessorCount	<< L"\r\n"		<<
// 		L"\tFeatures:\t\t";
	if (sysinfo.CPU.mmx) sysString << L"MMX ";
	if (sysinfo.CPU.sse) sysString << L"SSE ";
	if (sysinfo.CPU.sse2) sysString << L"SSE2 ";
	if (sysinfo.CPU.now3d) sysString << L"3DNow ";

// 	sysString << L"\r\n\r\n"				<<
// 		L"Physical memory information:\r\n"	<<
// 		L"\tTotal Memory:\t\t"				<< (int)sysinfo.MemorySize				<< L" KB\r\n"	<<
// 		L"\tAvailable Memory :\t"			<< (int)sx::sys::GetAvailableMemory()	<< L" KB\r\n";

	sxLog::Log_(sysString);
}

class Engine_internal
{
public:

	//! initialize the engine
	static void Initialize(void)
	{
		InitLogger();
		sx::core::Settings::Initialize();
	}

	//! finalize the engine
	static void Finalize(void){
		sxLog::Log_(L"\r\nShutting down SeganX ...\r\n");

		sx::io::Input::Finalize();
		sx::core::Terrain::Manager::ClearrAll();
		sx::d3d::Device3D::Destroy();
		sx::gui::Font::Manager::ClearAll();
		sx::d3d::Shader::Manager::ClearAll();
		sx::d3d::Texture::Manager::ClearAll();
		sx::d3d::Geometry::Manager::ClearAll();
		sx::d3d::Animation::Manager::ClearAll();
		
		sx::snd::SoundData::Manager::ClearAll();

		sx::sys::TaskManager::Finalize();
		sxLog::Log( L"SeganX terminated." );
	}
};



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		
		Engine_internal::Initialize();

		break;
	case DLL_PROCESS_DETACH:
		
		//Engine_internal::Finalize();

		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

