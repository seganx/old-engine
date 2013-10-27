#include <memory>
#include "protocol.h"


//////////////////////////////////////////////////////////////////////////
//	use protocol random class to handle multi threaded calls
class ProtocolRandom
{
public:
	ProtocolRandom( const uint seed = 1363 ): m_seed(seed), m_curr(seed) {}
	
	uint get( void )
	{
		m_curr += ( ( m_curr * m_seed * 28454642 ) + ( m_curr * 38745674 ) );
		return ( m_curr % 255 );
	}

public:
	uint	m_seed;
	uint	m_curr;
};




//////////////////////////////////////////////////////////////////////////
// helper functions
__inline int _formul( ProtocolRandom* randomer, const uint index )
{
	uint r = randomer->get() * 10;
	switch ( index % 10 )
	{
	case 0 :	return int( 100 * ( r * 0.5f ) );
	case 1 :	return int( 150 * ( r * 0.1f ) );
	case 2 :	return int( 170 * ( r * 0.6f ) );
	case 3 :	return int( 200 * ( r * 0.4f ) );
	case 4 :	return int( 110 * ( r * 0.3f ) );
	case 5 :	return int( 180 * ( r * 0.8f ) );
	case 6 :	return int( 130 * ( r * 0.7f ) );
	case 7 :	return int( 120 * ( r * 0.9f ) );
	case 8 :	return int( 240 * ( r * 0.1f ) );
	case 9 :	return int( 190 * ( r * 0.4f ) );
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//	protocol functions
//////////////////////////////////////////////////////////////////////////
__inline uint sx_hash( const void* data, const uint size, const uint key /*= 1363*/ )
{
	if ( !data || !size ) return 0;
	uint r = 0;
	const char* d = (char*)data;
	ProtocolRandom randomer(key);
	for ( uint i=0; i<size; ++i )
		r += d[i] + _formul( &randomer, i );
	return r;
}

__inline void sx_encrypt( void* dest, const void* src, const uint size, const uint key /*= 1363*/ )
{
	ProtocolRandom randomer(key);
	byte* d = (byte*)dest;
	byte* s = (byte*)src;
	for ( uint i=0; i<size; ++i )
	{
		byte a = _formul( &randomer, i );
		d[i] = s[i] + a;
	}
}

__inline void sx_decrypt( void* dest, const void* src, const uint size, const uint key /*= 1363*/ )
{
	ProtocolRandom randomer(key);
	byte* d = (byte*)dest;
	byte* s = (byte*)src;
	for ( uint i=0; i<size; ++i )
	{
		byte a = _formul( &randomer, i );
		d[i] = s[i] - a;
	}
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
	const uint newsize = m_size + sizeof(ProtocolHeader);
	char* newdata = (char*)malloc( newsize );

	//	fill out protocol header
	ProtocolHeader tmp;
	tmp.id = id;
	tmp.flag = flag;
	tmp.size = m_size;
	tmp.key	= key;
	tmp.dhash = sx_hash( m_data, m_size, key );
	tmp.hhash = sx_hash( &tmp.id, sizeof(tmp)-4, key );

	//	copy data to final memory block
	char* dest = newdata;
	memcpy( dest, &tmp, sizeof(ProtocolHeader) );
	dest += sizeof(ProtocolHeader);

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
	if ( !data || !size || size < sizeof(ProtocolHeader) ) return;

	//	read header of protocol
	ProtocolHeader* tmp = (ProtocolHeader*)data;
	if ( !tmp->size ) return;
		
	//	verify the header checksum
	const uint hhash = sx_hash( data + 4, sizeof(ProtocolHeader)-4, tmp->key );
	if ( hhash != tmp->hhash ) return;

	//	allocate necessary data block
	m_size = tmp->size + sizeof(ProtocolHeader);
	if ( m_data )
		m_data = (char*)realloc( m_data, m_size );
	else
		m_data = (char*)malloc( m_size );

	//	copy protocol header
	memcpy( m_data, tmp, sizeof(ProtocolHeader) );

	//	check data compression and/or encryption and copy data
	if ( tmp->flag & SX_PROTOCOL_ENCRYPT )
	{
		//	decrypt data
		sx_decrypt( m_data + sizeof(ProtocolHeader), data + sizeof(ProtocolHeader), tmp->size, tmp->key );
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

const ProtocolHeader Protocol::get_header( void )
{
	ProtocolHeader res;
	memset( &res, 0, sizeof(ProtocolHeader) );
	if ( m_data )
	{
		memcpy( &res, m_data, sizeof(ProtocolHeader) );
	}
	return res;
}

const ProtocolData Protocol::get_data( const uint index /*= 0 */ )
{
	ProtocolData res;
	memset( &res, 0, sizeof(ProtocolData) );

	if ( m_data )
	{
		const ProtocolHeader* header = (ProtocolHeader*)m_data;

		char* pos = m_data + sizeof(ProtocolHeader);
		for ( uint i=0; ; ++i )
		{
			if ( i == index )
			{
				memcpy( &res, pos, sizeof(ProtocolData)-4 );
				res.data = pos + sizeof(ProtocolData)-4;
				break;
			}
			else
			{
				ProtocolData* tmp = (ProtocolData*)pos;
				pos += sizeof(ProtocolData) - 4 + tmp->size;

				const uint64 p = pos - m_data - sizeof(ProtocolHeader);
				if ( p >= header->size ) break;
			}
		}
	}

	return res;
}
