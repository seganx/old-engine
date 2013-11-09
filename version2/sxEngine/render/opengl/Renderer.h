/********************************************************************
created:	2012/05/07
filename: 	d3dDevice.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain the class of device 3d used OpenGL
			most of codes from Humus :
			http://www.humus.name
*********************************************************************/
#ifndef GUARD_d3dRenderer_gl_HEADER_FILE
#define GUARD_d3dRenderer_gl_HEADER_FILE

#include "Device.h"

class SEGAN_ENG_API Renderer : public d3dRenderer
{
	SEGAN_STERILE_CLASS( Renderer );
public:
	Renderer( void );
	virtual ~Renderer( void );

	void initialize( dword flags );

#if defined(_WIN32)
	void set_size( const uint width, const uint height, const dword SX_D3D_ flags, const struct HWND__* hwnd );
#endif

	//!	create and return a texture object
	d3dTexture* create_texture( void );

	//! create and return a material object
	d3dMaterial* create_material( void );

	//! create and return a mesh
	d3dMesh* create_mesh( void );

	//! create and return a scene object
	d3dScene* create_scene( void );

	//!	set a new scene and apply scene configuration
	void set_scene( const d3dScene* scene );

	//!	update renderer
	void update( float elpstime );

	//!	start to draw the scene
	void begin_draw( const Color& bgcolor );

	//! finish drawing the scene
	void end_draw( void );

	//! draw scene to the display
	void render( float elpstime, uint flag );

	//! draw a simple line
	void draw_line( const float3& v1, const float3& v2, const Color& color );

	//! draw a grid, snapped to integer space but always in front of camera
	void draw_grid( const uint size, const Color& color );

	//! draw a compass by specified size
	void draw_compass( void );

	//! visualize a simple circle
	void draw_circle( const float3& center, const float radius, const dword flags, const Color& color );

	//! visualize a wired box
	void draw_box( const AABox& box, const Color& color );

	//! visualize a wired box
	void draw_box( const OBBox& box, const Color& color );

	//! visualize a simple sphere
	void draw_sphere( const Sphere& sphere, const dword flags, const Color& color, const uint stacks = 17, const uint slices = 20 );

public:
	d3dDevice*		m_device;
};

#endif // GUARD_d3dRenderer_gl_HEADER_FILE

