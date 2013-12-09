#include "uiDevice.h"


uiPanel::uiPanel( void ): uiControl()
{
	memcpy( m_type, "panel", 6 );

	m_context.create_vertices( 4 );
	
	m_context.m_uv[0].set( 0.0f, 0.0f );
	m_context.m_uv[1].set( 0.0f, 1.0f );
	m_context.m_uv[2].set( 1.0f, 1.0f );
	m_context.m_uv[3].set( 1.0f, 0.0f );

	for ( sint i=0; i<4; ++i )
	{
		m_context.m_color[i].c0 = 0xffffffff;
		m_context.m_color[i].c1 = 0xffffffff;
	}
}

uiPanel::~uiPanel( void )
{

}

void uiPanel::set_size( const float width, const float height )
{
	m_size.set( width, height );
	paint();
}

void uiPanel::paint( void )
{
	const float w = m_size.x * 0.5f;
	const float h = m_size.y * 0.5f;

	m_context.m_pos[0].set( -w,  h, 0.0f );
	m_context.m_pos[1].set( -w, -h, 0.0f );
	m_context.m_pos[2].set(  w, -h, 0.0f );
	m_context.m_pos[3].set(  w,  h, 0.0f );
}

