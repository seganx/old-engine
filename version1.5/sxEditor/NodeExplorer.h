/********************************************************************
	created:	2011/06/21
	filename: 	NodeExplorer.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain an object to explore selected node
				explore node will start from the node which has no parent
*********************************************************************/
#ifndef GUARD_NodeExplorer_HEADER_FILE
#define GUARD_NodeExplorer_HEADER_FILE

#include "sxListBox.h"

//  use this class to explore node
class NodeExplorer : public sx::gui::Form
{

public:
	NodeExplorer(void);
	virtual ~NodeExplorer(void);

	//! set parent
	void SetParent(sx::gui::PControl parent);

	//! set size of the form
	void SetSize(float width, float height);

	//! update the form
	void Update(float elpsTime);

	//! reference to the position
	float3& Position(void);

	//! assign node and member to explore. call it frequently in loop to check changes.
	void UpdateData(sx::core::PNode node, sx::core::PNodeMember member);

	//! return selected node in node tree
	sx::core::PNode GetSelectedNode(void);

	//! return selected member
	sx::core::PNodeMember GetSelectedMember(void);

private:

	void AddNodeToList(sx::core::PNode node, int level);

public:
	sx::gui::PPanel			m_pBack;			//  background of this object
	sx::gui::ListBox		m_Listbox;			//  list box will contain items
	sx::gui::PTrackBar		m_Scroll;			//  scroll bar to traverse horizontally
};
typedef NodeExplorer *PNodeExplorer;

#endif	//	GUARD_NodeExplorer_HEADER_FILE
