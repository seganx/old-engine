#include "d3dIndexBuffer_gl.h"
#include "../../system/Log.h"


d3dIndexBuffer_gl::d3dIndexBuffer_gl( void )
:	d3dIndexBuffer()
,	m_ibo(0)
,	m_data(0)
,	m_device(0)
{

}

d3dIndexBuffer_gl::~d3dIndexBuffer_gl( void )
{
	sx_mem_free( m_data );
	if ( m_ibo )
		glDeleteBuffers( 1, &m_ibo );
}

void d3dIndexBuffer_gl::SetDesc( d3dIndexBufferDesc& desc, void* data /*= null */ )
{
	if ( !m_ibo )
		glGenBuffers( 1, &m_ibo );

	sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	if ( desc.flag & SX_D3D_RESOURCE_DYNAMIC )
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, desc.size, data, GL_DYNAMIC_DRAW );
	else
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, desc.size, data, GL_STATIC_DRAW );
	sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	sx_mem_free( m_data );
	if( desc.size && desc.flag & SX_D3D_RESOURCE_DYNAMIC )
	{
		m_data = sx_mem_alloc( desc.size );
		memcpy( m_data, data, desc.size );
	}
	m_desc = desc;
}

void* d3dIndexBuffer_gl::Lock( void )
{
	if ( m_desc.flag && SX_D3D_RESOURCE_DYNAMIC )
	{
		return m_data;
	}
	else
	{
		g_logger->Log( L"WARNING : try to lock static index buffer !" );
		sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
		return glMapBufferARB( GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE_ARB );
	}
}

void d3dIndexBuffer_gl::Unlock( void )
{
	if ( m_desc.flag && SX_D3D_RESOURCE_DYNAMIC )
	{
		sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
		if ( m_desc.flag & SX_D3D_RESOURCE_DYNAMIC )
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_desc.size, m_data, GL_DYNAMIC_DRAW );
		else
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_desc.size, m_data, GL_STATIC_DRAW );
		sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	}
	else
	{
		g_logger->Log( L"WARNING : try to unlock static index buffer !" );
		glUnmapBufferARB( GL_ELEMENT_ARRAY_BUFFER );
	}
}

SEGAN_INLINE void d3dIndexBuffer_gl::SetToDevice( void )
{
	sx_assert( m_device );
	m_device->SetIndexBuffer( this );
}
