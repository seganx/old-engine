#include "uiDevice.h"



//////////////////////////////////////////////////////////////////////////
//	STATE CONTROLLER
//////////////////////////////////////////////////////////////////////////
uiStateController::uiStateController( void ): m_index(0)
{
	sx_mem_set( &m_blender, 0, sizeof(m_blender) );
	
	// add default state
	uiState one;
	m_states.PushBack( one );
}

uiStateController::~uiStateController( void )
{
	m_states.Clear();
}

void uiStateController::Clear( void )
{
	m_states.Clear();
	m_states.PushBack( m_curr );
	m_index = 0;
}

uint uiStateController::Add( void )
{
	uiState newone = m_states[m_index];
	m_states.PushBack( newone );
	return m_states.m_count - 1;
}

void uiStateController::Remove( const sint index )
{
	if ( index >= 0 && index < m_states.m_count && m_states.m_count > 1 )
	{
		m_states.RemoveByIndex( index );
	}
}

void uiStateController::SetIndex( const sint index )
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

SEGAN_INLINE uiState* uiStateController::GetCurrent( void )
{
	return &m_states[m_index];
}

SEGAN_INLINE uiState* uiStateController::GetByIndex( const sint index )
{
	if ( index >=0 && index < m_states.m_count )
		return &m_states[index];
	else
		return null;
}

SEGAN_INLINE uiState* uiStateController::GetBlended( void )
{
	return &m_curr;
}

SEGAN_INLINE bool uiStateController::IsBlending( void ) const
{
	return ( sx_abs_f( 1.0f - m_blender.w ) > EPSILON ) ;
}

SEGAN_INLINE void uiStateController::Update( const dword option, float elpsTime )
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
, m_numVertices(0)
, m_pos(null)
, m_uv(null)
, m_color(null)
, m_posfinal(null)
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
		if ( count > m_numVertices )
		{
			//	delete last buffer
			sx_mem_free_and_null( m_pos );

			const uint size_pos			= count * sizeof(float3);
			const uint size_uv			= count * sizeof(float2);
			const uint size_color		= count * sizeof(Color);
			const uint size_posfinal	= count * sizeof(float3);

			byte* buffer = (byte*)sx_mem_alloc( size_pos + size_uv + size_color + size_posfinal );

			m_pos		= (float3*)( buffer );
			m_uv		= (float2*)( buffer + ( size_pos ) );
			m_color		= (Color*) ( buffer + ( size_pos + size_uv ) );
			m_posfinal	= (float3*)( buffer + ( size_pos + size_uv + size_color ) );
		}
		m_numVertices = count;
	}
	else ClearVertives();
}

void uiElement::ClearVertives( void )
{
	if ( m_numVertices )
	{
		sx_mem_free_and_null( m_pos );
		m_uv		= null;
		m_color		= null;
		m_posfinal	= null;
		m_numVertices = 0;
	}
}


//////////////////////////////////////////////////////////////////////////
//	DEVICE
//////////////////////////////////////////////////////////////////////////
uiDevice::uiDevice( void )
:	m_batches(128)
{

}

uiDevice::~uiDevice( void )
{

}

uiControl* uiDevice::CreateContorl( const uiType type )
{
#if 0
	switch ( type )
	{
	case GUI_PANEL:			return sx_new( uiPanel );
//	case GUI_BUTTON:		return sx_new( uiButton );
//	case GUI_CHECKBOX:		return sx_new( uiCheckBox );
//	case GUI_TRACKBAR:		return sx_new( uiScroll );
//	case GUI_PROGRESSBAR:	return sx_new( uiProgress );
//	case GUI_LABEL:			return sx_new( uiLabel );
//	case GUI_EDITBOX:		return sx_new( uiEditBox );
//	case GUI_PANELEX:		return sx_new( uiPanelEx );
//	case GUI_LISTBOX:		return sx_new( uiListBox );
	}
#endif
	return null;
}

void uiDevice::Copy( uiElement* dest, uint& index, const uiElement* src )
{
	switch ( src->m_type )
	{
	case ET_NONE:
		{
			sx_assert( L"uiDevice::Copy : Element type is not defined !" );
		}
		break;

	case ET_LINES:
		{
			sx_assert( L"uiDevice::Copy : Copy Line elements is not implemented yet !" );
		}
		break;

	case ET_TRIANGLES:
		{
			const uint srcvertcount = src->m_numVertices;
			if ( srcvertcount )
			{
				sx_mem_copy( &dest->m_posfinal[index],	src->m_posfinal,	srcvertcount * sizeof(float3) );
				sx_mem_copy( &dest->m_uv[index],		src->m_uv,			srcvertcount * sizeof(float2) );
				sx_mem_copy( &dest->m_color[index],		src->m_color,		srcvertcount * sizeof(float4) );
				index += srcvertcount;
			}
		}
		break;

	case ET_QUADS:
		{
			if ( src->m_numVertices )
			{
				sx_convert_quat_triangle( &dest->m_posfinal[index],		src->m_posfinal		);
				sx_convert_quat_triangle( &dest->m_uv[index],			src->m_uv			);
				sx_convert_quat_triangle( &dest->m_color[index],		src->m_color		);
				index += src->m_numVertices + 2;
			}
		}
		break;
	}
}

void uiDevice::BeginBatch( const uint count )
{
	if ( count )
		m_batches.SetSize( count );
}

SEGAN_INLINE bool uiDevice::AddBatch( const uiElement* elem )
{
	//	verify that all these have the same image id
	if ( m_batches.m_count && m_batches[0]->m_image != elem->m_image ) return false;

	//	verify element type
	switch ( elem->m_type )
	{
	case ET_NONE:
		sx_assert( L"uiDevice::AddBatch : Element type is not defined !" );
		return false;

	case ET_LINES:
		sx_assert( L"uiDevice::AddBatch : Line elements is not implemented yet !" );
		return false;
	}

	m_batches.PushBack( (uiElement*)elem );
	return true;
}

SEGAN_INLINE uint uiDevice::GetBatchVertexCount( void )
{
	// compute number of vertices
	uint sumVertices = 0;
	for ( sint i=0; i<m_batches.m_count; ++i )
	{
		uiElement* elem = m_batches.m_item[i];
		if ( elem->m_type == ET_QUADS )
			sumVertices += elem->m_numVertices + 2;
	}
	return sumVertices;
}

void uiDevice::EndBatch( uiElement* dest )
{
	//	get number of vertices needed to batch them
	uint sumVertices = GetBatchVertexCount();

	//	prepare destination element
	uint destVertices = dest->m_numVertices;
	dest->CreateVertices( destVertices + sumVertices );

	//	copy batches to dest element
	uint index = destVertices;
	for ( sint i=0; i<m_batches.m_count; ++i )
	{
		Copy( dest, index, m_batches.m_item[i] );
	}
	
	//	release array
	m_batches.Clear();
}

void uiDevice::GetElements( const uiControl* control, Array<uiElement*> * elementArray )
{
	// extract current elements
	for ( uint i=0; i<SX_GUI_MAX_ELEMENT; ++i )
	{
		uiElement* element = (uiElement*)&control->m_element[i];
		if ( element->m_numVertices )
		{
			elementArray->PushBack( element );
		}
		else break;
	}

	// extract elements of children
	for ( sint i=0; i<control->m_child.m_count; ++i )
	{
		GetElements( control->m_child[i], elementArray );
	}
}
