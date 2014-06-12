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

	sxLog::Log_(L"SeganX Engine 0.1.0.1 \r\n\r\n");

	SystemInfo sysinfo;
	sx::sys::GetSystemInfo(sysinfo);
	if (sysinfo.LidPresent)		sxLog::Log_(L"------- Running on Laptop PC --------\r\n\r\n");
	else						sxLog::Log_(L"------- Running on Desktop PC -------\r\n\r\n");

	OSInfo osinfo;
	sx::sys::GetOSInfo(osinfo);
	sxLog::Log_(L"%S \r\n\r\n CPU information:\r\n", osinfo.Description);
	sxLog::Log_(L"\tBrand :\t\t%S\r\n", sysinfo.CPU.BrandName);
	sxLog::Log_(L"\tClock :\t\t\t%d MHz\r\n", sysinfo.CPU.Clock);
	sxLog::Log_(L"\tCache :\t\t\t%d KB\r\n", sysinfo.CPU.Cache);
	sxLog::Log_(L"\tNumber of core(s):\t%d\r\n", sysinfo.CPU.ProcessorCount);
	sxLog::Log_(L"\tFeatures:\t\t");
	if (sysinfo.CPU.mmx) sxLog::Log_(L"MMX ");
	if (sysinfo.CPU.sse) sxLog::Log_(L"SSE ");
	if (sysinfo.CPU.sse2) sxLog::Log_(L"SSE2 ");
	if (sysinfo.CPU.now3d) sxLog::Log_(L"3DNow ");

	sxLog::Log_(L"\r\n\r\nPhysical memory information:\r\n");
	sxLog::Log_(L"\tTotal Memory:\t\t%u KB\r\n",	sysinfo.MemorySize);
	sxLog::Log_(L"\tAvailable Memory :\t%u KB\r\n",	sx::sys::GetAvailableMemory());
}


void SEGAN_API sx_engine_init()
{
	InitLogger();
	sx::core::Settings::Initialize();
}

void SEGAN_API sx_engine_finit()
{
	sxLog::Log_(L"\r\nShutting down SeganX ...\r\n");

	//  finalize some remain things
	sx::core::Renderer::Finalize();
	sx::core::Scene::Finalize();
	sx::snd::Device::Destroy();
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



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
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


#ifdef _MANAGED
#pragma managed(pop)
#endif

