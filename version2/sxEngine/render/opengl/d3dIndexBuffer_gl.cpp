#include "d3dIndexBuffer_gl.h"


d3dIndexBuffer_gl::d3dIndexBuffer_gl( void )
:	m_ibo(0)
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
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, desc.size, data, ( desc.flag & SX_D3D_RESOURCE_DYNAMIC ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );

	sx_mem_free( m_data );
	if( desc.size && ( desc.flag & SX_D3D_RESOURCE_MANAGED ) )
	{
		m_data = sx_mem_alloc( desc.size );
		memcpy( m_data, data, desc.size );
	}
	m_desc = desc;
}

void* d3dIndexBuffer_gl::Lock( void )
{
	if ( m_desc.flag & SX_D3D_RESOURCE_MANAGED )
	{
		return m_data;
	}
	else
	{
		sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
		return glMapBufferARB( GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE_ARB );
	}
}

void d3dIndexBuffer_gl::Unlock( void )
{
	if ( m_desc.flag & SX_D3D_RESOURCE_MANAGED )
	{
		sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_desc.size, m_data, ( m_desc.flag & SX_D3D_RESOURCE_DYNAMIC ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
	}
	else
	{
		sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
		glUnmapBufferARB( GL_ELEMENT_ARRAY_BUFFER );
	}
}

SEGAN_INLINE void d3dIndexBuffer_gl::SetToDevice( void )
{
	sx_assert( m_device );
	m_device->SetIndexBuffer( this );
}
