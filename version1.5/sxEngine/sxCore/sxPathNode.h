/********************************************************************
	created:	2011/07/05
	filename: 	sxPathNode.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a basic class of path node. this class 
				used in path finding systems
*********************************************************************/
#ifndef GUARD_sxPathNode_HEADER_FILE
#define GUARD_sxPathNode_HEADER_FILE

#include "sxNodeMember.h"

namespace sx { namespace core {

	// forward declarations
	typedef class PathNode *PPathNode;
	typedef Array<PPathNode>	ArrayPPathNode;

	/*
	path node member will use in path finding system.
	*/
	class SEGAN_API PathNode : public NodeMember
	{
		SEGAN_STERILE_CLASS(PathNode);

	public:
		PathNode();
		virtual ~PathNode();

		//!	this will call when the owner's "Update" function called
		void Update(float elpsTime);

		//! draw this member depend on entry flag
		void Draw(DWORD flag);

		//! use to communicate by this member
		UINT MsgProc(UINT msgType, void* data);

		//! save this member to the stream
		void Save(Stream& stream);

		//! load data of this member from the stream
		void Load(Stream& stream);

		//! return position of this path node in world space
		const float3& GetPosition(void);

		//! connect this path node with the other one
		void ConnectTo(PathNode* other);

		//! disconnect this path node from the other one
		void DisconnectFrom(PathNode* other);

		//! use to find neighbors by specified position. return null if no path node found
		static PPathNode FindNeighbor(float3& pos, DWORD id = 0);

		//! return true if can find path from start node to end node
		static bool FindPath(PPathNode start, PPathNode end, ArrayPPathNode& pathNodeList);

	public:
		friend class Path_Internal;

		int				m_ID;		//  id of this path node
		ArrayPPathNode	m_Neighbor;	// hold neighbors as array

	//public:
		//  additional data
		float3			score;		//  values of node used in search algorithms
		PPathNode		next;		//  use to some internal options and operation
		PPathNode		prev;		//  use to some internal options and operation
		PPathNode		path;		//  use to some internal options and operation
		DWORD			reserved;	//  use to some internal options and operation


	public:
		//////////////////////////////////////////////////////////////////////////
		//	MANAGER
		//////////////////////////////////////////////////////////////////////////
		class SEGAN_API Manager
		{
		public:
			//! use in editors to draw/select an specified layer
			static int& DebugLayer(void);
		};

	};
	typedef PathNode *PPathNode;


}}	//  namespace sx { namespace core {

#endif	//	GUARD_sxPathNode_HEADER_FILE
