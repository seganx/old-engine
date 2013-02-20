/********************************************************************
	created:	2012/04/05
	filename: 	Stream.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a base class of any stream data class.
				this class will be ancestor of other stream class like
				memory stream, file stream, etc.
*********************************************************************/
#ifndef GUARD_Stream_HEADER_FILE
#define GUARD_Stream_HEADER_FILE

#include "Memory.h"


#define ST_
enum SeekType{
	ST_BEGIN,
	ST_CUR,
	ST_END
};

/*
Stream object is free type data container use for easy read/write data on stream.
This class will be ancestor of other stream class type like MemoryStream, FileStream, SocketStream ...
*/
class Stream
{
	SEGAN_STERILE_CLASS(Stream);

public:
	Stream( void ): m_size(0) {}
	virtual ~Stream( void ) {}
	virtual uint Write( const void* buf, const uint size ) = 0;
	virtual uint Read( void* buf, const uint size ) = 0;
	virtual uint Seek ( SeekType ST_ seekType, const uint offset = 0 ) = 0;
	virtual uint Size( void ) const {return m_size; }
	virtual uint GetPos( void ) { return Seek( ST_CUR ); }
	virtual void SetPos( uint newPos )
	{
		if ( newPos > m_size )
			newPos = m_size;
		Seek( ST_BEGIN, newPos );
	}
	/*! copy from source stream to current stream amount of size in byte. use size=0 to full copy stream. */
	virtual uint CopyFrom( Stream& source, const uint size = 0 )
	{
		uint result  = size;

		if ( result == 0 )									result = source.Size();
		if ( result > source.Size() - source.GetPos() )		result = source.Size() - source.GetPos();
		if ( !result )										return 0;

		uint tocopy = result;
		byte buffer[0x1fff];
		while ( tocopy > 0 )
		{
			const uint numBytes = ( tocopy > 0x1fff ) ? 0x1fff : tocopy;
			source.Read( buffer, numBytes );
			Write( buffer, numBytes );
			tocopy -= numBytes;
		}
		return result;
	}

	//! write bytes to stream
	SEGAN_LIB_INLINE void WriteByte( const byte& _byte, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Write( &_byte, count * sizeof(byte) );
	}

	//! write signed int 32 bit to stream
	SEGAN_LIB_INLINE void WriteInt32( const i32& _i32, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Write( &_i32, count * sizeof(i32) );
	}

	//! write unsigned int 32 bit to stream
	SEGAN_LIB_INLINE void WriteUInt32( const u32& _u32, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Write( &_u32, count * sizeof(u32) );
	}

	//! write word to stream
	SEGAN_LIB_INLINE void WriteWord( const word& _word, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Write( &_word , count * sizeof(word) );
	}

	//! write dword to stream
	SEGAN_LIB_INLINE void WriteDWord( const dword& _dword, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Write( &_dword , count * sizeof(dword) );
	}

	//! write float to stream
	SEGAN_LIB_INLINE void WriteFloat( const float& _float, const uint count = 1 )
	{
		Write( &_float , count * sizeof(float) );
	}

	//! write double to stream
	SEGAN_LIB_INLINE void WriteDouble( const double& _double, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Write( &_double , count * sizeof(double) );
	}

	//! write char* to stream. leave count to 0 to write all characters in null terminated string
	SEGAN_LIB_INLINE void WriteChar( const char* _char, const uint count = 0 )
	{
		sx_assert( _char );
		uint size = 0;
		if ( !count )
			while ( _char[size++] );
		else
			size = count;
		sx_assert( size );
		Write( _char , size * sizeof(char) );
	}

	//! write WChar to stream. leave count to 0 to write all characters in null terminated string
	SEGAN_LIB_INLINE void WriteWChar( const wchar* _wchar, const uint count = 0 )
	{
		sx_assert( _wchar );
		uint size = 0;
		if ( !count )
			while ( _wchar[size++] );
		else
			size = count;
		sx_assert( size );
		Write( _wchar , size * sizeof(wchar) );
	}

	//! write bool to stream
	SEGAN_LIB_INLINE void WriteBool( const bool& _bool, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Write( &_bool , count * sizeof(bool) );
	}

	//! read bytes to stream
	SEGAN_LIB_INLINE void ReadByte( byte& _byte, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Read( &_byte, count * sizeof(byte) );
	}

	//! read signed int 32 bit to stream
	SEGAN_LIB_INLINE void ReadInt( i32& _i32, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Read( &_i32, count * sizeof(i32) );
	}

	//! read unsigned int 32 bit to stream
	SEGAN_LIB_INLINE void ReadUInt( u32& _u32, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Read( &_u32 , count * sizeof(u32) );
	}

	//! read word to stream
	SEGAN_LIB_INLINE void ReadWord( word& _word, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Read( &_word , count * sizeof(word) );
	}

	//! read dword to stream
	SEGAN_LIB_INLINE void ReadDWord( dword& _dword, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Read( &_dword , count * sizeof(dword) );
	}

	//! read float to stream
	SEGAN_LIB_INLINE void ReadFloat( float& _float, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Read( &_float , count * sizeof(float) );
	}

	//! read double to stream
	SEGAN_LIB_INLINE void ReadDouble( double& _double, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Read( &_double , count * sizeof(double) );
	}

	//! read char to stream
	SEGAN_LIB_INLINE void ReadChar( char& _char, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Read( &_char , count * sizeof(char) );
	}

	//! read WChar to stream
	SEGAN_LIB_INLINE void ReadWChar( wchar& _wchar, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Read( &_wchar , count * sizeof(wchar) );
	}

	//! read bool to stream
	SEGAN_LIB_INLINE void ReadBool( bool& _bool, const uint count = 1 )
	{
		sx_assert( count > 0 );
		Read( &_bool , count * sizeof(bool) );
	}

protected:
	uint	m_size;
};
typedef Stream *PStream;

/*
MemoryStream is free type sequential data container which useful for easy read/write data on memory space.
*/
class MemoryStream : public Stream
{
	SEGAN_STERILE_CLASS(MemoryStream);

public:
	MemoryStream( sint sampler = 0 )
		: Stream()
		, m_buffer(0)
		, m_pointer(0)
		, m_capacity(0)
		, m_sampler(sampler)
	{
	}

	virtual ~MemoryStream( void )
	{
		mem_free( m_buffer );
	}

	SEGAN_LIB_INLINE void Clear( void )
	{
		m_pointer = 0;
		m_size = 0;
	}

	SEGAN_LIB_INLINE uint Write( const void* buf, const uint size )
	{
		if ( size == 0 ) return 0;
		if ( m_pointer + size > m_size )
		{
			m_size = m_pointer + size;
			_ReallocBuffer( m_size );
		}
		memcpy( m_buffer + m_pointer, buf, size );
		m_pointer += size;
		return size;
	}

	SEGAN_LIB_INLINE uint Read( void* buf, const uint size )
	{
		if ( size == 0 ) return 0;
		uint readSize = size; 
		if ( m_pointer + readSize > m_size )
			readSize = m_size - m_pointer;
		memcpy( buf, m_buffer + m_pointer, readSize );
		m_pointer += readSize;
		return readSize;
	}

	SEGAN_LIB_INLINE uint Seek( ST_ SeekType seekType, const uint offset = 0 )
	{
		switch ( seekType ) {
		case ST_BEGIN:	m_pointer = offset;		break;
		case ST_CUR:	m_pointer += offset;	break;
		case ST_END:	m_pointer = ( m_size - offset < 0 ) ? 0 : m_size - offset;
		}
		if ( m_pointer > m_size )
			m_pointer = m_size;
		return m_pointer;
	}

	//! copy data to a buffer and return number of byte copied. use size=0 to full copy stream.
	SEGAN_LIB_INLINE uint CopyTo( void* buffer, const uint sizeinbyte = 0 )
	{
		uint res = sizeinbyte ? sizeinbyte : m_size;
		if ( m_pointer + res > m_size )
			res = m_size - m_pointer;
		memcpy( buffer, m_buffer + m_pointer, res );
		m_pointer += res;
		return res;
	}

	SEGAN_LIB_INLINE operator const void* ( void ) const
	{
		return m_buffer + m_pointer;
	}

private:
	SEGAN_LIB_INLINE void _ReallocBuffer( uint newSize )
	{
		if ( m_sampler ) {
			if ( newSize >= m_capacity || ( m_capacity - newSize ) > m_sampler ) {
				m_capacity = sint( newSize / m_sampler + 1 ) * m_sampler;
				mem_realloc( (void*&)m_buffer, m_capacity );
			}
		} else {
			m_capacity = newSize;
			mem_realloc( (void*&)m_buffer, m_capacity );
		}
		
	}

	pbyte		m_buffer;		//  memory buffer
	uint		m_pointer;		//  pointer of memory in current position
	uint		m_capacity;		//  size of allocated memory
	uint		m_sampler;		//  use to sample memory to reduce allocation
};
typedef MemoryStream *PMemoryStream, StreamMemory, *PStreamMemory;

//! use only for distinct types ( int, float, enum, vector, ... )
#define sx_stream_write(variable)		stream.Write( &variable, sizeof(variable) )

//! use only for distinct types ( int, float, enum, vector, ... )
#define sx_stream_read(variable)		stream.Read( &variable, sizeof(variable) )

//! use only for distinct types ( int, float, enum, vector, ... )
#define SEGAN_STREAM_WRITE(stream, variable)	stream.Write(&variable, sizeof(variable))

//! use only for distinct types ( int, float, enum, vector, ... )
#define SEGAN_STREAM_READ(stream, variable)		stream.Read(&variable, sizeof(variable))

#endif	//	GUARD_Stream_HEADER_FILE
