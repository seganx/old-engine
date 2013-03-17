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
	for ( sint i=0; i<m_gui.m_count; ++i )
	{
		sx_delete( m_gui[i] );
	}
	m_gui.Clear();
}

void GUIManager::Update( float elpsTime )
{
	for ( sint i=0; i<m_gui.m_count; ++i )
	{
		m_gui[i]->Update( elpsTime );
	}
}

void GUIManager::ProcessInput( void )
{
	for ( sint i=0; i<m_gui.m_count; ++i )
	{
	//	m_gui[i]->Update( elpsTime );
	}
}

void GUIManager::Draw_topleft( const dword flag )
{
	static uiElement elemfinal;

#if 0

	//	extract elements
	uint elemcount = 0;
	uiElement* element[2048];

	for ( sint i=0; i<m_gui.m_count && i<2048; ++i )
	{
		uiControl* control = m_gui[i];
		for ( sint e=0; e<SX_GUI_MAX_ELEMENT; ++e )
		{
			if ( control->m_element[e].m_numVertices )
			{
				element[elemcount++] = &control->m_element[e];
				sx_assert( elemcount < 2048 );
				if ( elemcount >= 2048 ) break;
			}
		}
	}

	// compute number of vertices
	uint sumVertices = 0;
	for ( uint i=0; i<elemcount; ++i )
		sumVertices += ( element[i]->m_numVertices + 2 );

	//	prepare final destination element
	elemfinal.CreateVertices( sumVertices );

	uint index = 0;
	GUIBatchMode mode = ( g_engine->m_device3D->m_creationData.flag & SX_D3D_CREATE_GL ) ? GBM_QUADS_CCW : GBM_TRIANGLES;
	for ( uint i=0; i<elemcount && i<2048; ++i )
	{
		float3* pos = &elemfinal.m_pos[index];
		g_engine->m_deviceUI->Copy( &elemfinal, index, element[i], mode );

		//pos->x += 30.0f;
	}
	
#else

	uiBatchMode mode = ( g_engine->m_device3D->m_creationData.flag & SX_D3D_CREATE_GL ) ? BM_QUADS_CCW : BM_TRIANGLES;
	g_engine->m_deviceUI->BeginBatchElements( mode, 0 );

	for ( sint i=0; i<m_gui.m_count; ++i )
	{
		uiControl* control = m_gui[i];

		for ( sint e=0; e<SX_GUI_MAX_ELEMENT; ++e )
		{
			if ( control->m_element[e].m_numVertices )
			{
				g_engine->m_deviceUI->AddBatchElements( &control->m_element[e] );
			}
		}
	}

	g_engine->m_deviceUI->EndBatchElements( &elemfinal );
#endif

	for ( uint i=0; i<elemfinal.m_numVertices; ++i )
	{
		elemfinal.m_pos[i].x -= 200.0f;
		elemfinal.m_pos[i].y += 200.0f;
	}
	sx_debug_draw_gui_element( &elemfinal );
}

