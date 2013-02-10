#include "sxResource3D.h"
#include "sxDevice3D.h"
#include "../sxSystem/sxSystem.h"

//#define LOG_RESRC_VERTICES				//  use this flag to log vertices story
//#define LOG_RESRC_INDICES					//  use this flag to log vertices story
//#define LOG_RESRC_TEXTURE					//  use this flag to log vertices story

#define SX_RESRC_VB_SAMPLER					1000	//  vertex per sample
#define SX_RESRC_IB_SAMPLER					1000	//  index per sample
#define SX_RESRC_SAMPLE_MAXCOUNT			50		//  capacity of resource cache for each sample

#define SX_RESRC_IB_USAGE					0		//  no need D3DUSAGE_DYNAMIC yet
#define SX_RESRC_IB_FORMAT					D3DFMT_INDEX32
#define SX_RESRC_IB_POOL					D3DPOOL_MANAGED

#define SX_RESRC_VB_USAGE					0		//  no need D3DUSAGE_DYNAMIC yet
#define SX_RESRC_VB_FVF						0
#define SX_RESRC_VB_POOL					D3DPOOL_MANAGED

#define SX_RESRC_TX_USAGE					0
#define SX_RESRC_TX_FORMAT					D3DFMT_DXT5
#define SX_RESRC_TX_POOL					D3DPOOL_MANAGED

#define CleanupXBuffer(ArrayXB, Map_XB)	{											\
	ArrayXB* aBuf = NULL;															\
	/* find the maximum size in the map */											\
	for (Map<UINT, ArrayXB*>::Iterator it = Map_XB.First(); !it.IsLast(); it++)	\
	{																				\
		size = &it;																	\
		aBuf = *it;																	\
	}																				\
	/* try to cleanup the vertex buffer */											\
	if (aBuf && aBuf->Count())														\
	{																				\
		SEGAN_RELEASE_AND_NULL(aBuf->At(aBuf->Count()-1));							\
		aBuf->RemoveByIndex( aBuf->Count()-1 );										\
		if (aBuf->IsEmpty())														\
		{																			\
			sx_delete_and_null(aBuf);														\
			Map_XB.Remove(size);													\
		}																			\
	}																				\
}

#define ClearXBuffer(ArrayXB, Map_XB)	{											\
	for (Map<UINT, ArrayXB*>::Iterator it = Map_XB.First(); !it.IsLast(); it++)	\
	{																				\
		ArrayXB* aXB = *it;															\
		for (int i=0; i<aXB->Count(); i++)											\
		{																			\
			cacheCount++;															\
			SEGAN_RELEASE_AND_NULL(aXB->At(i));										\
		}																			\
		aXB->Clear();																\
		sx_delete_and_null(aXB);															\
	}																				\
	Map_XB.Clear();																	\
}


//! description of the resource
typedef struct ResourceDesc
{
	DWORD				flag;
	PDirect3DResource*	Resource;
	D3DResourceType		Type;
	int					Width;
	int					Height;
	UINT				Size;
	int					Levels;
	DWORD				Usage;
	D3DPool				Pool;
	D3DFormat			Format;

	ResourceDesc(): flag(0), Resource(0), Type(D3DRTYPE_TEXTURE), Width(0), Height(0), Size(0), 
		Levels(0), Usage(0), Pool(D3DPOOL_DEFAULT), Format(D3DFMT_A8R8G8B8) {}

	bool operator == (ResourceDesc& r)
	{
		return
		flag		==		r.flag		&&
		Resource	==		Resource	&&
		Type		==		Type		&&
		Width		==		Width		&&
		Height		==		Height		&&
		Size		==		Size		&&
		Levels		==		Levels		&&
		Usage		==		Usage		&&
		Pool		==		Pool		&&
		Format		==		Format		;

	}
}
*PResourceDesc;

typedef	Array<PDirect3DIndexBuffer>		ArrayIB;	//	index buffer array
typedef Array<PDirect3DVertexBuffer>	ArrayVB;	//  vertex buffer array
typedef Array<PDirect3DBaseTexture>		ArrayTX;	//  base texture array
typedef Array<PD3DXEffect>				ArrayEF;	//  base texture array
typedef Array<ResourceDesc>				ArrayRC;	//  resources in default pool need to recreated on device lost



//////////////////////////////////////////////////////////////////////////
//	SOME INTERNAL CLASSES AND FUNCTIONS
//////////////////////////////////////////////////////////////////////////
const WCHAR* GetFormatName(D3DFormat format)
{
	switch (format)
	{
	case D3DFMT_A8R8G8B8:	return L"A8R8G8B8";
	case D3DFMT_A8B8G8R8:	return L"A8B8G8R8";
	case D3DFMT_X8R8G8B8:	return L"X8R8G8B8";
	case D3DFMT_R8G8B8:		return L"R8G8B8";
	case D3DFMT_DXT5:		return L"DXT5";
	case D3DFMT_DXT4:		return L"DXT4";
	case D3DFMT_DXT3:		return L"DXT3";
	case D3DFMT_DXT2:		return L"DXT2";
	case D3DFMT_DXT1:		return L"DXT1";
	default:				return L"UNKNOWN";
	}
}

/*
this class can create and hold some static functions and help me to easy access to them
*/
class Resource3D_internal
{
public:
	
	// find and extract an index buffer from the map
	static bool ExtractIndexBuffer(UINT size, OUT PDirect3DIndexBuffer& outIB)
	{
		ArrayIB* aIB = NULL;

		if (Map_IB.Find(size, aIB))
		{
			if (!aIB->Count()) return false;

			outIB = aIB->At( aIB->Count()-1 );
			aIB->RemoveByIndex( aIB->Count()-1 );

#ifdef LOG_RESRC_INDICES
			sxLog::Log( L"indices resource extracted from cache : size[ %d ]", (int)size );			
#endif

			//if (aIB->IsEmpty())
			//{
			//	Map_IB.Remove(size);
			//	sx_delete_and_null(aIB);
			//}

			return true;
		}
		else return false;
	}

	// find and extract an vertex buffer from the map
	static bool ExtractVertexBuffer(UINT size, OUT PDirect3DVertexBuffer& outVB)
	{
		ArrayVB* aVB = NULL;

		if (Map_VB.Find(size, aVB))
		{
			if (!aVB->Count()) return false;

			outVB = aVB->At( aVB->Count()-1 );
			aVB->RemoveByIndex( aVB->Count()-1 );

#ifdef LOG_RESRC_VERTICES
			sxLog::Log( L"vertices resource extracted from cache : size[ %d ]", (int)size );
#endif
			//if (aVB->IsEmpty())
			//{
			//	Map_VB.Remove(size);
			//	sx_delete_and_null(aVB);
			//}

			return true;
		}
		else return false;
	}

	// find and extract an texture from the map
	static bool ExtractTexture(UINT size, OUT PDirect3DBaseTexture& outTX, D3DResourceType txType)
	{
		outTX = NULL;
		ArrayTX* aTX = NULL;

		if (Map_TX.Find(size, aTX))
		{
			for (int i=aTX->Count()-1; i>=0; i--)
			{
				if (aTX->At(i)->GetType() == txType)
				{
					outTX = aTX->At( i );
					aTX->RemoveByIndex( i );

#ifdef LOG_RESRC_TEXTURE
					sxLog::Log( L"texture resource extracted from cache : size[ %d ]", (int)size );
#endif
					break;
				}
			}

			//if (aTX->IsEmpty())
			//{
			//	Map_TX.Remove(size);
			//	sx_delete_and_null(aTX);
			//}

			if (outTX)
				return true;
			else
				return false;
		}
		else return false;
	}

	// try to clean up one unused vertex buffer and return freed size
	static UINT CleanupVertexBuffer(void)
	{
		UINT size = 0;
		CleanupXBuffer(ArrayVB, Map_VB);
		return size;
	}

	// try to clean up one unused index buffer and return freed size
	static UINT CleanupIndexBuffer(void)
	{
		UINT size = 0;
		CleanupXBuffer(ArrayIB, Map_IB);
		return size;
	}

	// try to clean up one unused texture buffer and return freed size
	static UINT CleanupTextureBuffer(void)
	{
		UINT size = 0;
		CleanupXBuffer(ArrayTX, Map_TX);
		return size;
	}

	// try to clean up some unused buffers to increase empty space in memory
	static bool Cleanup(UINT needSize)
	{
		UINT sumSize = 0;

	MyLoop:
		//  try to cleanup vertex buffers
		sumSize += CleanupVertexBuffer();

		//  try to cleanup index buffer
		if (sumSize<needSize)
			sumSize += CleanupIndexBuffer();

		//  try to cleanup texture buffer
		if (sumSize<needSize)
			sumSize += CleanupTextureBuffer();

		//  finally check to see how much memory freed
		if (sumSize<needSize)
		{
			if ( !Map_VB.IsEmpty() || !Map_IB.IsEmpty() || !Map_TX.IsEmpty() )
				goto MyLoop;
			else
				return false;
		}
		else return true;
	}

	static void Clear(void)
	{

		int cacheCount = 0;
		ClearXBuffer(ArrayVB, Map_VB);
		ClearXBuffer(ArrayIB, Map_IB);
		ClearXBuffer(ArrayTX, Map_TX);

		String str = L"Clearing API resource cache .\r\n";
		sxLog::Log(str);

		for (int i=0; i<Arr_EF.Count(); i++)
		{
			SEGAN_RELEASE_AND_NULL( Arr_EF[i] );
		}
		Arr_EF.Clear();
		SEGAN_RELEASE_AND_NULL(s_EffectPool);
	}

	static Map<UINT, ArrayIB*>			Map_IB;
	static Map<UINT, ArrayVB*>			Map_VB;
	static Map<UINT, ArrayTX*>			Map_TX;
	static ArrayEF						Arr_EF;
	static ArrayRC						Arr_RC;
	static PD3DXEffectPool				s_EffectPool;
};
Map<UINT, ArrayIB*>				Resource3D_internal::Map_IB;
Map<UINT, ArrayVB*>				Resource3D_internal::Map_VB;
Map<UINT, ArrayTX*>				Resource3D_internal::Map_TX;
ArrayEF							Resource3D_internal::Arr_EF;
ArrayRC							Resource3D_internal::Arr_RC;
PD3DXEffectPool					Resource3D_internal::s_EffectPool = NULL;


static bool	_ondeviceLost = false;

UINT GetNearestPower2(const UINT a)
{
	UINT Result = 1;
	UINT r = a;
	while(r-1)
	{
		r /= 2;
		Result *= 2;
	}
	return Result<a ? Result*2 : Result;
}

namespace sx { namespace d3d
{

	//////////////////////////////////////////////////////////////////////////
	//	DIRECT3D RESOURCE MANAGER
	//////////////////////////////////////////////////////////////////////////
	void Resource3D::Clear( void )
	{
		Resource3D_internal::Clear();
	}

	bool Resource3D::GetIndexBuffer( UINT FaceCount, OUT PDirect3DIndexBuffer& IB )
	{
		if (FaceCount<1) return false;
		
		UINT size = int( ((FaceCount-1) / SX_RESRC_IB_SAMPLER) + 1 ) * SX_RESRC_IB_SAMPLER * 12;

		//  warranty the vertex buffer size
		size += 120;

		if (Resource3D_internal::ExtractIndexBuffer(size, IB))
			return true;
		else
			return CreateIndexBuffer(size, IB);
	}

	bool Resource3D::GetVertexBuffer( UINT VertexCount, UINT VertexSize, OUT PDirect3DVertexBuffer& outVB )
	{
		if (VertexCount<1) return false;

		UINT size = int( ((VertexCount-1) / SX_RESRC_VB_SAMPLER) + 1 ) * SX_RESRC_VB_SAMPLER * VertexSize;
		if (size<12) return false;

		//  warranty the vertex buffer size
		size += VertexSize * 10;

		if (Resource3D_internal::ExtractVertexBuffer(size, outVB))
			return true;
		else
			return CreateVertexBuffer(size, outVB);
	}

	bool Resource3D::GetTexture2D( UINT width, OUT PDirect3DTexture& outTX )
	{
		if (width<1) return false;

		UINT size = GetNearestPower2(width);

		if (Resource3D_internal::ExtractTexture(size, (PDirect3DBaseTexture&)outTX, D3DRTYPE_TEXTURE))
			return true;
		else
			return CreateTexture2D(size, size, 0, SX_RESRC_TX_USAGE, SX_RESRC_TX_FORMAT, SX_RESRC_TX_POOL, outTX);
	}

	bool Resource3D::GetTextureCube( UINT width, OUT PDirect3DTextureCube& outTX )
	{
		if (width<1) return false;

		UINT size = GetNearestPower2(width);

		if (Resource3D_internal::ExtractTexture(size, (PDirect3DBaseTexture&)outTX, D3DRTYPE_CUBETEXTURE))
			return true;
		else
			return CreateTextureCube(size, 0, SX_RESRC_TX_USAGE, SX_RESRC_TX_FORMAT, SX_RESRC_TX_POOL, outTX);
	}

	bool Resource3D::CreateIndexBuffer( UINT size, OUT PDirect3DIndexBuffer& outIB, DWORD flag /*= 0*/ )
	{
		if (!Device3D::IsCreated() || size<12 || !SUCCEEDED(Device3D::GetDevice()->TestCooperativeLevel()) ) return false;

		int i = 0;
		bool Result = false;
		while (!Result)
		{
			HRESULT res = Device3D::GetDevice()->CreateIndexBuffer(
				size,
				(flag & SX_RESOURCE_DYNAMIC) ? D3DUSAGE_DYNAMIC : SX_RESRC_VB_USAGE,
				SX_RESRC_IB_FORMAT,
				(flag & SX_RESOURCE_DYNAMIC) ? D3DPOOL_DEFAULT : SX_RESRC_VB_POOL,
				&outIB,
				NULL);
			
			Result = SUCCEEDED( res );
			if (Result)
			{
				if ( (flag & SX_RESOURCE_CHECKLOST) )
				{
					SEGAN_SET_REM(flag, SX_RESOURCE_CHECKLOST);

					ResourceDesc rc;
					rc.flag		= flag;
					rc.Resource = (PDirect3DResource*)&outIB;
					rc.Type		= D3DRTYPE_INDEXBUFFER;
					rc.Size		= size;
					rc.Usage	= (flag & SX_RESOURCE_DYNAMIC) ? D3DUSAGE_DYNAMIC : SX_RESRC_VB_USAGE;
					rc.Format	= D3DFMT_UNKNOWN;
					rc.Pool		= (flag & SX_RESOURCE_DYNAMIC) ? D3DPOOL_DEFAULT : SX_RESRC_VB_POOL;
					Resource3D_internal::Arr_RC.PushBack( rc );
				}
				return true; 
			}
			else
			{
				switch (res)
				{
				case D3DERR_INVALIDCALL: 
					sxLog::Log(L"Creating index buffer returns 'D3DERR_INVALIDCALL' but I will try again.");
					break;

				case E_OUTOFMEMORY: 
					sxLog::Log(L"Direct3D could not allocate sufficient memory to create index buffer. Don't worry, I will try again.");
					break;

				case D3DERR_OUTOFVIDEOMEMORY:
					{
						// try to cleanup memory
						if (!Resource3D_internal::Cleanup(size*2))
						{
							sxLog::Log(L"Direct3D does not have enough display memory to create index buffer. So I'd give up and the object will not display.");
							return false;
						}
						break;
					}

				default: 
					{
						sxLog::Log(L"Creating index buffer returns '%d' but I will try again.", (int)res);
					}
				}

				Sleep(5);
				i++;
			}

			if (i>10)
			{
				sxLog::Log(L"Trying to create index buffer failed for 10 times ! so I'd give up and the object will not display.");
				return false;
			}
		}
		return Result;
	}

	bool Resource3D::CreateVertexBuffer( UINT size, OUT PDirect3DVertexBuffer& outVB, DWORD flag /*= 0*/ )
	{
		if (!Device3D::IsCreated() || size<12 || !SUCCEEDED(Device3D::GetDevice()->TestCooperativeLevel())) return false;

		int i = 0;
		bool Result = false;
		while (!Result)
		{
			HRESULT res = Device3D::GetDevice()->CreateVertexBuffer(
				size,
				(flag & SX_RESOURCE_DYNAMIC) ? D3DUSAGE_DYNAMIC : SX_RESRC_VB_USAGE,
				SX_RESRC_VB_FVF,
				(flag & SX_RESOURCE_DYNAMIC) ? D3DPOOL_DEFAULT : SX_RESRC_VB_POOL,
				&outVB,
				NULL);

			Result = SUCCEEDED( res );
			if (Result)
			{
				if ( (flag & SX_RESOURCE_CHECKLOST) )
				{
					SEGAN_SET_REM(flag, SX_RESOURCE_CHECKLOST);

					ResourceDesc rc;
					rc.flag		= flag;
					rc.Resource = (PDirect3DResource*)&outVB;
					rc.Type		= D3DRTYPE_VERTEXBUFFER;
					rc.Size		= size;
					rc.Usage	= (flag & SX_RESOURCE_DYNAMIC) ? D3DUSAGE_DYNAMIC : SX_RESRC_VB_USAGE;
					rc.Format	= D3DFMT_UNKNOWN;
					rc.Pool		= (flag & SX_RESOURCE_DYNAMIC) ? D3DPOOL_DEFAULT : SX_RESRC_VB_POOL;
					Resource3D_internal::Arr_RC.PushBack( rc );
				}
				return true;
			}
			else
			{
				switch (res)
				{
				case D3DERR_INVALIDCALL: 
					sxLog::Log(L"Creating vertex buffer returns 'D3DERR_INVALIDCALL' but I will try again.");
					break;

				case E_OUTOFMEMORY: 
					sxLog::Log(L"Direct3D could not allocate sufficient memory to create vertex buffer. Don't worry, I will try again.");
					break;

				case D3DERR_OUTOFVIDEOMEMORY:
					{
						// try to cleanup memory
						if (!Resource3D_internal::Cleanup(size*2))
						{
							sxLog::Log(L"Direct3D does not have enough display memory to create vertex buffer. So I'd give up and the object will not display.");
							return false;
						}
						break;
					}
				
				default: 
					{
						sxLog::Log(L"Creating vertex buffer returns '%d' but I will try again.", (int)res);
					}
				}

				Sleep(5);
				i++;
			}

			if (i>10)
			{
				sxLog::Log(L"Trying to create vertex buffer failed for 10 times ! so I'd give up and the object will not display.");
				return false;
			}
		}
		return Result;
	}

	bool Resource3D::CreateTexture2D( UINT width, UINT height, UINT levels, DWORD usage, D3DFormat format, D3DPool pool, OUT PDirect3DTexture& outTX, DWORD flag /*= 0*/ )
	{
		if (!Device3D::IsCreated() || width*height<1 || !SUCCEEDED(Device3D::GetDevice()->TestCooperativeLevel())) return false;

		int i = 0;
		bool Result = false;
		while (!Result)
		{
			HRESULT res = Device3D::GetDevice()->CreateTexture(
				width,
				height,
				levels,
				usage,
				format,
				pool,
				&outTX,
				NULL);

			Result = SUCCEEDED( res );
			if (Result)
			{
				if ( pool == D3DPOOL_DEFAULT && (flag & SX_RESOURCE_CHECKLOST) )
				{
					SEGAN_SET_REM(flag, SX_RESOURCE_CHECKLOST);

					ResourceDesc rc;
					rc.flag		= flag ;
					rc.Resource = (PDirect3DResource*)&outTX;
					rc.Type		= D3DRTYPE_TEXTURE;
					rc.Width	= width;
					rc.Height	= height;
					rc.Levels	= levels;
					rc.Usage	= usage;
					rc.Format	= format;
					rc.Pool		= pool;
					Resource3D_internal::Arr_RC.PushBack( rc );
				}
				return true;
			}
			else
			{
				switch (res)
				{
				case D3DERR_INVALIDCALL: 
					sxLog::Log(L"Creating texture returns 'D3DERR_INVALIDCALL' but I will try again.");
					break;

				case E_OUTOFMEMORY: 
					sxLog::Log(L"Direct3D could not allocate sufficient memory to create texture. Don't worry, I will try again.");
					break;

				case D3DERR_OUTOFVIDEOMEMORY:
					{
						// try to cleanup memory
						if ( !Resource3D_internal::Cleanup(GetNearestPower2( sx_max_i(width, height) )) )
						{
							sxLog::Log(L"Direct3D does not have enough display memory to create texture. So I'd give up and the texture will not display.");
							return false;
						}
						break;
					}

				default: 
					{
						sxLog::Log(L"Creating texture returns '%d' but I will try again.", (int)res);
					}
				}

				Sleep(5);
				i++;
			}

			if (i>10)
			{
				sxLog::Log(L"Trying to create texture failed for 10 times ! so I'd give up and the texture will not display.");
				return false;
			}
		}
		return Result;
	}

	bool Resource3D::CreateTextureCube( UINT width, UINT levels, DWORD usage, D3DFormat format, D3DPool pool, OUT PDirect3DTextureCube& outTX, DWORD flag /*= 0*/ )
	{
		if (!Device3D::IsCreated() || width<1 || !SUCCEEDED(Device3D::GetDevice()->TestCooperativeLevel())) return false;

		int i = 0;
		bool Result = false;
		while (!Result)
		{
			HRESULT res = Device3D::GetDevice()->CreateCubeTexture(
				width,
				levels,
				usage,
				format,
				pool,
				&outTX,
				NULL);

			Result = SUCCEEDED( res );
			if (Result)
			{
				if ( pool == D3DPOOL_DEFAULT && (flag & SX_RESOURCE_CHECKLOST) )
				{
					SEGAN_SET_REM(flag, SX_RESOURCE_CHECKLOST);

					ResourceDesc rc;
					rc.flag		= flag;
					rc.Resource = (PDirect3DResource*)&outTX;
					rc.Type		= D3DRTYPE_TEXTURE;
					rc.Width	= width;
					rc.Height	= width;
					rc.Levels	= levels;
					rc.Usage	= usage;
					rc.Format	= format;
					rc.Pool		= pool;
					Resource3D_internal::Arr_RC.PushBack( rc );
				}
				return true;
			}
			else
			{
				switch (res)
				{
				case D3DERR_INVALIDCALL: 
					sxLog::Log(L"Creating cube texture returns 'D3DERR_INVALIDCALL' but I will try again.");
					break;

				case E_OUTOFMEMORY: 
					sxLog::Log(L"Direct3D could not allocate sufficient memory to create cube texture. Don't worry, I will try again.");
					break;

				case D3DERR_OUTOFVIDEOMEMORY:
					{
						// try to cleanup memory
						if ( !Resource3D_internal::Cleanup(GetNearestPower2(width)) )
						{
							sxLog::Log(L"Direct3D does not have enough display memory to create cube texture. So I'd give up and the cube texture will not display.");
							return false;
						}
						break;
					}

				default: 
					{
						sxLog::Log(L"Creating cube texture returns '%d' but I will try again.", (int)res);
					}
				}

				Sleep(5);
				i++;
			}

			if (i>10)
			{
				sxLog::Log(L"Trying to create cube texture failed for 10 times ! so I'd give up and the texture will not display.");
				return false;
			}
		}
		return Result;
	}

	bool Resource3D::CreateDepthStencil( UINT width, UINT height, OUT PDirect3DSurface& outSurf )
	{
		if (!Device3D::IsCreated() || width<1 || !SUCCEEDED(Device3D::GetDevice()->TestCooperativeLevel())) return false;

		int i = 0;
		bool Result = false;
		while (!Result)
		{
			HRESULT res = Device3D::CreateDepthStencil(	width, height, outSurf );

			Result = SUCCEEDED( res );
			if (Result)
			{
				ResourceDesc rc;
				rc.flag		= 0;
				rc.Resource = (PDirect3DResource*)&outSurf;
				rc.Type		= D3DRTYPE_SURFACE;
				rc.Width	= width;
				rc.Height	= width;
				rc.Levels	= 0;
				rc.Usage	= 0;
				rc.Format	= D3DFMT_UNKNOWN;
				rc.Pool		= D3DPOOL_DEFAULT;
				Resource3D_internal::Arr_RC.PushBack( rc );
				
				return true;
			}
			else
			{
				switch (res)
				{
				case D3DERR_INVALIDCALL: 
					sxLog::Log(L"Creating cube texture returns 'D3DERR_INVALIDCALL' but I will try again.");
					break;

				case E_OUTOFMEMORY: 
					sxLog::Log(L"Direct3D could not allocate sufficient memory to create cube texture. Don't worry, I will try again.");
					break;

				case D3DERR_OUTOFVIDEOMEMORY:
					{
						// try to cleanup memory
						if ( !Resource3D_internal::Cleanup(GetNearestPower2(width)) )
						{
							sxLog::Log(L"Direct3D does not have enough display memory to create cube texture. So I'd give up and the cube texture will not display.");
							return false;
						}
						break;
					}

				default: 
					{
						sxLog::Log(L"Creating cube texture returns '%d' but I will try again.", (int)res);
					}
				}

				Sleep(5);
				i++;
			}

			if (i>10)
			{
				sxLog::Log(L"Trying to create cube texture failed for 10 times ! so I'd give up and the texture will not display.");
				return false;
			}
		}
		return Result;
	}

	bool Resource3D::CreateEffect( const char* srcCode, OUT PD3DXEffect& outEffect, const WCHAR* UserLog /*= NULL*/ )
	{
		sx_callstack_push(Resource3D::CreateEffect(code=%s), srcCode);

		if (!Resource3D_internal::s_EffectPool)
			D3DXCreateEffectPool(&Resource3D_internal::s_EffectPool);

		if (!srcCode) return false;
		int srcCodeLen = (int)strlen(srcCode);
		if (srcCodeLen < 30) return false;

		//  create and compile the effect
		LPD3DXBUFFER outBuff = NULL;
		HRESULT res = D3DXCreateEffect(
			Device3D::GetDevice(), 
			srcCode,
			srcCodeLen,
			NULL,
			NULL,
			D3DXSHADER_PACKMATRIX_COLUMNMAJOR
#ifdef D3DXSHADER_OPTIMIZATION_LEVEL3
			| D3DXSHADER_OPTIMIZATION_LEVEL3
#endif
			| D3DXSHADER_PARTIALPRECISION,
			Resource3D_internal::s_EffectPool,
			&outEffect,
			&outBuff
			);

		if (outBuff)
		{
			//  log that what happened
			String _log = UserLog;
			_log << (char*)outBuff->GetBufferPointer();
			sxLog::Log(_log);
			outBuff->Release();
		}

		if ( SUCCEEDED(res) )
		{
			Resource3D_internal::Arr_EF.PushBack(outEffect);
			return true;
		}
		else return false;
	}

	void Resource3D::ReleaseIndexBuffer( PDirect3DIndexBuffer& IB )
	{
		if ( !Device3D::IsCreated() || !IB ) return;

		D3DIndexBufferDesc desc;
		IB->GetDesc(&desc);
		
		if (desc.Format == SX_RESRC_IB_FORMAT && desc.Pool == SX_RESRC_IB_POOL && desc.Usage == SX_RESRC_IB_USAGE)
		{
			ArrayIB* aBuf = NULL;
			if (!Resource3D_internal::Map_IB.Find(desc.Size, aBuf))
			{
				aBuf = sx_new(ArrayIB);
				Resource3D_internal::Map_IB.Insert(desc.Size, aBuf);

#ifdef LOG_RESRC_INDICES
				sxLog::Log( L"indices resource cache created : size[ %d ]", (int)desc.Size );
#endif
			}

			if (aBuf->Count() < SX_RESRC_SAMPLE_MAXCOUNT)
			{
				aBuf->PushBack(IB);
#ifdef LOG_RESRC_INDICES
				sxLog::Log( L"indices resource cached : size[ %d ]", (int)desc.Size );
#endif
			}
			else
			{
				SEGAN_RELEASE_AND_NULL(IB);
			}

			IB = NULL;
		}
		else
		{
			SEGAN_RELEASE_AND_NULL(IB);
		}
	}

	void Resource3D::ReleaseVertexBuffer( PDirect3DVertexBuffer& VB )
	{
		if ( !Device3D::IsCreated() || !VB ) return;

		D3DVertexBufferDesc desc;
		VB->GetDesc(&desc);

		if (desc.FVF == SX_RESRC_VB_FVF		&& desc.Pool == SX_RESRC_VB_POOL && 
			desc.Usage == SX_RESRC_VB_USAGE && desc.Size > SX_RESRC_VB_SAMPLER * SEGAN_SIZE_VERTEX_1)
		{
			ArrayVB* aBuf = NULL;
			if (!Resource3D_internal::Map_VB.Find(desc.Size, aBuf))
			{
				aBuf = sx_new(ArrayVB);
				Resource3D_internal::Map_VB.Insert(desc.Size, aBuf);

#ifdef LOG_RESRC_VERTICES
				sxLog::Log( L"vertices resource cache created : size[ %d ]", (int)desc.Size );
#endif
			}

			if (aBuf->Count() < SX_RESRC_SAMPLE_MAXCOUNT)
			{
				aBuf->PushBack(VB);

#ifdef LOG_RESRC_VERTICES
				sxLog::Log( L"vertices resource cached : size[ %d ]", (int)desc.Size );
#endif
			}
			else
			{
				SEGAN_RELEASE_AND_NULL(VB);
			}
			VB = NULL;
		}
		else
		{
			for (int i=0; i<Resource3D_internal::Arr_RC.Count(); i++)
			{
				if ( *(Resource3D_internal::Arr_RC[i].Resource) == VB )
					Resource3D_internal::Arr_RC.RemoveByIndex(i);
			}
			SEGAN_RELEASE_AND_NULL(VB);
		}
	}

	void Resource3D::ReleaseTexture( PDirect3DTexture& TX )
	{
		if ( !Device3D::IsCreated() || !TX ) return;

		//sxLog::Log(L"start to get description");
		D3DSurfaceDesc desc;
		TX->GetLevelDesc(0, &desc);
		//sxLog::Log(L"end getting description");

		UINT size = GetNearestPower2(desc.Width);
		if (desc.Format == SX_RESRC_TX_FORMAT	&&	desc.Height == size	== desc.Width	&& 
			desc.Pool	== SX_RESRC_TX_POOL		&&	desc.Usage	== SX_RESRC_VB_USAGE	)
		{
			ArrayTX* aBuf = NULL;
			if (!Resource3D_internal::Map_TX.Find(size, aBuf))
			{
				aBuf = sx_new(ArrayTX);
				Resource3D_internal::Map_TX.Insert(size, aBuf);

#ifdef LOG_RESRC_TEXTURE
				sxLog::Log( L"textures resource cache created : size[ %d ]", (int)size );
#endif
			}
			
			if (aBuf->Count() < SX_RESRC_SAMPLE_MAXCOUNT)
			{
				aBuf->PushBack(TX);

#ifdef LOG_RESRC_TEXTURE
				sxLog::Log( L"textures resource cached: size[ %d ]", (int)size );
#endif
			}
			else
			{
#ifdef LOG_RESRC_TEXTURE
				sxLog::Log( L"cache is full so so start release and null : ptr %d - format %s - width %d - height %d"
					, int( (UINT64)TX )
					, GetFormatName(desc.Format)
					, (int)desc.Width
					, (int)desc.Height );

				try
				{
#endif
					SEGAN_RELEASE_AND_NULL(TX);
#ifdef LOG_RESRC_TEXTURE
					sxLog::Log( L"end release and null\r\n");
				}
				catch (int i)
				{
					i=0;
					sxLog::Log( L"releasing texture failed!\r\n");
				}
#endif
			}
			TX = NULL;
		}
		else
		{
			for (int i=0; i<Resource3D_internal::Arr_RC.Count(); i++)
			{
				if ( *(Resource3D_internal::Arr_RC[i].Resource) == TX)
					Resource3D_internal::Arr_RC.RemoveByIndex(i);
			}

#ifdef LOG_RESRC_TEXTURE
			sxLog::Log( L"texture was not cached so start release and null : ptr %d - format %s - width %d - height %d"
				, int( (UINT64)TX )
				, GetFormatName(desc.Format)
				, (int)desc.Width
				, (int)desc.Height );

			try
			{
#endif
				SEGAN_RELEASE_AND_NULL(TX);
#ifdef LOG_RESRC_TEXTURE
				sxLog::Log( L"end release and null\r\n");
			}
			catch (int i)
			{
				i=0;
				sxLog::Log( L"releasing texture failed!\r\n");
			}
#endif
		}
	}

	void Resource3D::ReleaseTexture( PDirect3DTextureCube& TX )
	{
		if ( !Device3D::IsCreated() || !TX ) return;

		D3DSurfaceDesc desc;
		TX->GetLevelDesc(0, &desc);

		UINT size = GetNearestPower2(desc.Width);
		if (desc.Format == SX_RESRC_TX_FORMAT	&&	desc.Height == size	== desc.Width	&& 
			desc.Pool	== SX_RESRC_TX_POOL		&&	desc.Usage	== SX_RESRC_VB_USAGE	)
		{
			ArrayTX* aBuf = NULL;
			if (!Resource3D_internal::Map_TX.Find(size, aBuf))
			{
				aBuf = sx_new(ArrayTX);
				Resource3D_internal::Map_TX.Insert(size, aBuf);

#ifdef LOG_RESRC_TEXTURE
				sxLog::Log( L"textures resource cache created : size[ %d ]", (int)size );
#endif
			}

			if (aBuf->Count() < SX_RESRC_SAMPLE_MAXCOUNT)
			{
				aBuf->PushBack(TX);

#ifdef LOG_RESRC_TEXTURE
				sxLog::Log( L"textures resource cached: size[ %d ]", (int)size );
#endif
			}
			else
			{
				SEGAN_RELEASE_AND_NULL(TX);
			}
			TX = NULL;
		}
		else
		{
			for (int i=0; i<Resource3D_internal::Arr_RC.Count(); i++)
			{
				if ( *(Resource3D_internal::Arr_RC[i].Resource) == TX)
					Resource3D_internal::Arr_RC.RemoveByIndex(i);
			}
			SEGAN_RELEASE_AND_NULL(TX);
		}
	}

	void Resource3D::ReleaseTexture( PDirect3DBaseTexture& TX )
	{
		sx_callstack_push(Resource3D::ReleaseTexture(PDirect3DBaseTexture));

		if ( !Device3D::IsCreated() || !TX ) return;

		D3DRESOURCETYPE rType = TX->GetType();
		switch (rType)
		{
		case D3DRTYPE_TEXTURE:		ReleaseTexture((PDirect3DTexture&)TX);			break;
		case D3DRTYPE_CUBETEXTURE:	ReleaseTexture((PDirect3DTextureCube&)TX);		break;

		default:	SEGAN_RELEASE_AND_NULL(TX);
		}
	}

	void Resource3D::ReleaseDepthStencil( PDirect3DSurface& SR )
	{
		if ( !Device3D::IsCreated() || !SR ) return;

		for (int i=0; i<Resource3D_internal::Arr_RC.Count(); i++)
		{
			if ( *(Resource3D_internal::Arr_RC[i].Resource) == SR)
				Resource3D_internal::Arr_RC.RemoveByIndex(i);
		}
		SEGAN_RELEASE_AND_NULL(SR);
	}

	void Resource3D::ReleaseEffect( PD3DXEffect& EF )
	{
		sx_callstack_push(Resource3D::ReleaseEffect());

		if (!EF) return;

		Device3D::SetEffect( NULL );
		Resource3D_internal::Arr_EF.Remove(EF);
		SEGAN_RELEASE_AND_NULL(EF);
	}

	void Resource3D::OnDeviceLost( void )
	{
		sx_callstack_push(Resource3D::OnDeviceLost());
		if ( _ondeviceLost ) return;
		_ondeviceLost = true;

		for (int i=0; i<Resource3D_internal::Arr_EF.Count(); i++)
		{
			Resource3D_internal::Arr_EF[i]->OnLostDevice();
		}

		for (int i=0; i<Resource3D_internal::Arr_RC.Count(); i++)
		{
			(*Resource3D_internal::Arr_RC[i].Resource)->Release();
		}
	}

	void Resource3D::OnDeviceReset( void )
	{
		sx_callstack_push(Resource3D::OnDeviceReset());
		if ( !_ondeviceLost ) return;
		_ondeviceLost = false;

		for (int i=0; i<Resource3D_internal::Arr_EF.Count(); i++)
		{
			Resource3D_internal::Arr_EF[i]->OnResetDevice();
		}

		for (int i=0; i<Resource3D_internal::Arr_RC.Count(); i++)
		{
			ResourceDesc* rc = &Resource3D_internal::Arr_RC[i];
			switch (rc->Type)
			{
			case D3DRTYPE_INDEXBUFFER:
				CreateIndexBuffer(rc->Size, ((PDirect3DIndexBuffer&)(*rc->Resource)), rc->flag);
				break;

			case D3DRTYPE_VERTEXBUFFER:
					CreateVertexBuffer(rc->Size, ((PDirect3DVertexBuffer&)(*rc->Resource)), rc->flag);
				break;

			case D3DRTYPE_SURFACE:
					Device3D::CreateDepthStencil( rc->Width, rc->Height, ((PDirect3DSurface&)(*rc->Resource)) );
				break;

			case D3DRTYPE_TEXTURE:
					CreateTexture2D(rc->Width, rc->Height, rc->Levels, rc->Usage, rc->Format, rc->Pool, ((PDirect3DTexture&)(*rc->Resource)), rc->flag );
				break;

			case D3DRTYPE_CUBETEXTURE:
					CreateTextureCube(rc->Width, rc->Levels, rc->Usage, rc->Format, rc->Pool, ((PDirect3DTextureCube&)(*rc->Resource)), rc->flag );
				break;
			}
		}
	}

} } // namespace sx { namespace d3d