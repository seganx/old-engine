#include "FileMan.h"
#include "File.h"
#include "OS.h"
#include "Log.h"

//  define the separator character of the file name
#define FILEMAN_CHAR_SEPARATOR	'^'

struct Package
{
	wchar	name[128];

	//  other information of package will be here
	//  ...
};

bool FindPackage( FileManager* fileMan, const wchar* packName, OUT Package*& package)
{
	str128 currName = packName;
	for ( int i=0; i<fileMan->m_packs.Count(); i++ )
	{
		if ( packName == fileMan->m_packs[i]->name )
		{
			package = fileMan->m_packs[i];
			return true;
		}
	}
	return false;
}


bool FileManager::Project_Open( const wchar* projectDir, FMM_ FileManMode mode )
{
	//  cleanup and close last opened project
	Project_Close();
	
	if ( !sx_os_dir_exist( projectDir ) )
	{
		g_logger->Log( L"File manager said : the requested to open project folder [%s] failed due to the directory is not exist !", projectDir );
		return false;
	}

	m_mode = mode;
	
	m_root = projectDir;
	m_root.MakePathStyle();

	if ( mode == FMM_ARCHIVE )
	{
		str1024	path = m_root;
		path << SEGAN_PACKAGENAME_GEOMETRY;
		sx_os_make_dir( path );

		path = m_root;
		path << SEGAN_PACKAGENAME_ANIMATION;
		sx_os_make_dir( path );

		path = m_root;
		path << SEGAN_PACKAGENAME_TEXTURE;
		sx_os_make_dir( path );

		path = m_root;
		path << SEGAN_PACKAGENAME_EFFECT;
		sx_os_make_dir( path );

		path = m_root;
		path << SEGAN_PACKAGENAME_PHYSICS;
		sx_os_make_dir( path );

		path = m_root;
		path << SEGAN_PACKAGENAME_MEIDA;
		sx_os_make_dir( path );

		path = m_root;
		path << SEGAN_PACKAGENAME_COMMON;
		sx_os_make_dir( path );

		path = m_root;
		path << SEGAN_PACKAGENAME_DRAFT;
		sx_os_make_dir( path );
	}
	else
	{
		//	TODO :
		//  other codes to open current packages will be here
		//  ...
	}

	m_projectIsOpen = true;
	return true;
}

void FileManager::Project_Close( void )
{
	//	TODO :
	//  codes to close the opened packages will be here
	//  ...

	m_root.Clear();
	m_projectIsOpen = false;
}

const wchar* FileManager::Project_GetDir( void )
{
	return m_root;
}

bool FileManager::Package_Create( const wchar* packageName )
{
	if ( !m_projectIsOpen ) return false;

	if  (m_mode == FMM_ARCHIVE )
	{
		Package* newPack = NULL;
		if ( FindPackage( this, packageName, newPack))
		{
			g_logger->Log( L"File manager said : the requested new package '%s' is already exist !", packageName );				
			return false;
		}

		newPack = sx_new( Package );
		sx_str_copy( newPack->name, 128, packageName );
		m_packs.PushBack( newPack );

		str1024 path = m_root;
		path << packageName;
		return sx_os_make_dir( path );
	}
	
	// else

	g_logger->Log( L"File manager said : I'm in Zipped Packages mode and the requested new package '%s' can not be created !", packageName );
	return false;
}

bool FileManager::Package_Delete( const wchar* packageName )
{
	if ( !m_projectIsOpen ) return false;
	
	if ( m_mode == FMM_ARCHIVE )
	{
		Package* curPack = NULL;
		if ( !FindPackage( this, packageName, curPack ) )
		{
			g_logger->Log( L"File manager said : the request to delete package '%s' failed because the package is not exist !", packageName );
			return false;
		}

		m_packs.Remove( curPack );
		sx_delete(curPack);

		str1024 path = m_root;
		path << packageName;
		return sx_os_remove_dir( path );
	}

	// else

	g_logger->Log( L"File manager said : I'm in Zipped mode and the requested to delete package '%s' can not be established !", packageName);
	return false;
}

bool FileManager::Package_GetPath( const wchar* packageName, OUT str1024& outPath )
{
	if (!m_projectIsOpen) return false;
	
	if ( m_mode == FMM_ARCHIVE )
	{
		outPath = m_root;
		outPath << packageName;
		outPath.MakePathStyle();
		return true;
	}

	Package* pkg = NULL;
	if ( FindPackage( this, packageName, pkg ) )
	{
		outPath = m_root;
		outPath << packageName;
		return true;
	}
	return false;
}

bool FileManager::File_Exist( const wchar* fileName, const wchar* packageName )
{
	if ( !fileName ) return false;

	bool res = false;

	//  check to see if fileName was full file path then check the file directly
	if ( sx_str_is_pathstyle( fileName ) )
	{
		res = sx_os_file_exist( fileName );
		if ( !res )
			g_logger->Log( L"File manager said : file '%s' is not exist !", fileName );
	}
	else
	{
		if ( !m_projectIsOpen ) return false;
		if ( m_mode == FMM_ARCHIVE )
		{
			str1024 path = m_root;
			path << packageName << PATH_PART << fileName;
			res = sx_os_file_exist( path );
			if ( !res )
				g_logger->Log( L"File manager said : file [%s] is not exist !", path.Text() );
		}
		else
		{
			//	TODO :
			//  add codes of zipped mode here
		}
	}

	return res;
}

bool FileManager::File_Open( const wchar* fileName, const wchar* packageName, Stream*& outStream )
{
	outStream = NULL;
	if ( !fileName ) return false;

	//  check to see if fileName was full file path then open the file directly
	str1024 path = fileName;
	if ( sx_str_is_fullpath( fileName ) )
	{
		FileStream* sfile = sx_new( FileStream );
		if ( sfile->Open( path, FM_OPEN_READ | FM_SHARE_READ ) )
		{
			sfile->Seek( ST_BEGIN );
			outStream = (Stream*)sfile;
			return true;
		}
		else 
		{
			g_logger->Log( L"File manager said : the request to open file '%s' failed !", path.Text() );
			sx_delete( sfile );
			outStream = NULL;
			return false;
		}
	}

	//  new we are sure that file name is a related file name
	if ( !m_projectIsOpen ) return false;

	path = m_root;
	if ( m_mode == FMM_ARCHIVE )
	{
		path << packageName << PATH_PART << fileName;

		FileStream* sfile = sx_new( FileStream );
		if ( sfile->Open( path, FM_OPEN_READ | FM_SHARE_READ ) )
		{
			sfile->Seek( ST_BEGIN );
			outStream = (Stream*)sfile;
			return true;
		}
		else 
		{
			g_logger->Log( L"File manager said : the request to open file '%s' failed !", path.Text() );
			sx_delete( sfile );
			outStream = NULL;
			return false;
		}
	}
	// else

	//	TODO :
	//  add codes of zipped mode here

	return false;
}

void FileManager::File_Close( Stream*& stream )
{
	sx_delete_and_null( stream );
}

bool FileManager::File_Save( const wchar* fileName, const wchar* packageName, Stream& srcStream )
{
	if ( !fileName ) return false;
	if ( m_projectIsOpen && !packageName ) return false;

	str1024 path = m_root;
	if ( m_mode == FMM_ARCHIVE )
	{
		if ( packageName )
			path << packageName << PATH_PART << fileName;
		else
			path = fileName;

		FileStream sfile;
		if ( sfile.Open( path, FM_CREATE ) )
		{
			uint curPos = srcStream.GetPos();
			srcStream.Seek( ST_BEGIN );
			sfile.CopyFrom( srcStream );
			srcStream.SetPos( curPos );
			sfile.Close();
			return true;
		}
		else 
		{
			g_logger->Log( L"File manager said : the request to save file '%s' failed !", path.Text() );
			return false;
		}
	}
	if ( !m_projectIsOpen ) return false;

	str1024 els = L"File manager said : I'm in Zipped mode and the requested to save file [";
	els << packageName << PATH_PART << fileName << L"] can not be established !";
	g_logger->Log( els );
	return false;
}

bool FileManager::File_GetPath( const wchar* FileName, const wchar* packageName, OUT str1024& outPath )
{
	if (!m_projectIsOpen || !FileName) return false;

	str1024 path = m_root;
	if ( m_mode == FMM_ARCHIVE )
	{
		path << packageName << PATH_PART << FileName;
		outPath = path;
		return true;
	}
	// else

	str1024 els = L"File manager said : I'm in Zipped mode and the requested to get path of [";
	els << packageName << PATH_PART << FileName << L"] can not be established !";
	g_logger->Log( els );
	return false;
}
