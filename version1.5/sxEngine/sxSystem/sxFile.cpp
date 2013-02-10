#include "sxFile.h"
#include "sxLog.h"

baseFileStream::baseFileStream(): Stream()
{
	fHandle = NULL;
}

baseFileStream::~baseFileStream()
{
	Close();
}

bool baseFileStream::Open( const WCHAR* FileName, DWORD FM_ mode )
{
	if (mode == FM_CREATE)
		fHandle = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	else
	{
		const DWORD AccessMode[3] = {GENERIC_READ, GENERIC_WRITE, GENERIC_READ | GENERIC_WRITE};
		const DWORD ShareMode[4] = {0, FILE_SHARE_READ, FILE_SHARE_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE};

		if ( ((mode & 3) <= FM_OPEN_READ_WRITE) && ((mode & 0xF0) <= FM_SHARE_READ_WRITE) )
			fHandle = CreateFile(	FileName, 
			AccessMode[(mode & 3)], 
			ShareMode[((mode & 0xF0) >> 4)], 
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, 
			0);
	}

	if (fHandle == INVALID_HANDLE_VALUE)
	{
		fHandle = NULL;
		return false;
	}
	else
	{
		m_size = Seek(ST_END);
		Seek(ST_BEGIN);
		return true;
	}
}

void baseFileStream::Close()
{
	if (fHandle)
	{
		if ( !CloseHandle(fHandle) )
		{
			sxLog::Log(L"Closing file handle failed !");
		}
		
		fHandle = NULL;
	}
}

uint baseFileStream::Write( const void* buf, const uint size )
{
	if (size<=0) return 0;
	DWORD Result;
	UINT currPos = Seek(ST_CUR);
	if (!WriteFile(fHandle, buf, size, &Result, NULL))
		return 0;
	if (currPos+size>m_size)
		m_size=currPos+size;
	return Result;
}

uint baseFileStream::Read(void* buf, const uint size)
{
	if (size<=0) return 0;
	DWORD Result = 0;
	if (!ReadFile(fHandle, buf, size, &Result, NULL))
		return 0;
	return Result;
}

UINT baseFileStream::Seek(SeekType seekType, const uint offset)
{
	return SetFilePointer(fHandle, offset, NULL, (DWORD)seekType);
}

bool baseFileStream::FileExist(const WCHAR* FileName)
{
	if (!FileName) return false;
	DWORD Res = GetFileAttributes(FileName);
	return ( Res!=INVALID_FILE_ATTRIBUTES && !SEGAN_SET_HAS(Res, FILE_ATTRIBUTE_DIRECTORY) );
}
