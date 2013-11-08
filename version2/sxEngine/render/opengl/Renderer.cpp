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
		m_device->Initialize( hwnd );
	}
	m_device->SetSize( width, height, flags );
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

}

void Renderer::draw( float elpstime, uint flag )
{
	m_device->BeginScene();	
	m_device->ClearScreen( 0xffccbbaa );

#if 0
	float vert[6] = {0, 0, 0, 1, 1, 1};
	m_device->DrawDebug( PT_LINE_LIST, 2, vert, Color(0.0f, 0.0f, 1.0f, 1.0f) );
#endif

	m_device->EndScene();
	m_device->Present();
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