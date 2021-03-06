/********************************************************************
	created:	2010/08/28
	filename: 	sxZip.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some simple function to compression
				data in streams.
				These functions use zLib compress / decompress functions
				from 'zlib' general purpose compression library. 
				
				Copyright (C) 1995-2010 Jean-loup Gailly and Mark Adler.

				For conditions of distribution and use, 
				see copyright notice in zlib.h
*********************************************************************/
#ifndef GUARD_sxZip_HEADER_FILE
#define GUARD_sxZip_HEADER_FILE

#include "sxSystem_def.h"

namespace sx { namespace sys
{
	bool ZCompressStream(Stream& srcStream, Stream& destStream, UINT srcSize = 0, UINT complevel = 6);
	bool ZDecompressStream(Stream& srcStream, Stream& destStream);
} }	// namespace sx { namespace sys


/*!	compress src buffer to the destination buffer and return size of compressed buffer.
return zero if compression process failed */
uint zlib_compress( void* dest, const uint destSize, const void* src, const uint srcSize, uint complevel = 6 );

/*! decompress src buffer to the destination buffer and return the size of decompressed buffer.
return zero if decompression process failed */
uint zlib_decompress( void* dest, const uint destSize, const void* src, const uint srcSize );

#endif // GUARD_sxZip_HEADER_FILE
