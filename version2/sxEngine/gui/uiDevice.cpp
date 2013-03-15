#include "uiDevice.h"


uiDevice::uiDevice( void )
:	m_batches(128)
{

}

uiDevice::~uiDevice( void )
{

}

uiContorl* uiDevice::CreateContorl( const GUIType type )
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

void uiDevice::BeginBatchElements( const dword flag, const uint count )
{
	m_batches.SetSize( count );
}

bool uiDevice::AddBatchElements( const uiElement* elem )
{
	bool res = false;
	if ( m_batches.m_count < m_batches.m_size )
	{
		m_batches.PushBack( (uiElement*)elem );
		res = true;
	}
	return res;
}

void uiDevice::EndBatchElements( uiElement* dest )
{
	// compute number of vertices
	uint sumVertices = 0;
	for ( sint i=0; i<m_batches.m_count; ++i )
		sumVertices += m_batches.m_item[i]->m_numVertices;

	//	prepare destination element
	uint destVertices = dest->m_numVertices;
	dest->CreateVertices( destVertices + sumVertices );

	//	copy batches to dest element
	uint index = destVertices;
	for ( sint i=0; i<m_batches.m_count; ++i )
	{
		const uiElement* src = m_batches.m_item[i];
		const uint srcvertcount = src->m_numVertices;
		sx_mem_copy( &dest->m_pos[index],	src->m_pos,		srcvertcount * sizeof(float3) );
		sx_mem_copy( &dest->m_uv[index],	src->m_uv,		srcvertcount * sizeof(float2) );
		sx_mem_copy( &dest->m_color[index], src->m_color,	srcvertcount * sizeof(float4) );
		index += srcvertcount;
	}

	//	release array
	m_batches.Clear();
}
