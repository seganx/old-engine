/********************************************************************
	created:	2012/04/14
	filename: 	os.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file you can find so many functions work with os, 
				directories, strings, files, thread and etc ...
*********************************************************************/
#ifndef GUARD_OS_HEADER_FILE
#define GUARD_OS_HEADER_FILE

#include "../Engine_def.h"

#ifndef DRIVE_UNKNOWN
#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6
#endif

//! specify how to sort files in a list of file info
#define SFT_
enum SortFilesType
{
	SFT_BYNAME = 0,
	SFT_BYTYPE,
	SFT_BYSIZE,
	SFT_BYDATE
};

#define BS_
enum BatteryState
{
	BS_AcOnline = 0,	// power source connected to AC adapter and battery is charged
	BS_Charging,		// the battery in currently charging
	BS_Discharging		// the battery is in use
};

//! time structure
struct TimeStruct
{
	word	year;
	word	month;
	word	dayOfWeek;
	word	day;
	word	hour;
	word	minute;
	word	second;
	word	milliSeconds;
};

//! monitor info
typedef struct MonitorInfo
{
	sint	areaWidth;
	sint	areaHeight;
	sint	workingWidth;
	sint	workingHeight;
} 
DesktopInfo;

//! contain information of current processor(s)
struct CPUInfo
{
	wchar	brand[0x40];
	sint	processors;
	sint	clock;			// MHz
	sint	cache;			// KB
	bool	sse;
	bool	sse2;
	bool	mmx;
	bool	now3d;
};

struct OSInfo
{
	wchar	desc[256];
	dword	major;
	dword	minor;
};

//!  retrieve system information
struct SystemInfo
{
	OSInfo		os;
	CPUInfo		cpu;
	uint		memory;
	bool		hasLid;
	bool		hasBattery;
};

//! retrieve battery information
struct BatteryStatus
{
	wchar			estimatedTime[8];	// The estimated time remaining on the battery, in seconds.
	uint			maxCap;				// The theoretical capacity of the battery when new, in mWh.
	uint			currCap;			// The estimated remaining capacity of the battery, in mWh.
	bool			hasBattery; 		// will be true if any battery detected
	bool			alert;				// true if system in going to turn off
	bool			warning;			// true if battery is going to empty
	BatteryState	state;				// battery state

};

//! contain cpu information per processor
typedef struct CPUStatus
{
	uint	maxMhz;			//  maximum speed of specified processor
	uint	currMhz;		//  current usage of specified processor
}
CPUStatusArray[32];

//! contain disk drive info and drive name
struct DriveInfo
{
	uint	size;
	uint	free;
	uint	type;		//  DRIVE_
	wchar	name;
	wchar	label[64];
	wchar	typeName[64];
};
typedef Array<DriveInfo>	DriveInfoArray;

//! contain file info and file name - size in Byte
struct FileInfo
{
	uint64		size;			//  size of in Byte
	uint		flag;
	wchar		name[256];
	wchar		type[32];
	TimeStruct	created;
	TimeStruct	modified;
};
typedef  Array<FileInfo>	FileInfoArray;


//! callback function will tell us what happening there
typedef sint (*CB_RemoveDir)(void* userdata, const sint count, const sint index, const wchar* srcPath, FileInfo& finfo);
typedef sint (*CB_CopyDir)(void* userdata, const sint count, const sint index, const wchar* srcPath, FileInfo& finfo, const wchar* destPath);

//! initialize system
SEGAN_ENG_API void sx_os_initialize( void );

//! finalize system
SEGAN_ENG_API void sx_os_finalize( void );

//! stop thread for a while in milliseconds
SEGAN_ENG_API void sx_os_sleep( const uint miliseconds );

//! enter critical section
SEGAN_ENG_API void sx_os_enter_critical_section( void );

//! leave critical section
SEGAN_ENG_API void sx_os_leave_critical_section( void );

//! return current system time in milliseconds
SEGAN_ENG_API double sx_os_get_timer( void );

//! return date in YYYY/MM/DD format
SEGAN_ENG_API const wchar* sx_os_get_date( void );

//! return time in HH::MM::SS format
SEGAN_ENG_API const wchar* sx_os_get_time( void );

//! return date and time stamp in YYYY/MM/DD HH:MM:SS
SEGAN_ENG_API const wchar* sx_os_get_time_stamp( void );

//! copy string to the clipboard
SEGAN_ENG_API void sx_os_copy_text_to_clipboard( const wchar* text );

//! return text in the clipboard. return null if no text found
SEGAN_ENG_API const wchar* sx_os_paste_text_from_clipboard( void );

//! return available memory sample in KB
SEGAN_ENG_API uint sx_os_get_available_memory( void );

//! return operating system information
SEGAN_ENG_API void sx_os_get_info( OUT OSInfo& osInfo );

//! return cpu information on system
SEGAN_ENG_API void sx_os_get_cpu_info( OUT CPUInfo& cpuInfo );

//! return system information
SEGAN_ENG_API void sx_os_get_system_info( SystemInfo& sysInfo );

//! return cpu status
SEGAN_ENG_API void sx_os_get_cpu_status( CPUStatus* pCPUStatus, const sint numberOfCPU );

//! return battery status
SEGAN_ENG_API void sx_os_get_battery_status( BatteryStatus& batteryStatus );

//! set screen saver activity
SEGAN_ENG_API void sx_os_set_screensaver_activity( bool active );

//! restore default screen saver settings
SEGAN_ENG_API void sx_os_set_screensaver_default( void );

//! prevent system to going to sleep or turn display off
SEGAN_ENG_API void sx_os_keep_wakeful( void );

//! return monitor dimensions
SEGAN_ENG_API MonitorInfo* sx_os_get_monitor( sint x = 0, sint y = 0 );

//! return desktop dimensions
SEGAN_ENG_API DesktopInfo* sx_os_get_desktop( sint x = 0, sint y = 0 );

//! return true if file was exist
SEGAN_ENG_API bool sx_os_file_exist( const wchar* fileName );

//! return true if directory exist
SEGAN_ENG_API bool sx_os_dir_exist( const wchar* dirName );

//! return the current user name
SEGAN_ENG_API const wchar* sx_os_get_user_name( void );

//! return current directory path
SEGAN_ENG_API const wchar* sx_os_get_current_dir( void );

//! set current directory path
SEGAN_ENG_API void sx_os_set_current_dir( const wchar* path );

//! return executional file path
SEGAN_ENG_API const wchar* sx_os_get_app_dir( void );

//! return system temporary folder
SEGAN_ENG_API const wchar* sx_os_get_temp_dir( void );

//! return os folder
SEGAN_ENG_API const wchar* sx_os_get_dir( void );

//! return user desktop folder
SEGAN_ENG_API const wchar* sx_os_get_desktop_dir( void );

//! return user documents folder
SEGAN_ENG_API const wchar* sx_os_get_documents_dir( void );

//! get list of drives
SEGAN_ENG_API bool sx_os_get_drives( DriveInfoArray& ddList );

//! return drive type in DRIVE_ series
SEGAN_ENG_API uint sx_os_get_drive_type( wchar DriveName );

//! return space of specified disk drive in MB
SEGAN_ENG_API uint sx_os_get_drive_space( wchar DriveName );

//! return free space of specified disk drive in MB
SEGAN_ENG_API uint sx_os_get_drive_free_space( wchar DriveName );

//! return label of specified disk drive
SEGAN_ENG_API const wchar* sx_os_get_drive_label( wchar DriveName );

//! extract information of drive
SEGAN_ENG_API void sx_os_get_drive_info( wchar DriveName, OUT DriveInfo& dinfo );

//! extract information of file of directory
SEGAN_ENG_API bool sx_os_get_file_info( const wchar* FileName, OUT FileInfo& finfo );

//! Delete a file
SEGAN_ENG_API bool sx_os_remove_file( const wchar* fileName );

//! copy a file to the new location
SEGAN_ENG_API bool sx_os_copy_file( const wchar* srcFile, const wchar* destFile );

//! move a file to the new location
SEGAN_ENG_API bool sx_os_move_file( const wchar* srcFile, const wchar* destFile );

//! Rename a file
SEGAN_ENG_API bool sx_os_rename_file( const wchar* srcFile, const wchar* newName );

//! Get list of files in a directory
SEGAN_ENG_API void sx_os_get_files( const wchar* path, const wchar* exten, OUT FileInfoArray& fileList );

//! Sort list of files by given sort type
SEGAN_ENG_API void sx_os_sort_files( SortFilesType SFT_ sftype, FileInfoArray& fileList );

//! Get list of all files in a folder and subdirectories
SEGAN_ENG_API void sx_os_get_filesX( const wchar* path, const wchar* exten, FileInfoArray& fileList );

//! Make a folder by name
SEGAN_ENG_API bool sx_os_make_dir( const wchar* folderName );

//! Delete a folder and it's content
SEGAN_ENG_API bool sx_os_remove_dir( const wchar* folderName, CB_RemoveDir callback = null, void* userdata = null );

//! Copy a folder and it's content to the new location
SEGAN_ENG_API bool sx_os_copy_dir( const wchar* srcFolder, const wchar* destFolder, CB_CopyDir callback = null, void* userdata = null );

//! Move a folder and it's content to the new location
SEGAN_ENG_API bool sx_os_move_dir( const wchar* srcFolder, const wchar* destFolder, CB_CopyDir callback = null, void* userdata = null );

//! Rename a folder
SEGAN_ENG_API bool sx_os_rename_dir( const wchar* srcFolder, const wchar* newName );


#endif	//	GUARD_OS_HEADER_FILE

