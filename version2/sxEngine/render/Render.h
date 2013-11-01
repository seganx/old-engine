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
#define	SX_D3D_VSYNC					0x00000001		//  init device with vertical synchronization
#define	SX_D3D_FULLSCREEN				0x00000002		//	init device in full screen mode
#define SX_D3D_RESOURCE_DYNAMIC			0x00000004		//	create hardware dynamic resource
#define SX_D3D_RESOURCE_MANAGED			0x00000008		//	hold a copy of data in system memory and use it at lock/unlock calls
#define SX_D3D_VISIBLE					0x00000010
#define SX_D3D_ENABLE					0x00000020
#define SX_D3D_CASTSHADOW				0x00000040
#define SX_D3D_RECEIVESHADOW			0x00000080
#define SX_D3D_REFLECT					0x00000100


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

	FMT_VNORMAL		= 0x10000000,
	FMT_VTANGENT	= 0x20000000,
	FMT_VCOORD0		= 0x40000000,
	FMT_VCOORD1		= 0x80000000,
	FMT_VCOLORS		= 0x01000000,
	FMT_VINDEX		= 0x02000000,
	FMT_VWEIGHT		= 0x04000000
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
	variable4			var4d[4];
	class d3dTexture*	texture[8];
	class d3dShader*	shader;
};

//////////////////////////////////////////////////////////////////////////
//! mesh description
struct d3dMeshDesc
{
	dword		flag;			//! resource flag SX_D3D_RESOURCE_
	d3dFormat	format;			//! combination of FMT_VXXX
	uint		numVertices;	//! number of vertices
	uint		numTriangles;	//!	number of triangles
};


//////////////////////////////////////////////////////////////////////////
//!	camera
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
	SEGAN_STERILE_CLASS( d3dMesh );

public:
	d3dMesh( void ) {};
	virtual ~d3dMesh( void ) {};

	//! set new mesh description. this may clear current data
	virtual void set_desc( d3dMesh& desc ) = 0;

	//! lock array of positions of the mesh. return null if the function failed to lock
	virtual float3* lock_positions( void ) = 0;

	//! unlock array of positions
	virtual void unlock_positions( void ) = 0;

	//! lock array of normals of the mesh. return null if the function failed to lock
	virtual float3* lock_normals( void ) =  0;

	//! unlock array of normals
	virtual void unlock_normals( void ) = 0;

	//! lock array of tangents of the mesh. return null if the function failed to lock
	virtual float3* lock_tangents( void ) = 0;

	//! unlock array of tangents
	virtual void unlock_tangents( void ) = 0;

	//! lock array of texture coordinates in the mesh. return null if the function failed to lock
	virtual float2* lock_texcoords( const uint index ) = 0;

	//! unlock array of texture coordinates
	virtual void unlock_texcoords( const uint index ) = 0;

	//! lock array of vertex colors in the mesh. return null if the function failed to lock
	virtual d3dColor* lock_colors( void ) = 0;

	//! unlock array of texture coordinates
	virtual void unlock_colors( void ) = 0;

public:

	Array<d3dMaterial*>		m_materials;	//	array of the material
	uint					m_matIndex;		//	index of the current material
};

//////////////////////////////////////////////////////////////////////////
//	renderer
class SEGAN_ENG_API Renderer
{
	SEGAN_STERILE_CLASS( Renderer );
public:
	virtual void initialize( dword flags ) = 0;
	virtual void set_size( const uint width, const uint height, const dword SX_D3D_ flags ) = 0;
	virtual void update( float elpstime ) = 0;
	virtual void draw( float elpstime, uint flag ) = 0;

	virtual d3dTexture* create_texture( void );
	virtual d3dMaterial* create_material( void );
	virtual d3dMesh* create_mesh( void );

public:

	d3dCamera			m_camera;
//	d3dLightMan*		m_lights;
//	d3dMaterialMan*		m_materials;
//	d3dTerrain*			m_terrain;
//	d3dStaticMan*		m_statics;
//	d3dMeshMan*			m_meshes;
};

//! create a renderer object
Renderer* sx_create_renderer( const dword SX_D3D_ flags );



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