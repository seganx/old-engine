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

bool zlib_compress_stream( Stream& srcStream, Stream& destStream, uint srcSize = 0, uint complevel = 6 );

bool zlib_decompress_stream( Stream& srcStream,  Stream& destStream);

#endif	//	GUARD_Zip_HEADER_FILE
