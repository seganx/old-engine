#include "uiDevice.h"


uiControl::uiControl( void )
: m_type(UT_NONE)
, m_id( sx_id_generate() )
, m_option(SX_GUI_VISIBLE | SX_GUI_ENABLE)
, m_size(0,0)
, m_position_offset(0,0,0)
, m_rotation_offset(0,0,0)
, m_scale_offset(1,1,1)
, m_parent(null)
, m_mouseState(MS_NORMAL)
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
		m_parent->m_child.remove( this );

	if ( parent )
		parent->m_child.push_back( this );

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
	//	check the option for 3D space
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

void uiControl::ProcessInput( uiInputReport* inputReport )
{
	if ( sx_set_has( m_option, _SX_GUI_NOT_VISIBLE_ ) || sx_set_has( m_option, _SX_GUI_NOT_ENABLE_ ) ) return;

	bool captured = false;

	// check the children for clipped space
	if ( !inputReport->mouseLocked )
	{
		Ray ray = sx_transform( inputReport->ray, m_matrix );
		captured = ( IntersectRay( &ray ) >= 0 );
	
		if ( m_option & SX_GUI_CLIPCHILDS )
		{
			//	verify that mouse cursor is on clip space
			if ( captured )
			{
				for ( sint i = m_child.m_count - 1; i >= 0; --i )
				{
					m_child[i]->ProcessInput( inputReport );
				}
			}
			else
			{
				//	prevent children from intersection and handle input report
				inputReport->mouseLocked = m_id;
				for ( sint i = m_child.m_count - 1; i >= 0; --i )
				{
					m_child[i]->ProcessInput( inputReport );
				}
				inputReport->mouseLocked = 0;
			}
		}
	}
	else
	{
		// traverse through children
		for ( sint i = m_child.m_count - 1; i >= 0; --i )
		{
			m_child[i]->ProcessInput( inputReport );
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//	perform mouse actions for current control
	if ( !inputReport->mouseLocked && captured )
	{
		switch ( m_mouseState )
		{
		case MS_NORMAL:
			if ( inputReport->mouseLeft == MS_NORMAL )	//	verify that mouse entered on control
			{
				m_mouseState = MS_ENTERED;
				m_onEnter( this );
			}
			break;

		case MS_ENTERED:
			if ( inputReport->mouseLeft == MS_DOWN )
			{
				m_mouseState = MS_DOWN;
			}
			break;

		case MS_DOWN:
			if ( inputReport->mouseLeft == MS_UP )	//	verify that mouse clicked on control
			{
				m_mouseState = MS_UP;
				m_onClick( this );
			}
			break;

		case MS_UP:
			{

			}
			break;
		}

		//	call move callback
		m_onMove( this );

		// lock the mouse report
		inputReport->mouseLocked = m_id;
	}

	//	when do we call Exit callback
	if ( !captured && m_mouseState != MS_NORMAL && inputReport->mouseLeft == MS_NORMAL )
	{
		m_onExit( this );
		m_mouseState = MS_NORMAL;
	}

	//////////////////////////////////////////////////////////////////////////
	//	handle keyboard
	if ( !inputReport->keyboardLocked )
	{

	}

}

void uiControl::GetElements( Array<uiElement*> * elementArray, const bool traversChilds /*= true*/ )
{
	// extract current elements
	for ( uint i=0; i<SX_GUI_MAX_ELEMENT; ++i )
	{
		uiElement* element = (uiElement*)&m_element[i];
		if ( element->m_numVertices )
		{
			elementArray->push_back( element );
		}
		else break;
	}

	// extract elements of children
	for ( sint i=0; i<m_child.m_count; ++i )
	{
		m_child[i]->GetElements( elementArray, traversChilds );
	}
}

sint uiControl::IntersectRay( const Ray* ray, const sint element /*= -1*/, OUT float2* uv /*= null */ )
{
	sint res = -1;

	if ( element < 0 )
	{
		for ( sint i = SX_GUI_MAX_ELEMENT-1; i >= 0; --i )
		{
			if ( m_element[i].m_numVertices && sx_intersect( ray, &m_element[i], uv ) )
			{
				res = i;
				break;
			}
		}
	}
	else
	{
		sx_assert( element < SX_GUI_MAX_ELEMENT );
		if ( sx_intersect( ray, &m_element[element], uv ) )
		{
			res = element;
		}
	}
	
	return res;
}
