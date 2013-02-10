/********************************************************************
	created:	2010/09/01
	filename: 	sxResource3D.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class for Direct3D resources that 
				I called it ResourceD3D. this classes have static 
				functions to create and release resources for 3d space.
*********************************************************************/
#ifndef GUARD_sxResource3D_HEADER_FILE
#define GUARD_sxResource3D_HEADER_FILE

#include "sxTypes3D.h"

namespace sx { namespace d3d
{
	/*
	the resource d3d class is just concern about Direct3D resources
	*/
	class SEGAN_API Resource3D
	{
	public:
		static void Clear(void);
		
		static bool GetIndexBuffer(UINT FaceCount, OUT PDirect3DIndexBuffer& outIB);
		static bool GetVertexBuffer(UINT VertexCount, UINT VertexSize, OUT PDirect3DVertexBuffer& outVB);
		static bool GetTexture2D(UINT width, OUT PDirect3DTexture& outTX);
		static bool GetTextureCube(UINT width, OUT PDirect3DTextureCube& outTX);

		static bool CreateIndexBuffer(UINT size, OUT PDirect3DIndexBuffer& outIB, DWORD flag = 0);
		static bool CreateVertexBuffer(UINT size, OUT PDirect3DVertexBuffer& outVB, DWORD flag = 0);
		static bool CreateTexture2D(UINT width, UINT height, UINT levels, DWORD usage, D3DFormat format, D3DPool pool, OUT PDirect3DTexture& outTX, DWORD flag = 0);
		static bool CreateTextureCube(UINT width, UINT levels, DWORD usage, D3DFormat format, D3DPool pool, OUT PDirect3DTextureCube& outTX, DWORD flag = 0);
		static bool CreateDepthStencil(UINT width, UINT height, OUT PDirect3DSurface& outSurf);
		static bool CreateEffect(const char* srcCode, OUT PD3DXEffect& outEffect, const WCHAR* UserLog = NULL);

		static void ReleaseIndexBuffer(PDirect3DIndexBuffer& IB);
		static void ReleaseVertexBuffer(PDirect3DVertexBuffer& VB);
		static void ReleaseTexture(PDirect3DBaseTexture& TX);
		static void ReleaseTexture(PDirect3DTexture& TX);
		static void ReleaseTexture(PDirect3DTextureCube& TX);
		static void ReleaseDepthStencil(PDirect3DSurface& SR);
		static void ReleaseEffect(PD3DXEffect& EF);

		static void OnDeviceLost(void);
		static void OnDeviceReset(void);
	};

} } // namespace sx { namespace d3d

#endif	//	GUARD_sxResource3D_HEADER_FILE
