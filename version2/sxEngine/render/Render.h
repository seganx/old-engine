/********************************************************************
	created:	2013/10/27
	filename: 	Render.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the interface of the rendering system
*********************************************************************/
#ifndef GUARD_Render_HEADER_FILE
#define GUARD_Render_HEADER_FILE

#include "../Engine_def.h"
#include "../math/Math.h"

//////////////////////////////////////////////////////////////////////////
//! flags of rendering device
#define	SX_D3D_
#define SX_D3D_CREATE_DX				0x00000001		//	create directX device
#define SX_D3D_CREATE_GL				0x00000002		//	create openGL device
#define	SX_D3D_VSYNC					0x00000004		//  init device with vertical synchronization
#define	SX_D3D_FULLSCREEN				0x00000008		//	init device in full screen mode
#define SX_D3D_RESOURCE_DYNAMIC			0x00000010		//	create hardware dynamic resource
#define SX_D3D_RESOURCE_MANAGED			0x00000020		//	hold a copy of data in system memory and use it at lock/unlock calls


//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
//! sampler UV used in texturing
enum d3dAddressMode 
{
	AM_WRAP = 0,
	AM_MIRROR,
	AM_CLAMP,
	AM_BORDER,

	AM_32BITENUM = 0xffffffff
};

//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
//! texture type
enum d3dTextureType
{
	TT_2D = 0,
	TT_CUBE,
	TT_VOLUME,

	TT_32BITENUM = 0xffffffff
};

//////////////////////////////////////////////////////////////////////////
//! describe the information of the display graphic card
struct d3dDriverInfo
{
	wchar	vendor[256];
	wchar	description[256];
};

//////////////////////////////////////////////////////////////////////////
//! describe the capabilities of display graphic card
struct d3dDriverCaps
{
	uint	maxAnisotropy;
	uint	maxTextureSize;
	uint	maxMrtCount;
};

//////////////////////////////////////////////////////////////////////////
//! describe the information of the current display mode
struct d3dDisplayeMode
{
	uint		width;
	uint		height;
	uint		refereshRate;
	d3dFormat	colorFormat;
	d3dFormat	depthFormat;
};

//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
//! color struct
struct d3dColor
{
	Color c0;
	Color c1;
};

//////////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////////
//!	material properties
struct d3dMaterial 
{
	Color				ambient;
	Color				diffuse;
	Color				specular;
	Color				emissive;
	variable			var[16];
	class d3dTexture*	texture[8];
	class d3dShader*	shader;
};


//////////////////////////////////////////////////////////////////////////
//	camera
struct d3dCamera
{
	float3	eye;
	float3	at;
	float3	up;
	float	fov;
	float	near_z;
	float	far_z;
};


//////////////////////////////////////////////////////////////////////////
//! abstract class of texture
class SEGAN_ENG_API d3dTexture
{
	SEGAN_STERILE_CLASS( d3dTexture );

public:

	d3dTexture( void ) {};
	virtual ~d3dTexture( void ) {};

	//! set new texture description. this may clear current data
	virtual void set_desc( d3dTextureDesc& desc ) = 0;

	/*! 
	copy data to the texture buffer in the specified level and face.
	NOTE: size of data in byte most be the same as size of the texture image.
	*/
	virtual void set_image( void* data, uint level = 0, uint face = 0 ) = 0;

	/*
	copy texture image to the data buffer from specified level and face.
	NOTE: size of data in byte most be the same as size of texture image.
	*/
	virtual void get_image( void* data, uint level = 0, uint face = 0 ) = 0;

	//! return size of image in byte
	virtual uint get_data_size( uint level = 0 ) = 0;

public:

	d3dTextureDesc		m_desc;		//  description of texture
};

//////////////////////////////////////////////////////////////////////////
//!	mesh
class d3dMesh
{
	SEGAN_STERILE_CLASS(d3dMesh)

public:
	d3dMesh( void ) {};
	virtual ~d3dMesh( void ) {};

public:

};

//////////////////////////////////////////////////////////////////////////
//	renderer
class SEGAN_ENG_API RenderMan
{
	SEGAN_STERILE_CLASS( RenderMan );
public:
	virtual void initialize( dword flags ) = 0;
	virtual void set_size( const uint width, const uint height, const dword SX_D3D_ flags ) = 0;
	virtual void update( float elpstime ) = 0;
	virtual void draw( float elpstime, uint flag ) = 0;

public:

	d3dCamera			m_camera;
//	d3dLightMan*		m_lights;
//	d3dMaterialMan*		m_materials;
//	d3dTerrain*			m_terrain;
//	d3dStaticMan*		m_statics;
//	d3dMeshMan*			m_meshes;
};

//! create a renderer object
RenderMan* sx_create_renderer( const dword SX_D3D_ flags );



#if 0
//////////////////////////////////////////////////////////////////////////
//	light
struct d3dLight
{
	enum type {
		POINT,
		SPOT,
		DIRECTIONAL = 0xaaaaaaaa
	}		m_type;
	float3	m_pos;
	float3	m_dir;
	float	m_radius;
	Color	m_color;
};

class SEGAN_ENG_API d3dLightMan
{
	SEGAN_STERILE_CLASS( d3dTexture );
public:
	d3dLightMan( void );

	uint count( void );
	d3dLight* add( void );
	void remove( d3dLight* cam );
	d3dLight* get_by_index( const uint index );
	d3dLight* get_by_name( const wchar* name );
	uint get_by_area( Array<d3dLight*>& result, const float3& pos, const float radius );
	uint get_by_frustum( Array<d3dLight*>& result, const Frustum& frustum );
};
#endif


#endif	//	GUARD_Render_HEADER_FILE