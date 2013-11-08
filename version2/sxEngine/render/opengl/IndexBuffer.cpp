#include "Device.h"


d3dIndexBuffer::d3dIndexBuffer( void )
:	m_ibo(0)
,	m_data(0)
,	m_device(0)
{

}

d3dIndexBuffer::~d3dIndexBuffer( void )
{
	sx_mem_free( m_data );
	if ( m_ibo )
		glDeleteBuffers( 1, &m_ibo );
}

void d3dIndexBuffer::SetDesc( const uint flag, const uint sizeinbytes, void* data /*= null */ )
{
	m_size = sizeinbytes;
	m_flag = flag;

	if ( !m_ibo )
		glGenBuffers( 1, &m_ibo );

	sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeinbytes, data, ( flag & SX_D3D_RESOURCE_DYNAMIC ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );

	sx_mem_free( m_data );
	if( sizeinbytes && ( flag & SX_D3D_RESOURCE_MANAGED ) )
	{
		m_data = sx_mem_alloc( sizeinbytes );
		memcpy( m_data, data, sizeinbytes );
	}
	
}

void* d3dIndexBuffer::Lock( void )
{
	if ( m_flag & SX_D3D_RESOURCE_MANAGED )
	{
		return m_data;
	}
	else
	{
		sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
		return glMapBufferARB( GL_ELEMENT_ARRAY_BUFFER, GL_READ_WRITE_ARB );
	}
}

void d3dIndexBuffer::Unlock( void )
{
	if ( m_flag & SX_D3D_RESOURCE_MANAGED )
	{
		sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, m_size, m_data, ( m_flag & SX_D3D_RESOURCE_DYNAMIC ) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW );
	}
	else
	{
		sx_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_ibo );
		glUnmapBufferARB( GL_ELEMENT_ARRAY_BUFFER );
	}
}
