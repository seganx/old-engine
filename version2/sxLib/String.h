/********************************************************************
	created:	201705/23
	filename: 	String.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain advanced string functions
*********************************************************************/
#ifndef String_DEFINED
#define String_DEFINED


#include "Def.h"


#if	defined(_WIN32)
	#define	PATH_PART	'/'
#elif defined(_MAC)
	//#define	PATH_PART
#else
	//#define	PATH_PART
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

SEGAN_LIB_API uint sx_str_len( const char* str );
SEGAN_LIB_API sint sx_str_cmp( const char* str1, const char* str2 );
SEGAN_LIB_API const char* sx_str_str( const char* str, const char* what );

//! copy the src string to the destination string and return number of characters which have copied contain null character
SEGAN_LIB_API sint sx_str_copy( char* dest, const sint dest_size_in_byte, const char* src );

SEGAN_LIB_API char sx_str_upper( char c );
SEGAN_LIB_API char sx_str_lower( char c );

SEGAN_LIB_API sint sx_str_to_int( const char* str, const sint defaul_val);

SEGAN_LIB_API uint sx_str_to_uint(const char* str, const uint defaul_val);

SEGAN_LIB_API uint64 sx_str_to_uint64( const char* str, const uint64 defaul_val );

SEGAN_LIB_API const char* sx_str_get_filename(const char* filename);


/*! convert one wide char to a UTF-8 character and return the length of the converted UTF-8 character in bytes.*/
SEGAN_LIB_API uint sx_wchar_to_utf8( char* dest, const uint destsize, const uint ch );

/*! convert one UTF-8 char to a wide character and return the length of the UTF-8 input character in bytes.*/
SEGAN_LIB_API uint sx_utf8_to_wchar( wchar dest, const uint destwords, const char* src );

/*! convert wide char string to a UTF-8 character string and return the length of the converted UTF-8 characters in bytes.*/
SEGAN_LIB_API uint sx_str_to_utf8( char* dest, const uint destsize, const wchar* src );

/*! convert UTF-8 string to the wide character string and return the size of the converted string in wide chars.*/
SEGAN_LIB_API uint sx_utf8_to_str( wchar* dest, const uint destwords, const char* src );


//! string structure stores strings of characters 
typedef struct sx_string
{
	char*   text;       //	main text
    sint    len;        //  length of string
}
sx_string;

SEGAN_LIB_API char* sx_string_clear(struct sx_string* obj);
SEGAN_LIB_API char* sx_string_set(struct sx_string* obj, const char* text);
SEGAN_LIB_API char* sx_string_format(struct sx_string* obj, const char* format, ...);
SEGAN_LIB_API char* sx_string_append(struct sx_string* obj, const char* str);
SEGAN_LIB_API char* sx_string_insert(struct sx_string* obj, const char* str, sint _where);
SEGAN_LIB_API char* sx_string_remove(struct sx_string* obj, sint index, sint count);
SEGAN_LIB_API char* sx_string_copy_to(struct sx_string* obj, struct sx_string* dest, sint index, sint count);
SEGAN_LIB_API sint sx_string_find(struct sx_string* obj, const char* substr, sint from);
SEGAN_LIB_API sint sx_string_find_back(struct sx_string* obj, const char* substr, sint from);
SEGAN_LIB_API char* sx_string_replace(struct sx_string* obj, const char* what, const char* with);
SEGAN_LIB_API char* sx_string_revers(struct sx_string* obj, sint from, sint to);
SEGAN_LIB_API char* sx_string_trim(struct sx_string* obj);
SEGAN_LIB_API char* sx_string_make_upper(struct sx_string* obj);
SEGAN_LIB_API char* sx_string_make_lower(struct sx_string* obj);
SEGAN_LIB_API bool sx_string_is_path_style(struct sx_string* obj);
SEGAN_LIB_API bool sx_string_is_full_path(struct sx_string* obj);
SEGAN_LIB_API char* sx_string_make_path_style(struct sx_string* obj);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif	//	String_DEFINED
