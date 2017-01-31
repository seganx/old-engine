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
#include <string.h>


#if	defined(_WIN32)
	#define	PATH_PART	'\\'
#elif defined(_MAC)
	//#define	PATH_PART
#else
	//#define	PATH_PART
#endif


//! copy the src string to the destination string and return number of characters which have copied contain null character
SEGAN_LIB_INLINE sint sx_str_copy( wchar* dest, const sint dest_size_in_word, const wchar* src )
{
	sx_assert(dest);
	sint res = 0;
	if ( src )
	{
		for ( ; res<dest_size_in_word; ++res )
		{
			dest[res] = src[res];
			if ( dest[res] == 0 ) break;
		}
		dest[dest_size_in_word-1] = 0;
	}
	return res;
}

//! copy the src string to the destination string and return number of characters which have copied contain null character
SEGAN_LIB_INLINE sint sx_str_copy( wchar* dest, const sint dest_size_in_word, const char* src )
{
	sx_assert(dest);
	sint res = 0;
	if ( src )
	{
		for ( ; res<dest_size_in_word; ++res )
		{
			dest[res] = src[res];
			if ( dest[res] == 0 ) break;
		}
		dest[dest_size_in_word-1] = 0;
	}
	return res;
}

SEGAN_LIB_INLINE uint sx_str_to_uint( const wchar* str )
{
	if ( !str ) return 0;
	uint res = 0;
	swscanf_s( str, L"%u", &res );
	return res;
}


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

	String( const char InChar )
		: m_text(0)
		, m_len(0)
		, m_size(0)
		, m_sampler(32)
		, m_tmp(0)
	{
		Append( InChar );
	}

	String( const char* InStr )
		: m_text(0)
		, m_len(0)
		, m_size(0)
		, m_sampler(32)
		, m_tmp(0)
	{
		Append( InStr );
	}

	String( const wchar* InStr )
		: m_text(0)
		, m_len(0)
		, m_size(0)
		, m_sampler(32)
		, m_tmp(0)
	{
		Append( InStr );
	}

	String( const String& InStr )
		: m_text(0)
		, m_len(0)
		, m_size(0)
		, m_sampler(InStr.m_sampler)
		, m_tmp(0)
	{
		Append( InStr.m_text );
	}

	~String( void )
	{
		mem_free( m_text );
	}

	SEGAN_LIB_INLINE void Clear( void )
	{
		mem_free( m_text );
		m_text = 0;
		m_size = 0;
		m_len = 0;
	}

	SEGAN_LIB_INLINE void SetText( const wchar* str )
	{
		if ( str == m_text ) return;
		if ( str )
		{
			m_len  = (uint)wcslen(str);
			_Realloc( m_len + 1 );
			memcpy( m_text, str, m_len * sizeof(wchar) );
			m_text[m_len] = 0;
			return;
		}
		else
		{
			Clear();
			return;
		}
	}

	SEGAN_LIB_INLINE void SetText( const char* str )
	{
		if ( str )
		{
			m_len  = (uint)strlen(str);
			_Realloc( m_len + 1 );
			for ( sint i=0; i < m_len; m_text[i] = str[i], i++ );
			m_text[m_len] = 0;
			return;
		}
		else
		{
			Clear();
			return;
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

	SEGAN_LIB_INLINE void Append( const wchar* str )
	{
		//	trusted . safe and optimized function
		if ( !str ) return;
		sint slen = (sint)wcslen(str);
		if ( !slen ) return;
		_Realloc( m_len + slen + 1 );
		for ( sint i=0; i<=slen; m_text[i+m_len]=str[i], i++ );
		m_len += slen;
	}

	SEGAN_LIB_INLINE void Append( const char* str )
	{
		//	trusted . safe and optimized function
		if ( !str ) return;
		sint slen = (sint)strlen(str);
		if ( !slen ) return;
		_Realloc( m_len + slen + 1 );
		for ( sint i=0; i<=slen; m_text[i+m_len]=str[i], i++ );
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
		sint slen = (sint)wcslen(str);
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
		sint slen = (sint)strlen(str);
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

		int lenwhat = (int)wcslen(what);
		int lenwith = (int)wcslen(with);
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
		if ( !m_text || index < 0 || index > m_len ) return m_tmp;
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
		return ( Find(L":")>-1 || Find(L"\\\\")>-1 || Find(L"//")>-1 );
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
	SEGAN_LIB_INLINE String& operator<< ( const String& Str )
	{
		Append( Str.m_text );
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

	SEGAN_LIB_INLINE String& operator<< (int number)
	{
		wchar tmp[64];
		_itow_s( number, tmp, 64, 10 );
		Append( tmp );
		return *this;
	}

	//  operator declarations for == //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE bool operator== ( const String& Str ) const
	{
		if ( !m_text || !Str.m_text )
			return ( m_text == Str.m_text );
		return ( wcscmp( m_text, Str.m_text ) == 0 );
	}

	SEGAN_LIB_INLINE bool operator== ( const wchar* str ) const
	{
		if ( !m_text || !str )
			return ( m_text == str );
		return ( wcscmp( m_text, str ) == 0 );
	}

	SEGAN_LIB_INLINE bool operator== ( const char* str ) const
	{
		if ( !m_text || !str )
			return ( (void*)m_text == (void*)str );
		String tmp = str;
		return ( wcscmp( m_text, tmp.m_text ) == 0 );
	}

	//  operator declarations for != //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE bool operator!= ( const String& Str ) const
	{
		if ( !m_text || !Str.m_text )
			return ( (void*)m_text != (void*)Str.m_text );
		return ( wcscmp( m_text, Str.m_text ) != 0 );
	}

	SEGAN_LIB_INLINE bool operator!= ( const wchar* str ) const
	{
		if ( !m_text || !str )
			return ( (void*)m_text != (void*)str );
		return ( wcscmp( m_text, str ) != 0 );
	}

	SEGAN_LIB_INLINE bool operator!= (const char* str) const {
		if (!m_text || !str)
			return ( (void*)m_text != (void*)str );
		String tmp = str;
		return ( wcscmp( m_text, tmp.m_text ) != 0 );
	}

	//////////////////////////////////////////////////////////////////////////
	//	STATICS FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	SEGAN_LIB_INLINE static void Copy( wchar* dest, const sint dest_size_in_word, const wchar* src )
	{
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

	SEGAN_LIB_INLINE static sint StrToInt( const wchar* str )
	{
		if ( !str ) return 0;
		return _wtoi( str );
	}

	SEGAN_LIB_INLINE static float StrToFloat( const wchar* str )
	{
		if ( !str ) return 0.0f;
		return (float)_wtof( str );
	}

	SEGAN_LIB_INLINE static wchar UpperChar( wchar c )
	{
		if ( 'a' <= c && c <= 'z' )
			c += 'A' - 'a';
		return c;
	}

	SEGAN_LIB_INLINE static wchar LowerChar( wchar c )
	{
		if ( 'A' <= c && c <= 'Z' )
			c += 'a' - 'A';
		return c;
	}

	SEGAN_LIB_INLINE static bool IsPathStyle( const wchar* Path )
	{
		if ( !Path ) return false;
		sint len = (sint)wcslen(Path) - 1;
		if ( len < 1 ) return false;
		return ( Path[len] == '/' || Path[len] == '\\' );
	}

	SEGAN_LIB_INLINE static bool IsFullPath( const wchar* Path )
	{
		if ( !Path ) return false;
		String path = Path;
		return ( path.Find( L":" )>-1 || path.Find( L"\\\\" )>-1 || path.Find( L"//" )>-1 );
	}

	SEGAN_LIB_INLINE static bool Compare( const wchar* str1, const wchar* str2 )
	{
		if ( !str1 || !str2 ) return ( str1 == str2 );
		return ( wcscmp( str1, str2 ) == 0 );
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

	String_fix( const char InChar ): m_len(0)
	{
		m_text[0] = 0;
		Append( InChar );
	}

	String_fix( const char* InStr ): m_len(0)
	{
		m_text[0] = 0;
		Append( InStr );
	}

	String_fix( const wchar* InStr ): m_len(0)
	{
		m_text[0] = 0;
		Append( InStr );
	}

	String_fix( const String_fix& InStr ): m_len(0)
	{
		m_text[0] = 0;
		Append( InStr.m_text );
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
		if ( str == m_text ) return;
		if ( str )
		{
			m_len  = (uint)wcslen( str );
			if ( m_len > count - 1 ) m_len = count - 1;
			for ( sint i = 0; i < m_len; m_text[i] = str[i], i++ );
			m_text[m_len] = 0;
			return;
		}
		else
		{
			Clear();
			return;
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
			return;
		}
		else
		{
			Clear();
			return;
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
		sint slen = (sint)wcslen(str);
		if ( slen < 1 ) return;
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
		if ( slen < 1 ) return;
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

		int lenwhat = (int)wcslen(what);
		int lenwith = (int)wcslen(with);
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
		if ( !m_text[0] || index < 0 || index > m_len ) return m_text[m_len-1];
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
		if ( !m_text[0] || !Str.m_text )
			return ( m_text == Str.m_text );
		return ( wcscmp( m_text, Str.m_text ) == 0 );
	}

	SEGAN_LIB_INLINE bool operator== ( const wchar* str ) const
	{
		if ( !m_text[0] || !str )
			return ( m_text == str );
		return ( wcscmp( m_text, str ) == 0 );
	}

	SEGAN_LIB_INLINE bool operator== ( const char* str ) const
	{
		if ( !m_text[0] || !str )
			return ( (void*)m_text == (void*)str );
		String_fix tmp = str;
		return ( wcscmp( m_text, tmp.m_text ) == 0 );
	}

	//  operator declarations for != //////////////////////////////////////////////////////////////////////////
	SEGAN_LIB_INLINE bool operator!= ( const String_fix& Str ) const
	{
		if ( !m_text[0] || !Str.m_text )
			return ( m_text != Str.m_text );
		return ( wcscmp( m_text, Str.m_text ) != 0 );
	}

	SEGAN_LIB_INLINE bool operator!= ( const wchar* str ) const
	{
		if ( !m_text[0] || !str )
			return ( m_text != str );
		return ( wcscmp( m_text, str ) != 0 );
	}

	SEGAN_LIB_INLINE bool operator!= (const char* str) const {
		if (!m_text[0] || !str)
			return ( (void*)m_text != (void*)str );
		String_fix tmp = str;
		return ( wcscmp( m_text, tmp.m_text ) != 0 );
	}

	//////////////////////////////////////////////////////////////////////////
	//	STATICS FUNCTIONS
	//////////////////////////////////////////////////////////////////////////

	SEGAN_LIB_INLINE static sint StrToInt( const wchar* str )
	{
		if ( !str ) return 0;
		return _wtoi(str);
	}

	SEGAN_LIB_INLINE static float StrToFloat( const wchar* str )
	{
		if ( !str ) return 0.0f;
		return (float)_wtof(str);
	}

	SEGAN_LIB_INLINE static wchar UpperChar( wchar c )
	{
		if ( 'a' <= c && c <= 'z' )
			c += 'A' - 'a';
		return c;
	}

	SEGAN_LIB_INLINE static wchar LowerChar( wchar c )
	{
		if ( 'A' <= c && c <= 'Z' )
			c += 'a' - 'A';
		return c;
	}

	SEGAN_LIB_INLINE static bool IsPathStyle( const wchar* Path )
	{
		if ( !Path ) return false;
		sint len = (sint)wcslen(Path) - 1;
		if ( len < 1 ) return false;
		return ( Path[len]=='/' || Path[len]=='\\' );
	}

	SEGAN_LIB_INLINE static bool IsFullPath( const wchar* Path )
	{
		if ( !Path ) return false;
		String_fix path = Path;
		return path.Find( L":" ) || path.Find( L"\\\\" ) || path.Find( L"//" );
	}

private:
	wchar			m_text[count];	//	main text
	sint			m_len;			//  length of String_fix
};

SEGAN_LIB_API const wchar* IntToStr( const int number );
SEGAN_LIB_API const wchar* FloatToStr( float number, int precision = 3 );

typedef String_fix<16>		str16;
typedef String_fix<32>		str32;
typedef String_fix<64>		str64;
typedef String_fix<128>		str128;
typedef String_fix<256>		str256;
typedef String_fix<512>		str512;
typedef String_fix<1024>	str1024;
typedef String_fix<2048>	str2048;


#endif	//	GUARD_String_HEADER_FILE
