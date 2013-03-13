#include "Element.h"


Element::Element( void )
: m_numVertices(0)
, m_pos(null)
, m_uv(null)
, m_color(null)
{

}

Element::~Element( void )
{
	if ( m_numVertices )
		ClearVertives();
}

void Element::CreateVertices( const uint count )
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

void Element::ClearVertives( void )
{
	if ( m_numVertices )
	{
		sx_mem_free_and_null( m_pos );
		sx_mem_free_and_null( m_uv );
		sx_mem_free_and_null( m_color );
		m_numVertices = 0;
	}
}



//////////////////////////////////////////////////////////////////////////
//	batch system
//////////////////////////////////////////////////////////////////////////
static uint			s_batch_count = 0;
static Element*		s_elements[2048];

SEGAN_INLINE void sx_element_begin_batch( void )
{
	s_batch_count = 0;
}

SEGAN_INLINE bool sx_element_add_batch( const Element* elem )
{
	bool res = false;
	if ( s_batch_count < 2048 )
	{
		s_elements[s_batch_count++] = (Element*)elem;
		res = true;
	}
	return res;
}

SEGAN_INLINE void sx_element_end_batch( Element* dest )
{
	// compute number of vertices
	uint sumVertices = 0;
	for ( uint i=0; i<s_batch_count; ++i )
		sumVertices += s_elements[i]->m_numVertices;
	
	//	prepare destination element
	uint destVertices = dest->m_numVertices;
	dest->CreateVertices( destVertices + sumVertices );

	//	copy batches to dest element
	uint index = destVertices;
	for ( uint i=0; i<s_batch_count; ++i )
	{
		const Element* src = s_elements[i];
		const uint srcvertcount = src->m_numVertices;
		sx_mem_copy( &dest->m_pos[index],	src->m_pos,		srcvertcount * sizeof(float3) );
		sx_mem_copy( &dest->m_uv[index],	src->m_uv,		srcvertcount * sizeof(float2) );
		sx_mem_copy( &dest->m_color[index], src->m_color,	srcvertcount * sizeof(float4) );
		index += srcvertcount;
	}
}
