/********************************************************************
	created:	2012/05/06
	filename: 	Device3D.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the abstract class of rendering device
*********************************************************************/
#ifndef GUARD_Device3D_HEADER_FILE
#define GUARD_Device3D_HEADER_FILE

#include "../../sxLib/Lib.h"


//! flags of rendering device
#define	 SX_D3D_
#define  SX_D3D_CREATE_DX				0x00000001		//	create directX device
#define  SX_D3D_CREATE_GL				0x00000002		//	create openGL device
#define	 SX_D3D_VSYNC					0x00000004		//  init device with vertical synchronization
#define	 SX_D3D_FULLSCREEN				0x00000008		//	init device in full screen mode
#define	 SX_D3D_RESOURCE_DYNAMIC		0x00000010		//	create dynamic resource

//! resource formats
enum d3dFormat
{
	FMT_UNKNOWN = 0,

	FMT_L8,
	FMT_A8L8,
	FMT_XRGB8,
	FMT_ARGB8,

	FMT_DXT1,
	FMT_DXT5,

	FMT_D16,
	FMT_D24X8,
	FMT_D24S8,
	FMT_D32,

	FMT_32BITENUM = 0xffffffff
};
#define FMT_

//! matrix modes
enum d3dMatrixMode
{
	MM_WORLD = 0,
	MM_VIEW,
	MM_PROJECTION,

	MM_32BITENUM = 0xffffffff
};
#define MM_

//! primitives supported by draw-primitive API
enum d3dPrimitiveType
{
	PT_POINT = 0,
	PT_LINE_LIST,
	PT_LINE_STRIP,
	PT_TRIANGLE_LIST,
	PT_TRIANGLE_STRIP,
	PT_TRIANGLE_FAN,

	PT_32BITENUM = 0xffffffff
};
#define PT_

//! sampler UV used in texturing
enum d3dAddressMode 
{
	AM_WRAP = 0,
	AM_MIRROR,
	AM_CLAMP,
	AM_BORDER,

	AM_32BITENUM = 0xffffffff
};
#define AM_

//! sampler filter used in texturing
enum d3dFilter
{
	FILTER_POINT = 0,
	FILTER_LINEAR,
	FILTER_BILINEAR,
	FILTER_TRILINEAR,
	FILTER_LINEAR_ANISOTROPIC,
	FILTER_BILINEAR_ANISOTROPIC,
	FILTER_TRILINEAR_ANISOTROPIC,

	FILTER_32BITENUM = 0xffffffff
};
#define FILTER_

//! texture type
enum d3dTextureType
{
	TT_2D = 0,
	TT_CUBE,
	TT_VOLUME,

	TT_32BITENUM = 0xffffffff
};
#define TT_

//! describe the information of the display graphic card
struct d3dDriverInfo
{
	wchar	vendor[256];
	wchar	description[256];
};

//! describe the capabilities of display graphic card
struct d3dDriverCaps
{
	uint	maxAnisotropy;
	uint	maxTextureSize;
	uint	maxMrtCount;
};

//! describe the information of the current display mode
struct d3dDisplayeMode
{
	uint		width;
	uint		height;
	uint		refereshRate;
	d3dFormat	colorFormat;
	d3dFormat	depthFormat;
};

//! contain parameters of debug information 
struct d3dDebugInfo
{
	uint		fps;			//! current FPS rate
	uint		drawCalls;		//! number of draw call
	uint		vertices;		//! number of vertices
	uint		triangles;		//!	number of triangles
	uint		setShader;		//! number of shader changes
	uint		setTextures;	//! number of texture changes
	uint		setVertices;	//! number of vertex buffer changes
	uint		setIndices;		//! number of index buffer changes
	float		frameTime;		//! frame time in milliseconds
};

//! color structure used in API
struct d3dColor
{
	float		r;		//	red channel
	float		g;		//	green channel
	float		b;		//	blue channel
	float		a;		//	alpha channel
};

//! view port structure
struct d3dViewport
{
	sint		x;
	sint		y;
	sint		width;
	sint		height;
};

//! vertex buffer description
struct d3dVertexBufferDesc
{
	uint		size;			//	size in bytes
	dword		flag;			//	resource flag SX_D3D_RESOURCE_
};

//! index buffer description
struct d3dIndexBufferDesc
{
	uint		size;			//	size in bytes
	dword		flag;			//	resource flag SX_D3D_RESOURCE_
};

//! texture description
struct d3dTextureDesc
{
	d3dTextureType	type;		//	type of texture
	d3dFormat		format;
	d3dFilter		filter;
	d3dAddressMode	addressMode;
	uint			width;
	uint			height;
	uint			depth;
	uint			mipmaps;	//	number of additional mipmaps
	uint			lod;
	uint			anisotropy;
	dword			flag;		//	resource flag SX_D3D_RESOURCE_
};

//! shader structure
struct d3dShaderDesc
{
	dword		flag;		//	resource flag SX_D3D_RESOURCE_
};

//! abstract class of hardware vertex buffer
class SEGAN_ENG_API d3dVertexBuffer
{
	SEGAN_STERILE_CLASS( d3dVertexBuffer );

public:

	d3dVertexBuffer( void );
	virtual ~d3dVertexBuffer( void );

	const d3dVertexBufferDesc* GetDesc( void ) const;

	virtual void SetDesc( d3dVertexBufferDesc& desc, void* data = null ) = 0;

	virtual void* Lock( void ) = 0;

	virtual void Unlock( void ) = 0;

	virtual void SetToDevice( uint streamIndex ) = 0;

public:

	d3dVertexBufferDesc		m_desc;		//  description of vertex buffer
};

//! abstract class of hardware index buffer
class SEGAN_ENG_API d3dIndexBuffer
{
	SEGAN_STERILE_CLASS( d3dIndexBuffer );

public:

	d3dIndexBuffer( void );
	virtual ~d3dIndexBuffer( void );

	const d3dIndexBufferDesc* GetDesc( void ) const;

	virtual void SetDesc( d3dIndexBufferDesc& desc, void* data = null ) = 0;

	virtual void* Lock( void ) = 0;

	virtual void Unlock( void ) = 0;

	virtual void SetToDevice( void ) = 0;

public:

	d3dIndexBufferDesc		m_desc;		//  description of index buffer
};

//! abstract class of texture
class SEGAN_ENG_API d3dTexture
{
	SEGAN_STERILE_CLASS( d3dTexture );

public:

	d3dTexture( void );
	virtual ~d3dTexture( void );

	const d3dTextureDesc* GetDesc( void ) const;

	virtual void SetDesc( d3dTextureDesc& desc ) = 0;

	/*! 
	copy data to the texture buffer in the specified level and face.
	NOTE: size of data in byte most be the same as size of the texture image.
	*/
	virtual void SetImage( void* data, uint level = 0, uint face = 0 ) = 0;

	/*
	copy texture image to the data buffer from specified level and face.
	NOTE: size of data in byte most be the same as size of texture image.
	*/
	virtual void GetImage( void* data, uint level = 0, uint face = 0 ) = 0;

	//! return size of image in byte
	virtual uint GetDataSize( uint level = 0 ) = 0;

	//! set this texture to the specified stage of device
	virtual void SetToDevice( uint stage = 0 ) = 0;

public:

	d3dTextureDesc		m_desc;		//  description of texture
};

//! abstract class of shader
class SEGAN_ENG_API d3dShader
{
	SEGAN_STERILE_CLASS( d3dShader );

public:

	d3dShader( void );
	virtual ~d3dShader( void );

	const d3dShaderDesc* GetDesc( void ) const;

	virtual void SetDesc( d3dShaderDesc& desc ) = 0;

public:

	d3dShaderDesc		m_desc;		//  description of shader
};

//! abstract class of rendering device
class SEGAN_ENG_API d3dDevice
{
	SEGAN_STERILE_CLASS( d3dDevice );

public:

	d3dDevice( void );
	virtual ~d3dDevice( void );

	//! initialize the graphic device
 	virtual void Initialize( const handle displayHandle ) = 0;
 
	//! destroy resources and finalize graphic device
 	virtual void Finalize( void ) = 0;
 
	//! set new size and behavior of device. pass -1 to each param to avoid change that param
	virtual bool SetSize( const uint width, const uint height, const dword SX_D3D_ flag ) = 0;

	//! create hardware vertex buffer
	virtual void CreateVertexBuffer( d3dVertexBuffer*& OUT vertexBuffer ) = 0;

	//! destroy hardware vertex buffer
	virtual void DestroyVertexBuffer( d3dVertexBuffer*& IN_OUT vertexBuffer ) = 0;

	//! set vertex buffer to the device
	virtual void SetVertexBuffer( const d3dVertexBuffer* vertexBuffer, uint streamIndex ) = 0;

	//! create hardware index buffer
	virtual void CreateIndexBuffer( d3dIndexBuffer*& OUT indexBuffer ) = 0;

	//! destroy hardware index buffer
	virtual void DestroyIndexBuffer( d3dIndexBuffer*& IN_OUT indexBuffer ) = 0;

	//! set index buffer to the device
	virtual void SetIndexBuffer( const d3dIndexBuffer* indexBuffer ) = 0;

	//! create texture
	virtual void CreateTexture( d3dTexture*& OUT texture ) = 0;

	//! destroy texture
	virtual void DestroyTexture( d3dTexture*& IN_OUT texture ) = 0;

	//! set texture object to the device
	virtual void SetTexture( const d3dTexture* texture, uint stage = 0 ) = 0;

	//! create shader
	virtual void CreateShader( d3dShader*& OUT shader ) = 0;

	//! destroy shader
	virtual void DestroyShader( d3dShader*& IN_OUT shader ) = 0;

	//! set rendering view port
	virtual void SetViewport( const d3dViewport* viewport ) = 0;

	//! set matrix
	virtual void SetMatrix( const d3dMatrixMode mode, const float* _matrix ) = 0;

	//! return selected matrix
	virtual const float* GetMatrix( const d3dMatrixMode mode ) = 0;

	//! draw primitive shapes
	virtual void DrawPrimitive( const d3dPrimitiveType primType, const int firstVertex, const int vertexCount ) = 0;

	//! draw indexed primitive shapes
	virtual void DrawIndexedPrimitive( const d3dPrimitiveType primType, const int firstIndex, const int indicesCount, const int firstVertex, const int vertexCount ) = 0;

	//! draw primitive by given vertices
	virtual void DrawDebug( const d3dPrimitiveType primType, const uint vertxcount, const float* vertices, const dword color ) = 0;

	//! begin to draw shapes
	virtual bool BeginScene( void )= 0;

	//! drawing shapes finished
	virtual void EndScene( void ) = 0;

	//! copy back buffer image to monitor
	virtual void Present( void ) = 0;

	//! clear depth buffer and back buffer with specified color
	virtual void ClearScreen( const dword bgcolor ) = 0;

	//! clear back buffer with specified color
	virtual void ClearTarget( const dword bgcolor ) = 0;

	//! clear back buffer
	virtual void ClearZBuffer( void ) = 0;
 
	//! set clip plane
 	virtual void SetClipPlane( const uint index, const float* plane ) = 0;
 
	//! get clip plane
 	virtual void GetClipPlane( const uint index, float* plane ) = 0;

public:
	struct {
		dword		flag;
		uint		width;
		uint		height;
	}					
						m_creationData;			//	describe parameters of creation data

	d3dDriverInfo		m_driverInfo;			//  contain driver information
	d3dDriverCaps		m_driverCaps;			//	contain driver capabilities
	d3dDisplayeMode		m_driverDisplayMode;	//	contain default driver display mode

	d3dDebugInfo		m_debugInfo;			//	describe debug information

	d3dViewport			m_defaultViewport;		//	default viewport
	d3dViewport			m_viewport;				//	current viewport

};


//////////////////////////////////////////////////////////////////////////
//	device 3d 
//////////////////////////////////////////////////////////////////////////

//! initialize 3d device depend on platform specification
SEGAN_ENG_API void sx_d3d_initialize( dword flag );

//! finalize 3d device
SEGAN_ENG_API void sx_d3d_finalize( void );

//! create and return a new 3d device. the new device can be draw shapes after initialization 
SEGAN_ENG_API d3dDevice* sx_d3d_create_device( dword flag );

//! destroy created device
SEGAN_ENG_API void sx_d3d_destroy_device( d3dDevice* &pdevice );


#endif	//	GUARD_Device3D_HEADER_FILE

