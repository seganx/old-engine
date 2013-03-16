#include "uiDevice.h"


//////////////////////////////////////////////////////////////////////////
//	ELEMENT
//////////////////////////////////////////////////////////////////////////
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
		if ( count > m_numVertices )
		{
			sx_mem_realloc( m_pos,		count * sizeof(float3) );
			sx_mem_realloc( m_uv,		count * sizeof(float2) );
			sx_mem_realloc( m_color,	count * sizeof(float4) );
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
		sx_mem_free_and_null( m_uv );
		sx_mem_free_and_null( m_color );
		m_numVertices = 0;
	}
}


//////////////////////////////////////////////////////////////////////////
//	CONTROL
//////////////////////////////////////////////////////////////////////////
uiControl::uiControl( void )
: m_type(GUI_NONE)
, m_option(0)
, m_size(0,0)
, m_position(0,0,0)
, m_position_offset(0,0,0)
, m_rotation(0,0,0)
, m_rotation_offset(0,0,0)
, m_scale(1,1,1)
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

void uiControl::Update( float elpsTime )
{

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

uiControl* uiDevice::CreateContorl( const GUIType type )
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

void uiDevice::Copy( uiElement* dest, uint& index, const uiElement* src, const GUIBatchMode mode )
{
	switch ( mode )
	{
	case GBM_SIMPLE:
		{
			sx_assert( L"uiControl::Batch mode can't be simple !" );
		}
		break;

	case GBM_TRIANGLES:
		{
			if ( src->m_numVertices )
			{
				sx_convert_quat_triangle( &dest->m_pos[index],		src->m_pos	 );
				sx_convert_quat_triangle( &dest->m_uv[index],		src->m_uv	 );
				sx_convert_quat_triangle( &dest->m_color[index],	src->m_color );
				index += src->m_numVertices + 2;
			}
		}
		break;

	case GBM_QUADS_CCW:
		{
			const uint srcvertcount = src->m_numVertices;
			if ( srcvertcount )
			{
				sx_mem_copy( &dest->m_pos[index],	src->m_pos,		srcvertcount * sizeof(float3) );
				sx_mem_copy( &dest->m_uv[index],	src->m_uv,		srcvertcount * sizeof(float2) );
				sx_mem_copy( &dest->m_color[index], src->m_color,	srcvertcount * sizeof(float4) );
				index += srcvertcount;
			}
		}
		break;

	case GMB_QUADS_CW:
		{
			sx_assert( L"uiControl::Batch mode can't be Quad CW because is not implemented yet !" );
		}
		break;
	}

}

void uiDevice::BeginBatchElements( const GUIBatchMode mode, const uint count )
{
	m_batchMode = mode;
	if ( count )
		m_batches.SetSize( count );
}

bool uiDevice::AddBatchElements( const uiElement* elem )
{
	//	verify that all these have the same image id
	if ( m_batches.m_count )
		if ( m_batches[0]->m_image != elem->m_image ) return false;

	//	check the batch mode
	if ( m_batchMode == GBM_SIMPLE )
	{
		m_batches.PushBack( (uiElement*)elem );
		return true;
	}

	//	for the other batch mode number of vertices should be 4
	if ( elem->m_numVertices == 4 )
	{
		m_batches.PushBack( (uiElement*)elem );
		return true;
	}

	return false;
}

void uiDevice::EndBatchElements( uiElement* dest )
{
	const uint additionalVertices = ( m_batchMode == GBM_TRIANGLES ? 2 : 0 );

	// compute number of vertices
	uint sumVertices = 0;
	for ( sint i=0; i<m_batches.m_count; ++i )
		sumVertices += m_batches.m_item[i]->m_numVertices + additionalVertices;

	//	prepare destination element
	uint destVertices = dest->m_numVertices;
	dest->CreateVertices( destVertices + sumVertices );

	//	copy batches to dest element
	uint index = destVertices;
	if ( m_batchMode == GBM_SIMPLE )
	{
		for ( sint i=0; i<m_batches.m_count; ++i )
		{
			const uiElement* src = m_batches.m_item[i];
			const uint srcvertcount = src->m_numVertices;
			sx_mem_copy( &dest->m_pos[index],	src->m_pos,		srcvertcount * sizeof(float3) );
			sx_mem_copy( &dest->m_uv[index],	src->m_uv,		srcvertcount * sizeof(float2) );
			sx_mem_copy( &dest->m_color[index], src->m_color,	srcvertcount * sizeof(float4) );
			index += srcvertcount;
		}
	}
	else
	{
		for ( sint i=0; i<m_batches.m_count; ++i )
		{
			Copy( dest, index, m_batches.m_item[i], m_batchMode );
		}
	}

	//	release array
	m_batches.Clear();
}



