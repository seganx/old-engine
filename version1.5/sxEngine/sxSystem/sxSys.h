/********************************************************************
	created:	2010/07/23
	filename: 	sxSys.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		in sxSys you can find so many functions work with OS, 
				directories, strings, files, thread and etc ...
*********************************************************************/
#ifndef GUARD_sxSys_HEADER_FILE
#define GUARD_sxSys_HEADER_FILE

#include "../../sxLib/lib.h"
#include "sxSystem_def.h"

//! contain information of current processor(s)
struct CPUInfo{
	char	BrandName[0x40];
	int		ProcessorCount;
	int		Clock;			// MHz
	int		Cache;			// KB
	BYTE	Family;
	BYTE	Model;
	BYTE	SteppingID;
	bool	sse;
	bool	sse2;
	bool	mmx;
	bool	now3d;
};

struct OSInfo{
	char	Description[256];
	DWORD	MajorVersion;
	DWORD	MinorVersion;
	DWORD	BuildNumber;
	DWORD	PlatformId;
	WORD	SuiteMask;
};

//!  retrieve system information
struct SystemInfo{
	OSInfo		OS;
	CPUInfo		CPU;
	UINT		MemorySize;
	bool		LidPresent;
	bool		BatteriesPresent;
};

//! retrieve battery information
struct BatteryStatus{
	enum BatteryState{
		AcOnline = 0,			// power source connected to AC adapter and battery is charged
		Charging,				// the battery in currently charging
		Discharging				// the battery is in use
	}				State;			// battery state
	UINT			MaxCapacity;	// The theoretical capacity of the battery when new, in mWh.
	UINT			CurCapacity;	// The estimated remaining capacity of the battery, in mWh.
	bool			BatteryPresent; // will be true if any battery detected
	bool			Alert;			// true if system in going to turn off
	bool			Warning;		// true if battery is going to empty
	WCHAR			EstimatedTime[8];	// The estimated time remaining on the battery, in seconds.
};

//! contain CPU information per processor
typedef struct CPUStatus{
	UINT	MaxMhz;		//  maximum speed of specified processor
	UINT	CurMhz;		//  current usage of specified processor
} CPUStatusArray[32];


//! contain disk drive info and drive name
struct DriveInfo{
	UINT	size;
	UINT	free;
	UINT	type;	//  DRIVE_
	WCHAR	name;
	WCHAR	label[64];
	WCHAR	typeName[64];
	
	bool operator == (DriveInfo& f){
		return size==f.size && free==f.free && name==f.name && type==f.type;
	}
};
typedef Array<DriveInfo>	DriveInfoArray;
#ifndef DRIVE_UNKNOWN
#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6
#endif

//! contain file info and file name - size in KB
struct FileInfo{
	UINT		size;
	UINT		flag;
	WCHAR		name[256];
	WCHAR		type[32];
	SYSTEMTIME	created;
	SYSTEMTIME	modified;

	bool operator == (FileInfo& f){
		return size==f.size && flag==f.flag && name==f.name && type==f.type;
	}
};
typedef  Array<FileInfo>	FileInfoArray;


//! specify how to sort files in a list of file info
#define SFT_
enum SortFilesType{
	SFT_BYNAME = 0,
	SFT_BYTYPE,
	SFT_BYSIZE,
	SFT_BYDATE
};


//! callback function will tell us what happening there
typedef int (*SysCallback_Remove)(void* UserObject, const int count, const int index, const WCHAR* srcPath, FileInfo& finfo);
typedef int (*SysCallback_Copy)(void* UserObject, const int count, const int index, const WCHAR* srcPath, FileInfo& finfo, const WCHAR* destPath);

namespace sx { namespace sys
{
	//! return current system time in milliseconds
	SEGAN_API float	GetSysTime(void);

	//! return date in YYYY/MM/DD format
	SEGAN_API const WCHAR* GetDate(void);

	//! return time in HH::MM::SS format
	SEGAN_API const WCHAR* GetTime(void);

	//! return date and time stamp in YYYY/MM/DD HH:MM:SS
	SEGAN_API const WCHAR* GetTimeStamp(void);

	//! copy string to the clipboard
	SEGAN_API void CopyText(const WCHAR* text);

	//! return text in the clipboard. return null if no text found
	SEGAN_API const WCHAR* PasteText(void);

	//! return available memory sample in KB
	SEGAN_API UINT GetAvailableMemory(void);

	//! return operating system information
	SEGAN_API void GetOSInfo(OSInfo& osInfo);

	//! return system information
	SEGAN_API void GetSystemInfo(SystemInfo& sysInfo);

	//! return CPU status
	SEGAN_API void GetCPUStatus(CPUStatus* pCPUStatus, const int numberOfCPU);

	//! return battery status
	SEGAN_API void GetBatteryStatus(BatteryStatus& batteryStatus);

	//! set screen saver activity
	SEGAN_API void ScreenSaverSetActivity(bool active);

	//! restore default screen saver settings
	SEGAN_API void ScreenSaverSetDefault(void);

	//! prevent system to going to sleep or turn display off
	SEGAN_API void KeepSystemWakeful(void);

	//! return desktop width
	SEGAN_API UINT GetDesktopWidth(void);

	//! return desktop height
	SEGAN_API UINT GetDesktopHeight(void);

	//! return true if file was exist
	SEGAN_API bool FileExist(const WCHAR* FileName);

	//! return true if directory exist
	SEGAN_API bool FolderExist(const WCHAR* DirName);

	//! return the current user name
	SEGAN_API const WCHAR* GetUserName(void);

	//! return current directory path
	SEGAN_API const WCHAR* GetCurrentFolder(void);

	//! set current directory path
	SEGAN_API void SetCurrentFolder(const WCHAR* path);

	//! return executional file path
	SEGAN_API const WCHAR* GetAppFolder(void);

	//! return system temporary folder
	SEGAN_API const WCHAR* GetTempFolder(void);

	//! return windows folder
	SEGAN_API const WCHAR* GetWinFolder(void);

	//! return user desktop folder
	SEGAN_API const WCHAR* GetDesktopFolder(void);

	//! return user documents folder
	SEGAN_API const WCHAR* GetDocumentsFolder(void);

	//! get list of drives
	SEGAN_API bool GetDrives(DriveInfoArray* ddList);

	//! return drive type in DRIVE_ series
	SEGAN_API UINT GetDriveKind(WCHAR DriveName);

	//! return space of specified disk drive in MB
	SEGAN_API UINT GetDriveSpace(WCHAR DriveName);

	//! return free space of specified disk drive in MB
	SEGAN_API UINT GetDriveFreeSpace(WCHAR DriveName);

	//! return label of specified disk drive
	SEGAN_API const WCHAR* GetDriveLabel(WCHAR DriveName);

	//! extract information of drive
	SEGAN_API void GetDriveInfo(WCHAR DriveName, DriveInfo& dinfo);

	//! extract information of file of directory
	SEGAN_API bool GetFileInfo(const WCHAR* FileName, FileInfo& finfo);

	//! Delete a file
	SEGAN_API bool RemoveFile(const WCHAR* fileName);

	//! copy a file to the new location
	SEGAN_API bool CopyFile(const WCHAR* srcFile, const WCHAR* destFile);

	//! move a file to the new location
	SEGAN_API bool MoveFile(const WCHAR* srcFile, const WCHAR* destFile);

	//! Rename a file
	SEGAN_API bool RenameFile(const WCHAR* srcFile, const WCHAR* newName);

	//! Get list of files in a directory
	SEGAN_API bool GetFilesInFolder(const WCHAR* path, const WCHAR* exten, FileInfoArray* fList, UINT ExceptAttribs = FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY );

	//! Sort list of files by given sort type
	SEGAN_API void SortFilesBy(SFT_ SortFilesType sftype, FileInfoArray* fList);

	//! Get list of all files in a folder and subdirectories
	SEGAN_API bool GetFilesInFolderX(const WCHAR* path, const WCHAR* exten, FileInfoArray* fList, UINT ExceptAttribs = FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY );

	//! Make a folder by name
	SEGAN_API bool MakeFolder(const WCHAR* folderName);

	//! Delete a folder and it's content
	SEGAN_API bool RemoveFolder(const WCHAR* folderName, SysCallback_Remove callback = NULL, void* UserObject = NULL);

	//! Copy a folder and it's content to the new location
	SEGAN_API bool CopyFolder(const WCHAR* srcFolder, const WCHAR* destFolder, SysCallback_Copy callback = NULL, void* UserObject = NULL);

	//! Move a folder and it's content to the new location
	SEGAN_API bool MoveFolder(const WCHAR* srcFolder, const WCHAR* destFolder, SysCallback_Copy callback = NULL, void* UserObject = NULL);

	//! Rename a folder
	SEGAN_API bool RenameFolder(const WCHAR* srcFolder, const WCHAR* newName);

} } // namespace sx { namespace sys

#endif // GUARD_sxSys_HEADER_FILE
