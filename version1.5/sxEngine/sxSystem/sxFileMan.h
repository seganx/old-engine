/********************************************************************
	created:	2010/09/04
	filename: 	sxFileMan.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain class of file manager to manage 
				project's files and directory. this class has two 
				modes for archiving files which listed bellow:
				FMM_ARCHIVE: packages means open directories that can
							 insert, modify and delete files. this
							 mode is suitable for editors.
				FMM_PACKAGE: packages means single zipped directory that
							 can only open files to read. this mode
							 just use for released project.
*********************************************************************/
#ifndef GUARD_sxFileMan_HEADER_FILE
#define GUARD_sxFileMan_HEADER_FILE

#include "sxSystem_def.h"

//! the name of miscellaneous package will use in file system
#define	 SEGAN_PACKAGENAME_COMMON		L"Common"

//! the name of the texture package will use in file system
#define  SEGAN_PACKAGENAME_TEXTURE		L"Texture"

//! the name of the geometry package will use in file system
#define  SEGAN_PACKAGENAME_GEOMETRY		L"Geometry"

//! the name of the animation package will use in file system
#define  SEGAN_PACKAGENAME_ANIMATION	L"Animation"

//! the name of the effect package will use in file system
#define  SEGAN_PACKAGENAME_EFFECT		L"Effect"

//! the name of the physics package will use in file system
#define  SEGAN_PACKAGENAME_PHYSICS		L"Physics"

//! the name of the media package will use in file system
#define  SEGAN_PACKAGENAME_MEIDA		L"MultiMedia"

//! the name of the draft package will use in file system
#define  SEGAN_PACKAGENAME_DRAFT		L"Draft"


#define FMM_
enum FileManMode{
	FMM_ARCHIVE,
	FMM_PACKAGE
};

namespace sx { namespace sys
{
	/*
	Standard file manager to manage files and folders. 
	*/
	class SEGAN_API FileManager
	{
	public:

		//! open if exist / create a project directory contain packages and files.
		static bool Project_Open(const WCHAR* ProjectDir, FMM_ FileManMode fMode);

		//! close a project directory and release packages if mode is FMM_PACKAGE
		static void Project_Close(void);

		//! return the address of opened project directory. return null if no project is in use.
		static const WCHAR* Project_GetDir(void);

		//! create a new packages to separates file types. this function works only in archive mode.
		static bool Package_Create(const WCHAR* PackageName);

		//! delete a packages. this function works only in archive mode.
		static bool Package_Delete(const WCHAR* PackageName);

		//! Get the full path of a package. this function works only in archive mode.
		static bool Package_GetPath(const WCHAR* PackageName, OUT String& outPath);

		//! return true if the file is exist in specified package
		static bool File_Exist(const WCHAR* FileName, const WCHAR* PackageName);

		//! load a file from package and send out new Stream. So use File_Close to close that Stream after the work has finished.
		static bool File_Open(const WCHAR* FileName, const WCHAR* PackageName, PStream& stream);

		//! close an stream that opened by File_Open function
		static void File_Close(PStream& stream);

		//! save the all data in srcStream to the a file. this function works only in archive mode.
		static bool File_Save(const WCHAR* FileName, const WCHAR* PackageName, Stream& srcStream);

		//! Get the full path of a file in the specified package. this function works only in archive mode.
		static bool File_GetPath(const WCHAR* FileName, const WCHAR* PackageName, OUT String& outPath);
	};


} } // namespace sx { namespace sys

#endif // GUARD_sxFileMan_HEADER_FILE
