#include "directx/d3dDevice_dx.h"
#include "opengl/d3dDevice_gl.h"

//////////////////////////////////////////////////////////////////////////
//	vertex buffer
//////////////////////////////////////////////////////////////////////////
d3dVertexBuffer::d3dVertexBuffer( void )
{
	memset( &m_desc, 0, sizeof(m_desc) );
}

d3dVertexBuffer::~d3dVertexBuffer( void )
{

}

SEGAN_INLINE const d3dVertexBufferDesc* d3dVertexBuffer::GetDesc( void ) const
{
	return &m_desc;
}

//////////////////////////////////////////////////////////////////////////
//	index buffer
//////////////////////////////////////////////////////////////////////////
d3dIndexBuffer::d3dIndexBuffer( void )
{
	memset( &m_desc, 0, sizeof(m_desc) );
}

d3dIndexBuffer::~d3dIndexBuffer( void )
{

}

SEGAN_INLINE const d3dIndexBufferDesc* d3dIndexBuffer::GetDesc( void ) const
{
	return &m_desc;
}

//////////////////////////////////////////////////////////////////////////
//	texture
//////////////////////////////////////////////////////////////////////////
d3dTexture::d3dTexture( void )
{
	m_desc.type			= TT_2D;
	m_desc.format		= FMT_ARGB8;
	m_desc.addressMode	= AM_WRAP;
	m_desc.filter		= FILTER_TRILINEAR_ANISOTROPIC;
	m_desc.width		= 0;
	m_desc.height		= 0;
	m_desc.depth		= 0;
	m_desc.mipmaps		= 0;
	m_desc.lod			= 0;
	m_desc.anisotropy	= 16;
	m_desc.flag			= 0;
}

d3dTexture::~d3dTexture( void )
{

}

SEGAN_INLINE const d3dTextureDesc* d3dTexture::GetDesc( void ) const
{
	return &m_desc;
}

//////////////////////////////////////////////////////////////////////////
//	shader
//////////////////////////////////////////////////////////////////////////
d3dShader::d3dShader( void )
{
	memset( &m_desc, 0, sizeof(m_desc) );
}

d3dShader::~d3dShader( void )
{

}

SEGAN_INLINE const d3dShaderDesc* d3dShader::GetDesc( void ) const
{
	return &m_desc;
}

//////////////////////////////////////////////////////////////////////////
//	device 3d 
//////////////////////////////////////////////////////////////////////////
d3dDevice::d3dDevice( void )
{
	memset( &m_driverInfo,			0, sizeof(m_driverInfo) );
	memset( &m_driverCaps,			0, sizeof(m_driverCaps) );
	memset( &m_driverDisplayMode,	0, sizeof(m_driverDisplayMode) );

	memset( &m_creationData,		0, sizeof(m_creationData) );

	memset( &m_debugInfo,			0, sizeof(m_debugInfo) );

	memset( &m_defaultViewport,		0, sizeof(m_defaultViewport) );
	memset( &m_viewport,			0, sizeof(m_viewport) );
}

d3dDevice::~d3dDevice( void )
{

}


//////////////////////////////////////////////////////////////////////////
//	
//////////////////////////////////////////////////////////////////////////
SEGAN_ENG_API void sx_d3d_initialize( dword flag )
{

}

SEGAN_ENG_API void sx_d3d_finalize( void )
{

}

SEGAN_ENG_API d3dDevice* sx_d3d_create_device( dword flag )
{
	if ( flag & SX_D3D_CREATE_DX )
		return sx_new( d3dDevice_dx );
	else
		return sx_new( d3dDevice_gl );
}

SEGAN_ENG_API void sx_d3d_destroy_device( d3dDevice* &pdevice )
{
	sx_delete_and_null( pdevice );
}

