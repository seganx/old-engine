#include "sxSys.h"

#if _WIN32_WINNT < 0x0601 // windows 7
#include "sxWin6.h"
#endif

#include <time.h>
#include <intrin.h>
#include <powrprof.h>
#include <ShlObj.h>
#ifndef POWER_PROFILER_LOADED
#define POWER_PROFILER_LOADED
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "powrprof.lib")
#endif

#include <MMSystem.h>
#pragma comment( lib, "winmm.lib" )

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

#define MAX_CPU_COUNT	32
#define HIDIGIT(d)		((BYTE)(d>>4))
#define LODIGIT(d)		((BYTE)(d&0x0F))
#define BACKSLASH(s)	{ if (!String::IsPathStyle(*s)) {s << PATH_PART;} }

static str1024		g_sTempString;
static str1024		g_sUserName;
static str1024		g_sAppFolder;
static str1024		g_sTempFolder;
static str1024		g_sWinFolder;
static SystemInfo	g_SystemInfo;
static OSInfo		g_OSInfo;
static POINT		g_pDesktopRes;
static bool			g_bScreenSaverActivity;

//////////////////////////////////////////////////////////////////////////
//		SORTING HELPER FUNCTIONS		//
int _SortFilsByName(const FileInfo& info1, const FileInfo& info2){
	if (*info1.name == null) return 1;
	if (*info2.name == null) return -1;

	//  make names lower case
	str256 s1 = info1.name, s2 = info2.name;
	s1.MakeLower();	s2.MakeLower();
	return ( wcscmp( s1, s2 ) );
}

int _SortFilsByType(const FileInfo& info1, const FileInfo& info2){
	if (info1.flag & FILE_ATTRIBUTE_DIRECTORY) return 1;
	if (info2.flag & FILE_ATTRIBUTE_DIRECTORY) return -1;
	if (*info1.type == null) return 1;
	if (*info2.type == null) return -1;
	
	//  make type names lower case
	str32 s1 = info1.type, s2 = info2.type;
	s1.MakeLower();	s2.MakeLower();
	return ( wcscmp( s1, s2 ) );
}

int  _SortFilsByDate( const FileInfo& info1, const FileInfo& info2){
	UINT64 res1 = info1.modified.wSecond + info1.modified.wMinute*60 + info1.modified.wHour*3600;
	res1 += info1.modified.wDay*43200 + info1.modified.wMonth*1296000 + (info1.modified.wYear-1800)*15552000;
	UINT64 res2 = info2.modified.wSecond + info2.modified.wMinute*60 + info2.modified.wHour*3600;
	res2 += info2.modified.wDay*43200 + info2.modified.wMonth*1296000 + (info2.modified.wYear-1800)*15552000;
	return (res1 < res2) ? -1 : (res1 == res2) ? 0 : 1;
}

int _SortFilsBySize( const FileInfo& info1, const FileInfo& info2){
	return (info1.size < info2.size) ? -1 : (info1.size == info2.size) ? 0 : 1;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//    ADITIONAL HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////
bool _GetListOfFilesInFolder(const WCHAR* root, const WCHAR* path, const WCHAR* exten, FileInfoArray* fList, UINT ExceptAttribs)
{
	if (!root || !sx::sys::FolderExist(root)) return false;

	str512 fpath = root;
	BACKSLASH(fpath);
	fpath << path;
	BACKSLASH(fpath);
	fpath << L"*.*";

	// Find the first file in the directory.
	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile(*fpath, &ffd);
	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	// List all the files in the directory with some info about them.
	fpath = path;
	if ( path )
		BACKSLASH(fpath);

	bool res = true;
	FILETIME		ft;
	LARGE_INTEGER	filesize;
	FileInfo		finfo;
	do{
		if ((ffd.cFileName[0]=='.' && ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
			ffd.dwFileAttributes & ExceptAttribs )
			continue;

		str64  filetype;
		str512 filename = fpath;
		filename << ffd.cFileName;

		str256 fileExtention = ffd.cFileName;	fileExtention.ExtractFileExtension();
		if ( fileExtention == exten )
		{
			filesize.LowPart  = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;

			finfo.flag		= ffd.dwFileAttributes;
			finfo.size		= int(filesize.QuadPart / 1024);

			FileTimeToLocalFileTime(&ffd.ftCreationTime, &ft);
			FileTimeToSystemTime(&ft, &finfo.created);

			FileTimeToLocalFileTime(&ffd.ftLastWriteTime, &ft);
			FileTimeToSystemTime(&ft, &finfo.modified);

			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				filename << PATH_PART;
				filename = L"<DIR>";
			}
			else
				filetype = fileExtention;


			String::Copy( finfo.name, 256, filename);
			String::Copy( finfo.type, 32, filetype );

			fList->PushBack( finfo );
		}

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			res = _GetListOfFilesInFolder(root, filename, exten, fList, ExceptAttribs);
	}
	while (FindNextFile(hFind, &ffd) != 0);

	FindClose(hFind);

	return res;
}

namespace sx { namespace sys
{
	SEGAN_API float GetSysTime( void ){
		static LARGE_INTEGER qFerquency = {0, 0};
		static LARGE_INTEGER qCounter  = {0, 0};
		static BOOL	useHighRes = QueryPerformanceFrequency(&qFerquency);

		if (useHighRes && QueryPerformanceCounter(&qCounter))
		{
			return (float)(((double)qCounter.LowPart / (double)qFerquency.LowPart) * 1000.0);
		}
		else
		{
			return (float)timeGetTime();
		}
	}

	SEGAN_API UINT GetSysTime_u( void ){
			return timeGetTime();
	}

	SEGAN_API const WCHAR* GetDate(void){
		static wchar res[32];
		res[0] = 0;
		time_t rawTime;
		time( &rawTime );
		struct tm timeInfo;
		localtime_s( &timeInfo, &rawTime );
		wcsftime( res, 32, L"%Y/%m/%d", &timeInfo );
		return res;
	}

	SEGAN_API const WCHAR* GetTime(void){
		static wchar res[32];
		res[0] = 0;
		time_t rawTime;
		time( &rawTime );
		struct tm timeInfo;
		localtime_s( &timeInfo, &rawTime );
		wcsftime( res, 32, L"%H:%M:%S", &timeInfo );
		return res;
	}

	SEGAN_API const WCHAR* GetTimeStamp(void){
		static wchar res[32];
		res[0] = 0;
		time_t rawTime;
		time( &rawTime );
		struct tm timeInfo;
		localtime_s( &timeInfo, &rawTime );
		wcsftime( res, 32, L"%Y/%m/%d %H:%M:%S", &timeInfo );
		return res;
	}

	SEGAN_API void CopyText( const WCHAR* text )
	{
		if ( !text ) return;
		int memSize = ( (int)wcslen(text) + 1 ) * sizeof(WCHAR);
		
		HGLOBAL memHandle = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, memSize );
		if ( !memHandle ) return;

		memcpy( GlobalLock(memHandle), text, memSize );
		GlobalUnlock(memHandle);

		if ( OpenClipboard(0) )
		{
			EmptyClipboard();
			SetClipboardData( CF_UNICODETEXT, memHandle );
			CloseClipboard();
		}
		else
			GlobalFree(memHandle);
	}

	SEGAN_API const WCHAR* PasteText( void )
	{
		static String tmp;
		tmp.Clear();

		bool isUnicodeText = IsClipboardFormatAvailable(CF_UNICODETEXT) > 0;
		bool isText = IsClipboardFormatAvailable(CF_TEXT) > 0;
		if ( (isUnicodeText || isText) && OpenClipboard(0) )
		{
			if ( isUnicodeText )
			{
				HGLOBAL memHandel = GetClipboardData( CF_UNICODETEXT );
				if ( memHandel )
				{
					int memSize = (int)GlobalSize( memHandel );
					WCHAR* c = (WCHAR*)sx_mem_alloc(memSize);
					memcpy(c, GlobalLock(memHandel), memSize);
					GlobalUnlock(memHandel);
					tmp = c;
					sx_mem_free(c);
				}
			}
			else if ( isText )
			{
				HGLOBAL memHandel = GetClipboardData( CF_TEXT );
				if ( memHandel )
				{
					int memSize = (int)GlobalSize( memHandel );
					char* c = (char*)sx_mem_alloc(memSize);
					memcpy(c, GlobalLock(memHandel), memSize);
					GlobalUnlock(memHandel);
					tmp = c;
					sx_mem_free(c);
				}
			}


			CloseClipboard();
		}

		return tmp;
	}

	SEGAN_API UINT GetAvailableMemory(void){
		MEMORYSTATUS minfo;
		minfo.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&minfo);
		return (UINT)(minfo.dwAvailPhys / 1024);	
	}

	SEGAN_API void GetOSInfo( OSInfo& osInfo ){
		osInfo = g_OSInfo;
	}

	SEGAN_API void GetSystemInfo(SystemInfo& sysInfo){
		sysInfo = g_SystemInfo;
	}

	SEGAN_API void GetCPUStatus(CPUStatus* pCPUStatus, int numberOfCPU){
		if (!pCPUStatus) return;
		SEGAN_CLAMP(numberOfCPU, 1, MAX_CPU_COUNT)
		PROCESSOR_POWER_INFORMATION ppi[MAX_CPU_COUNT];
		CallNtPowerInformation(ProcessorInformation, NULL, 0, ppi, sizeof(PROCESSOR_POWER_INFORMATION)*MAX_CPU_COUNT);
		for (int i=0; i<numberOfCPU; i++)
		{
			pCPUStatus[i].MaxMhz = ppi[i].MaxMhz;
			pCPUStatus[i].CurMhz = ppi[i].CurrentMhz;
		}
	}

	SEGAN_API void GetBatteryStatus(BatteryStatus& batteryStatus){
		SYSTEM_BATTERY_STATE sbs;
		CallNtPowerInformation(SystemBatteryState, NULL, 0, &sbs, sizeof(SYSTEM_BATTERY_STATE));
		float percent = (float)sbs.RemainingCapacity / (float)sbs.MaxCapacity;

		if (sbs.Charging)
			batteryStatus.State = BatteryStatus::Charging;
		else if (sbs.Discharging)
			batteryStatus.State = BatteryStatus::Discharging;
		else
			batteryStatus.State = BatteryStatus::AcOnline;

		batteryStatus.BatteryPresent= sbs.BatteryPresent>0;
		batteryStatus.MaxCapacity	= sbs.MaxCapacity;
		batteryStatus.CurCapacity	= sbs.RemainingCapacity;
		batteryStatus.Warning		= sbs.Discharging && (percent<=0.199f);
		batteryStatus.Alert			= sbs.Discharging && (percent<=0.059f);

		if ( sbs.EstimatedTime < 0xFFFFFF )
		{
			str32 tmp;
			tmp.Format( L"%.2d:%.2d", sint(sbs.EstimatedTime / 3600), sint((sbs.EstimatedTime % 3600) / 60) );
			String::Copy( batteryStatus.EstimatedTime, 8, tmp );
		}
		else memcpy( batteryStatus.EstimatedTime, L"00:00", 12 );
			
	}

	SEGAN_API void ScreenSaverSetActivity(bool active){
		SystemParametersInfo(17, (int)active, NULL, SPIF_SENDWININICHANGE);
	}

	SEGAN_API void ScreenSaverSetDefault(void){
		ScreenSaverSetActivity(g_bScreenSaverActivity);
	}

	SEGAN_API void KeepSystemWakeful(void){
		SetThreadExecutionState(ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
	}

	SEGAN_API UINT GetDesktopWidth(void){
		return g_pDesktopRes.x;
	}

	SEGAN_API UINT GetDesktopHeight(void){
		return g_pDesktopRes.y;
	}

	SEGAN_API bool FileExist(const WCHAR* FileName){
		if (!FileName)	return false;
		DWORD Res = GetFileAttributes(FileName);
		return ( Res!=INVALID_FILE_ATTRIBUTES && !SEGAN_SET_HAS(Res, FILE_ATTRIBUTE_DIRECTORY) );
	}

	SEGAN_API bool FolderExist(const WCHAR* DirName){
		if (!DirName)	return false;
		DWORD Res = GetFileAttributes(DirName);
		return ( Res!=INVALID_FILE_ATTRIBUTES && SEGAN_SET_HAS(Res, FILE_ATTRIBUTE_DIRECTORY) );
	}

	SEGAN_API const WCHAR* GetUserName(void){
		return g_sUserName.Text();
	}

	SEGAN_API const WCHAR* GetCurrentFolder(void){
		WCHAR res[1024]; res[0]=0;
		::GetCurrentDirectory(1024, res);
		g_sTempFolder = res;
		return g_sTempFolder.Text();
	}

	SEGAN_API void SetCurrentFolder( const WCHAR* path )
	{
		if ( String::IsFullPath( path ) )
			::SetCurrentDirectory(path);
	}

	SEGAN_API const WCHAR* GetAppFolder(void){
		return g_sAppFolder.Text();
	}

	SEGAN_API const WCHAR* GetTempFolder(void){
		return g_sTempFolder.Text();
	}

	SEGAN_API const WCHAR* GetWinFolder(void){
		return g_sWinFolder.Text();
	}

	SEGAN_API const WCHAR* GetDesktopFolder( void )
	{
		static WCHAR res[MAX_PATH];
		ZeroMemory(res, sizeof(res));
		SHGetSpecialFolderPath(NULL, res, CSIDL_DESKTOPDIRECTORY, FALSE);
		return res;
	}

	SEGAN_API const WCHAR* GetDocumentsFolder( void )
	{
		static WCHAR res[MAX_PATH];
		ZeroMemory(res, sizeof(res));
		SHGetSpecialFolderPath(NULL, res, CSIDL_PERSONAL, FALSE);
		return res;
	}

	SEGAN_API bool GetDrives(DriveInfoArray* ddList){
		if (!ddList) return false;

		WCHAR szDriveInformation[1024];
		GetLogicalDriveStrings(1024, szDriveInformation);
		WCHAR* szDriveLetters = szDriveInformation;
		DriveInfo dinfo;
		while (*szDriveLetters) {
			GetDriveInfo(*szDriveLetters, dinfo);
			ddList->PushBack(dinfo);
			szDriveLetters = &szDriveLetters[wcslen(szDriveLetters) + 1];
		}
		return true;
	}

	SEGAN_API UINT GetDriveKind(WCHAR DriveName){
		WCHAR dname[4] = {DriveName, ':', PATH_PART, 0};
		return GetDriveType(dname);
	}

	SEGAN_API UINT GetDriveSpace(WCHAR DriveName){
		WCHAR dname[4] = {DriveName, ':', PATH_PART, 0};
		DWORD SectorsPerCluster;
		DWORD BytesPerSector;
		DWORD NumberOfFreeClusters;
		DWORD TotalNumberOfClusters;
		GetDiskFreeSpace(dname, 
			&SectorsPerCluster,
			&BytesPerSector,
			&NumberOfFreeClusters,
			&TotalNumberOfClusters);
		UINT64 resByte = BytesPerSector * SectorsPerCluster;
		resByte *= TotalNumberOfClusters;
		return UINT(resByte / 1048576);
	}

	SEGAN_API UINT GetDriveFreeSpace(WCHAR DriveName){
		WCHAR dname[4] = {DriveName, ':', PATH_PART, 0};
		DWORD SectorsPerCluster;
		DWORD BytesPerSector;
		DWORD NumberOfFreeClusters;
		DWORD TotalNumberOfClusters;
		GetDiskFreeSpace(dname, 
			&SectorsPerCluster,
			&BytesPerSector,
			&NumberOfFreeClusters,
			&TotalNumberOfClusters);
		UINT64 resByte = BytesPerSector * SectorsPerCluster;
		resByte *= NumberOfFreeClusters;
		return UINT(resByte / 1048576);
	}

	SEGAN_API const WCHAR* GetDriveLabel(WCHAR DriveName){
		WCHAR dname[4] = {DriveName, ':', PATH_PART, 0};
		WCHAR buf[1024]; buf[0]=0;
		GetVolumeInformation(dname, buf, 1024, 0, 0, 0, 0, 0);
		g_sTempString = buf;
		return g_sTempString.Text();
	}

	SEGAN_API void GetDriveInfo(WCHAR DriveName, DriveInfo& dinfo){
		str512 tmp = GetDriveLabel(DriveName);
		String::Copy( dinfo.label, 64, tmp );

		dinfo.size	= GetDriveSpace(DriveName);
		dinfo.free	= GetDriveFreeSpace(DriveName);
		dinfo.name  = DriveName;

		dinfo.type	= GetDriveKind(DriveName);
		switch (dinfo.type)
		{
		case DRIVE_UNKNOWN:		memcpy( dinfo.typeName, L"Unknown type", 26 );				break;
		case DRIVE_REMOVABLE:	memcpy( dinfo.typeName, L"Removable storage", 36 );			break;
		case DRIVE_FIXED:		memcpy( dinfo.typeName, L"Hard drive", 22 );				break;
		case DRIVE_REMOTE:		memcpy( dinfo.typeName, L"Remote (network) drive", 46 );	break;
		case DRIVE_CDROM:		memcpy( dinfo.typeName, L"CD-ROM drive", 26 );				break;
		case DRIVE_RAMDISK:		memcpy( dinfo.typeName, L"RAM Drive", 20 );					break;
		}
	}

	SEGAN_API bool GetFileInfo(const WCHAR* FileName, FileInfo& finfo){
		if (!FileName) 
			return false;

		WIN32_FILE_ATTRIBUTE_DATA fileAD;
		if (!GetFileAttributesEx(FileName, GetFileExInfoStandard, &fileAD))
			return false;

		if (str512::IsPathStyle(FileName))
		{
			str512 filename = FileName;
			filename.Delete(filename.Length()-1);
			filename.ExtractFileName();
			filename.ExcludeFileExtension();
			String::Copy( finfo.name, 256, filename );
		}
		else
		{
			str512 filename = FileName;
			filename.ExtractFileName();
			filename.ExcludeFileExtension();
			String::Copy( finfo.name, 256, filename );
		}

		LARGE_INTEGER filesize;
		filesize.LowPart  = fileAD.nFileSizeLow;
		filesize.HighPart = fileAD.nFileSizeHigh;
		finfo.size = int(filesize.QuadPart / 1024);
		finfo.flag = fileAD.dwFileAttributes;

		str512 filetype  = FileName;
		filetype.ExtractFileExtension();
		String::Copy( finfo.type, 32, filetype );

		FILETIME ft;
		FileTimeToLocalFileTime(&fileAD.ftCreationTime, &ft);
		FileTimeToSystemTime(&ft, &finfo.created);

		FileTimeToLocalFileTime(&fileAD.ftLastWriteTime, &ft);
		FileTimeToSystemTime(&ft, &finfo.modified);

		return true;
	}

	SEGAN_API bool GetFilesInFolder(const WCHAR* path, const WCHAR* exten, FileInfoArray* fList, UINT ExceptAttribs){
		if (!path || !FolderExist(path)) return false;

		str1024 sdir = path;
		BACKSLASH(sdir);
		sdir << exten;

		// Find the first file in the directory.
		WIN32_FIND_DATA ffd;
		HANDLE hFind = FindFirstFile(sdir.Text(), &ffd);
		if (hFind == INVALID_HANDLE_VALUE)
			return false;

		// List all the files in the directory with some info about them.
		FILETIME ft;
		LARGE_INTEGER filesize;
		FileInfo	finfo;
		do{
			if ((ffd.cFileName[0]=='.' && ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
				ffd.dwFileAttributes & ExceptAttribs //FILE_ATTRIBUTE_SYSTEM || ffd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY ||
				)
				continue;

			filesize.LowPart  = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;

			finfo.flag		= ffd.dwFileAttributes;
			finfo.size		= int(filesize.QuadPart / 1024);
			//finfo.name		= str256::ExcludeFileExtension(ffd.cFileName);
			str512 tmpstr = ffd.cFileName; tmpstr.ExtractFileName();
			String::Copy( finfo.name, 256, tmpstr );

			FileTimeToLocalFileTime(&ffd.ftCreationTime, &ft);
			FileTimeToSystemTime(&ft, &finfo.created);

			FileTimeToLocalFileTime(&ffd.ftLastWriteTime, &ft);
			FileTimeToSystemTime(&ft, &finfo.modified);

			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				String::Copy( finfo.type, 6, L"<DIR>" );
			}
			else
			{
				str64 tmp = ffd.cFileName; tmp.ExtractFileExtension();
				tmp.MakeLower();
				String::Copy( finfo.type, 32, tmp );
			}

			fList->PushBack(finfo);
		}
		while (FindNextFile(hFind, &ffd) != 0);

		FindClose(hFind);

		return true;
	}

	SEGAN_API void SortFilesBy( SFT_ SortFilesType sftype, FileInfoArray* fList )
	{
		if (!fList || fList->Count()<2) return;

		switch (sftype)
		{
		case SFT_BYNAME:	fList->Sort(_SortFilsByName);	break;
		case SFT_BYTYPE:	fList->Sort(_SortFilsByType);	break;
		case SFT_BYDATE:	fList->Sort(_SortFilsByDate);	break;
		case SFT_BYSIZE:	fList->Sort(_SortFilsBySize);	break;
		}

		Array<FileInfo> tmpList(8);
		for (int i=0; i<fList->Count(); i++)
		{
			if (SEGAN_SET_HAS(fList->At(i).flag, FILE_ATTRIBUTE_DIRECTORY))
				tmpList.PushBack(fList->At(i));
		}
		for (int i=0; i<fList->Count(); i++)
		{
			if (!SEGAN_SET_HAS(fList->At(i).flag, FILE_ATTRIBUTE_DIRECTORY))
				tmpList.PushBack(fList->At(i));
		}
		fList->Clear();
		for (int i=0; i<tmpList.Count(); i++)
		{
			fList->PushBack(tmpList[i]);
		}
	}

	SEGAN_API bool GetFilesInFolderX( const WCHAR* path, const WCHAR* exten, FileInfoArray* fList, UINT ExceptAttribs /*= FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY */ )
	{
		return _GetListOfFilesInFolder(path, NULL, exten, fList, ExceptAttribs);
	}

	SEGAN_API bool RemoveFile( const WCHAR* fileName )
	{
		return ::DeleteFile(fileName)>0;
	}

	SEGAN_API bool CopyFile( const WCHAR* srcFile, const WCHAR* destFile )
	{
		return ::CopyFile(srcFile, destFile, false)>0;
	}

	SEGAN_API bool MoveFile( const WCHAR* srcFile, const WCHAR* destFile )
	{
		return ::MoveFile(srcFile, destFile)>0;
	}

	SEGAN_API bool RenameFile( const WCHAR* srcFile, const WCHAR* newName )
	{
		return ::MoveFile(srcFile, newName)>0;
	}

	SEGAN_API bool MakeFolder( const WCHAR* folderName )
	{
		return ::CreateDirectory(folderName, NULL)>0;
	}

	SEGAN_API bool RemoveFolder( const WCHAR* folderName, SysCallback_Remove callback, void* UserObject )
	{
		if (!FolderExist(folderName)) 
			return false;

		str1024 fName = folderName;
		BACKSLASH(fName)

		//  extract list of content of the folder
		FileInfoArray flist;
		GetFilesInFolderX(*fName, L"*.*", &flist, 0);

		int c = flist.Count();
		for (int i=c-1; i>=0; i--)
		{
			String path; path << *fName << *flist[i].name;
			if (flist[i].flag & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (callback)
				{
					if (!callback(UserObject, c, c-i-1, *fName, flist[i]))
						::RemoveDirectory(*path);
				}
				else
				{
					::RemoveDirectory(*path);
				}
			}
			else
			{
				if (callback)
				{
					if (!callback(UserObject, c, c-i-1, *fName, flist[i]))
						RemoveFile(*path);
				}
				else
				{
					RemoveFile(*path);
				}
			}
		}

		return ::RemoveDirectory(folderName)>0;
	}

	SEGAN_API bool CopyFolder( const WCHAR* srcFolder, const WCHAR* destFolder, SysCallback_Copy callback, void* UserObject )
	{
		if (!FolderExist(srcFolder)) 
			return false;

		str1024 srcDir = srcFolder;
		BACKSLASH(srcDir);

		str1024 destDir = destFolder;
		BACKSLASH(destDir)

		//  extract list of content of the folder
		FileInfoArray flist;
		GetFilesInFolderX(*srcDir, L"*.*", &flist, 0);

		//  make destination folder
		if (!::CreateDirectory(destFolder, 0))
			return false;

		bool result = true;
		int c = flist.Count();
		for (int i=0; i<c; i++)
		{
			String srcPath; srcPath << *srcDir  << *flist[i].name;
			String desPath; desPath << *destDir << *flist[i].name;

			if (flist[i].flag & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (callback)
				{
					if (!callback(UserObject, c, i, *srcPath, flist[i], *desPath))
						if(!::CreateDirectory(*desPath, 0))
							result = false;
				}
				else
				{
					if(!::CreateDirectory(*desPath, 0))
						result = false;
				}
			}
			else
			{
				if (callback)
				{
					if (!callback(UserObject, c, i, *srcDir, flist[i], *destDir))
						if (!CopyFile(*srcPath, *desPath))
							result = false;
				}
				else
				{
					if (!CopyFile(*srcPath, *desPath))
						result = false;
				}
			}
		}

		return result;
	}

	SEGAN_API bool MoveFolder( const WCHAR* srcFolder, const WCHAR* destFolder, SysCallback_Copy callback , void* UserObject )
	{
		if (!srcFolder || !destFolder) return false;
		if (*srcFolder == *destFolder)
			return ::MoveFile(srcFolder, destFolder)>0;
		else
		{
			if (CopyFolder(srcFolder, destFolder, callback, UserObject))
			{
				return RemoveFolder(srcFolder);
			}
			else
			{
				return false;
			}
		}
	}

	SEGAN_API bool RenameFolder( const WCHAR* srcFolder, const WCHAR* newName )
	{
		if (!srcFolder || !newName) return false;
		String fldr = srcFolder;
		if (String::IsPathStyle(srcFolder))
			fldr.Delete(fldr.Length()-1);
		fldr.ExtractFilePath();
		fldr << newName;
		return RenameFile(srcFolder, *fldr);
	}


} } // namespace

//////////////////////////////////////////////////////////////////////////
//				!	PRIVATE AREA    !
//			 HERE IS INTERNAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////


typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
bool _GetOSInfo( OSInfo& osinfo )
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if( !GetVersionEx ((OSVERSIONINFO *) &osvi) )
		return false;

	SYSTEM_INFO si;
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	GetSystemInfo(&si);

	str256 res;

	if ( osvi.dwPlatformId==VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion > 4 )
	{
		res = L"Microsoft ";

		// Test for the specific product.
		if ( osvi.dwMajorVersion == 6 )
		{
			if( osvi.dwMinorVersion == 0 )
			{
				if( osvi.wProductType == VER_NT_WORKSTATION )		res << L"Windows Vista ";
				else												res << L"Windows Server 2008 ";
			}
			else if ( osvi.dwMinorVersion == 1 )
			{
				if( osvi.wProductType == VER_NT_WORKSTATION )		res << L"Windows 7 ";
				else												res << L"Windows Server 2008 R2 ";
			}

			DWORD dwType;
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

		//res << L" (build " << uint(osvi.dwBuildNumber) << L")";
		if ( osvi.dwMajorVersion >= 6 )
		{
			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )		res << L", 64-bit";
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )	res << L", 32-bit";
		}

		for (int i=0; i<(int)res.Length(); i++)
			osinfo.Description[i] = (BYTE)res[i];
		osinfo.Description[res.Length()] = 0;

		osinfo.PlatformId	= osvi.dwPlatformId;
		osinfo.MajorVersion	= osvi.dwMajorVersion;
		osinfo.MinorVersion	= osvi.dwMinorVersion;
		osinfo.BuildNumber	= osvi.dwBuildNumber;
		osinfo.SuiteMask	= osvi.wSuiteMask;

		return true; 
	}

	return false;
}

int Initialization (void){
	//////////////////////////////////////////////////////////////////////////
	/*	by HUMUS : http://www.humus.name/index.php?page=3D
	Force the main thread to always run on CPU 0.
	This is done because on some systems QueryPerformanceCounter returns a bit different counter values
	on the different CPUs (contrary to what it's supposed to do), which can cause negative frame times
	if the thread is scheduled on the other CPU in the next frame. This can cause very jerky behavior and
	appear as if frames return out of order.
	*/
	SetThreadAffinityMask( GetCurrentThread(), 1 );

	//////////////////////////////////////////////////////////////////////////
	// prevent windows to show it's messages
	//////////////////////////////////////////////////////////////////////////
	SetErrorMode(SEM_FAILCRITICALERRORS);

	//////////////////////////////////////////////////////////////////////////
	//	get OS version and information
	//////////////////////////////////////////////////////////////////////////
	ZeroMemory(&g_OSInfo, sizeof(OSInfo));
	_GetOSInfo(g_OSInfo);

	//////////////////////////////////////////////////////////////////////////
	//	get monitor information
	//////////////////////////////////////////////////////////////////////////
	RECT rc; rc.left = 0; rc.top = 0; rc.right = 10; rc.bottom = 10;
	HMONITOR hMonitor = MonitorFromRect( &rc, MONITOR_DEFAULTTONEAREST );

	MONITORINFO monitorInfo;
	ZeroMemory( &monitorInfo, sizeof(MONITORINFO) );
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo( hMonitor, &monitorInfo );
	g_pDesktopRes.x		= monitorInfo.rcMonitor.right	- monitorInfo.rcMonitor.left;
	g_pDesktopRes.y		= monitorInfo.rcMonitor.bottom	- monitorInfo.rcMonitor.top;

	//////////////////////////////////////////////////////////////////////////
	//	get screen saver activity
	//////////////////////////////////////////////////////////////////////////
	SystemParametersInfo(16, 0, &g_bScreenSaverActivity, 0);

	//////////////////////////////////////////////////////////////////////////
	//	gathering system directory information
	//////////////////////////////////////////////////////////////////////////
	g_sAppFolder = sx::sys::GetCurrentFolder();
	BACKSLASH(g_sAppFolder)

	WCHAR res[1024]; res[0]=0; DWORD len = 1024;
	GetUserName(res, &len);
	g_sUserName = res;

	GetTempPath(1024, res);
	g_sTempFolder = res;
	BACKSLASH(g_sTempFolder)

	GetWindowsDirectory(res, 1024);
	g_sWinFolder = res;
	BACKSLASH(g_sWinFolder)

	//////////////////////////////////////////////////////////////////////////
	//  gathering system information
	//////////////////////////////////////////////////////////////////////////

	//  get total memory
	MEMORYSTATUS minfo;
	minfo.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&minfo);
	g_SystemInfo.MemorySize = (UINT)(minfo.dwTotalPhys / 1024);
	
	//  verify for laptop lid and batteries
	SYSTEM_POWER_CAPABILITIES spc;
	CallNtPowerInformation(SystemPowerCapabilities, NULL, 0, &spc,	sizeof(SYSTEM_POWER_CAPABILITIES));
	g_SystemInfo.LidPresent = spc.LidPresent>0;
	g_SystemInfo.BatteriesPresent = spc.SystemBatteriesPresent>0;

	//  get number of processors
	SYSTEM_INFO sinfo;
	GetSystemInfo(&sinfo);
	g_SystemInfo.CPU.ProcessorCount = sinfo.dwNumberOfProcessors;

	//  get CPU clock
	PROCESSOR_POWER_INFORMATION ppi[MAX_CPU_COUNT];
	CallNtPowerInformation(ProcessorInformation, NULL, 0, ppi,	sizeof(PROCESSOR_POWER_INFORMATION)*MAX_CPU_COUNT);
	g_SystemInfo.CPU.Clock = ppi[0].MaxMhz;

	//  get CPU features
	g_SystemInfo.CPU.mmx	= IsProcessorFeaturePresent(PF_MMX_INSTRUCTIONS_AVAILABLE)>0;
	g_SystemInfo.CPU.sse	= IsProcessorFeaturePresent(PF_XMMI_INSTRUCTIONS_AVAILABLE)>0;
	g_SystemInfo.CPU.sse2	= IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE)>0;
	g_SystemInfo.CPU.now3d	= IsProcessorFeaturePresent(PF_3DNOW_INSTRUCTIONS_AVAILABLE)>0;

	// get CPU model, family, stepping
	int _CPUInfo[4] = {-1};
	__cpuid(_CPUInfo, 1);
	g_SystemInfo.CPU.SteppingID	= (_CPUInfo[0] & 0xf);
	g_SystemInfo.CPU.Model		= (_CPUInfo[0] >> 4) & 0xf;
	g_SystemInfo.CPU.Family		= (_CPUInfo[0] >> 8) & 0xf;

	// Interpret CPU brand string and cache information.
	memset(g_SystemInfo.CPU.BrandName, 0, sizeof(g_SystemInfo.CPU.BrandName));
	__cpuid(_CPUInfo, 0x80000002);
	memcpy(g_SystemInfo.CPU.BrandName + 00, _CPUInfo, sizeof(_CPUInfo));
	__cpuid(_CPUInfo, 0x80000003);
	memcpy(g_SystemInfo.CPU.BrandName + 16, _CPUInfo, sizeof(_CPUInfo));
	__cpuid(_CPUInfo, 0x80000004);
	memcpy(g_SystemInfo.CPU.BrandName + 32, _CPUInfo, sizeof(_CPUInfo));
	__cpuid(_CPUInfo, 0x80000006);
	g_SystemInfo.CPU.Cache = (_CPUInfo[2] >> 16) & 0xffff;

	return 0;
}
static int res = Initialization();