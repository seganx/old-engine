#include "sxZip.h"

#include "../zLib/zlib.h"

#ifndef ZLIB_LOADED
#define ZLIB_LOADED

#ifdef _DEBUG
#pragma comment(lib, "zLib/zlibd.lib")
#else
#pragma comment(lib, "zLib/zlib.lib")
#endif

#endif

#define	SX_Z_BUFF_MAXSIZE		262144	// 256KB


namespace sx { namespace sys
{

bool ZCompressStream( Stream& srcStream, Stream& destStream, UINT srcSize, UINT complevel )
{
	//  check size bounds
	if (!srcSize)
		srcSize = srcStream.Size();
		//srcStream.SetPos(0);

	if (srcSize > srcStream.Size() - srcStream.GetPos()) 
		srcSize = srcStream.Size() - srcStream.GetPos();

	if (!srcSize) 
		return false;

	//  write the buffer size to the stream to allocate sufficient 
	//  memory in the next time when we try to decompress the stream
	destStream.Write(&srcSize, sizeof(srcSize));

	UINT BufSize = srcSize;
	if (BufSize > SX_Z_BUFF_MAXSIZE)
		BufSize = SX_Z_BUFF_MAXSIZE;

	Bytef* Buffer	= (Bytef*)sx_mem_alloc(BufSize);
	Bytef* cBuffer	= (Bytef*)sx_mem_alloc(BufSize);
	if (Buffer && cBuffer)
	{
		UINT N;
		while (srcSize > 0) 
		{
			(srcSize>BufSize) ? N=BufSize : N=srcSize;
			uLongf cN = srcStream.Read(Buffer, N);

			//  compress data
			compress2(cBuffer, &cN, Buffer, cN, complevel);
			
			//  write the size of compressed data in each step to
			//  determine size of it in the decompression process
			destStream.Write(&cN, sizeof(cN));

			//  write the compressed data
			destStream.Write(cBuffer, cN);
			srcSize -= N;
		}

		sx_mem_free(Buffer);
		sx_mem_free(cBuffer);
	}
	else return false;
	
	return true;
}

bool ZDecompressStream( Stream& srcStream, Stream& destStream )
{
	//  read the original buffer size
	UINT destSize;
	srcStream.Read(&destSize, sizeof(destSize));

	UINT BufSize = destSize;
	if (BufSize > SX_Z_BUFF_MAXSIZE)
		BufSize = SX_Z_BUFF_MAXSIZE;

	Bytef* cBuffer	= (Bytef*)sx_mem_alloc(BufSize);
	Bytef* uBuffer	= (Bytef*)sx_mem_alloc(BufSize);
	if (cBuffer && uBuffer)
	{
		uLongf N; uLongf cN = 1;
		while (destSize>0 && cN)
		{			
			(destSize>BufSize) ? N=BufSize : N=destSize;

			//  read the size of compressed chunk to read from stream
			srcStream.Read(&cN, sizeof(cN));

			//  read the compressed chunk
			cN = srcStream.Read(cBuffer, cN);

			uncompress(uBuffer, &N, cBuffer, cN);

			//  write uncompressed data
			destStream.Write(uBuffer, N);

			destSize -= N;
			if (destSize<0)
				break;	//  maybe the file damaged
		}

		sx_mem_free(cBuffer);
		sx_mem_free(uBuffer);

		if (destSize != 0)
			return false;
	}
	else return false;

	return true;
}

} } // namespace sx { namespace sys