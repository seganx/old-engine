#include "sxFileMan.h"
#include "sxFile.h"
#include "sxSys.h"
#include "sxLog.h"

//  define the separator character of the file name
#define FILEMAN_CHAR_SEPARATOR	'^'

typedef struct Package
{
	String	m_Name;

	//  other information of package will be here
	//  ...
} *PPackage;

/*
this class can create and hold some static functions and help me to easy access to them
*/
class FileMan_internal
{
public:
	static bool							m_ProjectIsOpen;
	static FileManMode					m_fMode;
	static String				m_Dir_Root;
	static Array<Package*>		m_Packs;

	static bool FindPackage(const WCHAR* packName, OUT PPackage& package){
		for (int i=0; i<m_Packs.Count(); i++)
		{
			if (m_Packs[i]->m_Name == packName)
			{
				package = m_Packs[i];
				return true;
			}
		}
		return false;
	}
};
bool						FileMan_internal::m_ProjectIsOpen = false;
FileManMode					FileMan_internal::m_fMode = FMM_ARCHIVE;
String				FileMan_internal::m_Dir_Root;
Array<Package*>	FileMan_internal::m_Packs;

namespace sx { namespace sys
{

	bool FileManager::Project_Open( const WCHAR* ProjectDir, FMM_ FileManMode fMode )
	{
		//  cleanup and close last opened project
		Project_Close();
		
		if (!sx::sys::FolderExist(ProjectDir))
		{
			sxLog::Log(L"File manager said : the requested to open project folder [%s] failed due to the directory is not exist !", ProjectDir);
			return false;
		}

		FileMan_internal::m_fMode = fMode;
		
		FileMan_internal::m_Dir_Root = ProjectDir;
		FileMan_internal::m_Dir_Root.MakePathStyle();

		if (fMode == FMM_ARCHIVE)
		{
			String	path = FileMan_internal::m_Dir_Root;
			path << SEGAN_PACKAGENAME_GEOMETRY;
			sx::sys::MakeFolder(path);

			path = FileMan_internal::m_Dir_Root;
			path << SEGAN_PACKAGENAME_ANIMATION;
			sx::sys::MakeFolder(path);

			path = FileMan_internal::m_Dir_Root;
			path << SEGAN_PACKAGENAME_TEXTURE;
			sx::sys::MakeFolder(path);

			path = FileMan_internal::m_Dir_Root;
			path << SEGAN_PACKAGENAME_EFFECT;
			sx::sys::MakeFolder(path);

			path = FileMan_internal::m_Dir_Root;
			path << SEGAN_PACKAGENAME_PHYSICS;
			sx::sys::MakeFolder(path);

			path = FileMan_internal::m_Dir_Root;
			path << SEGAN_PACKAGENAME_MEIDA;
			sx::sys::MakeFolder(path);

			path = FileMan_internal::m_Dir_Root;
			path << SEGAN_PACKAGENAME_COMMON;
			sx::sys::MakeFolder(path);

			path = FileMan_internal::m_Dir_Root;
			path << SEGAN_PACKAGENAME_DRAFT;
			sx::sys::MakeFolder(path);
		}
		else
		{
			//	TODO :
			//  other codes to open current packages will be here
			//  ...
		}

		FileMan_internal::m_ProjectIsOpen = true;
		return true;
	}

	void FileManager::Project_Close( void )
	{
		//	TODO :
		//  codes to close the opened packages will be here
		//  ...

		FileMan_internal::m_Dir_Root.SetText((char*)NULL);
		FileMan_internal::m_ProjectIsOpen = false;
	}

	const WCHAR* FileManager::Project_GetDir( void )
	{
		return FileMan_internal::m_Dir_Root;
	}

	bool FileManager::Package_Create( const WCHAR* PackageName )
	{
		if (!FileMan_internal::m_ProjectIsOpen) return false;

		if (FileMan_internal::m_fMode == FMM_ARCHIVE)
		{
			PPackage newPack = NULL;
			if (FileMan_internal::FindPackage(PackageName, newPack))
			{
				sxLog::Log(L"File manager said : the requested new package [%s] is already exist !", PackageName);				
				return false;
			}

			newPack = sx_new(Package);
			newPack->m_Name = PackageName;
			FileMan_internal::m_Packs.PushBack(newPack);

			String path = FileMan_internal::m_Dir_Root;
			path << PackageName;
			return sx::sys::MakeFolder(path);
		}
		
		// else

		sxLog::Log(L"File manager said : I'm in Zipped Packages mode and the requested new package [%s] can not be created !", PackageName);
		return false;
	}

	bool FileManager::Package_Delete( const WCHAR* PackageName )
	{
		if (!FileMan_internal::m_ProjectIsOpen) return false;
		
		if (FileMan_internal::m_fMode == FMM_ARCHIVE)
		{
			PPackage curPack = NULL;
			if (!FileMan_internal::FindPackage(PackageName, curPack))
			{
				sxLog::Log(L"File manager said : the request to delete package [%s] failed because the package is not exist !", PackageName);
				return false;
			}

			FileMan_internal::m_Packs.Remove(curPack);
			sx_delete_and_null(curPack);

			String path = FileMan_internal::m_Dir_Root;
			path << PackageName;
			return sx::sys::RemoveFolder(path);
		}

		// else

		sxLog::Log(L"File manager said : I'm in Zipped mode and the requested to delete package [%s] can not be established !", PackageName);
		return false;
	}

	bool FileManager::Package_GetPath( const WCHAR* PackageName, OUT String& outPath )
	{
		if (!FileMan_internal::m_ProjectIsOpen) return false;
		
		if ( FileMan_internal::m_fMode == FMM_ARCHIVE )
		{
			outPath = FileMan_internal::m_Dir_Root;
			outPath << PackageName << PATH_PART;
			return true;
		}

		PPackage pkg = NULL;
		if (FileMan_internal::FindPackage(PackageName, pkg))
		{
			outPath = FileMan_internal::m_Dir_Root;
			outPath << PackageName;
			return true;
		}
		return false;
	}

	bool FileManager::File_Exist( const WCHAR* FileName, const WCHAR* PackageName )
	{
		if ( !FileName ) return false;

		bool res = false;

		//  check to see if FileName was full file path then check the file directly
		if (String::IsFullPath(FileName))
		{
			res = sx::sys::FileExist(FileName);

			if ( !res )
				sxLog::Log(L"File manager said : file [%s] is not exist !", FileName);
		}
		else
		{
			if (!FileMan_internal::m_ProjectIsOpen) return false;
			if (FileMan_internal::m_fMode == FMM_ARCHIVE)
			{
				String path = FileMan_internal::m_Dir_Root;
				path << PackageName << PATH_PART << FileName;
				res = sx::sys::FileExist(path);

				if ( !res )
					sxLog::Log(L"File manager said : file [%s] is not exist !", path.Text());
			}
			else
			{
				//	TODO :
				//  add codes of zipped mode here
			}
		}

		return res;
	}

	bool FileManager::File_Open( const WCHAR* FileName, const WCHAR* PackageName, PStream& OutStream )
	{
		sx_callstack_param(FileManager::File_Open(FileName=%s, PackageName=%s), FileName, PackageName);

		OutStream = NULL;
		if (!FileName) return false;

		//  check to see if FileName was full file path then open the file directly
		String path = FileName;
		if (String::IsFullPath(FileName))
		{
			sx::sys::PFileStream sfile = sx_new(sx::sys::FileStream);
			if (sfile->Open(path, FM_OPEN_READ | FM_SHARE_READ))
			{
				sfile->Seek(ST_BEGIN);
				OutStream = (PStream)sfile;

				return true;
			}
			else 
			{
				sxLog::Log(L"File manager said : the request to open file [%s] failed !", *path);
				sx_delete_and_null(sfile);
				OutStream = NULL;

				return false;
			}
		}

		//  new we are sure that file name is a related file name
		if (!FileMan_internal::m_ProjectIsOpen) return false;

		path = FileMan_internal::m_Dir_Root;
		if (FileMan_internal::m_fMode == FMM_ARCHIVE)
		{
			path << PackageName << PATH_PART << FileName;

			sx::sys::PFileStream sfile = sx_new(sx::sys::FileStream);
			if (sfile->Open(path, FM_OPEN_READ | FM_SHARE_READ))
			{
				sfile->Seek(ST_BEGIN);
				OutStream = (PStream)sfile;

				return true;
			}
			else 
			{
				sxLog::Log(L"File manager said : the request to open file [%s] failed !", *path);
				sx_delete_and_null(sfile);
				OutStream = NULL;
				
				return false;
			}
		}
		// else

		//	TODO :
		//  add codes of zipped mode here

		return false;
	}

	void FileManager::File_Close( PStream& stream )
	{
		sx_delete_and_null(stream);
	}

	bool FileManager::File_Save( const WCHAR* FileName, const WCHAR* PackageName, Stream& srcStream )
	{
		if ( !FileName ) return false;
		if ( FileMan_internal::m_ProjectIsOpen && !PackageName ) return false;

		String path = FileMan_internal::m_Dir_Root;

		if (FileMan_internal::m_fMode == FMM_ARCHIVE)
		{
			if ( PackageName )
				path << PackageName << PATH_PART << FileName;
			else
				path = FileName;

			sx::sys::FileStream sfile;
			if (sfile.Open(path, FM_CREATE))
			{
				UINT curPos = srcStream.GetPos();
				srcStream.Seek(ST_BEGIN);
				sfile.CopyFrom(srcStream);
				srcStream.SetPos(curPos);

				sfile.Close();
				return true;
			}
			else 
			{
				sxLog::Log(L"File manager said : the request to save file [%s] failed !", *path);
				return false;
			}
		}
		if ( !FileMan_internal::m_ProjectIsOpen ) return false;

		String els = L"File manager said : I'm in Zipped mode and the requested to save file [";
		els << PackageName << PATH_PART << FileName << L"] can not be established !";
		sxLog::Log(els);
		return false;
	}

	bool FileManager::File_GetPath( const WCHAR* FileName, const WCHAR* PackageName, OUT String& outPath )
	{
		if (!FileMan_internal::m_ProjectIsOpen || !FileName) return false;

		String path = FileMan_internal::m_Dir_Root;

		if (FileMan_internal::m_fMode == FMM_ARCHIVE)
		{
			path << PackageName << PATH_PART << FileName;
			outPath = path;
			return true;
		}
		// else

		String els = L"File manager said : I'm in Zipped mode and the requested to get path of [";
		els << PackageName << PATH_PART << FileName << L"] can not be established !";
		sxLog::Log(els);
		return false;
	}

} } // namespace sx { namespace sys