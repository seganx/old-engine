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



bool zlib_compress_stream( Stream& srcStream, Stream& destStream, uint srcSize /*= 0*/, uint complevel /*= 6*/ )
{
	//  check size bounds
	if ( !srcSize )
		srcSize = srcStream.Size();
		//srcStream.SetPos(0);

	if ( srcSize > srcStream.Size() - srcStream.GetPos() )
		srcSize = srcStream.Size() - srcStream.GetPos();

	if ( !srcSize )
		return false;

	//  write the buffer size to the stream to allocate sufficient 
	//  memory in the next time when we try to decompress the stream
	destStream.Write( &srcSize, sizeof(srcSize) );

	uint buffSize = srcSize;
	if ( buffSize > SX_Z_BUFF_MAXSIZE )
		buffSize = SX_Z_BUFF_MAXSIZE;

	Bytef* buffData	= (Bytef*)sx_mem_alloc(buffSize);
	Bytef* compData	= (Bytef*)sx_mem_alloc(buffSize);
	if ( buffData && compData )
	{
		while ( srcSize > 0 )
		{
			uint numBytes = ( srcSize > buffSize ) ? buffSize : srcSize;
			uLongf compBytes = srcStream.Read( buffData, numBytes );

			//  compress data
			compress2( compData, &compBytes, buffData, compBytes, complevel );
			
			//  write the size of compressed data in each step to
			//  determine size of it in the decompression process
			destStream.Write( &compBytes, sizeof(compBytes) );

			//  write the compressed data
			destStream.Write( compData, compBytes );
			srcSize -= numBytes;
		}

		sx_mem_free( buffData );
		sx_mem_free( compData );
	}
	else return false;
	
	return true;
}

bool zlib_decompress_stream( Stream& srcStream,  Stream& destStream)
{
	//  read the original buffer size
	uint destSize;
	srcStream.Read( &destSize, sizeof(destSize) );

	uint buffSize = destSize;
	if ( buffSize > SX_Z_BUFF_MAXSIZE )
		buffSize = SX_Z_BUFF_MAXSIZE;

	Bytef* compData	= (Bytef*)sx_mem_alloc(buffSize);
	Bytef* buffData	= (Bytef*)sx_mem_alloc(buffSize);

	if ( compData && buffData )
	{
		uLongf compBytes = 1;
		
		while ( destSize > 0 && compBytes )
		{			
			uLongf numBytes = ( destSize > buffSize ) ? buffSize : destSize;

			//  read the size of compressed chunk to read from stream
			srcStream.Read( &compBytes, sizeof(compBytes) );

			//  read the compressed chunk
			compBytes = srcStream.Read( compData, compBytes );

			uncompress( buffData, &numBytes, compData, compBytes );

			//  write uncompressed data
			destStream.Write( buffData, numBytes );

			destSize -= numBytes;
			if ( destSize < 0 )
				break;	//  maybe the file damaged
		}

		sx_mem_free( compData );
		sx_mem_free( buffData );

		if ( destSize != 0 )
			return false;
	}
	else return false;

	return true;
}

