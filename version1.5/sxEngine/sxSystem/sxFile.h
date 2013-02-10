/********************************************************************
	created:	2010/07/19
	filename: 	sxFile.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain functions and classes for file handling
*********************************************************************/
#ifndef GUARD_sxFile_HEADER_FILE
#define GUARD_sxFile_HEADER_FILE

#include "../../sxLib/lib.h"
#include "sxSystem_def.h"

// FileStream create mode 
#define FM_
#define	FM_CREATE				0xFFFF		// If the file exists, open for write access, otherwise, create a new file

#define	FM_OPEN_READ			0x0000		// Open for read access only
#define	FM_OPEN_WRITE			0x0001		// Open for write access only
#define	FM_OPEN_READ_WRITE		0x0002		// Open for read and write access

#define	FM_SHARE_READ			0x0010		// Write access for other thread is denied
#define	FM_SHARE_WRITE			0x0020		// Read access for other thread is denied - not supported on all platforms
#define	FM_SHARE_READ_WRITE		0x0030		// Allows full access for others.


/*
base of file stream that use baseStream class functions and 
some additional functions to handle Files.
*/
class SEGAN_API baseFileStream : public Stream
{
	SEGAN_STERILE_CLASS(baseFileStream);

public:
	baseFileStream(void);
	~baseFileStream(void);

	//! open a new file by given file name and mode
	bool Open(const WCHAR* FileName, DWORD FM_ mode);

	//! close current file
	void Close();

	uint Write( const void* buf, const uint size );
	uint Read( void* buf, const uint size );
	uint Seek ( SeekType ST_ seekType, const uint offset = 0 );

	static bool FileExist(const WCHAR* FileName);

private:
	HANDLE fHandle;
};

namespace sx { namespace sys
{
	/*
	File stream that use Stream class functions and 
	some additional functions to handle Files.
	*/
	typedef baseFileStream	StreamFile,	FileStream, *PStreamFile,	*PFileStream;

} } // namespace sx { namespace sys

#endif // GUARD_sxFile_HEADER_FILE
