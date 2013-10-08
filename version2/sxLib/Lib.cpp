#include "Lib.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

extern bool	callstack_end;


#if ( SEGAN_CRITICAL_SECTION == 1 )

#if defined(_WIN32)
#include <Windows.h>
CRITICAL_SECTION	s_mem_cs;
SEGAN_INLINE void lib_init_cs( void )
{
	InitializeCriticalSection( &s_mem_cs );
}
SEGAN_INLINE void lib_finit_cs( void )
{
	DeleteCriticalSection( &s_mem_cs );
}
SEGAN_INLINE void lib_enter_cs( void )
{
	EnterCriticalSection( &s_mem_cs );
}
SEGAN_INLINE void lib_leave_cs( void )
{
	LeaveCriticalSection( &s_mem_cs );
}
#endif

#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//	string functions
//	for multi threaded support we can use a simple string pool
wchar* str_pop( void )
{
	typedef wchar StrItem[1024];
	static StrItem str_pool[512];
	static sint index = 0;
	sx_enter_cs();
	if ( index >= 512 ) index = 0;
	wchar* res = str_pool[index++];
	sx_leave_cs();
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


//////////////////////////////////////////////////////////////////////////


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
	if ( sx_abs_f( number ) < 0.000001f )
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


//////////////////////////////////////////////////////////////////////////
// initialize internal library
void sx_lib_initialize( void )
{
#if SEGAN_CRITICAL_SECTION
	lib_init_cs();
#endif

#if SEGAN_MEMLEAK
	mem_enable_debug( true, 0 );
#endif

#if SEGAN_CALLSTACK
	callstack_clear();
#endif

}

// finalize internal library
void sx_lib_finalize( void )
{

#if SEGAN_CALLSTACK
	if ( !callstack_end )	//	verify that application crashed
	{
		callstack_report_to_file( L"sx_crash_report", L"application closed unexpectedly !" );

#if SEGAN_MEMLEAK
		mem_report_debug_to_file( L"sx_mem_corruption_report.txt", -1 );
		mem_clear_debug();
#endif

	}
#if SEGAN_MEMLEAK
	else
	{
	mem_report_debug_to_file( L"sx_memleak_report.txt", 0 );
	mem_clear_debug();
	}
#endif

	callstack_clear();
#elif SEGAN_MEMLEAK
	mem_report_debug_to_file( L"sx_memleak_report.txt", 0 );
	mem_clear_debug();
#endif

#if SEGAN_CRITICAL_SECTION
	lib_finit_cs();
#endif
}

