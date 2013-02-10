/********************************************************************
	created:	2010/08/23
	filename: 	sxTypes3D.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain any types will used by any object
				related with 3D space, render devices, 3D math and etc.
*********************************************************************/
#ifndef GUARD_sxTypes3D_HEADER_FILE
#define GUARD_sxTypes3D_HEADER_FILE

#include "sxRender_def.h"
#include "../sxCommon/sxMath.h"

#include <d3d9.h>
#pragma comment( lib, "d3d9.lib" )

#ifndef D3DX_DEFAULT
#include <d3dx9.h>
#pragma comment( lib, "d3dx9.lib" )
#endif


typedef LPDIRECT3D9							PDirect3D;
typedef LPDIRECT3DDEVICE9					PDirect3DDevice;
typedef LPDIRECT3DRESOURCE9					PDirect3DResource;
typedef LPDIRECT3DVERTEXBUFFER9				PDirect3DVertexBuffer;
typedef LPDIRECT3DINDEXBUFFER9				PDirect3DIndexBuffer;
typedef	LPDIRECT3DSURFACE9					PDirect3DSurface;
typedef LPDIRECT3DBASETEXTURE9				PDirect3DBaseTexture;
typedef LPDIRECT3DTEXTURE9					PDirect3DTexture;
typedef	LPDIRECT3DCUBETEXTURE9				PDirect3DTextureCube;
typedef LPDIRECT3DSTATEBLOCK9				PDirect3DStateBlock;
typedef LPDIRECT3DVERTEXDECLARATION9		PDirect3DVertexDeclaration;
typedef LPDIRECT3DVERTEXSHADER9				PDirect3DVertexShader;
typedef LPDIRECT3DPIXELSHADER9				PDirect3DPixelShader;
typedef LPD3DXEFFECT						PD3DXEffect;
typedef LPD3DXEFFECTPOOL					PD3DXEffectPool;

typedef D3DRESOURCETYPE						D3DResourceType;
typedef D3DVERTEXBUFFER_DESC				D3DVertexBufferDesc;
typedef D3DINDEXBUFFER_DESC					D3DIndexBufferDesc;
typedef D3DSURFACE_DESC						D3DSurfaceDesc;
typedef D3DLOCKED_RECT						D3DLockedRect, *PD3DLockedRect;

typedef D3DPRESENT_PARAMETERS				D3DPresentParameters;
typedef D3DFORMAT							D3DFormat;
typedef D3DPOOL								D3DPool;
typedef	D3DCAPS9							D3DCaps,		*PD3DCaps;
typedef D3DVIEWPORT9						D3DViewport,	*PD3DViewport;
typedef	D3DXCOLOR							D3DColor,		*PD3DColor;
typedef D3DMATERIAL9						D3DMaterial,	*PD3DMaterial;
typedef D3DFOGMODE							D3DFogMode;

typedef D3DXHANDLE							D3DShaderHandle,*PD3DShaderHandle;
typedef D3DXEFFECT_DESC						D3DShaderDesc;
typedef D3DXPARAMETER_DESC					D3DShaderParamDesc;

//! use these flags for create rendering device
#define	 SX_D3D_
#define	 SX_D3D_VSYNC			0x00000200		//  create device with vertical synchronization
#define	 SX_D3D_FULLSCREEN		0x00000400		//	create device in full screen mode

/*! 
these flags used in Resource3D.
NOTE: use them in care
*/
#define SX_RESOURCE_
#define SX_RESOURCE_CHECKLOST			0x00000001	//  Resource3D will hold the pointer of pointer of resource to manage it on device lost
#define SX_RESOURCE_DYNAMIC				0x00000002	//  create dynamic resource

/*!
use these values as shader flag
shader flag and its combinations describe shader behavior or which shader tech should be set to device.
*/
#define SX_SHADER_
#define SX_SHADER_DEFAULT				0x00000001	//  use default render technique
#define SX_SHADER_ZDEPTH				0x00000002	//  use specified render technique to draw z buffer
#define SX_SHADER_NORMAL				0x00000004	//  use specified render technique to draw normal
#define SX_SHADER_DEFERRED				0x00000008	//  draw objects in MRT for use in deferred lighting
#define	SX_SHADER_SHADOW				0x00000010	//  apply shadow map as simple shadow


//! option of material
#define SX_MATERIAL_
#define SX_MATERIAL_ALPHABLEND			0x00000001		//  this material turn alpha blending enable on
#define SX_MATERIAL_CULLING				0x00000002		//  this material use back face culling
#define SX_MATERIAL_ZENABLE				0x00000004		//  this material use z enable
#define SX_MATERIAL_ZWRITING			0x00000008		//  this material use z writing
#define SX_MATERIAL_ALPHAADD			0x00000010		//  this material turn alpha adding enable on
#define SX_MATERIAL_ALPHAMUL			0x00000020		//  this material turn alpha multiply enable on

//! these flags used in geometry description
#define SX_GEOMETRY_OPTION_ANIMATED		0x00000001		//  geometry contain animation buffers

//! sampler uv used by device
enum SamplerUV 
{
	SUV_WRAP            = 1,
	SUV_MIRROR          = 2,
	SUV_CLAMP           = 3,
	SUV_BORDER          = 4,
	SUV_MIRRORONCE      = 5,
	SUV_FORCE_DWORD     = 0x7fffffff //	force 32-bit size enum
};
#define SUV_

//! shader quality in compile time
enum ShaderQuality
{
	SQ_HIGH,					//  high quality shader will compiled
	SQ_MEDIUM,					//  medium quality shader will compiled
	SQ_LOW,						//  low quality shader will compiled
	SQ_FORCE_DWORD = 0xffffffff	//	force 32-bit size enum
};
#define SQ_

//! type of the parameter used in the shader
enum ShaderParameterType
{
	SPT_UNKNOWN,
	SPT_FLOAT,					//  parameter is a float value
	SPT_FLOAT4,					//  parameter is a vector value
	SPT_SAMPLER,				//  parameter is a sampler

	SPT_FORCE_DWORD = 0xffffffff//	force 32-bit size enum
};
#define SPT_

//! describe fog properties
typedef struct FogDesc
{
	D3DColor	Color;			//  color of the fog
	D3DFogMode	Mode;			//  fog mode 1=exponential   2=exponential (dis)^2   3=linear
	float		Density;		//  density of the fog
	float		Start;			//  fog start plan
	float		End;			//  fog end plan
} *PFogDesc;

//! describe the Annotation in the shader
typedef struct ShaderAnnotation
{
	ShaderParameterType	Type;			//  type of the parameter described by annotation
	char				uiType[32];		//  type of the UI described by annotation
	char				uiName[32];		//  name of the UI described by annotation
	float4				uiData;			//	data represented by annotation include min, max ...
}
*PShaderAnnotation;

//! description of the texture resource
typedef struct TextureDesc
{
	D3DResourceType		rType;
	int					Width;
	D3DFormat			Format;
	DWORD				Levels;
} 
*PTextureDesc;

//! description of the Geometry resource
typedef struct GeometryDesc
{
	UINT	VertexCount[4];
	UINT	FaceCount[4];
	
	AABox	Box;
	Sphere	Sphere;

	DWORD	Option;			//  some option of the geometry ( has animation ? ... )
}
*PGeometryDesc;

//! describe the information of the display graphic card
typedef struct DisplayDriverInfo
{
	WCHAR	Driver[MAX_DEVICE_IDENTIFIER_STRING];
	WCHAR	Description[MAX_DEVICE_IDENTIFIER_STRING];

	int		Product;
	int		Version;
	int		SubVersion;
	int		Build;
} 
*PDisplayDriverInfo;

//! some simple details of the resource files
typedef struct ResourceFileDetails
{
	WCHAR	Name[64];
	WCHAR	Author[32];
	WCHAR	Comment[128];
} 
*PResourceFileDetails;

//! additional structure used in vertex declaration
struct D3DVECTOR2{
	float x;
	float y;

	operator const sx::math::Vector2 (void){
		return *((sx::math::Vector2*)this);
	}

	void operator= (const sx::math::Vector2& v){
		x = v.x;
		y = v.y;
	}
};

//! ground of other vertices
typedef struct D3DVertex {} *PD3DVertex, Vertex, *PVertex;

//! just include position
struct D3DVertex0 : public D3DVertex
{
	union{
		struct{
			float x;
			float y;
			float z;
		};

		D3DVECTOR pos;
	};

	D3DVertex0(){}

	void Transform(Matrix& m, float3& v)
	{
		x = v.x*m._11 + v.y*m._21 + v.z*m._31 + m._41;
		y = v.x*m._12 + v.y*m._22 + v.z*m._32 + m._42;
		z = v.x*m._13 + v.y*m._23 + v.z*m._33 + m._43;
	}
};	
typedef D3DVertex0		Vertex0, *PVertex0, *PD3DVertex0;
static const UINT SEGAN_SIZE_VERTEX_0 = sizeof(D3DVertex0);

//! include normal vector, texture coordinates and two color
struct D3DVertex1 : public D3DVertex
{
	union{
		struct{
			float nx;
			float ny;
			float nz;
		};

		D3DVECTOR nrm;
	};

	union{
		struct{
			float u;
			float v;
		};

		D3DVECTOR2 txc;
	};

	D3DCOLOR col0;
	D3DCOLOR col1;
};
typedef D3DVertex1		Vertex1, *PVertex1, *PD3DVertex1;
static const UINT SEGAN_SIZE_VERTEX_1 = sizeof(D3DVertex1);

//! include tangent vector and an additional texture coordinates
struct D3DVertex2 : public D3DVertex
{
	union{
		struct{
			float tx;
			float ty;
			float tz;
		};

		D3DVECTOR tng;
	};

	union{
		struct{
			float u;
			float v;
		};

		D3DVECTOR2 txc;
	};
};
typedef D3DVertex2		Vertex2, *PVertex2, *PD3DVertex2;
static const UINT SEGAN_SIZE_VERTEX_2 = sizeof(D3DVertex2);

//! include blend weights and blend indices
struct D3DVertex3 : public D3DVertex
{
	union{
		struct{
			float bw0;
			float bw1;
			float bw2;
			float bw3;
		};

		float bw[4];
	};

	union{
		struct{
			BYTE bi0;
			BYTE bi1;
			BYTE bi2;
			BYTE bi3;
		};

		BYTE bi[4];


	};
};
typedef D3DVertex3		Vertex3, *PVertex3, *PD3DVertex3;
static const UINT SEGAN_SIZE_VERTEX_3 = sizeof(D3DVertex3);

//! face structure of the triangle
struct D3DFace
{
	union{
		DWORD v[3];

		struct{
			DWORD v0;
			DWORD v1;
			DWORD v2;
		};
	};

	bool operator==(D3DFace& f){
		return v0==f.v0 && v1==f.v1 && v2==f.v2;
	}
};
typedef D3DFace		Face, *PFace, *PD3DFace;

//! this is the standard callback function of some renderer objects
typedef void (*D3DCallback) (void* useData);

#endif	//	GUARD_sxTypes3D_HEADER_FILE
