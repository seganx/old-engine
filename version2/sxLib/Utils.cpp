#include "Memory.h"
#include "Math.h"
#include "Utils.h"

#include <memory>
#include <time.h>


#define RSTS			sizeof(SX_RAW_SIZE_TYPE)
#define getsize(raw)	(*((SX_RAW_SIZE_TYPE*)raw))

//////////////////////////////////////////////////////////////////////////
//	RAW DATA TOOLS
//////////////////////////////////////////////////////////////////////////
SEGAN_LIB_API const char* sx_raw_read(char* dest, const SX_RAW_SIZE_TYPE destsize, const char* src)
{
	SX_RAW_SIZE_TYPE size = getsize(src);
	if ( size > 0 && size <= destsize )
	{
		src += RSTS;
		sx_mem_copy( dest, src, size );
		src += size;
		return src;
	}
	return null;
}

SEGAN_LIB_API bool sx_raw_read(char* dest, const SX_RAW_SIZE_TYPE destsize, const char* src, const uint dataindex)
{
	for ( uint i = 0; i < dataindex; ++i )
	{
		SX_RAW_SIZE_TYPE size = getsize(src);
		if ( size > 0 )
			src += RSTS + size;
		else return false;
	}

	SX_RAW_SIZE_TYPE size = getsize(src);
	if (size > 0 && size <= destsize)
	{
		src += RSTS;
		sx_mem_copy(dest, src, size);
		return true;
	}
	
	return false;
}

SEGAN_LIB_API uint sx_raw_write(char* dest, const uint destsize, const char* data, const SX_RAW_SIZE_TYPE datasize)
{
	uint occupied = 0;
	SX_RAW_SIZE_TYPE size = getsize(dest);
	while (size > 0)
	{
		size += RSTS;
		dest += size;
		occupied += size;
		size = getsize(dest);
	}

	uint res = occupied + datasize + RSTS;
	if (res <= destsize)
	{
		sx_mem_copy( dest, &datasize, RSTS );
		dest += RSTS;
		sx_mem_copy( dest, data, datasize );
		return res;
	}

	return 0;
}

SEGAN_LIB_API uint sx_raw_write_text(char* dest, const uint destsize, const char* text)
{
	if (!text) return 0;
	return sx_raw_write(dest, destsize, text, sx_str_len(text));
}

SEGAN_LIB_API void sx_raw_print(const char* src)
{
	printf("\nRaw Data:\n");

	while (1)
	{
		char buffer[1024] = {0};
		SX_RAW_SIZE_TYPE size = getsize(src);
		if (size > 0 && size < 1024)
		{
			src += RSTS;
			sx_mem_copy(buffer, src, size);
			printf("%u : %s\n", size, buffer);
			src += size;
		}
		else return;		
	}
}

//////////////////////////////////////////////////////////////////////////
//	STRING TOOLS
//////////////////////////////////////////////////////////////////////////
SEGAN_LIB_API bool sx_load_string( String& dest, const wchar* filename )
{
	FILE* f = 0;
	if ( _wfopen_s( &f, filename, L"r, ccs=UNICODE" ) == 0 )
	{
		wchar tmp[1024] = {0};
		while ( fread_s( tmp, 1023, 2, 1023, f ) )
		{
			dest << tmp;
		}
		fclose(f);
		return true;
	}
	else return false;
}

SEGAN_LIB_API bool sx_load_string_list( Array<String*>& dest, const wchar* filename )
{
	FILE* f = 0;
	if ( _wfopen_s( &f, filename, L"r, ccs=UNICODE" ) == 0 )
	{
		wchar tmp[2048] = {0};
		wchar c = 0; int index = 0;
		while ( fread_s( &c, 2, 2, 1, f ) )
		{
			if ( c == '\n' )
			{
				tmp[index] = 0;
				if ( index )
				{
					String* str = sx_new( String );
					str->set_text( tmp );
					dest.push_back( str );
				}
				index = 0;
				tmp[0] = 0;
			}
			else if ( c && c!='\r' )
			{
				tmp[index++] = c;
			}
		}
		if ( index )
		{
			String* str = sx_new( String );
			str->set_text( tmp );
			dest.push_back( str );
		}
		fclose(f);
		return true;
	}
	else return false;
}

SEGAN_LIB_API void sx_clear_string_list( Array<String*>& list )
{
	for ( sint i=0; i < list.m_count; ++i )
	{
		sx_safe_delete( list.m_item[i] );
	}
	list.clear();
}

SEGAN_LIB_API uint sx_time_to_str( wchar* dest, const uint destSize, const wchar* timeFormat )
{
	time_t rawTime;
	time( &rawTime );
	struct tm timeInfo;
	localtime_s( &timeInfo, &rawTime );
	return (uint)wcsftime( dest, destSize, timeFormat, &timeInfo );
}


//////////////////////////////////////////////////////////////////////////
//	main protocol class
//////////////////////////////////////////////////////////////////////////
Protocol::~Protocol( void )
{
	free( m_data );
}

void Protocol::add_text( const char* text )
{
	if ( !text ) return;
	add_data( "text", (uint)strlen( text )+1, text );
}

void Protocol::add_data( const char* type, const uint size, const void* data )
{
	if ( !type || !data || !size ) return;

	char dtype[8] = {0};
	for ( uint i=0; i<8 && type[i]; ++i )
		dtype[i] = type[i];

	const uint newsize = m_size + 8 + 4 + size; // + type and size of the data
	if ( m_data )
	{
		m_data = (char*)realloc( m_data, newsize );
	}
	else
	{
		m_data = (char*)malloc( newsize );
	}

	char* dest = m_data + m_size;
	memcpy( dest, dtype, 8 );
	dest += 8;
	memcpy( dest, &size, 4 );
	dest += 4;
	memcpy( dest, data, size );
	m_size = newsize;
}

void Protocol::pack( const dword flag, const uint id, const uint key )
{
#if 0
	printf( "data is : \n" );
	for ( uint i=0; i<m_size; ++i )
	{
		if ( !m_data[i] || m_data[i] == '\n' || m_data[i] == '\r' )
			printf( "0" );
		else
			printf( "%c", m_data[i] );
	}
	printf( "\n" );
#endif

	//	compute and allocate require memory space
	const uint newsize = m_size + sizeof(Header);
	char* newdata = (char*)malloc( newsize );

	//	fill out protocol header
	Header tmp;
	tmp.id = id;
	tmp.flag = flag;
	tmp.size = m_size;
	tmp.key	= key;
	tmp.dhash = sx_checksum( m_data, m_size, key );
	tmp.hhash = sx_checksum( &tmp.id, sizeof(tmp)-4, key );

	//	copy data to final memory block
	char* dest = newdata;
	memcpy( dest, &tmp, sizeof(Header) );
	dest += sizeof(Header);

	if ( flag & SX_PROTOCOL_COMPRESS )
	{

	}

	if ( flag & SX_PROTOCOL_ENCRYPT )
		sx_encrypt( dest, m_data, m_size, key );
	else
		memcpy( dest, m_data, m_size );

	free( m_data );
	m_data = newdata;
	m_size = newsize;
}

void Protocol::unpack( const char* data, const uint size )
{
	if ( !data || !size || size < sizeof(Header) ) return;

	//	read header of protocol
	Header* tmp = (Header*)data;
	if ( !tmp->size ) return;
		
	//	verify the header checksum
	const uint hhash = sx_checksum( data + 4, sizeof(Header)-4, tmp->key );
	if ( hhash != tmp->hhash ) return;

	//	allocate necessary data block
	m_size = tmp->size + sizeof(Header);
	if ( m_data )
		m_data = (char*)realloc( m_data, m_size );
	else
		m_data = (char*)malloc( m_size );

	//	copy protocol header
	memcpy( m_data, tmp, sizeof(Header) );

	//	check data compression and/or encryption and copy data
	if ( tmp->flag & SX_PROTOCOL_ENCRYPT )
	{
		//	decrypt data
		sx_decrypt( m_data + sizeof(Header), data + sizeof(Header), tmp->size, tmp->key );
	}

	if ( tmp->flag & SX_PROTOCOL_COMPRESS )
	{
		//	uncompress data
	}
	

#if 0
	printf( "data is : \n" );
	for ( uint i=0; i<m_size; ++i )
	{
		if ( !m_data[i] || m_data[i] == '\n' || m_data[i] == '\r' )
			printf( "0" );
		else
			printf( "%c", m_data[i] );
	}
	printf( "\n" );
#endif
}

const Protocol::Header Protocol::get_header( void )
{
	Header res;
	memset( &res, 0, sizeof(Header) );
	if ( m_data )
	{
		memcpy( &res, m_data, sizeof(Header) );
	}
	return res;
}

const Protocol::Data Protocol::get_data( const uint index /*= 0 */ )
{
	Data res;
	memset( &res, 0, sizeof(Data) );

	if ( m_data )
	{
		const Header* header = (Header*)m_data;

		char* pos = m_data + sizeof(Header);
		for ( uint i=0; ; ++i )
		{
			if ( i == index )
			{
				memcpy( &res, pos, sizeof(Data)-4 );
				res.data = pos + sizeof(Data)-4;
				break;
			}
			else
			{
				Data* tmp = (Data*)pos;
				pos += sizeof(Data) - 4 + tmp->size;

				const uint64 p = pos - m_data - sizeof(Header);
				if ( p >= header->size ) break;
			}
		}
	}

	return res;
}
