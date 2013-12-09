#include "uiManager.h"
#include "uiDevice.h"

uiManager::uiManager( void )
: m_controls(128)
, m_elements(128)
{
	//	create drawable element
	m_drawable = sx_new( uiContext );
}

uiManager::~uiManager( void )
{
	//	destroy drawable element
	sx_delete_and_null( m_drawable );
}

void uiManager::add( const uiControl* control )
{
	m_controls.push_back( (uiControl*)control );
}

void uiManager::remove( const uiControl* control )
{
	m_controls.remove( (uiControl*)control );
}

void uiManager::clear( void )
{
	for ( sint i=0; i<m_controls.m_count; ++i )
	{
		sx_delete( m_controls[i] );
	}
	m_controls.clear();
}


void uiManager::update( float elpsTime, const float vpwidth, const float vpheight )
{
	matrix proj = sx_orthographic( vpwidth, vpheight, -200.0f, 200.0f );
	matrix view = sx_lookat( float3(0, 0, 1), float3( 0, 0, 0), float3( 0, 1, 0) );
	matrix viewproj = sx_mul( view, proj );
	matrix viewinvr = sx_inverse( view );

	m_view = view;
	m_proj = proj;

	for ( sint i=0; i<m_controls.m_count; ++i )
	{
		m_controls.m_item[i]->update( elpsTime, vpwidth, vpheight );
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

void uiManager::draw( const dword flag )
{
	//	extract all elements that should be draw
	m_elements.clear();
	for ( sint i=0; i<m_controls.m_count; ++i )
	{
		sx_get_contexts( m_controls[i], &m_elements );
	}

#if 1

	//	batch elements and draw them
	while ( m_elements.m_count )
	{
		begin_batch( 0 );
		for ( sint i=0; i<m_elements.m_count; ++i )
		{
			if ( add_batch( m_elements.m_item[i] ) )
				m_elements.remove_index( i-- );
			else
				break;
		}

		//	verify that one element batched at least
		if ( m_batches.m_count < 1 ) break;

		//	end batch
		end_batch( m_drawable );
	}
#endif

}



uiControl* uiManager::create_contorl( const char* type )
{
#if 0
	switch ( type )
	{
	case UT_PANEL:			return sx_new( uiPanel );
	case GUI_BUTTON:		return sx_new( uiButton );
	case GUI_CHECKBOX:		return sx_new( uiCheckBox );
	case GUI_TRACKBAR:		return sx_new( uiScroll );
	case GUI_PROGRESSBAR:	return sx_new( uiProgress );
	case GUI_LABEL:			return sx_new( uiLabel );
	case GUI_EDITBOX:		return sx_new( uiEditBox );
	case GUI_PANELEX:		return sx_new( uiPanelEx );
	case GUI_LISTBOX:		return sx_new( uiListBox );
	}
#endif
	return null;
}

void uiManager::copy( uiContext* dest, uint& index, const uiContext* src )
{
	const uint srcvertcount = src->m_vcount;
	if ( srcvertcount )
	{
		sx_mem_copy( &dest->m_pos[index],		src->m_pos,			srcvertcount * sizeof(float3) );
		sx_mem_copy( &dest->m_uv[index],		src->m_uv,			srcvertcount * sizeof(float2) );
		sx_mem_copy( &dest->m_color[index],		src->m_color,		srcvertcount * sizeof(Color2) );
		index += srcvertcount;
	}
}

SEGAN_INLINE void uiManager::begin_batch( const uint count )
{
	if ( count )
		m_batches.set_size( count );
}

SEGAN_INLINE bool uiManager::add_batch( const uiContext* elem )
{
	//	verify that all these have the same image id
	if ( m_batches.m_count && m_batches[0]->m_image != elem->m_image ) return false;

	m_batches.push_back( (uiContext*)elem );
	return true;
}

SEGAN_INLINE uint uiManager::get_batch_vcount( void )
{
	// compute number of vertices
	uint sumVertices = 0;
	for ( sint i=0; i<m_batches.m_count; ++i )
	{
		uiContext* elem = m_batches.m_item[i];
		sumVertices += elem->m_vcount;
	}
	return sumVertices;
}

SEGAN_INLINE void uiManager::end_batch( uiContext* dest )
{
	//	get number of vertices needed to batch them
	const uint sumVertices = get_batch_vcount();

	//	prepare destination element
	dest->create_vertices( sumVertices );

	//	copy batches to dest element
	uint index = 0;
	for ( sint i=0; i<m_batches.m_count; ++i )
	{
		copy( dest, index, m_batches.m_item[i] );
	}

	//	release array
	m_batches.clear();
}

