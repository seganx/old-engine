#include "Zip.h"

#include "zLib/zlib.h"



#ifndef ZLIB_LOADED
	#define ZLIB_LOADED
	#ifdef _DEBUG
		#pragma comment(lib, "../System/zLib/zlibd.lib")
	#else
		#pragma comment(lib, "../System/zLib/zlib.lib")
	#endif
#endif

#define	SX_Z_BUFF_MAXSIZE		262144	// 256KB


uint zlib_compress( void* dest, const uint destSize, const void* src, const uint srcSize, uint complevel /*= 6 */ )
{
	uLongf res = destSize;
	if ( compress2( (Bytef*)dest, &res, (Bytef*)src, srcSize, complevel ) == Z_OK )
		return res;
	else
		return 0;
}

uint zlib_decompress( void* dest, const uint destSize, const void* src, const uint srcSize )
{
	uLongf res = destSize;
	if ( uncompress( (Bytef*)dest, &res, (Bytef*)src, srcSize ) == Z_OK )
		return res;
	else
		return 0;
}

