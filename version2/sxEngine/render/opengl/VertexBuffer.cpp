#include "Device.h"


VertexBuffer::VertexBuffer( void )
:	m_vbo(0)
,	m_data(0)
,	m_device(0)
{

}

VertexBuffer::~VertexBuffer( void )
{
	sx_mem_free( m_data );
	if ( m_vbo )
		glDeleteBuffers( 1, &m_vbo );
}

void VertexBuffer::set_desc( const uint flag, const uint sizeinbytes, void* data /*= null */ )
{
	m_size = sizeinbytes;
	m_flag = flag;

	if ( !m_vbo )
		glGenBuffers( 1, &m_vbo );

	sx_glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeinbytes, data, ( flag & SX_D3D_RESOURCE_DYNAMIC ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );

	sx_mem_free( m_data );
	if( sizeinbytes && ( flag & SX_D3D_RESOURCE_MANAGED ) )
	{
		m_data = sx_mem_alloc( sizeinbytes );
		memcpy( m_data, data, sizeinbytes );
	}
}

void* VertexBuffer::lock( void )
{
	if ( m_flag & SX_D3D_RESOURCE_MANAGED )
	{
		return m_data;
	}
	else
	{
		sx_glBindBuffer( GL_ARRAY_BUFFER_ARB, m_vbo );
		return glMapBuffer( GL_ARRAY_BUFFER_ARB, GL_READ_WRITE_ARB );
	}
}

void VertexBuffer::unlock( void )
{
 	if ( m_flag & SX_D3D_RESOURCE_MANAGED )
	{
		sx_glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
		glBufferData( GL_ARRAY_BUFFER, m_size, m_data, ( m_flag & SX_D3D_RESOURCE_DYNAMIC ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
	}
	else
	{
		sx_glBindBuffer( GL_ARRAY_BUFFER_ARB, m_vbo );
		glUnmapBufferARB( GL_ARRAY_BUFFER_ARB );
	}
}
