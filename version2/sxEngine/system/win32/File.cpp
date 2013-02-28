#if defined(_WIN32)

#include "../File.h"
#include "../Log.h"
#include "Win6.h"


FileStream::FileStream(): Stream()
{
	m_handle = null;
}

FileStream::~FileStream()
{
	Close();
}

bool FileStream::Open( const wchar* filename, const dword FM_ mode )
{
	m_fileName = filename;

	if ( mode == FM_CREATE ) {
		m_handle = CreateFile( filename, GENERIC_WRITE, 0, null, CREATE_ALWAYS, 0, null );
	} else {
		const dword AccessMode[3] = {GENERIC_READ, GENERIC_WRITE, GENERIC_READ | GENERIC_WRITE};
		const dword ShareMode[4] = {0, FILE_SHARE_READ, FILE_SHARE_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE};

		if ( ((mode & 3) <= FM_OPEN_READ_WRITE) && ((mode & 0xF0) <= FM_SHARE_READ_WRITE) ) {
			m_handle = CreateFile(
				filename, 
				AccessMode[(mode & 3)], 
				ShareMode[((mode & 0xF0) >> 4)], 
				null,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, 
				0
				);
		}
	}

	if ( m_handle == INVALID_HANDLE_VALUE )
	{
		m_handle = null;
		g_logger->Log( L"Error : Opening file '%s' failed !", filename );
		return false;
	}
	else
	{
		m_size = Seek( ST_END );
		Seek( ST_BEGIN );
		return true;
	}
}

void FileStream::Close()
{
	if ( m_handle )
	{
		if ( !CloseHandle( m_handle ) )
		{
			g_logger->Log( L"Error : Closing file '%s' failed !", m_fileName.Text() );
		}
		m_handle = null;
	}

	m_fileName.Clear();
}

uint FileStream::Write( const void* buf, const uint size )
{
	if ( size == 0 || !m_handle ) return 0;
	DWORD Result = 0;
	uint currPos = Seek( ST_CUR );
	if ( !WriteFile( m_handle, buf, size, &Result, null ) ) {
		g_logger->Log( L"Error : Writing to file '%s' failed !", m_fileName.Text() );
		return 0;
	}
	if ( currPos + size > m_size ) m_size = currPos + size;
	return (uint)Result;
}

uint FileStream::Read( void* buf, const uint size )
{
	if ( size == 0 || !m_handle ) return 0;
	DWORD Result = 0;
	if ( !ReadFile( m_handle, buf, size, &Result, null ) ) {
		g_logger->Log( L"Error : Reading from file '%s' failed !", m_fileName.Text() );
		return 0;
	}
	return Result;
}

uint FileStream::Seek( SeekType seekType, const uint offset )
{
	if ( !m_handle ) return 0;
	return SetFilePointer( m_handle, offset, null, (dword)seekType );
}

bool FileStream::FileExist( const wchar* FileName )
{
	if ( !FileName ) return false;
	dword Res = GetFileAttributes( FileName );
	return ( Res != INVALID_FILE_ATTRIBUTES && !(Res & FILE_ATTRIBUTE_DIRECTORY) );
}


#endif
