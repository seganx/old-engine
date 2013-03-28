#include "GUIManager.h"
#include "../sxEngine.h"


GUIManager::GUIManager( void )
: m_controls(128)
, m_elements(128)
{
	m_drawable = sx_new( uiElement );
	m_drawable->m_type = ET_TRIANGLES;
}

GUIManager::~GUIManager( void )
{
	sx_delete_and_null( m_drawable );
}

void GUIManager::Add( const uiControl* control )
{
	m_controls.PushBack( (uiControl*)control );
}

void GUIManager::Remove( const uiControl* control )
{
	m_controls.Remove( (uiControl*)control );
}

void GUIManager::Delete( uiControl*& control )
{
	m_controls.Remove( control );
	sx_delete_and_null( control );
}

void GUIManager::Clear( void )
{
	for ( sint i=0; i<m_controls.m_count; ++i )
	{
		sx_delete( m_controls[i] );
	}
	m_controls.Clear();
}

void GUIManager::Update( float elpsTime )
{
	const float width  = sx_vp_width;
	const float height = sx_vp_height;
	matrix proj = sx_orthographic( width, height, -200.0f, 200.0f );
	matrix view = sx_lookat( float3(0, 0, 1), float3( 0, 0, 0), float3( 0, 1, 0) );
	matrix viewproj = sx_mul( view, proj );
	matrix viewinvr = sx_inverse( view );

	m_view = view;
	m_proj = proj;

	for ( sint i=0; i<m_controls.m_count; ++i )
	{
		m_controls.m_item[i]->Update( elpsTime, viewinvr, viewproj, g_engine->m_device3D->m_viewport.width, g_engine->m_device3D->m_viewport.height );
	}
}

void GUIManager::ProcessInput( struct InputReport* inputReport )
{
	// collect necessary data
	uint pid = inputReport->playerID;

	//	fill input report structure
	uiInputReport ioreport;

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

void GUIManager::Draw( const dword flag )
{
	matrix mat; mat.Identity();
	g_engine->m_device3D->SetMatrix( MM_WORLD, mat );
	g_engine->m_device3D->SetMatrix( MM_VIEW, m_view );
	g_engine->m_device3D->SetMatrix( MM_PROJECTION, m_proj );

	g_engine->m_device3D->SetRenderState( RS_FILL, false );

	//	extract all elements that should be draw
	m_elements.Clear();
	for ( sint i=0; i<m_controls.m_count; ++i )
	{
		m_controls[i]->GetElements( &m_elements );
	}

	//	batch elements and draw them
	while ( m_elements.m_count )
	{
		m_drawable->m_numVertices = 0;

		g_engine->m_deviceUI->BeginBatch( 0 );
		for ( sint i=0; i<m_elements.m_count; ++i )
		{
			if ( g_engine->m_deviceUI->AddBatch( m_elements.m_item[i] ) )
				m_elements.RemoveByIndex( i-- );
			else
				break;
		}

		//	verify that one element batched at least
		if ( g_engine->m_deviceUI->m_batches.m_count < 1 ) break;

		//	end batch
		g_engine->m_deviceUI->EndBatch( m_drawable );

		//	draw the final element
		sx_debug_draw_gui_element( m_drawable );
	}

	g_engine->m_device3D->SetRenderState( RS_FILL, true );

}

