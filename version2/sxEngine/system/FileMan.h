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

#include "System_def.h"

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

/*
Standard file manager to manage files and folders. 
*/
class SEGAN_ENG_API FileManager
{
	SEGAN_STERILE_CLASS(FileManager);

public:

	//! open if exist / create a project directory contain packages and files.
	bool Project_Open(const wchar* projectDir, FMM_ FileManMode mode);

	//! close a project directory and release packages if mode is FMM_PACKAGE
	void Project_Close(void);

	//! return the address of opened project directory. return null if no project is in use.
	const wchar* Project_GetDir(void);

	//! create a new packages to separates file types. this function works only in archive mode.
	bool Package_Create(const wchar* packageName);

	//! delete a packages. this function works only in archive mode.
	bool Package_Delete(const wchar* packageName);

	//! Get the full path of a package. this function works only in archive mode.
	bool Package_GetPath(const wchar* packageName, OUT str1024& outPath);

	//! return true if the file is exist in specified package
	bool File_Exist(const wchar* fileName, const wchar* packageName);

	//! load a file from package and send out new Stream. So use File_Close to close that Stream after the work has finished.
	bool File_Open(const wchar* fileName, const wchar* packageName, Stream*& stream);

	//! close an stream that opened by File_Open function
	void File_Close(Stream*& stream);

	//! save the all data in srcStream to the a file. this function works only in archive mode.
	bool File_Save(const wchar* fileName, const wchar* packageName, Stream& srcStream);

	//! Get the full path of a file in the specified package. this function works only in archive mode.
	bool File_GetPath(const wchar* fileName, const wchar* packageName, OUT str1024& outPath);

public:

	bool						m_projectIsOpen;
	FileManMode					m_mode;
	String						m_root;
	Array<struct Package*>		m_packs;
};

#endif // GUARD_sxFileMan_HEADER_FILE
