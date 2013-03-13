#include "draw_debugger.h"

#include "../sxEngine.h"





SEGAN_INLINE void sx_debug_draw_line( const float3& v1, const float3& v2, const dword color )
{
	const float3 v[2] = { v1, v2 };
	g_engine->m_device3D->DrawDebug( PT_LINE_LIST, 2, &v->x, color );
}

SEGAN_ENG_API void sx_debug_draw_grid( const uint size, const dword color )
{
	const uint lines = size < 50 ? size : 50;
	const uint xpoints = lines * 4 + 4;
	const uint zpoints = lines * 4 + xpoints;
	const float s = (float)lines;

	matrix mat;
	const matrix* matview = (matrix*)g_engine->m_device3D->GetMatrix( MM_VIEW );
	sx_inverse( mat, *matview );
	float dis = 5.0f + lines;
	float3 cen( sx_round( mat.m30 + dis * matview->m02 ), 0.0f, sx_round( mat.m32 + dis * matview->m22 ) );

	mat.Identity();
	g_engine->m_device3D->SetMatrix( MM_WORLD, mat );

	float3 v[404];

	v[0].Set( cen.x,		0, cen.z + s );
	v[1].Set( cen.x,		0, cen.z - s );
	v[2].Set( cen.x + s,	0, cen.z );
	v[3].Set( cen.x - s,	0, cen.z );

	//  place lines along x
	float x = 1.0f;
	for ( uint i=4; i<xpoints; i+=4 )
	{
		v[i+0].Set( cen.x + x, 0.0f, cen.z + s );
		v[i+1].Set( cen.x + x, 0.0f, cen.z - s );
		v[i+2].Set( cen.x - x, 0.0f, cen.z + s );
		v[i+3].Set( cen.x - x, 0.0f, cen.z - s );

		x += 1.0f;
	}

	//  place lines along z
	float z = 1.0f;
	for ( uint i=xpoints; i<zpoints; i+=4 )
	{
		v[i+0].Set( cen.x + s, 0.0f, cen.z + z );
		v[i+1].Set( cen.x - s, 0.0f, cen.z + z );
		v[i+2].Set( cen.x + s, 0.0f, cen.z - z );
		v[i+3].Set( cen.x - s, 0.0f, cen.z - z );

		z += 1.0f;
	}

	g_engine->m_device3D->DrawDebug( PT_LINE_LIST, zpoints, &v->x, color );


	g_engine->m_device3D->SetRenderState( RS_ZENABLE, false );
	sx_debug_draw_line( float3(   -s, 0.0f, 0.0f ), float3(    s, 0.0f, 0.0f ), 0xffff0000 );
	sx_debug_draw_line( float3( 0.0f, 0.0f,   -s ), float3( 0.0f, 0.0f,    s ), 0xff0000ff );
	g_engine->m_device3D->SetRenderState( RS_ZENABLE, true );
	sx_debug_draw_line( float3( 0.0f, 0.0f, 0.0f ), float3( 0.0f, 2.0f, 0.0f ), 0xff00ff00 );
}

SEGAN_ENG_API void sx_debug_draw_compass( void )
{
	const uint vcont = 18;
	float3 v[vcont];

	float	bs = 0.15f;
	float	bl = 0.85f;

	//  define x line
	v[0].Set(0, 0, 0);
	v[1].Set(1, 0, 0);
	v[2].Set(bl, 0, bs);
	v[3].Set(bl, 0,-bs);
	v[4] = v[1];

	//  define y line
	v[5].Set(0, 0, 0);
	v[6].Set(0, 1, 0);
	v[7].Set(0, bl, bs);
	v[8].Set(0, bl, -bs);
	v[9] = v[6];
	v[10].Set( bs, bl, 0);
	v[11].Set(-bs, bl, 0);
	v[12] = v[6];

	//  define z line
	v[13].Set(0, 0, 0);
	v[14].Set(0, 0, 1);
	v[15].Set( bs, 0, bl);
	v[16].Set(-bs, 0, bl);
	v[17] = v[14];


	matrix currproj = g_engine->m_device3D->GetMatrix( MM_PROJECTION );
	matrix currview = g_engine->m_device3D->GetMatrix( MM_VIEW );

	//  compute compass matrix
	matrix cmpsworld = sx_scale( 20, 20, 20 );
	g_engine->m_device3D->SetMatrix( MM_WORLD, cmpsworld );

	matrix cmpsview = currview;
	cmpsview.m30 =  ( 0.5f * g_engine->m_device3D->m_viewport.width		- 40.0f );
	cmpsview.m31 = -( 0.5f * g_engine->m_device3D->m_viewport.height	- 40.0f );
	g_engine->m_device3D->SetMatrix( MM_VIEW, cmpsview );

	matrix cmpsproj = sx_orthographic( (float)g_engine->m_device3D->m_viewport.width, (float)g_engine->m_device3D->m_viewport.height, -6000.0f, +6000.0f );
	g_engine->m_device3D->SetMatrix( MM_PROJECTION, cmpsproj );

	//  draw compass
	g_engine->m_device3D->SetRenderState( RS_ZENABLE, false );
	g_engine->m_device3D->DrawDebug(PT_LINE_STRIP, 4, v[0], 0xffff0000 );
	g_engine->m_device3D->DrawDebug(PT_LINE_STRIP, 7, v[5], 0xff00ff00 );
	g_engine->m_device3D->DrawDebug(PT_LINE_STRIP, 4, v[13], 0xff0000ff );
	g_engine->m_device3D->SetRenderState( RS_ZENABLE, true );

	g_engine->m_device3D->SetMatrix( MM_VIEW, currview );
	g_engine->m_device3D->SetMatrix( MM_PROJECTION, currproj );
}

SEGAN_INLINE void sx_debug_draw_circle( const float3& center, const float radius, const dword color )
{
	const uint	vcount	= 36 + 1;
	const float vphi	= DEG_TO_RAD * 10.0f;
	float3 v[vcount];
	for ( int i=0; i<vcount; ++i )
	{
		const float phi = vphi * float(i);
		v[i].x = center.x + radius * sx_cos_fast( phi );
		v[i].y = center.y + radius * sx_sin_fast( phi );
		v[i].z = center.z;
	}
	g_engine->m_device3D->DrawDebug( PT_LINE_STRIP, vcount, &v->x, color );
}

SEGAN_INLINE void sx_debug_draw_box( const AABox& box, const dword color )
{
	OBBox obbox;
	obbox.SetAABox( box );
	sx_debug_draw_box( obbox, color );
}

SEGAN_INLINE void sx_debug_draw_box( const OBBox& box, const dword color )
{
	const float3 v[24] = { 
		box.v[0], box.v[1], box.v[1], box.v[2], box.v[2], box.v[3], box.v[3], box.v[0],
		box.v[4], box.v[5], box.v[5], box.v[6], box.v[6], box.v[7], box.v[7], box.v[4],
		box.v[0], box.v[5], box.v[1], box.v[6], box.v[2], box.v[7], box.v[3], box.v[4] };
	g_engine->m_device3D->DrawDebug( PT_LINE_LIST, 24, &v->x, color );
}

SEGAN_ENG_API void sx_debug_draw_sphere( const Sphere& sphere, const dword color, const uint stacks_ /*= 17*/, const uint slices_ /*= 20 */ )
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

			v[i + 0].Set( sphere.x + sphere.r * p1c * t1s, sphere.y + sphere.r * t1c, sphere.z + sphere.r * p1s * t1s );
			v[i + 1].Set( sphere.x + sphere.r * p2c * t1s, sphere.y + sphere.r * t1c, sphere.z + sphere.r * p2s * t1s );
			v[i + 2].Set( sphere.x + sphere.r * p1c * t2s, sphere.y + sphere.r * t2c, sphere.z + sphere.r * p1s * t2s );											
			v[i + 3].Set( sphere.x + sphere.r * p1c * t2s, sphere.y + sphere.r * t2c, sphere.z + sphere.r * p1s * t2s );
			v[i + 4].Set( sphere.x + sphere.r * p2c * t1s, sphere.y + sphere.r * t1c, sphere.z + sphere.r * p2s * t1s );
			v[i + 5].Set( sphere.x + sphere.r * p2c * t2s, sphere.y + sphere.r * t2c, sphere.z + sphere.r * p2s * t2s );
		}
	}

	g_engine->m_device3D->DrawDebug( PT_TRIANGLE_LIST, vcount, &v->x, color );
}

SEGAN_ENG_API void sx_debug_draw_gui_element( const class Element* elem )
{
	if ( elem && elem->m_numVertices && elem->m_pos )
	{
		const float width  = (float)g_engine->m_device3D->m_viewport.width;
		const float height = (float)g_engine->m_device3D->m_viewport.height;
		matrix mat = sx_orthographic( width, height, -2000.0f, 2000.0f );
		g_engine->m_device3D->SetMatrix( MM_PROJECTION, mat );
		mat.Identity();
		g_engine->m_device3D->SetMatrix( MM_VIEW, mat );
		g_engine->m_device3D->SetMatrix( MM_WORLD, mat );
		g_engine->m_device3D->DrawDebug( PT_TRIANGLE_LIST, elem->m_numVertices, &elem->m_pos->x, 0xffff3333 );
	}
}



