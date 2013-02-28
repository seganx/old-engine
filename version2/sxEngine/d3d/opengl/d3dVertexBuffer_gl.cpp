#include "d3dVertexBuffer_gl.h"
#include "../../system/Log.h"

d3dVertexBuffer_gl::d3dVertexBuffer_gl( void )
:	d3dVertexBuffer()
,	m_vbo(0)
,	m_data(0)
,	m_device(0)
{

}

d3dVertexBuffer_gl::~d3dVertexBuffer_gl( void )
{
	sx_mem_free( m_data );
	if ( m_vbo )
		glDeleteBuffers( 1, &m_vbo );
}

void d3dVertexBuffer_gl::SetDesc( d3dVertexBufferDesc& desc, void* data /*= null */ )
{
	if ( !m_vbo )
		glGenBuffers( 1, &m_vbo );

	sx_glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	if ( desc.flag & SX_D3D_RESOURCE_DYNAMIC )
		glBufferData( GL_ARRAY_BUFFER, desc.size, data, GL_DYNAMIC_DRAW );
	else
		glBufferData( GL_ARRAY_BUFFER, desc.size, data, GL_STATIC_DRAW );
	sx_glBindBuffer( GL_ARRAY_BUFFER, 0 );

	sx_mem_free( m_data );
	if( desc.size && desc.flag & SX_D3D_RESOURCE_DYNAMIC )
	{
		m_data = sx_mem_alloc( desc.size );
		memcpy( m_data, data, desc.size );
	}
	m_desc = desc;
}

void* d3dVertexBuffer_gl::Lock( void )
{
	if ( m_desc.flag && SX_D3D_RESOURCE_DYNAMIC )
	{
		return m_data;
	}
	else
	{
		g_logger->Log( L"WARNING : try to lock static vertex buffer !" );
		sx_glBindBuffer( GL_ARRAY_BUFFER_ARB, m_vbo );
		return glMapBufferARB( GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB );
	}
}

void d3dVertexBuffer_gl::Unlock( void )
{
 	if ( m_desc.flag && SX_D3D_RESOURCE_DYNAMIC )
	{
		sx_glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
		if ( m_desc.flag & SX_D3D_RESOURCE_DYNAMIC )
			glBufferData( GL_ARRAY_BUFFER, m_desc.size, m_data, GL_DYNAMIC_DRAW );
		else
			glBufferData( GL_ARRAY_BUFFER, m_desc.size, m_data, GL_STATIC_DRAW );
		sx_glBindBuffer( GL_ARRAY_BUFFER, 0 );
	}
	else
	{
		g_logger->Log( L"WARNING : try to unlock static vertex buffer !" );
		glUnmapBufferARB( GL_ARRAY_BUFFER_ARB );
	}
}

SEGAN_INLINE void d3dVertexBuffer_gl::SetToDevice( uint streamIndex )
{
	sx_assert( m_device );
	m_device->SetVertexBuffer( this, streamIndex );
}
