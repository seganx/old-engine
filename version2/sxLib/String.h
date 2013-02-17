/********************************************************************
	created:	2012/04/05
	filename: 	String.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain unicode string calss
*********************************************************************/
#ifndef GUARD_String_HEADER_FILE
#define GUARD_String_HEADER_FILE

#include "Memory.h"
#include <wchar.h>
#include <stdarg.h>


#if	defined(_WIN32)
	#define	PATH_PART	'\\'
#elif defined(_MAC)
	//#define	PATH_PART
#else
	//#define	PATH_PART
#endif


SEGAN_LIB_INLINE uint sx_str_len( const wchar* str )
{
	uint len = 0;
	if ( str )
	{
		while ( *str++ ) ++len;
	}
	return len;
}

SEGAN_LIB_INLINE uint sx_str_len( const char* str )
{
	uint len = 0;
	if ( str )
	{
		while ( *str++ ) ++len;
	}
	return len;
}

SEGAN_LIB_INLINE sint sx_str_cmp( const wchar* str1, const wchar* str2 )
{
	sint res = 0 ;
	if ( str1 && str2 && (void*)str1 != (void*)str2 )
	{
		while( !(res = (sint)(*str1 - *str2)) && *str1 )
			++str1, ++str2;
		if ( res < 0 )		res = -1;
		else if ( res > 0 )	res = 1;
	}
	else if ( str1 )
		res = 1;
	else if ( str2 )
		res = -1;	
	return res;
}

SEGAN_LIB_INLINE sint sx_str_cmp( const char* str1, const char* str2 )
{
	sint res = 0 ;
	if ( str1 && str2 && (void*)str1 != (void*)str2 )
	{
		while( !(res = (sint)(*str1 - *str2)) && *str1 )
			++str1, ++str2;
		if ( res < 0 )		res = -1;
		else if ( res > 0 )	res = 1;
	}
	else if ( str1 )
		res = 1;
	else if ( str2 )
		res = -1;	
	return res;
}

SEGAN_LIB_INLINE sint sx_str_cmp( const wchar* str1, const char* str2 )
{
	sint res = 0 ;
	if ( str1 && str2 && (void*)str1 != (void*)str2 )
	{
		while( !(res = (sint)(*str1 - *str2)) && *str1 )
			++str1, ++str2;
		if ( res < 0 )		res = -1;
		else if ( res > 0 )	res = 1;
	}
	else if ( str1 )
		res = 1;
	else if ( str2 )
		res = -1;	
	return res;
}

SEGAN_LIB_INLINE void sx_str_copy( wchar* dest, const sint dest_size_in_word, const wchar* src )
{
	sx_assert(dest);
	if ( src )
	{
		for ( int i=0; i<dest_size_in_word; i++ )
		{
			dest[i] = src[i];
			if ( dest[i] == 0 ) break;
		}
		dest[dest_size_in_word-1] = 0;
	}
}

SEGAN_LIB_INLINE void sx_str_copy( wchar* dest, const sint dest_size_in_word, const char* src )
{
	sx_assert(dest);
	if ( src )
	{
		for ( int i=0; i<dest_size_in_word; i++ )
		{
			dest[i] = src[i];
			if ( dest[i] == 0 ) break;
		}
		dest[dest_size_in_word-1] = 0;
	}
}

SEGAN_LIB_INLINE wchar sx_str_upper( wchar c )
{
	if ( 'a' <= c && c <= 'z' )
		c += 'A' - 'a';
	return c;
}

SEGAN_LIB_INLINE wchar sx_str_lower( wchar c )
{
	if ( 'A' <= c && c <= 'Z' )
		c += 'a' - 'A';
	return c;
}

SEGAN_LIB_INLINE sint sx_str_to_int( const wchar* str )
{
	if ( !str ) return 0;
	return _wtoi( str );
}

SEGAN_LIB_INLINE float sx_str_to_float( const wchar* str )
{
	if ( !str ) return 0.0f;
	return (float)_wtof( str );
}

/*! return true if entry string is type of file path and end with '/' or '\'*/
SEGAN_LIB_INLINE const bool sx_str_is_pathstyle( const wchar* filepath )
{
	if ( !filepath ) return false;
	sint len = sx_str_len( filepath );
	if ( len )
	{
		wchar ch = filepath[--len];
		return ( ch == '/' || ch == '\\' );
	}
	return false;
}

/*! return true if entry string is as complete filename*/
SEGAN_LIB_INLINE const bool sx_str_is_fullfilepath( const wchar* filepath )
{
	if ( !filepath ) return false;
	return ( filepath[1] == ':' || ( filepath[0] == '\\' && filepath[1] == '\\') || ( filepath[0] == '/' && filepath[1] == '/') );
}

/*! convert integer number to string.
NOTE: this function uses string memory pool and the returned string will be deleted in the next call*/
SEGAN_LIB_API const wchar* sx_int_to_str( const sint number );

/*! convert float number to string with specified precision.
NOTE: this function uses string memory pool and the returned string will be deleted in the next call*/
SEGAN_LIB_API const wchar* sx_float_to_str( float number, sint precision = 3 );

/*! extract file path from full filename.
NOTE: this function uses string memory pool and the returned string will be deleted in the next call*/
SEGAN_LIB_API const wchar* sx_str_extract_filepath( const wchar* filename );

/*! extract file name ( may contain file extension ) from full filename.
NOTE: this function uses string memory pool and the returned string will be deleted in the next call*/
SEGAN_LIB_API const wchar* sx_str_extract_filename( const wchar* filename );

/*! extract file extension from filename.
NOTE: this function uses string memory pool and the returned string will be deleted in the next call*/
SEGAN_LIB_API const wchar* sx_str_extract_extension( const wchar* filename );

/*! exclude file extension from full filename.
NOTE: this function uses string memory pool and the returned string will be deleted in the next call*/
SEGAN_LIB_API const wchar* sx_str_exclude_extension( const wchar* filename );

/*! return path file as string and guarantee the file path end with '/' or '\'
NOTE: this function uses string memory pool and the returned string will be deleted in the next call*/
SEGAN_LIB_API const wchar* sx_str_make_pathstyle( const wchar* filepath );

/*! 
string class stores strings of wide characters
*/
class String
{
public:
	String( sint sampler = 32 )
		: m_text(0)
		, m_len(0)
		, m_size(0)
		, m_sampler(sampler)
		, m_tmp(0)
	{
	}

	String( const char ch )
		: m_text(0)
		, m_len(0)
		, m_size(0)
		, m_sampler(32)
		, m_tmp(0)
	{
		Append( ch );
	}

	String( const char* str )
		: m_text(0)
		, m_len(0)
		, m_size(0)
		, m_sampler(32)
		, m_tmp(0)
	{
		Append( str );
	}

	String( const wchar* str )
		: m_text(0)
		, m_len(0)
		, m_size(0)
		, m_sampler(32)
		, m_tmp(0)
	{
		Append( str );
	}

	String( const String& str )
		: m_text(0)
		, m_len(0)
		, m_size(0)
		, m_sampler(str.m_sampler)
		, m_tmp(0)
	{
		Append( str );
	}

	~String( void )
	{
		mem_free( m_text );
	}

	SEGAN_LIB_INLINE void Clear( bool freemem = false )
	{
		if ( freemem )
		{
			mem_free( m_text );
			m_size = 0;
		}
		else if ( m_text )
		{
			m_text[0] = 0;
		}
		m_len = 0;
	}

	SEGAN_LIB_INLINE void SetText( const wchar* str )
	{
		if ( str != m_text )
		{
			if ( str )
			{
				m_len  = sx_str_len( str );
				_Realloc( m_len + 1 );
				memcpy( m_text, str, m_len * sizeof(wchar) );
				m_text[m_len] = 0;
			}
			else
			{
				Clear();
			}
		}
	}

	SEGAN_LIB_INLINE void SetText( const char* str )
	{
		if ( str )
		{
			m_len  = sx_str_len( str );
			_Realloc( m_len + 1 );
			for ( sint i=0; i < m_len; m_text[i] = str[i], i++ );
			m_text[m_len] = 0;
		}
		else
		{
			Clear();
		}
	}

	SEGAN_LIB_INLINE void SetText( const wchar ch )
	{
		m_len  = 1;
		_Realloc( 2 );
		m_text[0] = ch;
		m_text[1] = 0;
	}

	SEGAN_LIB_INLINE void Format( const wchar* format, ... )
	{
		//	trusted . safe and optimized function
		if( !format ) return;
		va_list argList;
		va_start(argList, format);
		m_len = _vscwprintf(format, argList);
		_Realloc( m_len + 1 );
		vswprintf_s(m_text, m_size, format, argList);
		va_end(argList);
	}

	SEGAN_LIB_INLINE const wchar* Text( void ) const
	{
		return m_text;
	}

	SEGAN_LIB_INLINE void Append( const String& str )
	{
		//	trusted . safe and optimized function
		sint slen = str.m_len;
		if ( slen )
		{
			_Realloc( m_len + slen + 1 );
			for ( sint i=0; i<=slen; m_text[i+m_len] = str.m_text[i], i++ );
			m_len += slen;
		}
	}

	SEGAN_LIB_INLINE void Append( const wchar* str )
	{
		//	trusted . safe and optimized function
		if ( !str ) return;
		sint slen = sx_str_len( str );
		if ( !slen ) return;
		_Realloc( m_len + slen + 1 );
		for ( sint i=0; i<=slen; m_text[i+m_len] = str[i], i++ );
		m_len += slen;
	}

	SEGAN_LIB_INLINE void Append( const char* str )
	{
		//	trusted . safe and optimized function
		if ( !str ) return;
		sint slen = sx_str_len( str );
		if ( !slen ) return;
		_Realloc( m_len + slen + 1 );
		for ( sint i=0; i<=slen; m_text[i+m_len] = str[i], i++ );
		m_len += slen;
	}

	SEGAN_LIB_INLINE void Append( const wchar c )
	{
		//	trusted . safe and optimized function
		m_len++;
		_Realloc( m_len + 1 );
		m_text[m_len-1] = c;
		m_text[m_len] = 0;
	}

	SEGAN_LIB_INLINE void Insert( const wchar* str, sint _where = 0 )
	{	
		//	trusted . safe and optimized function
		if ( !str ) return;
		if ( _where < 0 ) _where = 0;
		else if ( _where > m_len ) _where = m_len;
		sint slen = sx_str_len(str);
		_Realloc( m_len + slen + 1 );
		for ( sint i=m_len; i>=_where; m_text[i+slen] = m_text[i], i-- );
		for ( sint i=0; i<slen; m_text[i+_where] = str[i], i++ );
		m_len += slen;
	}

	SEGAN_LIB_INLINE void Insert( const char* str, sint _where = 0 )
	{
		//	trusted . safe and optimized function
		if ( !str ) return;
		if ( _where < 0 ) _where = 0;
		else if ( _where > m_len ) _where = m_len;
		sint slen = sx_str_len( str );
		_Realloc( m_len + slen + 1 );
		for ( sint i=m_len; i>=_where; m_text[i+slen] = m_text[i], i-- );
		for ( sint i=0; i<slen; m_text[i+_where]=str[i], i++ );
		m_len += slen;
	}

	SEGAN_LIB_INLINE void Insert( const wchar ch, sint _where = 0 )
	{
		//	trusted . safe and optimized function
		if ( _where < 0 ) _where = 0;
		else if ( _where > m_len ) _where = m_len;
		_Realloc( m_len + 2 );
		for ( sint i=m_len; i>=_where; m_text[i+1] = m_text[i], i-- );
		m_text[_where] = ch;
		m_len++;
	}

	SEGAN_LIB_INLINE void Delete( sint index, sint count = 1 )
	{
		//	trusted . safe and optimized function
		if ( !m_text || count < 1 ) return;
		if ( index<0 ) index = 0;
		else if ( index >= m_len ) return;
		if ( index+count > m_len ) count = m_len - index;
		for ( sint i= index + count; i<m_len; m_text[i-count] = m_text[i], i++ );
		m_len -= count;
		_Realloc( m_len + 1 );
		m_text[m_len] = 0;
	}

	SEGAN_LIB_INLINE sint Length( void ) const
	{
		return m_len;
	}

	SEGAN_LIB_INLINE void CopyTo( String& Dest, sint index, sint count )
	{
		//	trusted . safe and optimized function
		if ( !m_text || count < 1 ) return;
		if ( index < 0 || index >= m_len ) return;
		if ( index+count > m_len ) count = m_len-index;
		Dest._Realloc( Dest.m_len + count + 1 );
		for ( sint i=0; i<count; Dest.m_text[i+Dest.m_len]=m_text[i+index], i++ );
		Dest.m_len += count;
		Dest.m_text[Dest.m_len] = 0;
	}

	SEGAN_LIB_INLINE sint Find( const wchar* substr, sint from = 0 ) const
	{
		//	trusted . safe and optimized function
		if ( !m_text || !substr ) return -1;
		if ( from < 0 ) from = 0;
		wchar* p = wcsstr( m_text + from, substr );
		if ( p )
			return ( (sint)(p - m_text) );
		else
			return -1;
	}

	SEGAN_LIB_INLINE sint FindBack( const wchar* substr, sint from = 0 ) const
	{
		//	trusted . safe and optimized function
		if ( !m_text || !substr ) return -1;
		if ( from < 0 ) from = 0;
		for ( sint i=m_len; i>0; i-- ) {
			wchar* p = wcsstr( m_text + i - from, substr );
			if (p) return ( (sint)(p - m_text) );
		}
		return -1;
	}

	SEGAN_LIB_INLINE void Replace(const wchar* what, const wchar* with)
	{
		if ( !m_text || !what || !with ) return;

		int lenwhat = (int)sx_str_len(what);
		int lenwith = (int)sx_str_len(with);
		int index = 0;
		while ( ( index = Find(what, index) ) > -1 )
		{
			Delete(index, lenwhat);
			Insert(with, index);
			index += lenwith;
		}
	}

	SEGAN_LIB_INLINE void Revers( sint from, sint to )
	{
		//	trusted . safe and optimized function
		if ( from < 0 ) from = 0;
		if ( from >= m_len ) from = m_len-1;
		if ( to < 0 ) to = 0;
		if ( to >= m_len ) to = m_len-1;
		if ( from == to ) return;
		if ( from < to )
		{
			wchar c;
			while ( from < to )
			{
				c = m_text[from];
				m_text[from++] = m_text[to];
				m_text[to--] = c;
			}
		}
		else
		{
			wchar c;
			while ( to < from )
			{
				c = m_text[to];
				m_text[to++] = m_text[from];
				m_text[from--] = c;
			}
		}
	}

	SEGAN_LIB_INLINE void Trim( void )
	{
		//	trusted . safe and optimized function
		if ( !m_text ) return;
		//  trim right side of text before left side may cause to decrease traverse spaces
		{
			wchar *end = &m_text[m_len-1];
			while ( *end == ' ' || *end == '\t' ) {
				m_len--;
				end--;
			}
			*(++end) = 0;
		}
		//  trim left size of text
		{
			wchar *pos = m_text;
			while ( *pos == ' ' || *pos == '\t' || *pos == 0 ) {
				m_len--;
				pos++;
			}
			if ( pos != m_text ) {
				wchar *first = m_text;
				while ( *pos )
					*(first++) = *(pos++);
				*first = 0;
			}
		}
	}

	SEGAN_LIB_INLINE void MakeUpper( void )
	{
		if ( !m_text ) return;	
		for ( wchar* cp=m_text; *cp; ++cp ) {
			if ( 'a' <= *cp && *cp <= 'z' )
				*cp += 'A' - 'a';
		}
	}

	SEGAN_LIB_INLINE void MakeLower( void )
	{
		if ( !m_text ) return;	
		for ( wchar* cp = m_text; *cp; ++cp ) {
			if ( 'A' <= *cp && *cp <= 'Z' )
				*cp += 'a' - 'A';
		}
	}

	SEGAN_LIB_INLINE float ToFloat( void ) const
	{
		if ( !m_text ) return 0.0f;
		return (float)_wtof( m_text );
	}

	SEGAN_LIB_INLINE sint ToInt( void ) const
	{
		if ( !m_text ) return 0;
		return _wtoi( m_text );
	}

	SEGAN_LIB_INLINE operator const wchar* ( void ) const
	{
		return m_text;
	}

	SEGAN_LIB_INLINE const wchar* operator* ( void ) const
	{
		return m_text;
	}

	SEGAN_LIB_INLINE wchar& operator[] ( const sint index )
	{
		sx_assert( !m_text || index < 0 || index > m_len );
		return m_text[index];
	}

	SEGAN_LIB_INLINE String& ExtractFilePath( void )
	{
		//	trusted . safe and optimized function
		if ( !m_text ) return *this;
		sint counter = m_len;
		wchar* end = &m_text[m_len];
		while ( end != m_text )
		{
			counter--;
			end--;
			switch ( *end )
			{
			case '/' :
			case '\\': *end = 0; m_len = counter; return *this;
			}				
		}
		return *this;
	}

	SEGAN_LIB_INLINE String& ExtractFileName( void )
	{
		//	trusted . safe and optimized function
		if ( !m_text ) return *this;
		wchar* first = m_text;
		wchar* end	 = &m_text[m_len];
		while ( end != m_text )
		{
			end--;
			if ( *end == '\\' || *end == '/')
			{
				end++; m_len=0;
				while ( *end )
				{
					m_len++;
					*(first++) = *(end++);
				}
				*first = 0;
				return *this;
			}
		}
		return *this;
	}

	SEGAN_LIB_INLINE String& ExtractFileExtension( void )
	{
		//	trusted . safe and optimized function
		if ( !m_text ) return *this;
		wchar* first = m_text;
		wchar* end = &m_text[m_len];
		while ( end != m_text )
		{
			end--;
			if ( *end == '.' )
			{
				end++; m_len=0;
				while ( *end )
				{
					m_len++;
					*(first++) = *(end++);
				}
				*first = 0;
				return *this;
			}
		}
		Clear();
		return *this;
	}

	SEGAN_LIB_INLINE String& ExcludeFileExtension( void )
	{
		//	trusted . safe and optimized function
		if ( !m_text ) return *this;
		sint counter = m_len;
		wchar* end = &m_text[m_len];
		while ( end != m_text )
		{
			counter--;
			end--;
			switch ( *end )
			{
			case '.' : *end = 0; m_len = counter; return *this;
			case '/' :
			case '\\': return *this;
			}				
		}
		return *this;
	}

	SEGAN_LIB_INLINE bool IsPathStyle( void )
	{
		if ( !m_text ) return false;
		return ( m_text[m_len-1] == '/' || m_text[m_len-1] == '\\' );
	}

	SEGAN_LIB_INLINE bool IsFullPath( void )
	{
		if ( !m_text ) return false;
		return ( m_text[1] == ':' || ( m_text[0] == '\\' && m_text[1] == '\\') || ( m_text[0] == '/' && m_text[1] == '/') );
	}

	SEGAN_LIB_INLINE void MakePathStyle( void )
	{
		if ( !m_text ) return;
		if ( m_text[m_len-1] != '/' && m_text[m_len-1] != '\\' ) {
			m_len++;
			_Realloc( m_len + 1 );
			m_text[m_len-1] = PATH_PART;
			m_text[m_len] = 0;
		}
	}

	//  operator declarations for = //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE String& operator= ( const String& Str )
	{
		SetText( Str.m_text );
		return *this;
	}

	SEGAN_LIB_INLINE String& operator= ( const wchar* str )
	{
		SetText( str );
		return *this;
	}

	SEGAN_LIB_INLINE String& operator= ( const char* str )
	{
		SetText( str );
		return *this;
	}

	SEGAN_LIB_INLINE String& operator= ( const wchar ch )
	{
		SetText( ch );
		return *this;
	}

	//  operator declarations for << //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE String& operator<< ( const String& str )
	{
		Append( str );
		return *this;
	}

	SEGAN_LIB_INLINE String& operator<< ( const wchar* str )
	{
		Append( str );
		return *this;
	}

	SEGAN_LIB_INLINE String& operator<< ( const char* str )
	{
		Append( str );
		return *this;
	}

	SEGAN_LIB_INLINE String& operator<< ( const char ch )
	{
		Append( ch );
		return *this;
	}

	SEGAN_LIB_INLINE String& operator<< ( const wchar ch )
	{
		Append( ch );
		return *this;
	}

	SEGAN_LIB_INLINE String& operator<< ( const int number )
	{
		wchar tmp[64];
		_itow_s( (int)number, tmp, 64, 10 );
		Append( tmp );
		return *this;
	}

	//  operator declarations for == //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE bool operator== ( const String& Str ) const
	{
		return ( sx_str_cmp( m_text, Str.m_text ) == 0 );
	}

	SEGAN_LIB_INLINE bool operator== ( const wchar* str ) const
	{
		return ( sx_str_cmp( m_text, str ) == 0 );
	}

	SEGAN_LIB_INLINE bool operator== ( const char* str ) const
	{
		return ( sx_str_cmp( m_text, str ) == 0 );
	}

	//  operator declarations for != //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE bool operator!= ( const String& Str ) const
	{
		return ( sx_str_cmp( m_text, Str.m_text ) != 0 );
	}

	SEGAN_LIB_INLINE bool operator!= ( const wchar* str ) const
	{
		return ( sx_str_cmp( m_text, str ) != 0 );
	}

	SEGAN_LIB_INLINE bool operator!= (const char* str) const
	{
		return ( sx_str_cmp( m_text, str ) != 0 );
	}

private:

	SEGAN_LIB_INLINE void _Realloc( sint newSize ) {
		if ( m_sampler ) {
			if ( newSize >= m_size || ( m_size - newSize ) > m_sampler ) {
				m_size = sint( newSize / m_sampler ) * m_sampler + m_sampler;
				mem_realloc( (void*&)m_text, m_size * sizeof(wchar) );
			}
		} else {
			m_size = newSize;
			mem_realloc( (void*&)m_text, m_size * sizeof(wchar) );
		}
	}

	wchar*			m_text;			//	main text
	sint			m_len;			//  length of string
	sint			m_size;			//	size of the whole array
	sint			m_sampler;		//  use to sample memory to reduce allocation
	wchar			m_tmp;			//	used as temporary

};




/*! 
string class stores strings of wide characters in specified size
*/
template< uint count >
class String_fix
{
public:
	String_fix( void ): m_len(0)
	{
		m_text[0] = 0;
	}

	String_fix( const char ch ): m_len(0)
	{
		m_text[0] = 0;
		Append( ch );
	}

	String_fix( const char* str ): m_len(0)
	{
		m_text[0] = 0;
		Append( str );
	}

	String_fix( const wchar* str ): m_len(0)
	{
		m_text[0] = 0;
		Append( str );
	}

	String_fix( const String_fix& str ): m_len(0)
	{
		m_text[0] = 0;
		Append( str );
	}

	String_fix( const String& str ): m_len(0)
	{
		m_text[0] = 0;
		Append( str );
	}

	~String_fix( void )
	{
	}

	SEGAN_LIB_INLINE void Clear( void )
	{
		m_text[0] = 0;
		m_len = 0;
	}

	SEGAN_LIB_INLINE void SetText( const wchar* str )
	{
		if ( str == m_text )
		{
			if ( str )
			{
				m_len  = sx_str_len( str );
				if ( m_len > count - 1 ) m_len = count - 1;
				memcpy( m_text, str, m_len * sizeof(wchar) );
				m_text[m_len] = 0;
			}
			else
			{
				Clear();
			}
		}
	}

	SEGAN_LIB_INLINE void SetText( const char* str )
	{
		if ( str )
		{
			m_len  = (uint)strlen(str);
			if ( m_len + 1 > count ) m_len = count - 1;
			for ( sint i = 0; i < m_len; m_text[i] = str[i], i++ );
			m_text[m_len] = 0;
		}
		else
		{
			Clear();
		}
	}

	SEGAN_LIB_INLINE void SetText( const wchar ch )
	{
		m_len  = 1;
		m_text[0] = ch;
		m_text[1] = 0;
	}

	SEGAN_LIB_INLINE void Format( const wchar* format, ... )
	{
		//	trusted . safe and optimized function
		if( !format ) return;
		va_list argList;
		va_start( argList, format );
		if ( _vscwprintf(format, argList) < count - 1 )
			m_len = vswprintf_s( m_text, count - 1, format, argList );
		else
			Clear();
		va_end(argList);
	}

	SEGAN_LIB_INLINE const wchar* Text( void ) const
	{
		return m_text;
	}

	SEGAN_LIB_INLINE void Append( const wchar* str )
	{
		if ( str )
		{
			for ( int i=0; m_len<count; i++, m_len++ )
			{
				m_text[m_len] = str[i];
				if ( m_text[m_len] == 0 ) break;
			}
			m_text[count-1] = 0;
		}
	}

	SEGAN_LIB_INLINE void Append( const char* str )
	{
		if ( str )
		{
			for ( int i=0; m_len<count; i++, m_len++ )
			{
				m_text[m_len] = str[i];
				if ( m_text[m_len] == 0 ) break;
			}
			m_text[count-1] = 0;
		}
	}

	SEGAN_LIB_INLINE void Append( const wchar c )
	{
		//	trusted . safe and optimized function
		if ( m_len + 1 >= count ) return;
		m_len++;
		m_text[m_len-1] = c;
		m_text[m_len] = 0;
	}

	SEGAN_LIB_INLINE void Insert( const wchar* str, sint _where = 0 )
	{	
		//	trusted . safe and optimized function
		if ( !str ) return;
		if ( _where < 0 ) _where = 0;
		else if ( _where > m_len ) _where = m_len;
		sint slen = (sint)sx_str_len(str);
		if ( m_len + slen > count ) return;
		for ( sint i = m_len; i >= _where; m_text[i+slen] = m_text[i], i-- );
		for ( sint i = 0; i < slen; m_text[i+_where] = str[i], i++ );
		m_len += slen;
	}

	SEGAN_LIB_INLINE void Insert( const char* str, sint _where = 0 )
	{
		//	trusted . safe and optimized function
		if ( !str ) return;
		if ( _where < 0 ) _where = 0;
		else if ( _where > m_len ) _where = m_len;
		sint slen = (sint)strlen(str);
		if ( m_len + slen > count ) return;
		for ( sint i = m_len; i >= _where; m_text[i+slen] = m_text[i], i-- );
		for ( sint i = 0; i < slen; m_text[i+_where] = str[i], i++ );
		m_len += slen;
	}

	SEGAN_LIB_INLINE void Insert( const wchar ch, sint _where = 0 )
	{
		//	trusted . safe and optimized function
		if ( _where < 0 ) _where = 0;
		else if ( _where > m_len ) _where = m_len;
		if ( m_len + 1 > count ) return;
		for ( sint i = m_len; i >= _where; m_text[i+1] = m_text[i], i-- );
		m_text[_where] = ch;
		m_len++;
	}

	SEGAN_LIB_INLINE void Delete( sint index, sint charcount = 1 )
	{
		//	trusted . safe and optimized function
		if ( !m_text[0] || charcount < 1 ) return;
		if ( index<0 ) index = 0;
		else if ( index >= m_len ) return;
		if ( index + charcount > m_len ) charcount = m_len - index;
		for ( sint i= index + charcount; i < m_len; m_text[i-charcount] = m_text[i], i++ );
		m_len -= charcount;
		m_text[m_len] = 0;
	}

	SEGAN_LIB_INLINE sint Length( void ) const
	{
		return m_len;
	}

	SEGAN_LIB_INLINE void CopyTo( String_fix& Dest, sint index, sint charcount )
	{
		//	trusted . safe and optimized function
		if ( !m_text[0] || charcount < 1 ) return;
		if ( index < 0 || index >= m_len ) return;
		if ( index+charcount > m_len ) charcount = m_len - index;
		if ( Dest.m_len + charcount > count ) return;
		for ( sint i = 0; i < charcount; Dest.m_text[i+Dest.m_len] = m_text[i+index], i++ );
		Dest.m_len += charcount;
		Dest.m_text[Dest.m_len] = 0;
	}

	SEGAN_LIB_INLINE sint Find( const wchar* substr, sint from = 0 ) const
	{
		//	trusted . safe and optimized function
		if ( !m_text[0] || !substr ) return -1;
		if ( from < 0 ) from = 0;
		const wchar* p = wcsstr( m_text + from, substr );
		if ( p )
			return ( (sint)(p - m_text) );
		else
			return -1;
	}

	SEGAN_LIB_INLINE sint FindBack( const wchar* substr, sint from = 0 ) const
	{
		//	trusted . safe and optimized function
		if ( !m_text[0] || !substr ) return -1;
		if ( from < 0 ) from = 0;
		for ( sint i=m_len; i>0; i-- ) {
			wchar* p = wcsstr( m_text + i - from, substr );
			if (p) return ( (sint)(p - m_text) );
		}
		return -1;
	}

	SEGAN_LIB_INLINE void Replace(const wchar* what, const wchar* with)
	{
		if ( !m_text || !what || !with ) return;

		int lenwhat = (int)sx_str_len(what);
		int lenwith = (int)sx_str_len(with);
		int index = 0;
		while ( ( index = Find(what, index) ) > -1 )
		{
			Delete(index, lenwhat);
			Insert(with, index);
			index += lenwith;
		}
	}

	SEGAN_LIB_INLINE void Revers( sint from, sint to )
	{
		//	trusted . safe and optimized function
		if ( from < 0 ) from = 0;
		if ( from >= m_len ) from = m_len - 1;
		if ( to < 0 ) to = 0;
		if ( to >= m_len ) to = m_len - 1;
		if ( from == to ) return;
		if ( from < to )
		{
			wchar c;
			while ( from < to )
			{
				c = m_text[from];
				m_text[from++] = m_text[to];
				m_text[to--] = c;
			}
		}
		else
		{
			wchar c;
			while ( to < from )
			{
				c = m_text[to];
				m_text[to++] = m_text[from];
				m_text[from--] = c;
			}
		}
	}

	SEGAN_LIB_INLINE void Trim( void )
	{
		//	trusted . safe and optimized function
		if ( !m_text[0] ) return;
		//  trim right side of text before left side may cause to decrease traverse spaces
		{
			wchar *end = &m_text[m_len-1];
			while ( *end == ' ' || *end == '\t' ) {
				m_len--;
				end--;
			}
			*(++end) = 0;
		}
		//  trim left size of text
		{
			wchar *pos = m_text;
			while ( *pos == ' ' || *pos == '\t' || *pos == 0 ) {
				m_len--;
				pos++;
			}
			if ( pos != m_text ) {
				wchar *first = m_text;
				while ( *pos )
					*(first++) = *(pos++);
				*first = 0;
			}
		}
	}

	SEGAN_LIB_INLINE void MakeUpper( void )
	{
		if ( !m_text[0] ) return;	
		for ( wchar* cp = m_text; *cp; ++cp ) {
			if ( 'a' <= *cp && *cp <= 'z' )
				*cp += 'A' - 'a';
		}
	}

	SEGAN_LIB_INLINE void MakeLower( void )
	{
		if ( !m_text[0] ) return;	
		for ( wchar* cp = m_text; *cp; ++cp ) {
			if ( 'A' <= *cp && *cp <= 'Z' )
				*cp += 'a' - 'A';
		}
	}

	SEGAN_LIB_INLINE float ToFloat( void ) const
	{
		if ( !m_text[0] ) return 0.0f;
		return (float)_wtof( m_text );
	}

	SEGAN_LIB_INLINE sint ToInt( void ) const
	{
		if ( !m_text[0] ) return 0;
		return _wtoi( m_text );
	}

	SEGAN_LIB_INLINE operator const wchar* ( void ) const
	{
		return m_text;
	}

	SEGAN_LIB_INLINE const wchar* operator* ( void ) const
	{
		return m_text;
	}

	SEGAN_LIB_INLINE wchar& operator[] ( const sint index )
	{
		sx_assert( !m_text || index < 0 || index > m_len );
		return m_text[index];
	}

	SEGAN_LIB_INLINE String_fix& ExtractFilePath( void )
	{
		//	trusted . safe and optimized function
		if ( !m_text[0] ) return *this;
		sint counter = m_len;
		wchar* end = &m_text[m_len];
		while ( end != m_text )
		{
			counter--;
			end--;
			switch ( *end )
			{
			case '/' :
			case '\\': *end = 0; m_len = counter; return *this;
			}				
		}
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& ExtractFileName( void )
	{
		//	trusted . safe and optimized function
		if ( !m_text[0] ) return *this;
		wchar* first = m_text;
		wchar* end	 = &m_text[m_len];
		while ( end != m_text )
		{
			end--;
			if ( *end == '\\' || *end == '/')
			{
				end++; m_len=0;
				while ( *end )
				{
					m_len++;
					*(first++) = *(end++);
				}
				*first = 0;
				return *this;
			}
		}
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& ExtractFileExtension( void )
	{
		//	trusted . safe and optimized function
		if ( !m_text[0] ) return *this;
		wchar* first = m_text;
		wchar* end = &m_text[m_len];
		while ( end != m_text )
		{
			end--;
			if ( *end == '.' )
			{
				end++; m_len=0;
				while ( *end )
				{
					m_len++;
					*(first++) = *(end++);
				}
				*first = 0;
				return *this;
			}
		}
		Clear();
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& ExcludeFileExtension( void )
	{
		//	trusted . safe and optimized function
		if ( !m_text[0] ) return *this;
		sint counter = m_len;
		wchar* end = &m_text[m_len];
		while ( end != m_text )
		{
			counter--;
			end--;
			switch ( *end )
			{
			case '.' : *end = 0; m_len = counter; return *this;
			case '/' :
			case '\\': return *this;
			}				
		}
		return *this;
	}

	SEGAN_LIB_INLINE bool IsPathStyle( void )
	{
		if ( !m_text[0] ) return false;
		return ( m_text[m_len-1] == '/' || m_text[m_len-1] == '\\' );
	}

	SEGAN_LIB_INLINE bool IsFullPath( void )
	{
		if ( !m_text[0] ) return false;
		return ( Find(L":") || Find(L"\\\\") || Find(L"//") );
	}

	SEGAN_LIB_INLINE void MakePathStyle( void )
	{
		if ( !m_text[0] ) return;
		if ( m_len + 1 > count ) return;
		if ( m_text[m_len-1] != '/' && m_text[m_len-1] != '\\' ) {
			m_len++;
			m_text[m_len-1] = PATH_PART;
			m_text[m_len] = 0;
		}
	}

	//  operator declarations for = //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE String_fix& operator= ( const String_fix& Str )
	{
		SetText( Str.m_text );
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& operator= ( const wchar* str )
	{
		SetText( str );
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& operator= ( const char* str )
	{
		SetText( str );
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& operator= ( const wchar ch )
	{
		SetText( ch );
		return *this;
	}

	//  operator declarations for << //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE String_fix& operator<< ( const String_fix& Str )
	{
		Append( Str.m_text );
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& operator<< ( const wchar* str )
	{
		Append( str );
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& operator<< ( const char* str )
	{
		Append( str );
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& operator<< ( const char ch )
	{
		Append( ch );
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& operator<< ( const wchar ch )
	{
		Append( ch );
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& operator<< (sint number)
	{
		wchar tmp[64];
		_itow_s( number, tmp, 64, 10 );
		Append( tmp );
		return *this;
	}

	SEGAN_LIB_INLINE String_fix& operator<< (uint number)
	{
		wchar tmp[64];
		_itow_s( (int)number, tmp, 64, 10 );
		Append( tmp );
		return *this;
	}

	//  operator declarations for == //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE bool operator== ( const String_fix& Str ) const
	{
		return ( sx_str_cmp( m_text, Str.m_text ) == 0 );
	}

	SEGAN_LIB_INLINE bool operator== ( const wchar* str ) const
	{
		return ( sx_str_cmp( m_text, str ) == 0 );
	}

	SEGAN_LIB_INLINE bool operator== ( const char* str ) const
	{
		return ( sx_str_cmp( m_text, str ) == 0 );
	}

	//  operator declarations for != //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE bool operator!= ( const String_fix& Str ) const
	{
		return ( sx_str_cmp( m_text, Str.m_text ) != 0 );
	}

	SEGAN_LIB_INLINE bool operator!= ( const wchar* str ) const
	{
		return ( sx_str_cmp( m_text, str ) != 0 );
	}

	SEGAN_LIB_INLINE bool operator!= (const char* str) const
	{
		return ( sx_str_cmp( m_text, str ) != 0 );
	}

private:
	wchar			m_text[count];	//	main text
	sint			m_len;			//  length of String_fix
};



typedef String_fix<16>		str16;
typedef String_fix<32>		str32;
typedef String_fix<64>		str64;
typedef String_fix<128>		str128;
typedef String_fix<256>		str256;
typedef String_fix<512>		str512;
typedef String_fix<1024>	str1024;
typedef String_fix<2048>	str2048;


#endif	//	GUARD_String_HEADER_FILE
