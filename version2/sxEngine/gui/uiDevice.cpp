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
//	CONTROL
//////////////////////////////////////////////////////////////////////////
uiControl::uiControl( void )
: m_type(UT_NONE)
, m_option(SX_GUI_VISIBLE | SX_GUI_ENABLE)
, m_size(0,0)
, m_position_offset(0,0,0)
, m_rotation_offset(0,0,0)
, m_scale_offset(1,1,1)
, m_parent(null)
{

}

uiControl::~uiControl( void )
{
	//	detach from parent
	if ( m_parent )	SetParent( null );

	//	delete all children
	for ( sint i=0; i<m_child.m_count; ++i )
	{
		uiControl* child = m_child[i];
		child->m_parent = null;
		sx_delete( child );
	}
}

void uiControl::SetParent( uiControl* parent )
{
	if ( parent == m_parent ) return;

	if ( m_parent )
		m_parent->m_child.Remove( this );
	
	if ( parent )
		parent->m_child.PushBack( this );
	
	m_parent = parent;
}

void uiControl::SetSize( const float width, const float height )
{
	m_size.Set( width, height );
}

void uiControl::Update( float elpsTime, const matrix& viewInverse, const matrix& viewProjection, const uint vpwidth, const uint vpheight )
{
	if ( m_option & SX_GUI_ENABLE )
		sx_set_rem( m_option, _SX_GUI_NOT_ENABLE_ );
	else
		sx_set_add( m_option, _SX_GUI_NOT_ENABLE_ );

	if ( m_option & SX_GUI_VISIBLE )
		sx_set_rem( m_option, _SX_GUI_NOT_VISIBLE_ );
	else
	{
		sx_set_add( m_option, _SX_GUI_NOT_VISIBLE_ );
		return;
	}

	if ( m_option & SX_GUI_3DSPACE )
		sx_set_add( m_option, _SX_GUI_IN_3DSPACE_ );
	else
		sx_set_rem( m_option, _SX_GUI_IN_3DSPACE_ );

	//	update states
	m_state.Update( m_option, elpsTime );

	//	check color visibility
	uiState* state = m_state.GetBlended();
	if ( state->color.w < SX_GUI_MINIMUM_ALPHA )
	{
		sx_set_add( m_option, _SX_GUI_NOT_VISIBLE_ );
		return;
	}

	//	check scale visibility
	// check the option for 3D space
	const float sclvalue = ( m_option & _SX_GUI_IN_3DSPACE_ ) ? 0.01f : 1.0f;
	state->scale.x *= m_scale_offset.x * sclvalue;
	state->scale.y *= m_scale_offset.y * sclvalue;
	state->scale.z *= m_scale_offset.z * sclvalue;
	if (( state->scale.x < SX_GUI_MINIMUM_SCALE ) ||
		( state->scale.y < SX_GUI_MINIMUM_SCALE ) ||
		( state->scale.z < SX_GUI_MINIMUM_SCALE ) )
	{
		sx_set_add( m_option, _SX_GUI_NOT_VISIBLE_ );
		return;
	}

	//  start building matrix
	{
		matrix mat, matTmp;
		mat.Identity();

		//  apply center
		mat.SetTranslation( - state->center.x * m_size.x, - state->center.y * m_size.y, state->center.z );

		//  apply scale to matrix
		//mat.SetScale( state->scale.x, state->scale.y, state->scale.z );

		//  apply rotation
		sx_set_rotation_xyz( matTmp, 
			state->rotation.x + m_rotation_offset.x,
			state->rotation.y + m_rotation_offset.y,
			state->rotation.z + m_rotation_offset.z );

		m_matrix = sx_mul( mat, matTmp );
	}

	//  apply aligned position
	if ( ( m_option & SX_GUI_BILLBOARD )/* && !m_parent*/ )
	{
		if ( m_option & _SX_GUI_IN_3DSPACE_ )
		{
			m_matrix = sx_mul( m_matrix, viewInverse );
		}
		else
		{		
			float3 pos = state->position + m_position_offset;
			float3 projpos = sx_project_to_screen( pos, viewProjection, 0, 0, vpwidth, vpheight );

			m_matrix.m30 = projpos.x - ( vpwidth * 0.5f );
			m_matrix.m31 = ( vpheight * 0.5f ) - projpos.y;
			m_matrix.m32 = pos.z;
		}
	}
	else
	{
		if ( m_parent )
		{
			m_matrix.m30 += sclvalue * ( state->position.x + m_position_offset.x + m_parent->m_size.x * state->align.x );
			m_matrix.m31 += sclvalue * ( state->position.y + m_position_offset.y + m_parent->m_size.y * state->align.y );
		}
		else
		{
			m_matrix.m30 += sclvalue * ( state->position.x + m_position_offset.x + vpwidth  * state->align.x );
			m_matrix.m31 += sclvalue * ( state->position.y + m_position_offset.y + vpheight * state->align.y );
		}
		
		m_matrix.m32 += sclvalue * ( state->position.z + m_position_offset.z );
	}

	//	apply parent properties
	if ( m_parent )
	{
		m_matrix = sx_mul( m_matrix, m_parent->m_matrix );

		if ( m_parent->m_option & _SX_GUI_NOT_ENABLE_ )
			sx_set_add( m_option, _SX_GUI_NOT_ENABLE_ );

		//  check control in 3d space
		if ( m_parent->m_option & _SX_GUI_IN_3DSPACE_ )
			sx_set_add( m_option, _SX_GUI_IN_3DSPACE_ );
		else
			sx_set_rem( m_option, _SX_GUI_IN_3DSPACE_ );
	}

	//	transform vertices from local space to the world space
	for ( uint i=0; i<SX_GUI_MAX_ELEMENT && m_element[i].m_numVertices; ++i )
	{
		for ( uint j=0; j<m_element[i].m_numVertices; ++j )
		{
			sx_transform_point( m_element[i].m_posfinal[j], m_element[i].m_pos[j], m_matrix );
		}
	}

	// after that all let's update the children
	for ( sint i = (sint)m_child.m_count - 1; i >= 0; --i )
	{
		uiControl* child = m_child[i];

		if ( child->m_option & _SX_GUI_IN_3DSPACE_ )
		{
			child->m_position_offset.z += SX_GUI_Z_BIAS;
			child->Update( elpsTime, viewInverse, viewProjection, vpwidth, vpheight );
			child->m_position_offset.z -= SX_GUI_Z_BIAS;
		}
		else
		{
			child->Update( elpsTime, viewInverse, viewProjection, vpwidth, vpheight );
		}
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

void uiDevice::GetElements( const uiControl* control, Array<uiElement*> *elementArray )
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

