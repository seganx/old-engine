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

#include "Def.h"


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
	virtual uint write( const void* buf, const uint size ) = 0;
	virtual uint read( void* buf, const uint size ) = 0;
	virtual uint seek ( SeekType ST_ seekType, const uint offset = 0 ) = 0;
	virtual uint size( void ) const {return m_size; }
	virtual uint get_pos( void ) { return seek( ST_CUR ); }
	virtual void set_pos( uint newpos )
	{
		if ( newpos > m_size )
			newpos = m_size;
		seek( ST_BEGIN, newpos );
	}

	/*! copy from source stream to current stream amount of size in byte. use size=0 to full copy stream. */
	SEGAN_LIB_INLINE virtual uint copy_from( Stream& source, const uint size = 0 )
	{
		uint result  = size;

		if ( result == 0 )									result = source.m_size;
		if ( result > source.m_size - source.get_pos() )	result = source.m_size - source.get_pos();
		if ( !result )										return 0;

		uint tocopy = result;
		byte buffer[0x1fff];
		while ( tocopy > 0 )
		{
			const uint numBytes = ( tocopy > 0x1fff ) ? 0x1fff : tocopy;
			source.read( buffer, numBytes );
			write( buffer, numBytes );
			tocopy -= numBytes;
		}
		return result;
	}

	//! write bytes to stream
	SEGAN_LIB_INLINE void write_byte( const byte& _byte, const uint count = 1 )
	{
		sx_assert( count > 0 );
		write( &_byte, count * sizeof(byte) );
	}

	//! write signed int 32 bit to stream
	SEGAN_LIB_INLINE void write_int32( const i32& _i32, const uint count = 1 )
	{
		sx_assert( count > 0 );
		write( &_i32, count * sizeof(i32) );
	}

	//! write unsigned int 32 bit to stream
	SEGAN_LIB_INLINE void write_uint32( const u32& _u32, const uint count = 1 )
	{
		sx_assert( count > 0 );
		write( &_u32, count * sizeof(u32) );
	}

	//! write unsigned int 64 bit to stream
	SEGAN_LIB_INLINE void write_uint64( const u64& _u64, const uint count = 1 )
	{
		sx_assert( count > 0 );
		write( &_u64, count * sizeof(u64) );
	}

	//! write word to stream
	SEGAN_LIB_INLINE void write_word( const word& _word, const uint count = 1 )
	{
		sx_assert( count > 0 );
		write( &_word , count * sizeof(word) );
	}

	//! write dword to stream
	SEGAN_LIB_INLINE void write_dword( const dword& _dword, const uint count = 1 )
	{
		sx_assert( count > 0 );
		write( &_dword , count * sizeof(dword) );
	}

	//! write float to stream
	SEGAN_LIB_INLINE void write_float( const float& _float, const uint count = 1 )
	{
		write( &_float , count * sizeof(float) );
	}

	//! write double to stream
	SEGAN_LIB_INLINE void write_double( const double& _double, const uint count = 1 )
	{
		sx_assert( count > 0 );
		write( &_double , count * sizeof(double) );
	}

	//! write char* to stream. leave count to 0 to write all characters in null terminated string format
	SEGAN_LIB_INLINE void write_char( const char* _char, const uint count = 0 )
	{
		sx_assert( _char );
		uint size = 0;
		if ( !count )
			while ( _char[size++] );
		else
			size = count;
		sx_assert( size );
		write( _char , size * sizeof(char) );
	}

	//! write WChar to stream. leave count to 0 to write all characters in null terminated string format
	SEGAN_LIB_INLINE void write_wchar( const wchar* _wchar, const uint count = 0 )
	{
		if ( !_wchar )
		{
			write( L"", sizeof(wchar) );
		}
		else
		{
			uint size = 0;
			if ( !count )
				while ( _wchar[size++] );
			else
				size = count;
			sx_assert( size );
			write( _wchar , size * sizeof(wchar) );
		}		
	}

	//! write bool to stream
	SEGAN_LIB_INLINE void write_bool( const bool& _bool, const uint count = 1 )
	{
		sx_assert( count > 0 );
		write( &_bool , count * sizeof(bool) );
	}

	//! read bytes to stream
	SEGAN_LIB_INLINE void read_byte( byte& _byte, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_byte, count * sizeof(byte) );
	}

	//! read signed int 32 bit to stream
	SEGAN_LIB_INLINE void read_int32( i32& _i32, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_i32, count * sizeof(i32) );
	}

	//! read unsigned int 32 bit from stream
	SEGAN_LIB_INLINE void read_uint32( u32& _u32, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_u32 , count * sizeof(u32) );
	}

	//! read unsigned int 64 bit from stream
	SEGAN_LIB_INLINE void read_uint64( u64& _u64, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_u64 , count * sizeof(u64) );
	}

	//! read word to stream
	SEGAN_LIB_INLINE void read_word( word& _word, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_word , count * sizeof(word) );
	}

	//! read dword to stream
	SEGAN_LIB_INLINE void read_dword( dword& _dword, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_dword , count * sizeof(dword) );
	}

	//! read float to stream
	SEGAN_LIB_INLINE void read_float( float& _float, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_float , count * sizeof(float) );
	}

	//! read double to stream
	SEGAN_LIB_INLINE void read_double( double& _double, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_double , count * sizeof(double) );
	}

	//! read char to stream
	SEGAN_LIB_INLINE void read_char( char& _char, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_char , count * sizeof(char) );
	}

	//! read WChar to stream
	SEGAN_LIB_INLINE void read_wchar( wchar& _wchar, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_wchar , count * sizeof(wchar) );
	}

	//! read null terminated string
	SEGAN_LIB_INLINE void read_string_w( wchar* dest, const uint destwords )
	{
		sx_assert( dest > 0 );
		for ( uint i = 0; i<destwords; ++i )
		{
			read( &dest[i], sizeof(wchar) );
			if ( ! dest[i] ) return;
		}
	}

	//! read null terminated string
	SEGAN_LIB_INLINE void read_string_a( char* dest, const uint destwords )
	{
		sx_assert( dest > 0 );
		for ( uint i = 0; i<destwords; ++i )
		{
			read( &dest[i], sizeof(char) );
			if ( ! dest[i] ) return;
		}
	}

	//! read bool to stream
	SEGAN_LIB_INLINE void read_bool( bool& _bool, const uint count = 1 )
	{
		sx_assert( count > 0 );
		read( &_bool , count * sizeof(bool) );
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

	SEGAN_LIB_INLINE void clear( void )
	{
		m_pointer = 0;
		m_size = 0;
	}

	SEGAN_LIB_INLINE uint write( const void* buf, const uint size )
	{
		if ( size == 0 ) return 0;
		if ( m_pointer + size > m_size )
		{
			m_size = m_pointer + size;
			_realloc_buffer( m_size );
		}
		memcpy( m_buffer + m_pointer, buf, size );
		m_pointer += size;
		return size;
	}

	SEGAN_LIB_INLINE uint read( void* buf, const uint size )
	{
		if ( size == 0 ) return 0;
		uint readSize = size; 
		if ( m_pointer + readSize > m_size )
			readSize = m_size - m_pointer;
		memcpy( buf, m_buffer + m_pointer, readSize );
		m_pointer += readSize;
		return readSize;
	}

	SEGAN_LIB_INLINE uint seek( ST_ SeekType seekType, const uint offset = 0 )
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

	//! copy data to a dest and return number of byte copied. use size=0 to full copy stream.
	SEGAN_LIB_INLINE uint copy_to( void* dest, const uint sizeinbyte = 0 )
	{
		uint res = sizeinbyte ? sizeinbyte : m_size;
		if ( m_pointer + res > m_size )
			res = m_size - m_pointer;
		memcpy( dest, m_buffer + m_pointer, res );
		m_pointer += res;
		return res;
	}

	SEGAN_LIB_INLINE operator const void* ( void ) const
	{
		return m_buffer + m_pointer;
	}

private:
	SEGAN_LIB_INLINE void _realloc_buffer( const uint newsize )
	{
		if ( m_sampler ) {
			if ( newsize >= m_capacity || ( m_capacity - newsize ) > m_sampler ) {
				m_capacity = sint( newsize / m_sampler + 1 ) * m_sampler;
				m_buffer = (pbyte)mem_realloc( m_buffer, m_capacity );
			}
		} else {
			m_capacity = newsize;
			m_buffer = (pbyte)mem_realloc( m_buffer, m_capacity );
		}
		
	}

	pbyte		m_buffer;		//  memory dest
	uint		m_pointer;		//  pointer of memory in current position
	uint		m_capacity;		//  size of allocated memory
	uint		m_sampler;		//  use to sample memory to reduce allocation
};
typedef MemoryStream *PMemoryStream, StreamMemory, *PStreamMemory;

//! use only for distinct types ( int, float, enum, vector, ... )
#define sx_stream_write(variable)		stream.write( &variable, sizeof(variable) )

//! use only for distinct types ( int, float, enum, vector, ... )
#define sx_stream_read(variable)		stream.read( &variable, sizeof(variable) )

//! use only for distinct types ( int, float, enum, vector, ... )
#define SEGAN_STREAM_WRITE(stream, variable)	stream.write(&variable, sizeof(variable))

//! use only for distinct types ( int, float, enum, vector, ... )
#define SEGAN_STREAM_READ(stream, variable)		stream.read(&variable, sizeof(variable))

#endif	//	GUARD_Stream_HEADER_FILE
