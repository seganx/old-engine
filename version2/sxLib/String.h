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


#include "Def.h"


#if	defined(_WIN32)
	#define	PATH_PART	'/'
#elif defined(_MAC)
	//#define	PATH_PART
#else
	//#define	PATH_PART
#endif


SEGAN_LIB_API uint sx_str_len( const wchar* str );
SEGAN_LIB_API uint sx_str_len( const char* str );
SEGAN_LIB_API sint sx_str_cmp( const wchar* str1, const wchar* str2 );
SEGAN_LIB_API sint sx_str_cmp( const char* str1, const char* str2 );
SEGAN_LIB_API sint sx_str_cmp( const wchar* str1, const char* str2 );

//! copy the src string to the destination string and return number of characters which have copied contain null character
SEGAN_LIB_API sint sx_str_copy( wchar* dest, const sint dest_size_in_word, const wchar* src );

//! copy the src string to the destination string and return number of characters which have copied contain null character
SEGAN_LIB_API sint sx_str_copy( wchar* dest, const sint dest_size_in_word, const char* src );

//! copy the src string to the destination string and return number of characters which have copied contain null character
SEGAN_LIB_API sint sx_str_copy( char* dest, const sint dest_size_in_word, const wchar* src );

//! copy the src string to the destination string and return number of characters which have copied contain null character
SEGAN_LIB_API sint sx_str_copy( char* dest, const sint dest_size_in_byte, const char* src );

SEGAN_LIB_API wchar sx_str_upper( wchar c );
SEGAN_LIB_API wchar sx_str_lower( wchar c );
SEGAN_LIB_API char sx_str_upper( char c );
SEGAN_LIB_API char sx_str_lower( char c );
SEGAN_LIB_API sint sx_str_to_int( const wchar* str, const sint defaul_val = 0 );
SEGAN_LIB_API sint sx_str_to_int( const char* str, const sint defaul_val = 0 );
SEGAN_LIB_API uint sx_str_to_uint( const wchar* str, const uint defaul_val = 0 );
SEGAN_LIB_API uint64 sx_str_to_uint64( const wchar* str, const uint64 defaul_val = 0 );
SEGAN_LIB_API uint sx_str_to_uint( const char* str, const uint defaul_val = 0 );
SEGAN_LIB_API uint64 sx_str_to_uint64( const char* str, const uint64 defaul_val = 0 );
SEGAN_LIB_API float sx_str_to_float( const wchar* str, const float defaul_val = 0 );

/*! return true if entry string is type of file path and end with '/' or '\'*/
SEGAN_LIB_API const bool sx_str_is_pathstyle( const wchar* filepath );

/*! return true if entry string is as complete filename*/
SEGAN_LIB_API const bool sx_str_is_fullpath( const wchar* filepath );

/*! convert one wide char to a UTF-8 character and return the length of the converted UTF-8 character in bytes.*/
SEGAN_LIB_API uint sx_wchar_to_utf8( char* dest, const uint destsize, const uint ch );

/*! convert wide char string to a UTF-8 character string and return the length of the converted UTF-8 characters in bytes.*/
SEGAN_LIB_API uint sx_str_to_utf8( char* dest, const uint destsize, const wchar* src );

/*! convert UTF-8 char to a wide character and return the length of the UTF-8 input character in bytes.*/
SEGAN_LIB_API uint sx_utf8_to_wchar( wchar* dest, const uint destwords, const char* src );

/*! convert UTF-8 string to the wide character string and return the size of the converted string in wide chars.*/
SEGAN_LIB_API uint sx_utf8_to_str( wchar* dest, const uint destwords, const char* src );

/*! convert UTF-8 string to the wide character string.
NOTE: maximum length of wide character string is 1024
NOTE: this function uses string memory pool and the returned string will be deleted in the next calls*/
SEGAN_LIB_API const wchar* sx_utf8_to_str( const char* src );

/*! convert wide character string to the UTF-8 string
NOTE: maximum length of UTF-8 character string is less than 2048
NOTE: this function uses string memory pool and the returned string will be deleted in the next calls*/
SEGAN_LIB_API const char* sx_str_to_utf8( const wchar* src );

/*! convert integer number to string.
NOTE: this function uses string memory pool and the returned string will be deleted in the next calls*/
SEGAN_LIB_API const wchar* sx_int_to_str( const sint number );

/*! convert unsigned integer number to string.
NOTE: this function uses string memory pool and the returned string will be deleted in the next calls*/
SEGAN_LIB_API const wchar* sx_uint_to_str( const uint number );

/*! convert unsigned integer 64bit number to string.
NOTE: this function uses string memory pool and the returned string will be deleted in the next calls*/
SEGAN_LIB_API const wchar* sx_uint64_to_str( const uint64 number );

/*! convert float number to string with specified precision.
NOTE: this function uses string memory pool and the returned string will be deleted in the next calls*/
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
class SEGAN_LIB_API String
{
public:
	String( sint sampler = 32 );
	String( const char ch );
	String( const char* str );
	String( const wchar* str );
	String( const String& str );
	~String( void );
	SEGAN_INLINE void clear( bool freemem = false );
	SEGAN_INLINE void set_text( const wchar* str );
	SEGAN_INLINE void set_text( const char* str );
	SEGAN_INLINE void set_text( const wchar ch );
	SEGAN_INLINE void format( const wchar* format, ... );
	SEGAN_INLINE const wchar* text( void ) const;
	SEGAN_INLINE void append( const String& str );
	SEGAN_INLINE void append( const wchar* str );
	SEGAN_INLINE void append( const char* str );
	SEGAN_INLINE void append( const wchar c );
	SEGAN_INLINE void insert( const wchar* str, sint _where = 0 );
	SEGAN_INLINE void insert( const char* str, sint _where = 0 );
	SEGAN_INLINE void insert( const wchar ch, sint _where = 0 );
	SEGAN_INLINE void remove( sint index, sint count = 1 );
	SEGAN_INLINE sint length( void ) const;
	SEGAN_INLINE void copy_to( String& dest, sint index, sint count );
	SEGAN_INLINE sint find( const wchar* substr, sint from = 0 ) const;
	SEGAN_INLINE sint find_back( const wchar* substr, sint from = 0 ) const;
	SEGAN_INLINE void replace(const wchar* what, const wchar* with);
	SEGAN_INLINE void revers( sint from, sint to );
	SEGAN_INLINE void trim( void );
	SEGAN_INLINE void make_upper( void );
	SEGAN_INLINE void make_lower( void );
	SEGAN_INLINE float to_float( void ) const;
	SEGAN_INLINE sint to_int( void ) const;
	SEGAN_INLINE String& extract_file_path( void );
	SEGAN_INLINE String& extract_file_name( void );
	SEGAN_INLINE String& extract_file_extension( void );
	SEGAN_INLINE String& exclude_file_extension( void );
	SEGAN_INLINE bool is_path_style( void );
	SEGAN_INLINE bool is_full_path( void );
	SEGAN_INLINE void make_path_style( void );
	SEGAN_INLINE operator const wchar* ( void ) const;
	SEGAN_INLINE const wchar* operator* ( void ) const;
	SEGAN_INLINE wchar& operator[] ( const sint index );
	SEGAN_INLINE String& operator= ( const String& Str );
	SEGAN_INLINE String& operator= ( const wchar* str );
	SEGAN_INLINE String& operator= ( const char* str );
	SEGAN_INLINE String& operator= ( const wchar ch );
	SEGAN_INLINE String& operator<< ( const String& str );
	SEGAN_INLINE String& operator<< ( const wchar* str );
	SEGAN_INLINE String& operator<< ( const char* str );
	SEGAN_INLINE String& operator<< ( const char ch );
 	SEGAN_INLINE String& operator<< ( const wchar ch );
	SEGAN_INLINE String& operator<< ( const int number );
	SEGAN_INLINE bool operator== ( const String& Str ) const;
	SEGAN_INLINE bool operator== ( const wchar* str ) const;
	SEGAN_INLINE bool operator== ( const char* str ) const;
	SEGAN_INLINE bool operator!= ( const String& Str ) const;
	SEGAN_INLINE bool operator!= ( const wchar* str ) const;
	SEGAN_INLINE bool operator!= (const char* str) const;

private:
	SEGAN_INLINE void _Realloc( sint newSize );

	wchar*		m_text;			//	main text
	sint		m_len;			//  length of string
	sint		m_size;			//	size of the whole array
	sint		m_sampler;		//  use to sample memory to reduce allocation
	wchar		m_tmp;			//	used as temporary
};

#endif	//	GUARD_String_HEADER_FILE
