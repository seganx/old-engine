#include "uiDevice.h"


uiElement::uiElement( void )
: m_numVertices(0)
, m_pos(null)
, m_uv(null)
, m_color(null)
{

}

uiElement::~uiElement( void )
{
	if ( m_numVertices )
		ClearVertives();
}

void uiElement::CreateVertices( const uint count )
{
	if ( count )
	{
		sx_mem_realloc( m_pos,		count * sizeof(float3) );
		sx_mem_realloc( m_uv,		count * sizeof(float2) );
		sx_mem_realloc( m_color,	count * sizeof(float4) );
		m_numVertices = count;
	}
	else ClearVertives();
}

void uiElement::ClearVertives( void )
{
	if ( m_numVertices )
	{
		sx_mem_free_and_null( m_pos );
		sx_mem_free_and_null( m_uv );
		sx_mem_free_and_null( m_color );
		m_numVertices = 0;
	}
}
