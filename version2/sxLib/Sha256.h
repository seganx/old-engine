/********************************************************************
    created:	2017/11/07
    filename: 	SHA256.h
    author:		Sajad Beigjani based on code from Brad Conte (brad AT bradconte.com)
    email:		sajad.b@gmail.com
    Site:		www.SeganX.com
    Desc:		Defines the API for the corresponding SHA1 implementation.
*********************************************************************/
#ifndef DEFINED_SHA256
#define DEFINED_SHA256

#include "Def.h"

//! SHA256 hash given strings. Buffer 'dest' must be 65 bytes long
SEGAN_LIB_API char* sx_sha256(char dest[65], const void * src, const int src_size_in_byte);

#endif   // DEFINED_SHA256