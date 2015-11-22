#include <memory>
#include "Database_Result.h"


//////////////////////////////////////////////////////////////////////////
//	DATA BASE RESULT
//////////////////////////////////////////////////////////////////////////
DatabaseResult::DatabaseResult( void ): m_data(0), m_count(0), m_index(0) {}

DatabaseResult::~DatabaseResult( void )
{
	for ( uint i=0; i<m_count; ++i )
	{
		sx_mem_free_and_null( m_data[i] );
	}
	sx_mem_free_and_null( m_data );
}

void DatabaseResult::add( const char* val )
{
	//	copy last data to new one
	wchar** temp = (wchar**)sx_mem_alloc( ( m_count + 1 ) * sizeof(wchar*) );
	for ( uint i=0; i<m_count; ++i )
		temp[i] = m_data[i];
	sx_mem_free( m_data );
	m_data = temp;

	//	add final data
	const uint len = val ? (uint)strlen( val ) : 1;
	wchar*  tmpval = (wchar*)sx_mem_alloc( len * 2 + 2 );
	sx_utf8_to_str( tmpval, len + 1, val ? val : "" );
	m_data[m_count] = tmpval;

	//	increase count
	++m_count;
}

void DatabaseResult::curr( wchar* dest, const uint destcount )
{
	if ( !dest ) return;
	memset( dest, 0, destcount * 2 );
	if ( !m_data || !m_count ) return;

	wchar* val = m_data[m_index];
	for ( uint i=0; i<destcount && val[i]; ++i )
		dest[i] = val[i];
	dest[destcount-1] = 0;
}

const wchar* DatabaseResult::curr( void )
{
	if ( !m_data || !m_count ) return L"";
	return m_data[m_index];
}

bool DatabaseResult::next( void )
{
	if ( m_index < m_count - 1 )
	{
		++m_index;
		return true;
	}
	else return false;
}

void DatabaseResult::Print()
{
	for (uint i = 0; i < m_count; ++i )
	{
		sx_print( L"[%d] %s", i, m_data[i] );
	}
}
