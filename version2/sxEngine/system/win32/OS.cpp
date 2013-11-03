#if defined(_WIN32)

#include "../OS.h"
#include "Win6.h"

#include <time.h>
#include <intrin.h>
#include <powrprof.h>
#include <ShlObj.h>
#include <mmsystem.h>

#pragma comment( lib, "winmm.lib" )

#ifndef POWER_PROFILER_LOADED
#define POWER_PROFILER_LOADED
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "powrprof.lib")
#endif

#ifndef PROCESSOR_POWER_INFORMATION
// missing Windows processor power information struct
typedef struct PROCESSOR_POWER_INFORMATION {
	ULONG  Number;
	ULONG  MaxMhz;
	ULONG  CurrentMhz;
	ULONG  MhzLimit;
	ULONG  MaxIdleState;
	ULONG  CurrentIdleState;
} *PPROCESSOR_POWER_INFORMATION;
#endif

#define MAX_TMP_COUNT	4
#define MAX_CS_COUNT	8
#define MAX_CPU_COUNT	32
#define HIDIGIT(d)		((byte)(d>>4))
#define LODIGIT(d)		((byte)(d&0x0F))


bool				s_screensaverActivity = false;


//////////////////////////////////////////////////////////////////////////
//		SORTING HELPER FUNCTIONS		//
SEGAN_INLINE sint _sort_fils_by_name( const FileInfo& info1, const FileInfo& info2 )
{
	if (*info1.name == null) return 1;
	if (*info2.name == null) return -1;

	//  make names lower case
	wchar s1[256], s2[256];
	for ( uint i=0; i<256; ++i ) s1[i] = sx_str_lower( info1.name[i] );
	for ( uint i=0; i<256; ++i ) s2[i] = sx_str_lower( info2.name[i] );
	return ( wcscmp( s1, s2 ) );
}

SEGAN_INLINE sint _sort_fils_by_type( const FileInfo& info1, const FileInfo& info2 )
{
	if (info1.flag & FILE_ATTRIBUTE_DIRECTORY) return 1;
	if (info2.flag & FILE_ATTRIBUTE_DIRECTORY) return -1;
	if (*info1.type == null) return 1;
	if (*info2.type == null) return -1;
	
	//  make type names lower case
	wchar s1[32], s2[32];
	for ( uint i=0; i<32; ++i ) s1[i] = sx_str_lower( info1.type[i] );
	for ( uint i=0; i<32; ++i ) s2[i] = sx_str_lower( info2.type[i] );
	return ( wcscmp( s1, s2 ) );
}

SEGAN_INLINE sint _sort_fils_by_date( const FileInfo& info1, const FileInfo& info2 )
{
	uint64 res1 = info1.modified.second + info1.modified.minute * 60 + info1.modified.hour * 3600 + 
		info1.modified.day * 43200 + info1.modified.month * 1296000 + ( info1.modified.year - 1800 ) * 15552000;

	uint64 res2 = info2.modified.second + info2.modified.minute * 60 + info2.modified.hour * 3600 +
		info2.modified.day * 43200 + info2.modified.month * 1296000 + ( info2.modified.year - 1800 ) * 15552000;

	return ( res1 < res2 ) ? -1 : ( res1 == res2 ) ? 0 : 1;
}

SEGAN_INLINE sint _sort_fils_by_size( const FileInfo& info1, const FileInfo& info2 )
{
	return ( info1.size < info2.size ) ? -1 : ( info1.size == info2.size ) ? 0 : 1;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//    ADITIONAL HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////
void _systemtime_to_timestruct( SYSTEMTIME& sysTime, TimeStruct& timeStruct )
{
	timeStruct.year			= sysTime.wYear;
	timeStruct.month		= sysTime.wMonth;
	timeStruct.dayOfWeek	= sysTime.wDayOfWeek;
	timeStruct.day			= sysTime.wDay;
	timeStruct.hour			= sysTime.wHour;
	timeStruct.minute		= sysTime.wMinute;
	timeStruct.second		= sysTime.wSecond;
	timeStruct.milliSeconds	= sysTime.wMilliseconds;
}


void _get_files_in_dir_x( const wchar* root, const wchar* path, const String& exten, FileInfoArray& fileList )
{
	if ( !root || !sx_os_dir_exist( root ) ) return;

	String filePath = root;
	filePath.make_path_style();
	filePath << path;
	filePath.make_path_style();
	filePath << L"*.*";

	// find the first file in the directory.
	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile( *filePath, &ffd );
	if ( hFind == INVALID_HANDLE_VALUE )
		return;
	
	filePath = path;
	filePath.make_path_style();

	// List all the files in the directory with some info about them.
	bool checkExten = exten.length() && exten != L"*.*";
	FILETIME		ft;
	LARGE_INTEGER	filesize;
	FileInfo		finfo;
	do
	{
		bool fileIsDir = ( ffd.cFileName[0] == '.' ) && ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY );
		if ( fileIsDir || ( ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) || ( ffd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY ) )
			continue;

		bool extenAccepted = true;
		if ( checkExten )
		{
			extenAccepted = exten.find( sx_str_extract_extension( ffd.cFileName ) ) > -1;
		}

		if ( extenAccepted )
		{
			sx_str_copy( finfo.name, 256, filePath );
			sx_str_copy( finfo.name + filePath.length(), 256 - filePath.length(), ffd.cFileName );

			if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				sx_str_copy( finfo.type, 32, L"<DIR>" );
			else
			{
				const wchar* ext = sx_str_extract_extension( ffd.cFileName );
				for ( uint i=0; i<32; ++i ) finfo.type[i] = sx_str_lower( ext[i] );
			}

			filesize.LowPart  = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;

			finfo.flag		= ffd.dwFileAttributes;
			finfo.size		= filesize.QuadPart;

			SYSTEMTIME sysTime;

			FileTimeToLocalFileTime( &ffd.ftCreationTime, &ft );
			FileTimeToSystemTime( &ft, &sysTime );
			_systemtime_to_timestruct( sysTime, finfo.created );

			FileTimeToLocalFileTime( &ffd.ftLastWriteTime, &ft );
			FileTimeToSystemTime( &ft, &sysTime );
			_systemtime_to_timestruct( sysTime, finfo.modified );

			fileList.push_back( finfo );
		}

		if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			_get_files_in_dir_x (root, finfo.name, exten, fileList );
	}
	while ( FindNextFile( hFind, &ffd ) );

	FindClose( hFind );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void sx_os_initialize( void )
{
	//////////////////////////////////////////////////////////////////////////
	/*	by HUMUS : http://www.humus.name/index.php?page=3D
	Force the main thread to always run on cpu 0.
	This is done because on some systems QueryPerformanceCounter returns a bit different counter values
	on the different CPUs (contrary to what it's supposed to do), which can cause negative frame times
	if the thread is scheduled on the other cpu in the next frame. This can cause very jerky behavior and
	appear as if frames return out of order.
	*/
	SetThreadAffinityMask( GetCurrentThread(), 1 );

	// prevent windows to show it's messages
	SetErrorMode( SEM_FAILCRITICALERRORS );

	//	get screen saver activity
	SystemParametersInfo( 16, 0, &s_screensaverActivity, 0 );

}

void sx_os_finalize( void )
{

}

SEGAN_INLINE void sx_os_sleep( const uint miliseconds )
{
	Sleep( (DWORD)miliseconds );
}


SEGAN_ENG_API double sx_os_get_timer( void )
{
	static LARGE_INTEGER qFerquency;
	static LARGE_INTEGER qCounter;
	static BOOL	useHighRes = QueryPerformanceFrequency( &qFerquency );

	if ( useHighRes && QueryPerformanceCounter( &qCounter ) )
	{
		return ( ( (double)qCounter.LowPart / (double)qFerquency.LowPart ) * 1000.0 );
	}
	else
	{
		return (double)timeGetTime();
	}
}

const wchar* sx_os_get_date( void )
{
	static wchar res[32];
	res[0] = 0;
	time_t rawTime;
	time( &rawTime );
	struct tm timeInfo;
	localtime_s( &timeInfo, &rawTime );
	wcsftime( res, 32, L"%Y/%m/%d", &timeInfo );
	return res;
}

const wchar* sx_os_get_time( void )
{
	static wchar res[32];
	res[0] = 0;
	time_t rawTime;
	time( &rawTime );
	struct tm timeInfo;
	localtime_s( &timeInfo, &rawTime );
	wcsftime( res, 32, L"%H:%M:%S", &timeInfo );
	return res;
}

const wchar* sx_os_get_time_stamp( void )
{
	static wchar res[32];
	res[0] = 0;
	time_t rawTime;
	time( &rawTime );
	struct tm timeInfo;
	localtime_s( &timeInfo, &rawTime );
	wcsftime( res, 32, L"%Y/%m/%d %H:%M:%S", &timeInfo );
	return res;
}

void sx_os_copy_text_to_clipboard( const wchar* text )
{
	if ( !text ) return;
	sx_callstack();

	sint memSize = ( (sint)wcslen(text) + 1 ) * sizeof(wchar);
	
	HGLOBAL memHandle = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, memSize );
	if ( !memHandle ) return;

	memcpy( GlobalLock(memHandle), text, memSize );
	GlobalUnlock( memHandle );

	if ( OpenClipboard( 0 ) )
	{
		EmptyClipboard();
		SetClipboardData( CF_UNICODETEXT, memHandle );
		CloseClipboard();
	}
	else GlobalFree( memHandle );
}

const wchar* sx_os_paste_text_from_clipboard( void )
{
	sx_callstack();

	static String res;
	res.clear();

	bool isUnicodeText = IsClipboardFormatAvailable(CF_UNICODETEXT) > 0;
	bool isText = IsClipboardFormatAvailable(CF_TEXT) > 0;
	if ( (isUnicodeText || isText) && OpenClipboard(0) )
	{
		if ( isUnicodeText )
		{
			HGLOBAL memHandel = GetClipboardData( CF_UNICODETEXT );
			if ( memHandel )
			{
				sint memSize = (sint)GlobalSize( memHandel );
				wchar* c = (wchar*)mem_alloc( memSize );
				memcpy(c, GlobalLock(memHandel), memSize);
				GlobalUnlock( memHandel );
				res = c;
				mem_free( c );
			}
		}
		else if ( isText )
		{
			HGLOBAL memHandel = GetClipboardData( CF_TEXT );
			if ( memHandel )
			{
				sint memSize = (sint)GlobalSize( memHandel );
				char* c = (char*)mem_alloc( memSize );
				memcpy(c, GlobalLock(memHandel), memSize);
				GlobalUnlock( memHandel );
				res = c;
				mem_free( c );
			}
		}

		CloseClipboard();
	}

	return res;
}

uint sx_os_get_available_memory( void )
{
	MEMORYSTATUS minfo;
	minfo.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus( &minfo );
	return (uint)( minfo.dwAvailPhys / 1024 );
}

void sx_os_get_info( OSInfo& osInfo )
{
	sx_callstack();

	ZeroMemory( &osInfo, sizeof(osInfo) );

	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if( !GetVersionEx ((OSVERSIONINFO *) &osvi) )
		return;

	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	GetSystemInfo(&si);

	if ( osvi.dwPlatformId != VER_PLATFORM_WIN32_NT || osvi.dwMajorVersion <= 4 )
		return;

	String res = L"Microsoft ";

	// Test for the specific product.
	if ( osvi.dwMajorVersion == 6 )
	{
		if( osvi.dwMinorVersion == 0 )
		{
			res << ( osvi.wProductType == VER_NT_WORKSTATION ? L"Windows Vista " : L"Windows Server 2008 " );
		}
		else if ( osvi.dwMinorVersion == 1 )
		{
			if( osvi.wProductType == VER_NT_WORKSTATION )		res << L"Windows 7 ";
			else												res << L"Windows Server 2008 R2 ";
		}

		DWORD dwType;
		typedef BOOL (WINAPI *PGPI)(dword, dword, dword, dword, PDWORD);
		PGPI pGPI = (PGPI)GetProcAddress( GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
		pGPI( osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

		switch( dwType )
		{
		case PRODUCT_ULTIMATE:						res << L"Ultimate Edition";								break;
		case PRODUCT_PROFESSIONAL:					res << L"Professional";									break;
		case PRODUCT_HOME_PREMIUM:					res << L"Home Premium Edition";							break;
		case PRODUCT_HOME_BASIC:					res << L"Home Basic Edition";							break;
		case PRODUCT_ENTERPRISE:					res << L"Enterprise Edition";							break;
		case PRODUCT_BUSINESS:						res << L"Business Edition";								break;
		case PRODUCT_STARTER:						res << L"Starter Edition";								break;
		case PRODUCT_CLUSTER_SERVER:				res << L"Cluster Server Edition";						break;
		case PRODUCT_DATACENTER_SERVER:				res << L"Data center Edition";							break;
		case PRODUCT_DATACENTER_SERVER_CORE:		res << L"Data center Edition (core installation)";		break;
		case PRODUCT_ENTERPRISE_SERVER:				res << L"Enterprise Edition";							break;
		case PRODUCT_ENTERPRISE_SERVER_CORE:		res << L"Enterprise Edition (core installation)";		break;
		case PRODUCT_ENTERPRISE_SERVER_IA64:		res << L"Enterprise Edition for Itanium-based Systems";	break;
		case PRODUCT_SMALLBUSINESS_SERVER:			res << L"Small Business Server";						break;
		case PRODUCT_STANDARD_SERVER:				res << L"Standard Edition";								break;
		case PRODUCT_STANDARD_SERVER_CORE:			res << L"Standard Edition (core installation)";			break;
		case PRODUCT_WEB_SERVER:					res << L"Web Server Edition";							break;
		}
	}
	else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
	{
		if ( osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER )					res << L"Windows Storage Server 2003";
		else if( osvi.wProductType == VER_NT_WORKSTATION &&	
			si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)		res << L"Windows XP Professional x64 Edition";
		else																res << L"Windows Server 2003, ";

		if ( osvi.wProductType != VER_NT_WORKSTATION )
		{
			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )		res << L"Data center Edition for Itanium-based Systems";
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )	res << L"Enterprise Edition for Itanium-based Systems";
			}
			else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )		res << L"Data center x64 Edition";
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )	res << L"Enterprise x64 Edition";
				else												res << L"Standard x64 Edition";
			}
			else
			{
				if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )	res << L"Compute Cluster Edition";
				else if( osvi.wSuiteMask & VER_SUITE_DATACENTER )	res << L"Data center Edition";
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )	res << L"Enterprise Edition";
				else if ( osvi.wSuiteMask & VER_SUITE_BLADE )		res << L"Web Edition";
				else												res << L"Standard Edition";
			}
		}
	}
	else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
	{
		res << L"Windows XP ";
		if( osvi.wSuiteMask & VER_SUITE_PERSONAL )	res << L"Home Edition";
		else										res << L"Professional";
	}
	else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
	{
		res << L"Windows 2000 ";
		if ( osvi.wProductType == VER_NT_WORKSTATION )			
			res << L"Professional";
		else 
		{
			if( osvi.wSuiteMask & VER_SUITE_DATACENTER )		res << L"Data center Server";
			else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )	res << L"Advanced Server";
			else												res << L"Server";
		}
	}

	// Include service pack (if any) and build number.
	if( wcslen(osvi.szCSDVersion) > 0 )	res << L" " << osvi.szCSDVersion;

	res << L" (build " << sx_uint_to_str( osvi.dwBuildNumber ) << L")";
	if ( osvi.dwMajorVersion >= 6 )
	{
		if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )		res << L", 64-bit";
		else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )	res << L", 32-bit";
	}

	sx_str_copy( osInfo.desc, 256, res );
	osInfo.major = osvi.dwMajorVersion;
	osInfo.minor = osvi.dwMinorVersion;
}

void sx_os_get_cpu_info( OUT CPUInfo& cpuInfo )
{
	//  get number of processors
	SYSTEM_INFO sinfo;
	GetSystemInfo( &sinfo );
	cpuInfo.processors = sinfo.dwNumberOfProcessors;

	//  get cpu clock
	PROCESSOR_POWER_INFORMATION ppi[MAX_CPU_COUNT];
	CallNtPowerInformation( ProcessorInformation, null, 0, ppi, sizeof(PROCESSOR_POWER_INFORMATION)*MAX_CPU_COUNT );
	cpuInfo.clock = ppi[0].MaxMhz;

	//  get cpu features
	cpuInfo.mmx		= IsProcessorFeaturePresent( PF_MMX_INSTRUCTIONS_AVAILABLE ) > 0;
	cpuInfo.sse		= IsProcessorFeaturePresent( PF_XMMI_INSTRUCTIONS_AVAILABLE ) > 0;
	cpuInfo.sse2	= IsProcessorFeaturePresent( PF_XMMI64_INSTRUCTIONS_AVAILABLE ) > 0;
	cpuInfo.now3d	= IsProcessorFeaturePresent( PF_3DNOW_INSTRUCTIONS_AVAILABLE ) > 0;

	// Interpret cpu brand string and cache information.
	int _CPUInfo[4] = {-1};
	__cpuid(_CPUInfo, 1);
	char brandName[0x40];
	__cpuid( _CPUInfo, 0x80000002 );
	memcpy(brandName + 00, _CPUInfo, sizeof(_CPUInfo));
	__cpuid( _CPUInfo, 0x80000003 );
	memcpy(brandName + 16, _CPUInfo, sizeof(_CPUInfo));
	__cpuid( _CPUInfo, 0x80000004 );
	memcpy(brandName + 32, _CPUInfo, sizeof(_CPUInfo));
	__cpuid( _CPUInfo, 0x80000006 );
	cpuInfo.cache = ( _CPUInfo[2] >> 16 ) & 0xffff;

	for( sint i=0; i<0x40; i++ )
		cpuInfo.brand[i] = brandName[i];
}

void sx_os_get_system_info( SystemInfo& sysInfo )
{
	ZeroMemory( &sysInfo, sizeof(sysInfo) );

	sx_os_get_info( sysInfo.os );

	sx_os_get_cpu_info( sysInfo.cpu );

	//  get total memory
	MEMORYSTATUS minfo;
	minfo.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus( &minfo );
	sysInfo.memory = (uint)( minfo.dwTotalPhys / 1024 );

	//  verify for laptop lid and batteries
	SYSTEM_POWER_CAPABILITIES spc;
	CallNtPowerInformation( SystemPowerCapabilities, null, 0, &spc, sizeof(SYSTEM_POWER_CAPABILITIES) );
	sysInfo.hasLid = spc.LidPresent > 0;
	sysInfo.hasBattery = spc.SystemBatteriesPresent > 0;
}

void sx_os_get_cpu_status( CPUStatus* pCPUStatus, const sint numberOfCPU )
{
	if ( !pCPUStatus || !numberOfCPU ) return;
	sx_callstack();

	PROCESSOR_POWER_INFORMATION ppi[MAX_CPU_COUNT];
	CallNtPowerInformation( ProcessorInformation, null, 0, ppi, sizeof(PROCESSOR_POWER_INFORMATION) * MAX_CPU_COUNT );

	for ( sint i=0; i<numberOfCPU && i<MAX_CPU_COUNT; i++ )
	{
		pCPUStatus[i].maxMhz = ppi[i].MaxMhz;
		pCPUStatus[i].currMhz = ppi[i].CurrentMhz;
	}
}

void sx_os_get_battery_status( BatteryStatus& batteryStatus )
{
	sx_callstack();

	SYSTEM_BATTERY_STATE sbs;
	CallNtPowerInformation( SystemBatteryState, null, 0, &sbs, sizeof(SYSTEM_BATTERY_STATE) );
	float percent = (float)sbs.RemainingCapacity / (float)sbs.MaxCapacity;

	if (sbs.Charging)
		batteryStatus.state = BS_Charging;
	else if (sbs.Discharging)
		batteryStatus.state = BS_Discharging;
	else
		batteryStatus.state = BS_AcOnline;

	batteryStatus.maxCap		= sbs.MaxCapacity;
	batteryStatus.currCap		= sbs.RemainingCapacity;
	batteryStatus.hasBattery	= ( sbs.BatteryPresent > 0 );
	batteryStatus.warning		= sbs.Discharging && ( percent <= 0.199f );
	batteryStatus.alert			= sbs.Discharging && ( percent <= 0.059f );

	if ( sbs.EstimatedTime < 0xFFFFFF )
	{
		swprintf_s( batteryStatus.estimatedTime, 8, L"%.2d:%.2d", sint(sbs.EstimatedTime / 3600), sint((sbs.EstimatedTime % 3600) / 60) );
	}
	else memcpy( batteryStatus.estimatedTime, L"00:00", 12 );
}

void sx_os_set_screensaver_activity( bool active )
{
	SystemParametersInfo( 17, (sint)active, null, SPIF_SENDWININICHANGE );
}

void sx_os_set_screensaver_default( void )
{
	sx_os_set_screensaver_activity( s_screensaverActivity );
}

void sx_os_keep_wakeful( void )
{
	SetThreadExecutionState( ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED );
}

MonitorInfo* sx_os_get_monitor( sint x /*= 0*/, sint y /*= 0 */ )
{
	sx_callstack();

	static MonitorInfo monitorInfo;

	static sint lastx = -9999999, lasty = -9999999;
	if ( lastx == x || lasty == y ) 
		return &monitorInfo;
	lastx = x; lasty = y;

	RECT rc; rc.left = x; rc.top = y; rc.right = 400; rc.bottom = 300;
	HMONITOR hMonitor = MonitorFromRect( &rc, MONITOR_DEFAULTTONEAREST );

	MONITORINFO monInfo;
	ZeroMemory( &monInfo, sizeof(MONITORINFO) );
	monInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo( hMonitor, &monInfo );
	monitorInfo.areaWidth		= monInfo.rcMonitor.right	- monInfo.rcMonitor.left;
	monitorInfo.areaHeight		= monInfo.rcMonitor.bottom	- monInfo.rcMonitor.top;
	monitorInfo.workingWidth	= monInfo.rcWork.right		- monInfo.rcWork.left;
	monitorInfo.workingHeight	= monInfo.rcWork.bottom		- monInfo.rcWork.top;

	return &monitorInfo;
}

DesktopInfo* sx_os_get_desktop( sint x /*= 0*/, sint y /*= 0 */ )
{
	sx_callstack();

	static DesktopInfo desktopInfo;

	static sint lastx = -9999999, lasty = -9999999;
	if ( lastx == x || lasty == y ) 
		return &desktopInfo;
	lastx = x; lasty = y;

	//	TODO : get desktop info on different monitors
	WINDOWINFO winfo;
	GetWindowInfo( GetDesktopWindow(), &winfo );
	desktopInfo.areaWidth		= winfo.rcWindow.right	- winfo.rcWindow.left;
	desktopInfo.areaHeight		= winfo.rcWindow.bottom - winfo.rcWindow.top;

	RECT rc; ZeroMemory( &rc, sizeof(rc) );
	SystemParametersInfo( SPI_GETWORKAREA, 0, &rc, 0 );
	desktopInfo.workingWidth	= rc.right - rc.left;
	desktopInfo.workingHeight	= rc.bottom - rc.top;

	return &desktopInfo;
}

bool sx_os_file_exist( const wchar* fileName)
{
	if ( !fileName ) return false;
	dword Res = GetFileAttributes( fileName );
	return ( Res != INVALID_FILE_ATTRIBUTES && !sx_set_has(Res, FILE_ATTRIBUTE_DIRECTORY) );
}

bool sx_os_dir_exist( const wchar* dirName )
{
	if (!dirName) return false;
	dword Res = GetFileAttributes(dirName);
	return ( Res != INVALID_FILE_ATTRIBUTES && sx_set_has(Res, FILE_ATTRIBUTE_DIRECTORY) );
}

const wchar* sx_os_get_user_name( void )
{
	static wchar res[1024];
	res[0] = 0; 
	DWORD len = 1024;
	GetUserName( res, &len );
	return res;
}

const wchar* sx_os_get_current_dir( void )
{
	static wchar res[1024];
	res[0] = 0;
	GetCurrentDirectory( 1024, res );
	return res;
}

void sx_os_set_current_dir( const wchar* path )
{
	if ( sx_str_is_fullpath( path ) )
		SetCurrentDirectory( path );
}

const wchar* sx_os_get_app_dir( void )
{
	static wchar res[1024];
	res[0] = 0;
	GetModuleFileName( null, res, 1024 );
	wchar* slash = wcsrchr( res, PATH_PART );
	if (slash) *slash = 0;
	return res;
}

const wchar* sx_os_get_temp_dir( void )
{
	static wchar res[1024];
	res[0] = 0; 
	GetTempPath( 1024, res );
	wchar* slash = wcsrchr( res, PATH_PART );
	if (slash) *slash = 0;
	return res;
}

const wchar* sx_os_get_dir( void )
{
	static wchar res[1024];
	res[0] = 0; 
	GetWindowsDirectory( res, 1024 );
	wchar* slash = wcsrchr( res, PATH_PART );
	if (slash) *slash = 0;
	return res;
}

const wchar* sx_os_get_desktop_dir( void )
{
	static wchar res[1024];
	res[0] = 0;
	SHGetSpecialFolderPath( null, res, CSIDL_DESKTOPDIRECTORY, FALSE );
	return res;
}

const wchar* sx_os_get_documents_dir( void )
{
	static wchar res[1024];
	res[0] = 0;
	SHGetSpecialFolderPath( null, res, CSIDL_PERSONAL, FALSE );
	return res;
}

bool sx_os_get_drives( DriveInfoArray& ddList )
{
	wchar szDriveInformation[1024];
	GetLogicalDriveStrings( 1024, szDriveInformation );
	wchar* szDriveLetters = szDriveInformation;
	DriveInfo dinfo;
	while ( *szDriveLetters ) {
		sx_os_get_drive_info( *szDriveLetters, dinfo );
		ddList.push_back( dinfo );
		szDriveLetters = &szDriveLetters[wcslen(szDriveLetters) + 1];
	}
	return true;
}

uint sx_os_get_drive_type( wchar DriveName )
{
	wchar dname[4] = { DriveName, ':', PATH_PART, 0 };
	return GetDriveType( dname );
}

uint sx_os_get_drive_space( wchar DriveName )
{
	wchar dname[4] = { DriveName, ':', PATH_PART, 0 };
	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD NumberOfFreeClusters;
	DWORD TotalNumberOfClusters;
	GetDiskFreeSpace(dname, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters );
	uint64 resByte = BytesPerSector * SectorsPerCluster;
	resByte *= TotalNumberOfClusters;
	return uint( resByte / 1048576 );
}

uint sx_os_get_drive_free_space( wchar DriveName )
{
	wchar dname[4] = { DriveName, ':', PATH_PART, 0 };
	DWORD SectorsPerCluster;
	DWORD BytesPerSector;
	DWORD NumberOfFreeClusters;
	DWORD TotalNumberOfClusters;
	GetDiskFreeSpace(dname, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters );
	uint64 resByte = BytesPerSector * SectorsPerCluster;
	resByte *= NumberOfFreeClusters;
	return uint( resByte / 1048576 );
}

const wchar* sx_os_get_drive_label( wchar DriveName )
{
	static wchar buf[1024];
	buf[0]=0;
	wchar dname[4] = { DriveName, ':', PATH_PART, 0 };
	GetVolumeInformation( dname, buf, 1024, 0, 0, 0, 0, 0 );
	return buf;
}

void sx_os_get_drive_info( wchar DriveName, DriveInfo& dinfo )
{
	sx_str_copy( dinfo.label, 64, sx_os_get_drive_label( DriveName ) );

	dinfo.size	= sx_os_get_drive_space( DriveName );
	dinfo.free	= sx_os_get_drive_free_space( DriveName );
	dinfo.type	= sx_os_get_drive_type( DriveName );
	dinfo.name  = DriveName;

	switch (dinfo.type)
	{
	case DRIVE_UNKNOWN:		sx_str_copy( dinfo.typeName, 64, L"Unknown type" );				break;
	case DRIVE_REMOVABLE:	sx_str_copy( dinfo.typeName, 64, L"Removable storage" );			break;
	case DRIVE_FIXED:		sx_str_copy( dinfo.typeName, 64, L"Hard drive" );					break;
	case DRIVE_REMOTE:		sx_str_copy( dinfo.typeName, 64, L"Remote (network) drive" );		break;
	case DRIVE_CDROM:		sx_str_copy( dinfo.typeName, 64, L"CD-ROM drive" );				break;
	case DRIVE_RAMDISK:		sx_str_copy( dinfo.typeName, 64, L"RAM Drive" );					break;
	}
}

bool sx_os_get_file_info( const wchar* fileName, FileInfo& finfo )
{
	if ( !fileName ) return false;

	WIN32_FILE_ATTRIBUTE_DATA fileAD;
	if ( !GetFileAttributesEx( fileName, GetFileExInfoStandard, &fileAD ) ) return false;

	ZeroMemory( &finfo, sizeof(FileInfo) );

	String tmpStr;

	tmpStr = fileName;
	if ( sx_str_is_pathstyle( fileName ) )
		tmpStr.remove( tmpStr.length() - 1 );
	tmpStr.extract_file_name();
	//tmpStr.ExcludeFileExtension();
	sx_str_copy( finfo.name, 256, tmpStr );

	tmpStr = fileName;
	tmpStr.extract_file_extension();
	sx_str_copy( finfo.type, 256, tmpStr );

	LARGE_INTEGER filesize;
	filesize.LowPart  = fileAD.nFileSizeLow;
	filesize.HighPart = fileAD.nFileSizeHigh;
	finfo.size = filesize.QuadPart;
	finfo.flag = fileAD.dwFileAttributes;

	FILETIME ft; SYSTEMTIME st;
	FileTimeToLocalFileTime( &fileAD.ftCreationTime, &ft );
	FileTimeToSystemTime( &ft, &st );
	_systemtime_to_timestruct( st, finfo.created ); 

	FileTimeToLocalFileTime( &fileAD.ftLastWriteTime, &ft );
	FileTimeToSystemTime( &ft, &st );
	_systemtime_to_timestruct( st, finfo.modified );

	return true;
}

void sx_os_get_files( const wchar* path, const wchar* exten, FileInfoArray& fileList )
{
	if (!path || !sx_os_dir_exist( path ) ) return;

	// find the first file in the directory.
	WIN32_FIND_DATA ffd;
	HANDLE hFind = NULL;
	{
		wchar dir[400];
		swprintf_s( dir, 400, L"%s*.*", sx_str_make_pathstyle( path ) );
		hFind = FindFirstFile( dir, &ffd);
	}
	if ( hFind == INVALID_HANDLE_VALUE )
		return;

	// List all the files in the directory with some info about them.
	FILETIME		fileTime;
	LARGE_INTEGER	fileSize;
	FileInfo		fileInfo;
	bool			checkExtn = exten && wcscmp( exten, L"*.*" );
	ZeroMemory( &fileInfo, sizeof(fileInfo) );

	//  traverse between files
	do {
		bool fileIsDir = ( ffd.cFileName[0] == '.' ) && ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY );
		if ( fileIsDir || ( ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) || ( ffd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY ) )
			continue;

		bool extenAccepted = true;
		if ( checkExtn )
		{
			extenAccepted = wcsstr( exten, sx_str_extract_extension( ffd.cFileName ) ) != null;
		}

		if ( extenAccepted )
		{
			sx_str_copy( fileInfo.name, 256, ffd.cFileName );

			if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				sx_str_copy( fileInfo.type, 32, L"<DIR>" );
			else
			{
				sx_str_copy( fileInfo.type, 32, sx_str_extract_extension( ffd.cFileName ) );
			}

			fileSize.LowPart  = ffd.nFileSizeLow;
			fileSize.HighPart = ffd.nFileSizeHigh;

			fileInfo.flag		= ffd.dwFileAttributes;
			fileInfo.size		= fileSize.QuadPart;

			SYSTEMTIME sysTime;

			FileTimeToLocalFileTime( &ffd.ftCreationTime, &fileTime );
			FileTimeToSystemTime( &fileTime, &sysTime );
			_systemtime_to_timestruct( sysTime, fileInfo.created );

			FileTimeToLocalFileTime( &ffd.ftLastWriteTime, &fileTime );
			FileTimeToSystemTime( &fileTime, &sysTime );
			_systemtime_to_timestruct( sysTime, fileInfo.modified );

			fileList.push_back( fileInfo );
		}
	}
	while ( FindNextFile( hFind, &ffd ) );

	FindClose(hFind);
}

void sx_os_sort_files( SortFilesType SFT_ sftype, FileInfoArray& fileList )
{
	if ( fileList.m_count < 2 ) return;

	switch (sftype)
	{
	case SFT_BYNAME:	fileList.sort( _sort_fils_by_name );	break;
	case SFT_BYTYPE:	fileList.sort( _sort_fils_by_type );	break;
	case SFT_BYDATE:	fileList.sort( _sort_fils_by_date );	break;
	case SFT_BYSIZE:	fileList.sort( _sort_fils_by_size );	break;
	}

	const sint n = fileList.m_count;
	Array<FileInfo> tmpList;
	tmpList.set_count( n );

	for ( sint i=0; i<n; i++ )
	{
		if ( fileList[i].flag & FILE_ATTRIBUTE_DIRECTORY )
			tmpList.push_back( fileList[i] );
	}
	for ( sint i=0; i<n; i++ )
	{
		if ( (fileList[i].flag & FILE_ATTRIBUTE_DIRECTORY) == 0 )
			tmpList.push_back( fileList[i] );
	}

	fileList.clear();
	for ( sint i=0; i<tmpList.m_count; i++ )
		fileList.push_back( tmpList[i] );
}

void sx_os_get_filesX( const wchar* path, const wchar* exten, FileInfoArray& fileList )
{
	String ext = exten;
	_get_files_in_dir_x( path, null, ext, fileList );
}

bool sx_os_remove_file( const wchar* fileName )
{
	return DeleteFile( fileName ) > 0;
}

bool sx_os_copy_file( const wchar* srcFile, const wchar* destFile )
{
	return CopyFile( srcFile, destFile, false ) > 0;
}

bool sx_os_move_file( const wchar* srcFile, const wchar* destFile )
{
	return MoveFile( srcFile, destFile ) > 0;
}

bool sx_os_rename_file( const wchar* srcFile, const wchar* newName )
{
	return MoveFile( srcFile, newName ) > 0;
}

bool sx_os_make_dir( const wchar* folderName )
{
	return CreateDirectory( folderName, null ) > 0;
}

SEGAN_ENG_API bool sx_os_remove_dir( const wchar* folderName, CB_RemoveDir callback /*= null*/, void* userdata /*= null */ )
{
	sx_callstack_param(sx_os_remove_dir(folderName=%s), folderName);

	if ( !sx_os_dir_exist( folderName ) ) return false;

	String dirName = folderName;
	dirName.make_path_style();

	//  extract list of content of the folder
	FileInfoArray flist;
	sx_os_get_filesX( dirName, L"*.*", flist );
	
	sint c = flist.m_count;
	for ( sint i=c-1; i>=0; i-- )
	{
		String path = dirName;
		path << flist[i].name;
		
		if ( flist[i].flag & FILE_ATTRIBUTE_DIRECTORY )
		{
			if ( callback )
			{
				if ( !callback(userdata, c, c-i-1, *dirName, flist[i]) )
					RemoveDirectory( *path );
			}
			else RemoveDirectory( *path );
		}
		else
		{
			if ( callback )
			{
				if ( !callback(userdata, c, c-i-1, *dirName, flist[i]) )
					sx_os_remove_file( *path );
			}
			else sx_os_remove_file(*path);
		}
	}

	return RemoveDirectory( folderName ) > 0;
}

SEGAN_ENG_API bool sx_os_copy_dir( const wchar* srcFolder, const wchar* destFolder, CB_CopyDir callback /*= null*/, void* userdata /*= null */ )
{
	sx_callstack_param(sx_os_remove_dir(srcFolder=%s, destFolder=%s), srcFolder, destFolder);

	if ( !sx_os_dir_exist( srcFolder ) ) return false;

	String srcDir = srcFolder;
	srcDir.make_path_style();

	String destDir = destFolder;
	destDir.make_path_style();

	//  extract list of content of the folder
	FileInfoArray flist;
	sx_os_get_filesX( srcDir, L"*.*", flist );

	//  make destination folder
	if ( !CreateDirectory( destFolder, 0 ) )return false;

	bool result = true;
	sint c = flist.m_count;
	for ( sint i=0; i<c; i++ )
	{
		String srcPath; srcPath << *srcDir  << *flist[i].name;
		String desPath; desPath << *destDir << *flist[i].name;

		if ( flist[i].flag & FILE_ATTRIBUTE_DIRECTORY )
		{
			if ( callback )
			{
				if ( !callback(userdata, c, i, *srcPath, flist[i], *desPath) )
					if( !CreateDirectory(*desPath, 0) )
						result = false;
			}
			else
			{
				if( !CreateDirectory(*desPath, 0) )
					result = false;
			}
		}
		else
		{
			if ( callback )
			{
				if ( !callback(userdata, c, i, *srcDir, flist[i], *destDir) )
					if ( !sx_os_copy_file( *srcPath, *desPath ) )
						result = false;
			}
			else
			{
				if ( !sx_os_copy_file( *srcPath, *desPath ) )
					result = false;
			}
		}
	}

	return result;
}

SEGAN_ENG_API bool sx_os_move_dir( const wchar* srcFolder, const wchar* destFolder, CB_CopyDir callback /*= null*/, void* userdata /*= null */ )
{
	if ( !srcFolder || !destFolder ) return false;
	sx_callstack_param(sx_os_remove_dir(srcFolder=%s, destFolder=%s), srcFolder, destFolder);

	//  verify that both of them are in the same drive
	if ( *srcFolder == *destFolder )
	{
		return MoveFile( srcFolder, destFolder ) > 0;
	}
	else
	{
		if ( sx_os_copy_dir( srcFolder, destFolder, callback, userdata ) )
		{
			return sx_os_remove_dir( srcFolder );
		}
		else return false;
	}
}

bool sx_os_rename_dir( const wchar* srcFolder, const wchar* newName )
{
	if ( !srcFolder || !newName ) return false;

	sx_callstack_param(sx_os_remove_dir(srcFolder=%s, newName=%s), srcFolder, newName);

	String fldr = srcFolder;
	if ( sx_str_is_pathstyle( srcFolder ) )
		fldr.remove( fldr.length() - 1 );
	
	fldr.extract_file_path();
	fldr << newName;
	
	return sx_os_rename_file( srcFolder, fldr );
}


#endif
