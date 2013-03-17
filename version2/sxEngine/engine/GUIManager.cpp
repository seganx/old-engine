#include "GUIManager.h"
#include "../sxEngine.h"


GUIManager::GUIManager( void )
{

}

GUIManager::~GUIManager( void )
{

}

void GUIManager::Add( const uiControl* control )
{
	m_gui.PushBack( (uiControl*)control );
}

void GUIManager::Remove( const uiControl* control )
{
	m_gui.Remove( (uiControl*)control );
}

void GUIManager::Delete( uiControl*& control )
{
	m_gui.Remove( control );
	sx_delete_and_null( control );
}

void GUIManager::Clear( void )
{
	for ( uint i=0; i<m_gui.m_count; ++i )
	{
		sx_delete( m_gui[i] );
	}
	m_gui.Clear();
}

void GUIManager::Update( float elpsTime )
{
	const matrix& view = g_engine->m_device3D->GetMatrix( MM_VIEW );
	const matrix& proj = g_engine->m_device3D->GetMatrix( MM_PROJECTION );
	matrix viewproj = sx_mul( view, proj );
	matrix viewinvr = sx_inverse( view );

	for ( uint i=0; i<m_gui.m_count; ++i )
	{
		m_gui[i]->Update( elpsTime, viewinvr, viewproj, g_engine->m_device3D->m_viewport.width, g_engine->m_device3D->m_viewport.height );
	}
}

void GUIManager::ProcessInput( void )
{
	for ( uint i=0; i<m_gui.m_count; ++i )
	{
	//	m_gui[i]->Update( elpsTime );
	}
}

void GUIManager::Draw( const dword flag )
{
	static uiElement elemfinal;
	elemfinal.m_numVertices = 0;

	uiBatchMode mode = ( g_engine->m_device3D->m_creationData.flag & SX_D3D_CREATE_GL ) ? BM_QUADS_CCW : BM_TRIANGLES;
	g_engine->m_deviceUI->BeginBatchElements( mode, 0 );

	for ( uint i=0; i<m_gui.m_count; ++i )
	{
		uiControl* control = m_gui[i];

		for ( sint e=0; e<SX_GUI_MAX_ELEMENT; ++e )
		{
			uiElement* element = &control->m_element[e];
			const uint numvertices = element->m_numVertices;
			if ( numvertices )
			{
				for ( uint v=0; v < numvertices; ++v )
					sx_transform_point( element->m_posfinal[v], element->m_pos[v], control->m_matrix );

				g_engine->m_deviceUI->AddBatchElements( &control->m_element[e] );
			}
		}
	}

	g_engine->m_deviceUI->EndBatchElements( &elemfinal );

	sx_debug_draw_gui_element( &elemfinal );
}

