#include "uiDevice.h"



//////////////////////////////////////////////////////////////////////////
//	STATE CONTROLLER
//////////////////////////////////////////////////////////////////////////
uiStateController::uiStateController( void ): m_index(0)
{
	sx_mem_set( &m_blender, 0, sizeof(m_blender) );
	
	// add default state
	uiState one;
	m_states.push_back( one );
}

uiStateController::~uiStateController( void )
{
	m_states.clear();
}

void uiStateController::clear( void )
{
	m_states.clear();
	m_states.push_back( m_curr );
	m_index = 0;
}

uint uiStateController::add( void )
{
	uiState newone = m_states[m_index];
	m_states.push_back( newone );
	return m_states.m_count - 1;
}

void uiStateController::remove( const sint index )
{
	if ( index >= 0 && index < m_states.m_count && m_states.m_count > 1 )
	{
		m_states.remove_index( index );
	}
}

void uiStateController::set_current( const sint index )
{
	if ( index != m_index )
	{
		const sint newindex = sx_clamp_i( index, 0, m_states.m_count - 1 );

		m_blender.vel = m_states[newindex].blender.x;
		m_blender.amp = m_states[newindex].blender.y;
		m_blender.a = m_blender.v = m_blender.w = 0;

		if ( m_blender.amp > 0.99f )
			m_last = m_states[m_index];
		else
			m_last = m_curr;

		m_index = newindex;
	}
}

SEGAN_INLINE bool uiStateController::is_blending( void ) const
{
	return ( sx_abs_f( 1.0f - m_blender.w ) > EPSILON ) ;
}

SEGAN_INLINE void uiStateController::update( const dword option, float elpsTime )
{
	if ( option & SX_GUI_BLENDSTATES )
	{
		//	update blending system
		{
			m_blender.a = ( 1.0f - m_blender.w ) * m_blender.vel;
			if ( sx_abs_f( m_blender.a ) > 0.00001f || m_blender.amp >= 1.0f )
			{
				m_blender.v += m_blender.a * ( elpsTime * 0.06f );
				m_blender.w = ( m_blender.w + m_blender.v ) * m_blender.amp;
			}
			else m_blender.w = 1.0f;
		}

		// interpolate between states
		const float w = m_blender.w;
		const uiState* state = &m_states[m_index];
		sx_lerp( m_curr.align,		m_last.align,		state->align,		w );
		sx_lerp( m_curr.center,		m_last.center,		state->center,		w );
		sx_lerp( m_curr.position,	m_last.position,	state->position,	w );
		sx_lerp( m_curr.rotation,	m_last.rotation,	state->rotation,	w );
		sx_lerp( m_curr.scale,		m_last.scale,		state->scale,		w );
		sx_lerp( m_curr.color,		m_last.color,		state->color,		w );
	}
	else
	{
		m_curr = m_states[m_index];
		m_last = m_curr;
	}
}




//////////////////////////////////////////////////////////////////////////
//	ELEMENT
//////////////////////////////////////////////////////////////////////////
uiElement::uiElement( void )
: m_type(ET_QUADS)
, m_vcount(0)
, m_pos(null)
, m_uv(null)
, m_color(null)
, m_posfinal(null)
{

}

uiElement::~uiElement( void )
{
	if ( m_vcount )
		clear_vertices();
}

void uiElement::create_vertices( const uint count )
{
	if ( count )
	{
		if ( count > m_vcount )
		{
			//	delete last buffer
			sx_mem_free_and_null( m_pos );

			const uint size_pos			= count * sizeof(float3);
			const uint size_uv			= count * sizeof(float2);
			const uint size_color		= count * sizeof(Color2);
			const uint size_posfinal	= count * sizeof(float3);

			byte* buffer = (byte*)sx_mem_alloc( size_pos + size_uv + size_color + size_posfinal );

			m_pos		= (float3*)( buffer );
			m_uv		= (float2*)( buffer + ( size_pos ) );
			m_color		= (Color2*)( buffer + ( size_pos + size_uv ) );
			m_posfinal	= (float3*)( buffer + ( size_pos + size_uv + size_color ) );
		}
		m_vcount = count;
	}
	else clear_vertices();
}

void uiElement::clear_vertices( void )
{
	if ( m_vcount )
	{
		sx_mem_free_and_null( m_pos );
		m_uv		= null;
		m_color		= null;
		m_posfinal	= null;
		m_vcount = 0;
	}
}

SEGAN_INLINE bool sx_intersect( const Ray* ray, const uiElement* element, OUT float2* uv /*= null */ )
{
	bool res = false;
	if ( element->m_vcount )
	{
		switch ( element->m_type )
		{
		case ET_QUADS:
			{
				float2 outuv;
				for ( uint i=0; i<element->m_vcount; i += 4 )
				{
					//	test first triangle
					res = sx_intersect( *ray, element->m_pos[i], element->m_pos[i+1], element->m_pos[i+2], outuv, true );
					
					//	test second triangle
					if ( !res )
					{
						res = sx_intersect( *ray, element->m_pos[i], element->m_pos[i+2], element->m_pos[i+3], outuv, true );
					}

					//	fill out the result and break the loop
					if ( res )
					{
						if ( uv )
							*uv = outuv;
						res = true;
						break;
					}
				}
			}
			break;

		case ET_TRIANGLES:
			{
				float2 outuv;
				for ( uint i=0; i<element->m_vcount; i += 3 )
				{
					//	test the triangle
					if ( sx_intersect( *ray, element->m_pos[i], element->m_pos[i+1], element->m_pos[i+2], outuv, true ) )
					{
						//	fill out the result and break the loop
						if ( uv )
							*uv = outuv;
						res = true;
						break;
					}
				}
			}
			break;
		}
	}
	return res;
}

