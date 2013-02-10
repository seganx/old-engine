/********************************************************************
created:	2011/03/15
filename: 	sxSceneManager_SBVH.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain a simple scene manager that use
			Bounding Volume Hierarchy (BVH) with bound sphere represent 
			as volume of nodes in the scene.

			NOTE: the scene managers should manage only the parent nodes.
			modifying the child nodes and/or member of nodes in scene manager 
			is highly not recommended. to access the child nodes or member of
			a node, find the parent and access them directly from the parents.
*********************************************************************/
#ifndef GUARD_sxSceneManager_SBVH_HEADER_FILE
#define GUARD_sxSceneManager_SBVH_HEADER_FILE

#include "sxSceneManager.h"


namespace sx { namespace core {

	//  forward declaration
	typedef struct Sector *PSector;

	/*! use Bounding Volume Hierarchy (BVH) with bound sphere represent as volume of nodes in the scene.*/
	class SEGAN_API SceneManager_SBVH : public SceneManager
	{
		SEGAN_STERILE_CLASS(SceneManager_SBVH);

	public:
		SceneManager_SBVH(void);
		virtual ~SceneManager_SBVH(void);

		/*!
		clear all the nodes from Scene
		NOTE: this function does NOT delete any node structure and just remove node's pointer from scene manager
		*/
		void Clear(void);

		//! return the number of nodes in the scene
		int Count(void);

		//! add new node to scene.
		void Add(const PNode pNode);

		/*!
		remove node from Scene
		NOTE: this function does NOT delete the node structure and just remove node's pointer from scene manager
		*/
		void Remove(const PNode pNode);

		//! fill the node list by all nodes in the scene
		void GetAllNodes(IN_OUT ArrayPNode_abs& nodeList);

		/*!
		fill the node list by founded nodes in the frustum. use node types as filter
		NOTE: this function called by many parts of core/rendering/AI/etc and should be fast as possible.
		*/
		void GetNodesByFrustum(const Frustum& frustum, IN_OUT ArrayPNode_abs& nodeList, DWORD nmType = NMT_ALL);

		/*!
		fill the node list by founded nodes in specified area. position and threshold or radius. use node types as filter
		also the application can use nodeTag to additional filter for faster search in scene
		NOTE: this function called by many parts of core/rendering/AI/etc and should be fast as possible.
		*/
		void GetNodesByArea(const float3& position, const float threshold, IN_OUT ArrayPNode_abs& nodeList, DWORD nmType = NMT_ALL, DWORD nodeTag = 0);

		/*!
		search the scene and fill the node list by founded node with specified name
		NOTE: this function is not fast as the others. however using Hash/CRC code can increase performance
		*/
		void GetNodesByName(const WCHAR* nodeName, IN_OUT ArrayPNode_abs& nodeList);

		//!	send a ray to the scene and fill ray structure as result
		void GetNodeByRay(IN_OUT msg_IntersectRay& msgRay);

		//! update transformed node to update scene structure
		void UpdateNode(const PNode pNode);

		//! draw sectors spheres to debug scene
		void Debug_Draw(const float3& position, const float threshold, DWORD flag);

	private:
		PSector		m_root;
		int			m_count;
	};
	typedef SceneManager_SBVH *PSceneManager_SBVH;

}} // namespace sx { namespace core {

#endif // GUARD_sxSceneManager_SBVH_HEADER_FILE