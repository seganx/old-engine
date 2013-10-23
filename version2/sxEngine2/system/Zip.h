/********************************************************************
	created:	2012/05/01
	filename: 	Zip.h
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
#ifndef GUARD_Zip_HEADER_FILE
#define GUARD_Zip_HEADER_FILE

#include "System_def.h"

/*!	compress src buffer to the destination buffer and return size of compressed buffer.
return zero if compression process failed */
uint zlib_compress( void* dest, const uint destSize, const void* src, const uint srcSize, uint complevel = 6 );

/*! decompress src buffer to the destination buffer and return the size of decompressed buffer.
return zero if decompression process failed */
uint zlib_decompress( void* dest, const uint destSize, const void* src, const uint srcSize );

#endif	//	GUARD_Zip_HEADER_FILE
