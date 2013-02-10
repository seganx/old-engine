#include "NodeExplorer.h"
#include "MainEditor.h"
#include "EditorSettings.h"
#include "EditorUI.h"
#include "sxItemUI.h"

using namespace sx;

#define NODE_EXPLORERE_MAX_WIDTH	600.0f

typedef struct nxItemInfo
{
	void*	data;
	DWORD	info;
}
*PnxItemInfo;

NodeExplorer::NodeExplorer( void ) : sx::gui::Form(), m_Listbox()
{
	m_pBack	 = sx_new( sx::gui::Panel );
	m_pBack->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.2f);
	m_pBack->AddProperty(SX_GUI_PROPERTY_CLIPCHILDS);
	m_pBack->AddProperty(SX_GUI_PROPERTY_ACTIVATE);

	m_Listbox.SetParent( m_pBack );
	m_Scroll = EditorUI::CreateTrackbar( m_pBack, 100.0f, 0.0f, NODE_EXPLORERE_MAX_WIDTH*0.5f );
}

NodeExplorer::~NodeExplorer( void )
{
	for (int i=0; i<m_Listbox.Count(); i++)
		sx_mem_free( m_Listbox[i].UserData );
}

void NodeExplorer::SetParent( sx::gui::PControl parent )
{
	m_pBack->SetParent( parent );
}

void NodeExplorer::SetSize( float width, float height )
{
	m_pBack->SetSize( float2( width, height ) );
	
	m_Listbox.SetSize( NODE_EXPLORERE_MAX_WIDTH, height-16.0f );
	m_Listbox.Position().y = 8.0f;

	m_Scroll->SetSize( float2(width, 16.0f) );
	m_Scroll->Position().y = - height * 0.5f + 10.0f;
}

void NodeExplorer::Update( float elpsTime )
{
	float w = m_pBack->GetSize().x;
	float offset = NODE_EXPLORERE_MAX_WIDTH*0.5f - w*0.5f;
	m_Listbox.Position().x = offset -  m_Scroll->GetBlendingValue();
	m_Listbox.GetScroll()->Position().x =  w*0.5f - m_Listbox.Position().x - 10.0f;
	m_Listbox.Update(elpsTime);
}

float3& NodeExplorer::Position( void )
{
	return m_pBack->Position();
}

void NodeExplorer::UpdateData( sx::core::PNode node, sx::core::PNodeMember member )
{
	//  monitor changes
	static int numChildren = 0;
	static core::PNode lastRoot = 0;

	//  verify data
	if (!node )
	{
		numChildren = 0;
		lastRoot = NULL;
		for (int i=0; i<m_Listbox.Count(); i++)
			sx_mem_free( m_Listbox[i].UserData );
		m_Listbox.Clear();
		return;
	}

	//  find the root
	core::PNode root = node;
	while ( root->GetParent() )
	{
		root = root->GetParent();
	}

	static core::ArrayPNode nodeList;
	nodeList.Clear();

	//  try to find any changes
	int curChildren = root->GetMemberCount();
	root->GetChildren(nodeList, true);
	curChildren += nodeList.Count();
	for (int i=0; i<nodeList.Count(); i++)
		curChildren += nodeList[i]->GetMemberCount();

	//  verify that no changes happened
	if ( lastRoot == root && numChildren == curChildren )
	{

		int index = m_Listbox.GetItemIndex();
		for (int i=0; i<m_Listbox.Count() && !m_Listbox.GetFocused(); i++)
		{
			PnxItemInfo pinfo = static_cast<PnxItemInfo>(m_Listbox[i].UserData);
			if (pinfo)
			{
				if (member)
				{
					if (pinfo->data == member)
					{
						if ( i != index )
							m_Listbox.SetItemIndex(i);
						break;
					}
				}
				else
				{
					if (pinfo->data == node)
					{
						if ( i != index )
							m_Listbox.SetItemIndex(i);
						break;
					}
				}
			}
		}

		return;
	}
	numChildren = curChildren;
	lastRoot = root;

	//  clear current data
	for (int i=0; i<m_Listbox.Count(); i++)
		sx_mem_free( m_Listbox[i].UserData );
	m_Listbox.Clear();
	AddNodeToList(root, 0);
	
	int index = m_Listbox.GetItemIndex();
	for (int i=0; i<m_Listbox.Count() && !m_Listbox.GetFocused(); i++)
	{
		PnxItemInfo pinfo = static_cast<PnxItemInfo>(m_Listbox[i].UserData);
		if (pinfo)
		{
			if (member)
			{
				if (pinfo->data == member)
				{
					if ( i != index )
						m_Listbox.SetItemIndex(i);
					break;
				}
			}
			else
			{
				if (pinfo->data == node)
				{
					if ( i != index )
						m_Listbox.SetItemIndex(i);
					break;
				}
			}
		}
	}
}

void NodeExplorer::AddNodeToList( sx::core::PNode node, int level )
{
	if (!node) return;

	PnxItemInfo pinfo = (PnxItemInfo)sx_mem_alloc( sizeof(nxItemInfo) );
	pinfo->data = node;
	pinfo->info = 0; // node
	m_Listbox.Add( node->GetName() ? node->GetName() : L"No Name", EditorUI::GetMemberTexture(NMT_UNKNOWN), pinfo, level );
	level++;

	for (int i=0; i<node->GetMemberCount(); i++)
	{
		pinfo = (PnxItemInfo)sx_mem_alloc( sizeof(nxItemInfo) );
		pinfo->data = node->GetMemberByIndex(i);
		pinfo->info = 1; // member

		if ( node->GetMemberByIndex(i)->GetName() )
			m_Listbox.Add( node->GetMemberByIndex(i)->GetName(), EditorUI::GetMemberTexture(node->GetMemberByIndex(i)->GetType()), pinfo, level );
		else
			m_Listbox.Add( L"No Name", EditorUI::GetMemberTexture(node->GetMemberByIndex(i)->GetType()), pinfo, level );
	}

	for (int i=0; i<node->GetChildCount(); i++)
	{
		AddNodeToList(node->GetChildByIndex(i), level);
	}
}

sx::core::PNode NodeExplorer::GetSelectedNode( void )
{
	int index = m_Listbox.GetItemIndex();
	if (index<0) return NULL;

	PnxItemInfo pinfo = (PnxItemInfo)m_Listbox[index].UserData;
	if (!pinfo) return NULL;

	if (pinfo->info==0) // node
	{
		return (sx::core::PNode)pinfo->data;
	}
	else if (pinfo->info==1) // member
	{
		core::PNodeMember member = (core::PNodeMember)pinfo->data;
		if (member)
			return member->GetOwner();
		else
			return NULL;
	}

	return NULL;
}

sx::core::PNodeMember NodeExplorer::GetSelectedMember( void )
{
	int index = m_Listbox.GetItemIndex();
	if (index<0) return NULL;

	PnxItemInfo pinfo = (PnxItemInfo)m_Listbox[index].UserData;
	if (!pinfo) return NULL;

	if (pinfo->info==1) // member
		return (core::PNodeMember)pinfo->data;;
	
	return NULL;
}