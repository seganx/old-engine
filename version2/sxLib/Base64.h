/********************************************************************
	created:	2016/5/19
	filename: 	Base64.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		Base64 encoder/decoder. Originally Apache file ap_base64.c
*********************************************************************/
#ifndef DEFINED_Base64
#define DEFINED_Base64

#include "Def.h"

//! return new length of encoded data
SEGAN_LIB_API uint sx_base64_encode_len( const uint len );

//! encode source and return number of bytes encoded
SEGAN_LIB_API uint sx_base64_encode( char* dest, const uint dest_size_in_byte, const void* src, const int src_size_in_byte );

//! return new length of decoded data
SEGAN_LIB_API uint sx_base64_decode_len( const char* src );

//! decode source and return number of bytes decoded
SEGAN_LIB_API uint sx_base64_decode( void* dest, const uint dest_size_in_byte, const char* src );

#endif // DEFINED_Base64

