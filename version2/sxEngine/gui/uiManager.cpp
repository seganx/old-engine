#include "uiManager.h"
#include "uiDevice.h"

GUIManager::GUIManager( void )
: m_controls(128)
, m_elements(128)
{
	//	create drawable element
	m_drawable = sx_new( uiElement );
	m_drawable->m_type = ET_TRIANGLES;
}

GUIManager::~GUIManager( void )
{
	//	destroy drawable element
	sx_delete_and_null( m_drawable );
}

void GUIManager::Add( const uiControl* control )
{
	m_controls.push_back( (uiControl*)control );
}

void GUIManager::Remove( const uiControl* control )
{
	m_controls.remove( (uiControl*)control );
}

void GUIManager::Delete( uiControl*& control )
{
	m_controls.remove( control );
	sx_delete_and_null( control );
}

void GUIManager::Clear( void )
{
	for ( sint i=0; i<m_controls.m_count; ++i )
	{
		sx_delete( m_controls[i] );
	}
	m_controls.clear();
}


void GUIManager::Update( float elpsTime, const float vpwidth, const float vpheight )
{
	matrix proj = sx_orthographic( vpwidth, vpheight, -200.0f, 200.0f );
	matrix view = sx_lookat( float3(0, 0, 1), float3( 0, 0, 0), float3( 0, 1, 0) );
	matrix viewproj = sx_mul( view, proj );
	matrix viewinvr = sx_inverse( view );

	m_view = view;
	m_proj = proj;

	for ( sint i=0; i<m_controls.m_count; ++i )
	{
//		m_controls.m_item[i]->Update( elpsTime, viewinvr, viewproj, g_engine->m_device3D->m_viewport.width, g_engine->m_device3D->m_viewport.height );
	}
}

#if 0
void GUIManager::ProcessInput( struct InputReport* inputReport )
{
	// collect necessary data
	uint pid = inputReport->playerID;

	//	fill input report structure
	uiInputReport ioreport;
	ioreport.mouseLocked	= inputReport->locked;
	ioreport.keyboardLocked = inputReport->locked;

	//	fill mouse structure
	ioreport.ray = sx_ray( sx_mouse_absx(pid), sx_mouse_absy(pid), sx_vp_width, sx_vp_height, m_view, m_proj );
	switch ( g_engine->m_input->GetKeys( pid )->mouse_left )
	{
	case IS_NORMAL:		ioreport.mouseLeft = MS_NORMAL;		break;
	case IS_DOWN:
	case IS_HOLD:		ioreport.mouseLeft = MS_DOWN;		break;
	case IS_UP:			ioreport.mouseLeft = MS_UP;			break;
	}

	//	fill keyboard structure


	//	traverse through controls
	for ( sint i=m_controls.m_count-1; i >= 0; --i )
	{
		m_controls.m_item[i]->ProcessInput( &ioreport );
	}
}
#endif

void GUIManager::Draw( const dword flag )
{
	//	extract all elements that should be draw
	m_elements.clear();
	for ( sint i=0; i<m_controls.m_count; ++i )
	{
		m_controls[i]->get_elements( &m_elements );
	}

#if 0

	//	batch elements and draw them
	while ( m_elements.m_count )
	{
		g_engine->m_deviceUI->BeginBatch( 0 );
		for ( sint i=0; i<m_elements.m_count; ++i )
		{
			if ( g_engine->m_deviceUI->AddBatch( m_elements.m_item[i] ) )
				m_elements.remove_index( i-- );
			else
				break;
		}

		//	verify that one element batched at least
		if ( g_engine->m_deviceUI->m_batches.m_count < 1 ) break;

		//	end batch
		g_engine->m_deviceUI->EndBatch( m_drawable );

		//	draw the final element
		vbdescPos.size = m_drawable->m_numVertices * sizeof(float3);
		vbdescUV.size = m_drawable->m_numVertices * sizeof(float2);
		vbdescColor.size = m_drawable->m_numVertices * sizeof(Color2);

		m_vb_pos->SetDesc( vbdescPos, m_drawable->m_posfinal );
		m_vb_uv->SetDesc( vbdescUV, m_drawable->m_uv );
		m_vb_color->SetDesc( vbdescColor, m_drawable->m_color );

		g_engine->m_device3D->SetVertexBuffer( m_vb_pos,	SX_VERTEX_POSITION );
		g_engine->m_device3D->SetVertexBuffer( m_vb_uv,		SX_VERTEX_UV0 );
		g_engine->m_device3D->SetVertexBuffer( m_vb_color,	SX_VERTEX_COLORS );

		g_engine->m_device3D->DrawPrimitive( PT_TRIANGLE_LIST, 0, m_drawable->m_numVertices );
	}
#endif

}



uiControl* GUIManager::CreateContorl( const uiType type )
{
	switch ( type )
	{
	case UT_PANEL:			return sx_new( uiPanel );
#if 0
	case GUI_BUTTON:		return sx_new( uiButton );
	case GUI_CHECKBOX:		return sx_new( uiCheckBox );
	case GUI_TRACKBAR:		return sx_new( uiScroll );
	case GUI_PROGRESSBAR:	return sx_new( uiProgress );
	case GUI_LABEL:			return sx_new( uiLabel );
	case GUI_EDITBOX:		return sx_new( uiEditBox );
	case GUI_PANELEX:		return sx_new( uiPanelEx );
	case GUI_LISTBOX:		return sx_new( uiListBox );
#endif
	}
	return null;
}

void GUIManager::Copy( uiElement* dest, uint& index, const uiElement* src )
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
			const uint srcvertcount = src->m_vcount;
			if ( srcvertcount )
			{
				sx_mem_copy( &dest->m_posfinal[index],	src->m_posfinal,	srcvertcount * sizeof(float3) );
				sx_mem_copy( &dest->m_uv[index],		src->m_uv,			srcvertcount * sizeof(float2) );
				sx_mem_copy( &dest->m_color[index],		src->m_color,		srcvertcount * sizeof(Color2) );
				index += srcvertcount;
			}
		}
		break;

	case ET_QUADS:
		{
			if ( src->m_vcount )
			{
				sx_convert_quat_triangle( &dest->m_posfinal[index],		src->m_posfinal		);
				sx_convert_quat_triangle( &dest->m_uv[index],			src->m_uv			);
				sx_convert_quat_triangle( &dest->m_color[index],		src->m_color		);
				index += src->m_vcount + 2;
			}
		}
		break;
	}
}

SEGAN_INLINE void GUIManager::BeginBatch( const uint count )
{
	if ( count )
		m_batches.set_size( count );
}

SEGAN_INLINE bool GUIManager::AddBatch( const uiElement* elem )
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

	m_batches.push_back( (uiElement*)elem );
	return true;
}

SEGAN_INLINE uint GUIManager::GetBatchVertexCount( void )
{
	// compute number of vertices
	uint sumVertices = 0;
	for ( sint i=0; i<m_batches.m_count; ++i )
	{
		uiElement* elem = m_batches.m_item[i];
		if ( elem->m_type == ET_QUADS )
			sumVertices += elem->m_vcount + 2;
	}
	return sumVertices;
}

SEGAN_INLINE void GUIManager::EndBatch( uiElement* dest )
{
	//	get number of vertices needed to batch them
	const uint sumVertices = GetBatchVertexCount();

	//	prepare destination element
	dest->create_vertices( sumVertices );

	//	copy batches to dest element
	uint index = 0;
	for ( sint i=0; i<m_batches.m_count; ++i )
	{
		Copy( dest, index, m_batches.m_item[i] );
	}

	//	release array
	m_batches.clear();
}

