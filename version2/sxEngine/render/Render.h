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



//////////////////////////////////////////////////////////////////////////
//	camera
//////////////////////////////////////////////////////////////////////////
struct d3dCamera
{
	float3	m_eye;
	float3	m_at;
	float3	m_up;
	float	m_fov;
	float	m_near_z;
	float	m_far_z;
};

//////////////////////////////////////////////////////////////////////////
//	light
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API d3dLight
{
	enum type{ POINT, SPOT, DIRECTIONAL = 0xaaaaaaaa };
public:
	d3dLight( void );

public:
	type	m_type;
	float3	m_pos;
	float3	m_dir;
	float	m_radius;
	Color	m_color;
};

class SEGAN_ENG_API d3dLightMan
{
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

//////////////////////////////////////////////////////////////////////////
//	material
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API d3dMaterial
{
public:
	d3dMaterial( void );

public:
	Color	m_ambient;
	Color	m_diffuse;
	Color	m_specular;
};

class SEGAN_ENG_API d3dMaterialMan
{
public:
	d3dMaterialMan( void );

	uint count( void );
	d3dMaterial* add( void );
	void remove( d3dMaterial* cam );
	d3dMaterial* get_by_index( const uint index );
	d3dMaterial* get_by_name( const wchar* name );
};

//////////////////////////////////////////////////////////////////////////
//	terrain
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API d3dTerrain
{
public:
	d3dTerrain( void );

public:
	int2	m_size;
};

//////////////////////////////////////////////////////////////////////////
//	static mesh
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API d3dStatic
{
public:
	d3dStatic( void );

public:

};

class SEGAN_ENG_API d3dStaticMan
{
public:
	d3dStaticMan( void );

	uint count( void );
	d3dStatic* add( void );
	void remove( d3dStatic* cam );
	d3dStatic* get_by_index( const uint index );
	d3dStatic* get_by_name( const wchar* name );
	uint get_by_area( Array<d3dStatic*>& result, const float3& pos, const float radius );
	uint get_by_frustum( Array<d3dStatic*>& result, const Frustum& frustum );
};


//////////////////////////////////////////////////////////////////////////
//	dynamic mesh
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API d3dMesh
{
public:
	d3dMesh( void );

public:

};

class SEGAN_ENG_API d3dMeshMan
{
public:
	d3dMeshMan( void );

	uint count( void );
	d3dMesh* add( void );
	void remove( d3dMesh* cam );
	d3dMesh* get_by_index( const uint index );
	d3dMesh* get_by_name( const wchar* name );
	uint get_by_area( Array<d3dMesh*>& result, const float3& pos, const float radius );
	uint get_by_frustum( Array<d3dMesh*>& result, const Frustum& frustum );
};

//////////////////////////////////////////////////////////////////////////
//	renderer
//////////////////////////////////////////////////////////////////////////
class SEGAN_ENG_API RenderMan
{
public:
	RenderMan( void );
	virtual ~RenderMan( void );

	void initialize( dword flags );
	void set_size( const uint width, const uint height );

	void update( float elpstime );
	void draw( float elpstime, uint flag );

public:
	int2				m_size;
	d3dCamera			m_camera;
	d3dLightMan*		m_lights;
	d3dMaterialMan*		m_materials;
	d3dTerrain*			m_terrain;
	d3dStaticMan*		m_statics;
	d3dMeshMan*			m_meshes;
};


#endif	//	GUARD_Render_HEADER_FILE