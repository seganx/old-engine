#include "Renderer.h"

Renderer::Renderer( void ): d3dRenderer(), m_device(0)
{
	memset( &m_driverinfo,	0, sizeof(d3dDriverInfo)	);
	memset( &m_displayinfo, 0, sizeof(d3dDisplayeInfo)	);
	memset( &m_camera,		0, sizeof(d3dCamera)		);
	memset( &m_driverinfo,	0, sizeof(d3dDriverInfo)	);

	d3dScene* m_scene = 0;
}

Renderer::~Renderer( void )
{

}

void Renderer::initialize( dword flags )
{
	m_device = sx_new( d3dDevice );
}

void Renderer::set_size( const uint width, const uint height, const dword SX_D3D_ flags, const struct HWND__* hwnd )
{
	if ( m_device->m_initParam.context == null )
	{
		m_device->initialize( hwnd );
	}
	m_device->set_size( width, height, flags );
}

d3dTexture* Renderer::create_texture( void )
{
	d3dTexture_gl* res = sx_new( d3dTexture_gl );
	return res;
}

d3dMaterial* Renderer::create_material( void )
{
	return null;
}

d3dMesh* Renderer::create_mesh( void )
{
	return null;
}

d3dScene* Renderer::create_scene( void )
{
	return null;
}

void Renderer::set_scene( const d3dScene* scene )
{

}

void Renderer::update( float elpstime )
{
	static float timer = 0;
	timer += elpstime;
	float aspect = (float)m_device->m_viewport.height / (float)m_device->m_viewport.width;

	matrix proj = sx_perspective_fov( PI/3.0f, aspect, 0.5f, 1000.0f );
	matrix view = sx_lookat( float3( sx_sin( timer * 0.001f ) * 5.0f, 5.0f, -5.0f ), float3(0,0,0), float3(0,1.0f,0) );

	m_device->set_matrix( MM_VIEW, view );
	m_device->set_matrix( MM_PROJECTION, proj );
}

void Renderer::begin_draw( const Color& bgcolor )
{
	m_device->begin_scene();
	m_device->clear_screen( bgcolor );
}

void Renderer::end_draw( void )
{
	m_device->end_scene();
	m_device->present();
}

void Renderer::render( float elpstime, uint flag )
{
#if 1
	float vert[9] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	m_device->draw_debug( PT_LINE_STRIP, 3, vert, Color(0.0f, 0.0f, 1.0f, 1.0f) );
#endif

#if 1
	for ( sint i=0; i<m_elements.m_count; ++i )
	{
		d3dElement* elmnt = m_elements[i];
		m_device->draw_debug( PT_LINE_STRIP, elmnt->vcount, &elmnt->pos->x, 0xffffffff );
	}
#endif
}

void Renderer::draw_line( const float3& v1, const float3& v2, const Color& color )
{
	const float3 v[2] = { v1, v2 };
	m_device->draw_debug( PT_LINE_LIST, 2, &v->x, color );
}

void Renderer::draw_grid( const uint size, const Color& color )
{
	const uint lines = size < 50 ? size : 50;
	const uint xpoints = lines * 4 + 4;
	const uint zpoints = lines * 4 + xpoints;
	const float s = (float)lines;

	matrix mat;
	const matrix& matview = m_device->get_matrix( MM_VIEW );
	sx_inverse( mat, matview );
	float dis = 5.0f + lines;
	float3 cen( sx_round( mat.m30 + dis * matview.m02 ), 0.0f, sx_round( mat.m32 + dis * matview.m22 ) );

	mat.identity();
	m_device->set_matrix( MM_WORLD, mat );

	float3 v[404];

	v[0].set( cen.x,		0, cen.z + s );
	v[1].set( cen.x,		0, cen.z - s );
	v[2].set( cen.x + s,	0, cen.z );
	v[3].set( cen.x - s,	0, cen.z );

	//  place lines along x
	float x = 1.0f;
	for ( uint i=4; i<xpoints; i+=4 )
	{
		v[i+0].set( cen.x + x, 0.0f, cen.z + s );
		v[i+1].set( cen.x + x, 0.0f, cen.z - s );
		v[i+2].set( cen.x - x, 0.0f, cen.z + s );
		v[i+3].set( cen.x - x, 0.0f, cen.z - s );

		x += 1.0f;
	}

	//  place lines along z
	float z = 1.0f;
	for ( uint i=xpoints; i<zpoints; i+=4 )
	{
		v[i+0].set( cen.x + s, 0.0f, cen.z + z );
		v[i+1].set( cen.x - s, 0.0f, cen.z + z );
		v[i+2].set( cen.x + s, 0.0f, cen.z - z );
		v[i+3].set( cen.x - s, 0.0f, cen.z - z );

		z += 1.0f;
	}

	m_device->draw_debug( PT_LINE_LIST, zpoints, &v->x, color );


	m_device->set_render_state( RS_ZENABLE, false );
	draw_line( float3(   -s, 0.0f, 0.0f ), float3(    s, 0.0f, 0.0f ), 0xffff0000 );
	draw_line( float3( 0.0f, 0.0f,   -s ), float3( 0.0f, 0.0f,    s ), 0xff0000ff );
	m_device->set_render_state( RS_ZENABLE, true );
	draw_line( float3( 0.0f, 0.0f, 0.0f ), float3( 0.0f, 2.0f, 0.0f ), 0xff00ff00 );
}

void Renderer::draw_compass( void )
{
	const uint vcont = 18;
	float3 v[vcont];

	float	bs = 0.15f;
	float	bl = 0.85f;

	//  define x line
	v[0].set(0, 0, 0);
	v[1].set(1, 0, 0);
	v[2].set(bl, 0, bs);
	v[3].set(bl, 0,-bs);
	v[4] = v[1];

	//  define y line
	v[5].set(0, 0, 0);
	v[6].set(0, 1, 0);
	v[7].set(0, bl, bs);
	v[8].set(0, bl, -bs);
	v[9] = v[6];
	v[10].set( bs, bl, 0);
	v[11].set(-bs, bl, 0);
	v[12] = v[6];

	//  define z line
	v[13].set(0, 0, 0);
	v[14].set(0, 0, 1);
	v[15].set( bs, 0, bl);
	v[16].set(-bs, 0, bl);
	v[17] = v[14];


	matrix currproj = m_device->get_matrix( MM_PROJECTION );
	matrix currview = m_device->get_matrix( MM_VIEW );

	//  compute compass matrix
	matrix cmpsworld = sx_scale( 20, 20, 20 );
	m_device->set_matrix( MM_WORLD, cmpsworld );

	matrix cmpsview = currview;
	cmpsview.m30 =  ( 0.5f * m_device->m_viewport.width		- 40.0f );
	cmpsview.m31 = -( 0.5f * m_device->m_viewport.height	- 40.0f );
	m_device->set_matrix( MM_VIEW, cmpsview );

	matrix cmpsproj = sx_orthographic( (float)m_device->m_viewport.width, (float)m_device->m_viewport.height, -6000.0f, +6000.0f );
	m_device->set_matrix( MM_PROJECTION, cmpsproj );

	//  draw compass
	m_device->set_render_state( RS_ZENABLE, false );
	m_device->draw_debug(PT_LINE_STRIP, 4, v[13], 0xff0000ff );
	m_device->draw_debug(PT_LINE_STRIP, 4, v[0], 0xffff0000 );
	m_device->draw_debug(PT_LINE_STRIP, 7, v[5], 0xff00ff00 );
	m_device->set_render_state( RS_ZENABLE, true );

	m_device->set_matrix( MM_VIEW, currview );
	m_device->set_matrix( MM_PROJECTION, currproj );

}

void Renderer::draw_circle( const float3& center, const float radius, const dword flags, const Color& color )
{
	const uint	vcount	= 36 + 1;
	const float vphi	= DEG_TO_RAD * 10.0f;
	float3 v[vcount];
	for ( int i=0; i<vcount; ++i )
	{
		const float phi = vphi * float(i);
		v[i].x = radius * sx_cos_fast( phi );
		v[i].y = radius * sx_sin_fast( phi );
		v[i].z = 0;
	}

	if ( flags & SX_D3D_BILLBOARD )
	{
		matrix view = m_device->get_matrix( MM_VIEW );
		matrix oldworld = m_device->get_matrix( MM_WORLD );
		matrix world = sx_inverse( view );
		world.set_translation( center.x, center.y, center.z );
		m_device->set_matrix( MM_WORLD, world );
		m_device->draw_debug( PT_LINE_STRIP, vcount, &v->x, color );
		m_device->set_matrix( MM_WORLD, oldworld );
	}
	else m_device->draw_debug( PT_LINE_STRIP, vcount, &v->x, color );
}

void Renderer::draw_box( const AABox& box, const Color& color )
{
	OBBox obbox;
	obbox.set_box( box );
	draw_box( obbox, color );
}

void Renderer::draw_box( const OBBox& box, const Color& color )
{
	float3 box_v[8];
	sx_get_points( box_v, box );
	const float3 v[24] = { 
		box_v[0], box_v[1], box_v[1], box_v[2], box_v[2], box_v[3], box_v[3], box_v[0],
		box_v[4], box_v[5], box_v[5], box_v[6], box_v[6], box_v[7], box_v[7], box_v[4],
		box_v[0], box_v[5], box_v[1], box_v[6], box_v[2], box_v[7], box_v[3], box_v[4] };
		m_device->draw_debug( PT_LINE_LIST, 24, &v->x, color );
}

void Renderer::draw_sphere( const Sphere& sphere, const dword flags, const Color& color, const uint stacks_ /*= 17*/, const uint slices_ /*= 20 */ )
{
	float3 v[2400];

	const uint stacks = ( stacks_ < 20 ? stacks_ : 20 );
	const uint slices = ( slices_ < 20 ? slices_ : 20 );
	const float theta = PI / stacks;
	const float phi = PI_MUL_2 / slices;
	const sint vcount = stacks * slices * 6;

	// compute sphere vertices
	for ( uint t=0; t<stacks; ++t )
	{
		for ( uint p=0; p<slices; ++p )
		{
			const float p1 = phi * float(p);
			const float t1 = theta * float(t);
			const float p2 = p1 + phi;
			const float t2 = t1 + theta;

			float p1s, p1c, t1s, t1c, p2s, p2c, t2s, t2c;
			sx_sin_cos_fast( p1, p1s, p1c );
			sx_sin_cos_fast( t1, t1s, t1c );
			sx_sin_cos_fast( p2, p2s, p2c );
			sx_sin_cos_fast( t2, t2s, t2c );

			sint i = ( t * slices + p ) * 6;

			v[i + 0].set( sphere.x + sphere.r * p1c * t1s, sphere.y + sphere.r * t1c, sphere.z + sphere.r * p1s * t1s );
			v[i + 1].set( sphere.x + sphere.r * p2c * t1s, sphere.y + sphere.r * t1c, sphere.z + sphere.r * p2s * t1s );
			v[i + 2].set( sphere.x + sphere.r * p1c * t2s, sphere.y + sphere.r * t2c, sphere.z + sphere.r * p1s * t2s );											
			v[i + 3].set( sphere.x + sphere.r * p1c * t2s, sphere.y + sphere.r * t2c, sphere.z + sphere.r * p1s * t2s );
			v[i + 4].set( sphere.x + sphere.r * p2c * t1s, sphere.y + sphere.r * t1c, sphere.z + sphere.r * p2s * t1s );
			v[i + 5].set( sphere.x + sphere.r * p2c * t2s, sphere.y + sphere.r * t2c, sphere.z + sphere.r * p2s * t2s );
		}
	}

	if ( flags & SX_D3D_WIREFRAME )
	{
		m_device->set_render_state( RS_FILL, false );
		m_device->draw_debug( PT_TRIANGLE_LIST, vcount, &v->x, color );
		m_device->set_render_state( RS_FILL, true );
	}
	else m_device->draw_debug( PT_TRIANGLE_LIST, vcount, &v->x, color );
}



//////////////////////////////////////////////////////////////////////////
//	helper function
//////////////////////////////////////////////////////////////////////////

//! create a renderer object
d3dRenderer* sx_create_renderer( const dword SX_D3D_ flags )
{
	Renderer* res = sx_new( Renderer );
	return res;
}