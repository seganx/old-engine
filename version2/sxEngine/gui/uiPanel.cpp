#include "uiDevice.h"


uiPanel::uiPanel( void ): uiControl()
{
	m_type = UT_PANEL;

	m_element[0].CreateVertices( 4 );
	
	m_element[0].m_uv[0].Set( 0.0f, 0.0f );
	m_element[0].m_uv[1].Set( 0.0f, 1.0f );
	m_element[0].m_uv[2].Set( 1.0f, 1.0f );
	m_element[0].m_uv[3].Set( 1.0f, 0.0f );

	for ( sint i=0; i<4; ++i )
	{
		m_element[0].m_color[i].c0 = 0xffffffff;
		m_element[0].m_color[i].c1 = 0xffffffff;
	}
}

uiPanel::~uiPanel( void )
{

}

void uiPanel::SetSize( const float width, const float height )
{
	m_size.Set( width, height );

	const float w = width * 0.5f;
	const float h = height * 0.5f;

	m_element[0].m_pos[0].Set( -w,  h, 0.0f );
	m_element[0].m_pos[1].Set( -w, -h, 0.0f );
	m_element[0].m_pos[2].Set(  w, -h, 0.0f );
	m_element[0].m_pos[3].Set(  w,  h, 0.0f );

}

