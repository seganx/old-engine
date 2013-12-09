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
#define SX_D3D_WIREFRAME				0x00000200
#define SX_D3D_BOUNINGBOX				0x00000400
#define SX_D3D_BILLBOARD				0x00000800


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
//! describe the information and capabilities of display graphic card
struct d3dDriverInfo
{
	wchar	vendor[256];
	wchar	description[256];
	uint	maxAnisotropy;
	uint	maxTextureSize;
	uint	maxMrtCount;
};

//////////////////////////////////////////////////////////////////////////
//! describe the information of the current display mode
struct d3dDisplayeInfo
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
//! shader description
struct d3dShaderDesc
{
	uint				numTexture;		//!	number of texture used in shader
	uint				numVariable;	//! number of variable used in shader
	uint				numVariable4;	//!	number of 4D variable used in shader
	float				var[16];		//! variables of the shader
	float4				var4d[8];		//! 4D variables of the shader
};

//////////////////////////////////////////////////////////////////////////
//!	material properties
struct d3dMaterial 
{
	variable			var[16];		//! variables of the material will pass to shader
	variable4			var4d[8];		//! 4D variables of the material will pass to shader
	class d3dTexture*	texture[16];	//! textures of the material will pass to shader
	class d3dShader*	shader;			//! shader of material
};

//////////////////////////////////////////////////////////////////////////
//!	basic element of a graphical user interface
struct d3dContext
{
	uint				vcount;			//!	number of vertices
	d3dMaterial*		material;		//!	material used to draw element
	float3*				pos;			//!	positions
	float2*				uv;				//!	UV coordinates
	Color2*				color;			//!	colors
};

//////////////////////////////////////////////////////////////////////////
//! mesh description
struct d3dMeshDesc
{
	dword		flag;			//! resource flag SX_D3D_RESOURCE_
	d3dFormat	format;			//! combination of FMT_VXXX
	uint		vercount;		//! number of vertices
	uint		tricount;		//!	number of triangles
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

	d3dTexture( void ) {}
	virtual ~d3dTexture( void ) {}

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
//! abstract class of shader
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API d3dShader
{
	SEGAN_STERILE_CLASS( d3dShader );

public:
	d3dShader( void ) {}
	virtual ~d3dShader( void ) {}

	//! set shader code
	virtual void set_code( const wchar* code ) = 0;

	//! compile the shader. return false if operation failed and fill out the destlog with error message
	virtual bool compile( const char* defines, wchar* destlog = null ) = 0;

	//! return the name of float variable parameter
	virtual const wchar* get_float_name( const uint index ) = 0;

	//! return the name of float4 variable parameter
	virtual const wchar* get_float4_name( const uint index ) = 0;

	//! return the name of texture parameter
	virtual const wchar* get_texture_name( const uint index ) = 0;

	//! set new float variable
	virtual void set_float( const uint index, const float var ) = 0;

	//! set new 4D float variable
	virtual void set_float4( const uint index, const float4 var ) = 0;

	//! set new texture
	virtual void set_texture( const uint index, const d3dTexture* tex ) = 0;

public:

	d3dShaderDesc	m_desc;		//	description of the shader
};


//////////////////////////////////////////////////////////////////////////
//!	mesh
class SEGAN_ENG_API d3dMesh
{
	SEGAN_STERILE_CLASS( d3dMesh );

public:
	d3dMesh( void ) {}
	virtual ~d3dMesh( void ) {}

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

	AABox					m_box;			//	axis aligned bounding box of the mesh
	Sphere					m_sphere;		//	bounding sphere around the mesh
	Array<d3dMaterial*>		m_materials;	//	array of the material
	uint					m_matIndex;		//	index of the current material
};


//////////////////////////////////////////////////////////////////////////
//	mesh manager
class SEGAN_ENG_API d3dMeshMan
{
	SEGAN_STERILE_CLASS( d3dMeshMan );
public:
	d3dMeshMan( void ) {}
	virtual ~d3dMeshMan( void ) {}

	/*! 
	add a mesh to the scene 
	NOTE: this function must add the new mesh to the list of meshes for additional access
	*/
	virtual void add( const d3dMesh* mesh ) = 0;

	/*!
	remove mesh from the scene
	NOTE: this function must remove the mesh from the list of meshes
	*/
	virtual void remove( const d3dMesh* mesh ) = 0;

	/*!
	get meshes from an specified area
	NOTE: this function should uses a tree structure for fast search
	*/
	virtual uint get_by_area( Array<d3dMesh*>& result, const float3& pos, const float radius ) = 0;

	/*!
	get meshes from an specified frustum
	NOTE: this function should uses a tree structure for fast search
	*/
	virtual uint get_by_frustum( Array<d3dMesh*>& result, const Frustum& frustum ) = 0;

public:
	Array<d3dMesh*>		m_list;
};

//////////////////////////////////////////////////////////////////////////
//	scene manager
class SEGAN_ENG_API d3dScene
{
	SEGAN_STERILE_CLASS( d3dScene );
public:
	d3dScene( void ) {}
	virtual ~d3dScene( void ) {}


public:
	d3dCamera			m_camera;
	d3dMeshMan*			m_meshes;
	//	d3dLightMan*		m_lights;
	//	d3dMaterialMan*		m_materials;
	//	d3dTerrain*			m_terrain;
	//	d3dStaticMan*		m_statics;


};

//////////////////////////////////////////////////////////////////////////
//	renderer
class SEGAN_ENG_API d3dRenderer
{
	SEGAN_STERILE_CLASS( d3dRenderer );
public:
	d3dRenderer( void ) {}
	virtual ~d3dRenderer( void ) {}

	virtual void initialize( dword flags ) = 0;

#if defined(_WIN32)
	virtual void set_size( const uint width, const uint height, const dword SX_D3D_ flags, const struct HWND__* hwnd ) = 0;
#endif

	//!	create and return a texture object
	virtual d3dTexture* create_texture( void ) = 0;

	//! create and return a material object
	virtual d3dMaterial* create_material( void ) = 0;

	//! create and return a mesh
	virtual d3dMesh* create_mesh( void ) = 0;

	//! create and return a scene object
	virtual d3dScene* create_scene( void ) = 0;

	//!	set a new scene and apply scene configuration
	virtual void set_scene( const d3dScene* scene ) = 0;

	//!	update renderer
	virtual void update( float elpstime ) = 0;

	//!	start to draw the scene
	virtual void begin_draw( const Color& bgcolor ) = 0;

	//! finish drawing the scene
	virtual void end_draw( void ) = 0;

	//! draw scene to the display
	virtual void render( float elpstime, uint flag ) = 0;

	//! draw a simple line
	virtual void draw_line( const float3& v1, const float3& v2, const Color& color ) = 0;

	//! draw a grid, snapped to integer space but always in front of camera
	virtual void draw_grid( const uint size, const Color& color ) = 0;

	//! draw a compass by specified size
	virtual void draw_compass( void ) = 0;

	//! visualize a simple circle
	virtual void draw_circle( const float3& center, const float radius, const dword flags, const Color& color ) = 0;

	//! visualize a wired box
	virtual void draw_box( const AABox& box, const Color& color ) = 0;

	//! visualize a wired box
	virtual void draw_box( const OBBox& box, const Color& color ) = 0;

	//! visualize a simple sphere
	virtual void draw_sphere( const Sphere& sphere, const dword flags, const Color& color, const uint stacks = 17, const uint slices = 20 ) = 0;

public:

	d3dDriverInfo			m_driverinfo;		//!	describe the device capabilities
	d3dDisplayeInfo			m_displayinfo;		//!	describe the display information
	d3dCamera				m_camera;			//!	camera used in rendering
	d3dScene*				m_scene;			//!	the scene should be draw in render function
	Array<d3dContext*>		m_elements;			//!	all elements should be draw at the end of rendering function 
};



//////////////////////////////////////////////////////////////////////////
//	HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////

//! create a renderer object
SEGAN_ENG_API d3dRenderer* sx_create_renderer( const dword SX_D3D_ flags );



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