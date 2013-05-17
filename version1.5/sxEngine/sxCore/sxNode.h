/********************************************************************
	created:	2010/11/05
	filename: 	sxNode.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a base node of the engine. any thing
				in the world of engine will be a type of node. a node 
				may contain one or more type of NodeMember. some other
				features will comes by NodeMembers.

				NOTE : '
				> 'SetPosition()' will check to see if node is in the scene
				and has no parent then update node position in the scene 
				automatically by calling 'Scene::UpdateNode()'


*********************************************************************/
#ifndef GUARD_sxNode_HEADER_FILE
#define GUARD_sxNode_HEADER_FILE

#include "sxCore_def.h"
#include "../sxCommon/sxCommon.h"
#include "sxNodeMember.h"


namespace sx { namespace core {

	//  forward declaration
	typedef class	Node	*PNode;
	typedef struct	Sector	*PSector;
	typedef Array<PNode>	ArrayPNode;
	typedef Array<PNode>	ArrayPNode_abs;
	typedef Array<PNode>	ArrayPNode_inline;

	//typedef Array<PNode>			ArrayPNode_abs;
	//typedef Array<PNode>			ArrayPNode_inline;
	

	/*
	this is the base node of the engine. other features can attach to this node in body of NodeMembers
	*/
	class SEGAN_API Node
	{	
		SEGAN_STERILE_CLASS(Node);

	public:
		Node();
		~Node();

		//! remove childes and node members and clean up the node
		void Clear(void);

		//! set a new name
		void SetName(const WCHAR* pwcName);

		//! return the name of the node
		const WCHAR* GetName(void);

		//! set user tag
		void SetUserTag(UINT tag);

		//! return user tag
		UINT GetUserTag(void);

		//! set user data
		void SetUserData(void* data);

		//! return user data
		void* GetUserData(void);

		//! add a new property 'SX_NODE_PROPERTY_'
		void AddProperty(DWORD prop);

		//! remove a property from current property set 'SX_NODE_PROPERTY_'
		void RemProperty(DWORD prop);

		//! return true if this node has specified property 'SX_NODE_PROPERTY_'
		bool HasProperty(DWORD prop);

		//! set a new parent.
		void SetParent(const PNode pParent);

		//! get the current parent
		PNode GetParent(void);

		//! return the child of the node. return null if no child found
		PNode GetChildByIndex(int index);

		//! find and return true if child founded by name. return false if no match found. this method is not efficient
		bool GetChildByName(const WCHAR* pwcName, OUT PNode& pNode, bool traversChildren = true);

		//! return the all children of this node
		void GetChildren(ArrayPNode_abs& nodeList, bool traverseChildes);

		//! return number of childes
		int	GetChildCount(void);

		//! return the member of the node by index. return null if no member found.
		PNodeMember GetMemberByIndex(int index);

		//! find and return the index of member by name. return -1 if no match found. this method is not efficient
		int GetMemberByName(const WCHAR* pwcName, OUT PNodeMember& pMember);

		//! fill the array list by required type
		void GetMembersByType(DWORD NMT_ nmType, ArrayPNodeMember& memberList, bool traverseChildes);

		//! return number of members
		int	GetMemberCount(void);

		//! swap members
		void SwapMembers(int index1, int index2);

		//! verify member type exist in member list
		bool MemberTypeExist(DWORD NMT_ nmType);

		//!	set node position. also if you want to set rotation, you can call SetTransform() instead of this to increase efficiency.
		void SetPosition(const float3& vPosition);

		//! get node position in local space
		const float3& GetPosition_local(void);

		//! get node position in world space
		const float3& GetPosition_world(void);

		//! set node direction in local space
		void SetDirection_local(const float3& dir);

		//! set node direction in world space
		void SetDirection_world(const float3& dir);

		//! set node quaternion. also if you want to set position, you can call SetTransformQ() instead of this to increase efficiency.
		void SetQuaternion(floatQ& qQuaternion);

		//! set node rotation. this function will recompute quaternion of this node
		void SetRotation(const float rx, const float ry, const float rz);

		//! get node quaternion
		const floatQ& GetQuaternion(void);

		//! set node transformation. using this function can improve performance
		void SetTransformQ(const float3& vPosition, const floatQ& qQuaternion);

		//! set node transformation offset. this function just apply on offset matrix
		void SetTransformQ_offset(const float3& vPosition, const floatQ& qQuaternion);

		//! update bounding volumes depend on child's bounds
		void UpdateBoundingVolumes(void);

		//! return matrix base which parent matrix NOT applied
		const Matrix& GetMatrix_local(void);

		//! return final matrix which parent matrix applied
		const Matrix& GetMatrix_world(void);

		//! return none transformed box
		const AABox& GetBox_local(void);

		//! return transformed box in final matrix
		OBBox& GetBox_world(void);

		//! return none transformed sphere
		const Sphere& GetSphere_local(void);

		//! return transformed sphere in final matrix
		Sphere& GetSphere_world(void);

		//! this will update all members and childes by calling their "update" function
		void Update(float elpsTime);

		//! this will call "draw" function of all members and childes
		void Draw(DWORD flag);

		//! use to communicate by this node
		UINT MsgProc(MT_ UINT msgType, void* data);

		//! save this node to the stream
		void Save(Stream& stream);

		//! load data of this node from the stream
		void Load(Stream& stream);

		//! create and return a same node just like this
		PNode Clone(void);

	private:

		//! this function apply parent matrix to local matrix
		void UpdateMatrix(void);

	public:
		DWORD							m_reserved;			//  used in internal functions of engine
		int								m_drawOrder;		//	use to control drawing
		PSector							m_Sector;			//  usable in scene manager

	private:
		String							m_Name;				//  the name of this node
		PNode							m_Parent;			//  parent of this node
		DWORD							m_Option;			//	I just feel that it will be useful
		DWORD							m_RegTypes;			//  registry types used to verify existing member types in the list

		floatQ							m_Quat;				//	quaternion of the node
		float3							m_Pos;				//	position of the node

		Matrix							m_Matrix_offset;	//	offset matrix of the node will useful for animation
		Matrix							m_Matrix_local;		//	base matrix of the node calculated by position and rotation
		Matrix							m_Matrix_world;		//  final matrix after applying parent matrix

		AABox							m_aaBox;			//  local bounding box of the node
		OBBox							m_obBox;			//  world bounding box of the node
		Sphere							m_Sphere;			//  bounding sphere of the node

		Array<PNode>					m_Child;			//	the array of node child
		Array<PNodeMember>				m_Member;			//	the array of the members

		UINT							m_UserTag;			//  user tag
		void*							m_UserData;			//  user data

		friend class NodeMember;

	};
	

} } // namespace sx { namespace core

#endif	//	GUARD_sxNode_HEADER_FILE