/********************************************************************
	created:	2012/04/06
	filename: 	File.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain functions and classes for file handling
*********************************************************************/
#ifndef GUARD_File_HEADER_FILE
#define GUARD_File_HEADER_FILE

#include "System_def.h"

//! FileStream create mode 
#define FM_
#define	FM_CREATE				0xFFFF		// If the file exists then overwrite and otherwise create a new file
#define	FM_OPEN_READ			0x0000		// Open for read access only
#define	FM_OPEN_WRITE			0x0001		// Open for write access only
#define	FM_OPEN_READ_WRITE		0x0002		// Open for read and write access
#define	FM_SHARE_READ			0x0010		// Write access for other thread is denied
#define	FM_SHARE_WRITE			0x0020		// Read access for other thread is denied - not supported on all platforms
#define	FM_SHARE_READ_WRITE		0x0030		// Allows full access for others.


//! file stream use Stream class functions and some additional functions to handle files.
class SEGAN_ENG_API FileStream : public Stream
{
	SEGAN_STERILE_CLASS(FileStream);

public:
	FileStream( void );
	virtual ~FileStream( void );

	//! open a new file by given file name and mode
	bool Open( const wchar* FileName, const dword FM_ mode );

	//! close current file
	void Close( void );

	//! write data to file
	uint Write( const void* buf, const uint size);

	//! read data from file
	uint Read( void* buf, const uint size );

	//! seek in the file
	uint Seek( SeekType ST_ seekType, const uint offset = 0 );

	//! return true if file exist
	static bool FileExist( const wchar* FileName );

private:

	String	m_fileName;
	void*	m_handle;

};
typedef FileStream StreamFile, *PStreamFile, *PFileStream;

#endif // GUARD_File_HEADER_FILE
