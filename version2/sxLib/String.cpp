#include "String.h"
#include "Assert.h"

#if SEGAN_LIB_MULTI_THREADED
#include "Mutex.h"
#endif

#include <string>

#define char_upper(c) { if ( 'a' <= c && c <= 'z' ) c += 'A' - 'a'; return c; }
#define char_lower(c) { if ( 'A' <= c && c <= 'Z' ) c += 'a' - 'A'; return c; }
#define str_len(str) { uint len = 0; if ( str ) while ( *str++ ) ++len; return len; }

#define str_cmp(str1, str2) {\
	if ( str1 && str2 )	{\
		sint res = (sint)(*str1 - *str2);\
		while (!res && *str1++ && *str2++) res = (sint)(*str1 - *str2);\
		return res;	}\
	else if ( str1 ) return 1; else if ( str2 ) return -1; else return 0; }

#define str_copy(dest, dest_size_in_word, src) {\
	sx_assert(dest);\
	sint res = 0;\
	if (src) {\
		for (sint m = dest_size_in_word - 1; res < m; ++res) {\
			dest[res] = src[res];\
			if (dest[res] == 0) break; }\
		dest[res] = 0; }\
	return res;\
}


SEGAN_INLINE uint sx_str_len(const char* str) { str_len(str) }
SEGAN_INLINE uint sx_str_len(const wchar* str) { str_len(str) }

SEGAN_INLINE sint sx_str_cmp(const char* str1, const char* str2) { str_cmp(str1, str2) }
SEGAN_INLINE sint sx_str_cmp(const char* str1, const wchar* str2) { str_cmp(str1, str2) }
SEGAN_INLINE sint sx_str_cmp(const wchar* str1, const char* str2) { str_cmp(str1, str2) }
SEGAN_INLINE sint sx_str_cmp( const wchar* str1, const wchar* str2 ) { str_cmp(str1, str2) }

SEGAN_INLINE sint sx_str_copy(char* dest, const sint dest_size_in_byte, const char* src) { str_copy(dest, dest_size_in_byte, src) }
SEGAN_INLINE sint sx_str_copy(wchar* dest, const sint dest_size_in_word, const char* src) { str_copy(dest, dest_size_in_word, src) }
SEGAN_INLINE sint sx_str_copy(wchar* dest, const sint dest_size_in_word, const wchar* src) { str_copy(dest, dest_size_in_word, src) }
SEGAN_INLINE sint sx_str_copy(char* dest, const sint dest_size_in_word, const wchar* src) { str_copy(dest, dest_size_in_word, src) }

SEGAN_INLINE wchar sx_str_upper(wchar c) { char_upper(c) }
SEGAN_INLINE char sx_str_upper(char c) { char_upper(c) }

SEGAN_INLINE wchar sx_str_lower(wchar c) { char_lower(c) }
SEGAN_INLINE char sx_str_lower(char c) { char_lower(c) }

SEGAN_INLINE sint sx_str_to_int( const wchar* str, const sint defaul_val /*= 0 */ )
{
	if ( !str ) return defaul_val;
	return _wtoi( str );
}

SEGAN_INLINE sint sx_str_to_int( const char* str, const sint defaul_val /*= 0 */ )
{
	if ( !str ) return defaul_val;
	return atoi( str );
}

SEGAN_INLINE uint sx_str_to_uint( const wchar* str, const uint defaul_val /*= 0 */ )
{
	if ( !str ) return defaul_val;
	uint res = defaul_val;
	swscanf_s( str, L"%u", &res, sizeof(res) );
	return res;
}

SEGAN_INLINE uint64 sx_str_to_uint64( const wchar* str, const uint64 defaul_val /*= 0 */ )
{
	if ( !str ) return defaul_val;
	uint64 res = defaul_val;
	swscanf_s( str, L"%llu", &res, sizeof(res) );
	return res;
}

SEGAN_INLINE uint sx_str_to_uint( const char* str, const uint defaul_val /*= 0 */ )
{
	if ( !str ) return defaul_val;
	uint res = defaul_val;
	sscanf_s( str, "%u", &res, sizeof(res) );
	return res;
}

SEGAN_INLINE uint64 sx_str_to_uint64( const char* str, const uint64 defaul_val /*= 0 */ )
{
	if ( !str ) return defaul_val;
	uint64 res = defaul_val;
	sscanf_s( str, "%llu", &res, sizeof(res) );
	return res;
}

SEGAN_INLINE float sx_str_to_float( const wchar* str, const float defaul_val /*= 0 */ )
{
	if ( !str ) return defaul_val;
	return (float)_wtof( str );
}

SEGAN_INLINE const bool sx_str_is_pathstyle( const wchar* filepath )
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

SEGAN_INLINE const bool sx_str_is_fullpath( const wchar* filepath )
{
	if ( !filepath ) return false;
	return ( filepath[1] == ':' || ( filepath[0] == '\\' && filepath[1] == '\\') || ( filepath[0] == '/' && filepath[1] == '/') );
}


//////////////////////////////////////////////////////////////////////////
//	string functions
//	for multi threaded support we can use a simple string pool
wchar* str_pop( void )
{
	typedef wchar StrItem[1024];
	static StrItem str_pool[512];
	static sint index = 0;

#if SEGAN_LIB_MULTI_THREADED
	static Mutex s_mutex;
	s_mutex.lock();
#endif

	if ( index >= 512 ) index = 0;
	wchar* res = str_pool[index++];
	
#if SEGAN_LIB_MULTI_THREADED
	s_mutex.lock();
#endif

	return res;
}

SEGAN_INLINE uint sx_wchar_to_utf8( char* dest, const uint destsize, const uint ch )
{//	code from : http://www.opensource.apple.com/source/OpenLDAP/OpenLDAP-186/OpenLDAP/libraries/libldap/utf-8-conv.c
	uint len = 0;
	if ( !dest || !destsize )   /* just determine the required UTF-8 char length. */
	{
		if ( ch < 0 )			return 0;
		if ( ch < 0x80 )		return 1;
		if ( ch < 0x800 )		return 2; 
		if ( ch < 0x10000 )		return 3;
		if ( ch < 0x200000 )	return 4;
		if ( ch < 0x4000000 )	return 5;
	}
	else if ( ch < 0 )
	{
		len = 0;
	}
	else if ( ch < 0x80 )
	{
		if ( destsize >= 1 )
		{
			dest[len++] = (char)ch;
		}

	}
	else if ( ch < 0x800 )
	{
		if ( destsize >= 2 )
		{
			dest[len++] = 0xc0 | ( ch >> 6 );
			dest[len++] = 0x80 | ( ch & 0x3f );
		}

	}
	else if ( ch < 0x10000 )
	{
		if ( destsize >= 3 )
		{	
			dest[len++] = 0xe0 | ( ch >> 12 );
			dest[len++] = 0x80 | ( (ch >> 6) & 0x3f );
			dest[len++] = 0x80 | ( ch & 0x3f );
		}
	
	}
	else if ( ch < 0x200000 )
	{
		if (destsize >= 4)
		{
			dest[len++] = 0xf0 | ( ch >> 18 );
			dest[len++] = 0x80 | ( (ch >> 12) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 6) & 0x3f );
			dest[len++] = 0x80 | ( ch & 0x3f );
		}

	}
	else if ( ch < 0x4000000 )
	{
		if ( destsize >= 5 )
		{
			dest[len++] = 0xf8 | ( ch >> 24 );
			dest[len++] = 0x80 | ( (ch >> 18) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 12) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 6) & 0x3f );
			dest[len++] = 0x80 | ( ch & 0x3f );
		}
	}
	else
	{
		if ( destsize >= 6 )
		{
			dest[len++] = 0xfc | ( ch >> 30 );
			dest[len++] = 0x80 | ( (ch >> 24) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 18) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 12) & 0x3f );
			dest[len++] = 0x80 | ( (ch >> 6) & 0x3f );
			dest[len++] = 0x80 | ( ch & 0x3f );
		}
	}
	return len;
}


SEGAN_INLINE uint sx_str_to_utf8( char* dest, const uint destsize, const wchar* src )
{
	int r = 0;
	char tmp[32];
	char* d = dest;
	while ( *src )
	{
		r = sx_wchar_to_utf8( tmp, 32, *src++ );
		if ( r > 0 )
		{
			memcpy( d, tmp, r );
			d += r;
		}
		else
		{
			*d++ = (char)*src++;
		}
	}
	*d = 0;
	return (uint)( d - dest );
}


//////////////////////////////////////////////////////////////////////////
//	code from :
//	http://www.opensource.apple.com/source/OpenLDAP/OpenLDAP-186/OpenLDAP/libraries/libldap/utf-8.c

const char ldap_utf8_lentab[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 0, 0 };

const char ldap_utf8_mintab[] = {
	(char)0x20, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80,
	(char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80,
	(char)0x30, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80,
	(char)0x38, (char)0x80, (char)0x80, (char)0x80, (char)0x3c, (char)0x80, (char)0x00, (char)0x00 };

	/* LDAP_MAX_UTF8_LEN is 3 or 6 depending on size of wchar_t */
#define LDAP_MAX_UTF8_LEN		 ( sizeof(wchar) * 3/2 )
#define LDAP_UTF8_ISASCII(p)	 ( !(*(const unsigned char *)(p) & 0x80 ) )
#define LDAP_UTF8_CHARLEN(p)	 ( LDAP_UTF8_ISASCII(p) ? 1 : ldap_utf8_lentab[*(const unsigned char *)(p) ^ 0x80] )
#define LDAP_UTF8_CHARLEN2(p, l) ( ( ( l = LDAP_UTF8_CHARLEN( p )) < 3 || ( ldap_utf8_mintab[*(const unsigned char *)(p) & 0x1f] & (p)[1] ) ) ? l : 0 )

SEGAN_INLINE uint sx_utf8_to_wchar( wchar dest, const uint destwords, const char* src )
{
	if ( !src ) return 0;

	/* Get UTF-8 sequence length from 1st byte */
	sint utflen = LDAP_UTF8_CHARLEN2(src, utflen);

	if ( utflen==0 || utflen > (int)LDAP_MAX_UTF8_LEN ) return 0;

	/* First byte minus length tag */
	unsigned char mask[] = { 0, 0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
	wchar ch = (wchar)( src[0] & mask[utflen] );

	for( sint i = 1; i < utflen; ++i )
	{
		/* Subsequent bytes must start with 10 */
		if ( ( src[i] & 0xc0 ) != 0x80 ) return 0;

		ch <<= 6;			/* 6 bits of data in each subsequent byte */
		ch |= (wchar)( src[i] & 0x3f );
	}

	dest = ch;
	return utflen;
}

SEGAN_INLINE uint sx_utf8_to_str( wchar* dest, const uint destwords, const char* src )
{
	/* If input ptr is NULL or empty... */
	if ( !src || !*src )
	{
		if ( dest ) *dest = 0;
		return 0;
	}

	/* Examine next UTF-8 character.  If output buffer is NULL, ignore count */
	uint wclen = 0;
	while ( *src && ( !dest || wclen < destwords ) )
	{
		/* Get UTF-8 sequence length from 1st byte */
		sint utflen = LDAP_UTF8_CHARLEN2(src, utflen);

		if( !utflen || utflen > (sint)LDAP_MAX_UTF8_LEN ) return 0;

		/* First byte minus length tag */
		unsigned char mask[] = { 0, 0x7f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
		wchar ch = (wchar)( src[0] & mask[utflen] );

		for( sint i = 1; i < utflen; ++i )
		{
			/* Subsequent bytes must start with 10 */
			if ( (src[i] & 0xc0) != 0x80 ) return 0;

			ch <<= 6;			/* 6 bits of data in each subsequent byte */
			ch |= (wchar)( src[i] & 0x3f );
		}

		if ( dest ) dest[wclen] = ch;

		src += utflen;		/* Move to next UTF-8 character */
		wclen++;			/* Count number of wide chars stored/required */
	}

	/* Add null terminator if there's room in the buffer. */
	if ( dest && wclen < destwords ) dest[wclen] = 0;

	return wclen;
}


SEGAN_INLINE uint sx_str_format(char* dest, const uint destsize, const char* format, ...)
{
	sx_assert(null && "should be implemented!");
	return 0;
}


SEGAN_INLINE const wchar* sx_utf8_to_str( const char* src )
{
	wchar* tmp = str_pop();
	sx_utf8_to_str( tmp, 1024, src );
	return tmp;
}

SEGAN_LIB_API const char* sx_str_to_utf8( const wchar* src )
{
	char* tmp = (char*)str_pop();
	sx_str_to_utf8( tmp, 2048, src );
	return tmp;
}

SEGAN_INLINE const wchar* sx_int_to_str( const int number )
{
	wchar* tmp = str_pop();
	_itow_s( number, tmp, 64, 10 );
	return tmp;
}

SEGAN_INLINE const wchar* sx_uint_to_str( const uint number )
{
	wchar* tmp = str_pop();
	_ultow_s( number, tmp, 64, 10 );
	return tmp;
}

SEGAN_INLINE const wchar* sx_uint64_to_str( const uint64 number )
{
	wchar* tmp = str_pop();
	_ui64tow_s( number, tmp, 64, 10 );
	return tmp;
}

SEGAN_INLINE const wchar* sx_float_to_str( float number, int precision )
{
	sx_assert( precision > 0 && precision < 7 );
	wchar* res = str_pop();
	if ( abs( number ) < 0.000001f )
	{
		sx_str_copy( res, precision + 3, L"0.000000" );
	}
	else
	{
		char tmp[64];
		int decimal, sign;
		_fcvt_s( tmp, 64, number, precision, &decimal, &sign );
		if ( tmp[0] )
		{
			int k = 0;
			if ( sign ) res[k++] = '-';
			if ( decimal > 0 )
			{
				int d = 0;
				for ( ; d < decimal; )
					res[k++] = tmp[d++];
				res[k++] = '.';
				for ( ; tmp[d]; )
					res[k++] = tmp[d++];
			}
			else
			{
				res[k++] = '0';
				res[k++] = '.';
				for (; decimal<0; decimal++ )
					res[k++] = '0';
				for (; tmp[decimal]; decimal++ )
					res[k++] = tmp[decimal];
			}
			res[k++] = 0;
		}
		else sx_str_copy( res, precision + 3, L"0.000000" );
	}
	return res;
}

SEGAN_INLINE const wchar* sx_str_extract_filepath( const wchar* filename )
{
	if ( !filename ) return null;
	wchar* res = str_pop();
	sint p = 0;
	for ( sint i=0; filename[i]; ++i )
	{
		if ( filename[i] == '\\' || filename[i] == '/' )
			p = i + 1;
	}
	sint i = 0;
	for ( ; i < p; i++ )
		res[i] = filename[i];
	res[i] = 0;
	return res;
}

SEGAN_INLINE const wchar* sx_str_extract_filename( const wchar* filename )
{
	if ( !filename ) return null;
	wchar* res = str_pop();
	sint p = 0;
	for ( sint i=0; filename[i]; ++i )
	{
		if ( filename[i] == '\\' || filename[i] == '/' )
			p = i + 1;
	}
	sint i = 0;
	for ( ; filename[p]; ++i, ++p )
		res[i] = filename[p];
	res[i] = 0;
	return res;
}

SEGAN_INLINE const wchar* sx_str_extract_extension( const wchar* filename )
{
	if ( !filename ) return null;
	wchar* res = str_pop();
	sint p = 0;
	for ( sint i=0; filename[i]; ++i )
	{
		if ( filename[i] == '.' )
			p = i + 1;
	}
	sint i = 0;
	for ( ; filename[p]; ++i, ++p )
		res[i] = filename[p];
	res[i] = 0;
	return res;
}

SEGAN_INLINE const wchar* sx_str_exclude_extension( const wchar* filename )
{
	if ( !filename ) return null;
	wchar* res = str_pop();
	sint p = 0;
	for ( sint i=0; filename[i]; ++i )
	{
		if ( filename[i] == '.' )
			p = i;
	}
	sint i = 0;
	for ( ; i < p; ++i )
		res[i] = filename[i];
	res[i] = 0;
	return res;
}

SEGAN_INLINE const wchar* sx_str_make_pathstyle( const wchar* filepath )
{
	if ( !filepath ) return null;
	sint len = sx_str_len( filepath );
	if ( len )
	{
		wchar ch = filepath[--len];
		if ( ch != '/' && ch != '\\' )
		{
			++len;
			wchar* res = str_pop();
			for ( int i=0; i<len; ++i )
				res[i] = filepath[i];
			res[len++] = PATH_PART;
			res[len] = 0;
			return res;
		}
	}
	return filepath;
}

SEGAN_LIB_API const wchar* sx_str_get_filename(const wchar* filename)
{
	const wchar* res = filename;
	for (const wchar* c = filename; *c != 0; ++c)
		if (*c == '/' || *c == '\\')
			res = c + 1;
	return res;
}

SEGAN_LIB_API const char* sx_str_get_filename(const char* filename)
{
	const char* res = filename;
	for (const char* c = filename; *c != 0; ++c)
		if (*c == '/' || *c == '\\')
			res = c + 1;
	return res;
}

SEGAN_LIB_API const wchar* sx_str_get_value(const wchar* str, const wchar* key)
{
	if ( str != null )
	{
		const wchar* c = wcsstr(str, key);
		if ( c != null )
		{
			while ( *c != '\n' && *c != 0 )
			{
				c++;
				if ( *c == ':' )
				{
					while ( *c != '\n' && *c != 0 && (*c == ':' || *c == ' ' || *c == '	') )
						c++;
					if ( *c != '\n' && *c != 0 )
						return c;
				}
			}
		}		
	}	
	return null;
}

SEGAN_LIB_API const char* sx_str_get_value(const char* str, const char* key)
{
	if ( str != null )
	{
		const char* c = strstr( str, key );
		if (c != null)
		{
			while (*c != '\n' && *c != 0)
			{
				c++;
				if (*c == ':')
				{
					while (*c != '\n' && *c != 0 && (*c == ':' || *c == ' ' || *c == '	'))
						c++;
					if (*c != '\n' && *c != 0)
						return c;
				}
			}
		}
	}
	return null;
}

SEGAN_LIB_API bool sx_str_get_value(wchar* dest, const uint dest_size_in_word, const wchar* str, const wchar* key)
{
	const wchar* c = sx_str_get_value( str, key );
	if ( c )
	{
		uint i = 0;
		for ( ; i < dest_size_in_word - 1; ++i )
		{
			if ( *c != '\n' && *c != '"' )
				dest[i] = *c++;
			else break;
		}
		dest[i] = 0;
		return true;
	}
	return false;
}

SEGAN_LIB_API bool sx_str_get_value(char* dest, const uint dest_size_in_byte, const wchar* str, const wchar* key)
{
	const wchar* c = sx_str_get_value(str, key);
	if (c)
	{
		uint i = 0;
		for (; i < dest_size_in_byte - 1; ++i)
		{
			if (*c != '\n' && *c != '"')
				dest[i] = (char)*c++;
			else break;
		}
		dest[i] = 0;
		return true;
	}
	return false;
}

SEGAN_LIB_API sint sx_str_get_value_int( const wchar* str, const wchar* key, const sint default_val )
{
	const wchar* c = sx_str_get_value( str, key );
	return sx_str_to_int( c, default_val );
}

SEGAN_LIB_API sint sx_str_get_value_int(const char* str, const char* key, const sint default_val)
{
	const char* c = sx_str_get_value(str, key);
	return sx_str_to_int(c, default_val);
}

SEGAN_LIB_API uint sx_str_get_value_uint(const wchar* str, const wchar* key, const uint default_val)
{
	const wchar* c = sx_str_get_value(str, key);
	return sx_str_to_uint(c, default_val);
}

SEGAN_LIB_API uint sx_str_get_value_uint(const char* str, const char* key, const uint default_val)
{
	const char* c = sx_str_get_value(str, key);
	return sx_str_to_uint(c, default_val);
}


//////////////////////////////////////////////////////////////////////////
//	string class stores strings of wide characters
//////////////////////////////////////////////////////////////////////////
String::String( sint sampler /*= 32*/ ): m_text(0), m_len(0), m_size(0), m_sampler(sampler), m_tmp(0) {}

String::String( const char ch ): m_text(0), m_len(0), m_size(0), m_sampler(32), m_tmp(0)
{
	append( ch );
}

String::String( const char* str ): m_text(0), m_len(0), m_size(0), m_sampler(32), m_tmp(0)
{
	append( str );
}

String::String( const wchar* str ): m_text(0), m_len(0), m_size(0), m_sampler(32), m_tmp(0)
{
	append( str );
}

String::String( const String& str ): m_text(0), m_len(0), m_size(0), m_sampler(str.m_sampler), m_tmp(0)
{
	append( str );
}

String::~String( void )
{
	free( m_text );
}

SEGAN_INLINE void String::clear( bool freemem /*= false*/ )
{
	if ( freemem )
	{
		free( m_text );
		m_size = 0;
	}
	else if ( m_text )
	{
		m_text[0] = 0;
	}
	m_len = 0;
}

SEGAN_INLINE void String::set_text( const wchar* str )
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
			clear();
		}
	}
}

SEGAN_INLINE void String::set_text( const char* str )
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
		clear();
	}
}

SEGAN_INLINE void String::set_text( const wchar ch )
{
	m_len  = 1;
	_Realloc( 2 );
	m_text[0] = ch;
	m_text[1] = 0;
}

SEGAN_INLINE void String::format( const wchar* format, ... )
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

SEGAN_INLINE const wchar* String::text( void ) const
{
	return m_text;
}

SEGAN_INLINE void String::append( const String& str )
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

SEGAN_INLINE void String::append( const wchar* str )
{
	//	trusted . safe and optimized function
	if ( !str ) return;
	sint slen = sx_str_len( str );
	if ( !slen ) return;
	_Realloc( m_len + slen + 1 );
	for ( sint i=0; i<=slen; m_text[i+m_len] = str[i], i++ );
	m_len += slen;
}

SEGAN_INLINE void String::append( const char* str )
{
	//	trusted . safe and optimized function
	if ( !str ) return;
	sint slen = sx_str_len( str );
	if ( !slen ) return;
	_Realloc( m_len + slen + 1 );
	for ( sint i=0; i<=slen; m_text[i+m_len] = str[i], i++ );
	m_len += slen;
}

SEGAN_INLINE void String::append( const wchar c )
{
	//	trusted . safe and optimized function
	m_len++;
	_Realloc( m_len + 1 );
	m_text[m_len-1] = c;
	m_text[m_len] = 0;
}

SEGAN_INLINE void String::insert( const wchar* str, sint _where /*= 0*/ )
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

SEGAN_INLINE void String::insert( const char* str, sint _where /*= 0*/ )
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

SEGAN_INLINE void String::insert( const wchar ch, sint _where /*= 0*/ )
{
	//	trusted . safe and optimized function
	if ( _where < 0 ) _where = 0;
	else if ( _where > m_len ) _where = m_len;
	_Realloc( m_len + 2 );
	for ( sint i=m_len; i>=_where; m_text[i+1] = m_text[i], i-- );
	m_text[_where] = ch;
	m_len++;
}

SEGAN_INLINE void String::remove( sint index, sint count /*= 1*/ )
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

SEGAN_INLINE sint String::length( void ) const
{
	return m_len;
}

SEGAN_INLINE void String::copy_to( String& dest, sint index, sint count )
{
	//	trusted . safe and optimized function
	if ( !m_text || count < 1 ) return;
	if ( index < 0 || index >= m_len ) return;
	if ( index+count > m_len ) count = m_len-index;
	dest._Realloc( dest.m_len + count + 1 );
	for ( sint i=0; i<count; dest.m_text[i+dest.m_len]=m_text[i+index], i++ );
	dest.m_len += count;
	dest.m_text[dest.m_len] = 0;
}

SEGAN_INLINE sint String::find( const wchar* substr, sint from /*= 0*/ ) const
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

SEGAN_INLINE sint String::find_back( const wchar* substr, sint from /*= 0*/ ) const
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

SEGAN_INLINE void String::replace( const wchar* what, const wchar* with )
{
	if ( !m_text || !what || !with ) return;

	int lenwhat = (int)sx_str_len(what);
	int lenwith = (int)sx_str_len(with);
	int index = 0;
	while ( ( index = find(what, index) ) > -1 )
	{
		remove(index, lenwhat);
		insert(with, index);
		index += lenwith;
	}
}

SEGAN_INLINE void String::revers( sint from, sint to )
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

SEGAN_INLINE void String::trim( void )
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

SEGAN_INLINE void String::make_upper( void )
{
	if ( !m_text ) return;	
	for ( wchar* cp=m_text; *cp; ++cp ) {
		if ( 'a' <= *cp && *cp <= 'z' )
			*cp += 'A' - 'a';
	}
}

SEGAN_INLINE void String::make_lower( void )
{
	if ( !m_text ) return;	
	for ( wchar* cp = m_text; *cp; ++cp ) {
		if ( 'A' <= *cp && *cp <= 'Z' )
			*cp += 'a' - 'A';
	}
}

SEGAN_INLINE float String::to_float( void ) const
{
	if ( !m_text ) return 0.0f;
	return (float)_wtof( m_text );
}

SEGAN_INLINE sint String::to_int( void ) const
{
	if ( !m_text ) return 0;
	return _wtoi( m_text );
}

SEGAN_INLINE String::operator const wchar* ( void ) const
{
	return m_text;
}

SEGAN_INLINE const wchar* String::operator* ( void ) const
{
	return m_text;
}

SEGAN_INLINE wchar& String::operator[] ( const sint index )
{
	sx_assert( m_text && index>=0 && index <= m_len );
	return m_text[index];
}

SEGAN_INLINE String& String::extract_file_path( void )
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

SEGAN_INLINE String& String::extract_file_name( void )
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

SEGAN_INLINE String& String::extract_file_extension( void )
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
	clear();
	return *this;
}

SEGAN_INLINE String& String::exclude_file_extension( void )
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

SEGAN_INLINE bool String::is_path_style( void )
{
	if ( !m_text ) return false;
	return ( m_text[m_len-1] == '/' || m_text[m_len-1] == '\\' );
}

SEGAN_INLINE bool String::is_full_path( void )
{
	if ( !m_text ) return false;
	return ( m_text[1] == ':' || ( m_text[0] == '\\' && m_text[1] == '\\') || ( m_text[0] == '/' && m_text[1] == '/') );
}

SEGAN_INLINE void String::make_path_style( void )
{
	if ( !m_text ) return;
	if ( m_text[m_len-1] != '/' && m_text[m_len-1] != '\\' ) {
		m_len++;
		_Realloc( m_len + 1 );
		m_text[m_len-1] = PATH_PART;
		m_text[m_len] = 0;
	}
}

SEGAN_INLINE String& String::operator= ( const String& Str )
{
	set_text( Str.m_text );
	return *this;
}

SEGAN_INLINE String& String::operator= ( const wchar* str )
{
	set_text( str );
	return *this;
}

SEGAN_INLINE String& String::operator= ( const char* str )
{
	set_text( str );
	return *this;
}

SEGAN_INLINE String& String::operator= ( const wchar ch )
{
	set_text( ch );
	return *this;
}

SEGAN_INLINE String& String::operator<< ( const String& str )
{
	append( str );
	return *this;
}

SEGAN_INLINE String& String::operator<< ( const wchar* str )
{
	append( str );
	return *this;
}

SEGAN_INLINE String& String::operator<< ( const char* str )
{
	append( str );
	return *this;
}

SEGAN_INLINE String& String::operator<< ( const char ch )
{
	append( ch );
	return *this;
}

SEGAN_INLINE String& String::operator<< ( const wchar ch )
{
	append( ch );
	return *this;
}

SEGAN_INLINE String& String::operator<< ( const int number )
{
	wchar tmp[64];
	_itow_s( (int)number, tmp, 64, 10 );
	append( tmp );
	return *this;
}

SEGAN_INLINE bool String::operator== ( const String& Str ) const
{
	return ( sx_str_cmp( m_text, Str.m_text ) == 0 );
}

SEGAN_INLINE bool String::operator== ( const wchar* str ) const
{
	return ( sx_str_cmp( m_text, str ) == 0 );
}

SEGAN_INLINE bool String::operator== ( const char* str ) const
{
	return ( sx_str_cmp( m_text, str ) == 0 );
}

SEGAN_INLINE bool String::operator!= ( const String& Str ) const
{
	return ( sx_str_cmp( m_text, Str.m_text ) != 0 );
}

SEGAN_INLINE bool String::operator!= ( const wchar* str ) const
{
	return ( sx_str_cmp( m_text, str ) != 0 );
}

SEGAN_INLINE bool String::operator!= (const char* str) const
{
	return ( sx_str_cmp( m_text, str ) != 0 );
}

SEGAN_INLINE void String::_Realloc( sint newSize )
{
	if ( m_sampler ) {
		if ( newSize >= m_size || ( m_size - newSize ) > m_sampler ) {
			m_size = sint( newSize / m_sampler ) * m_sampler + m_sampler;
			m_text = (wchar*)::realloc( m_text, m_size * sizeof(wchar) );
		}
	} else {
		m_size = newSize;
		m_text = (wchar*)::realloc( m_text, m_size * sizeof(wchar) );
	}
}

