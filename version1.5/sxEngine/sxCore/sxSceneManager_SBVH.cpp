#include "sxSceneManager_SBVH.h"
#include "sxCamera.h"

namespace sx { namespace core {

#define SECTOR_SET_NODE(sector, node)	{ sector->m_node = node; node->m_Sector = sector; }

	typedef struct Sector
	{
		Sphere		m_sphere;
		Node	*	m_node;

		Sector	*	m_parent;
		Sector	*	m_left;
		Sector	*	m_right;

		Sector(void): m_node(NULL), m_parent(NULL), m_left(NULL), m_right(NULL) {}

		static FORCEINLINE void Free(Sector*& sector)
		{
			sx_callstack_push(Sector::Free(%.8x),(uint)sector);

			sx_delete(sector);
			sector = NULL;
		}

		static FORCEINLINE Sector* Create(void)
		{
			PSector sector = sx_new(Sector);
			sector->m_sphere.Set( float3(FLT_MAX, FLT_MAX, FLT_MAX), 0.0 );
			return sector;
		}
	}
	*PSector;


	FORCEINLINE void FillSphere(Sphere& dest, Sphere& s1, Sphere& s2)
	{
		float3 v( s1.x-s2.x, s1.y-s2.y, s1.z-s2.z );
		float  l = v.Length();

		//////////////////////////////////////////////////////////////////////////
		//  verify that spheres in not completely inside together

		//  check to see if s1 is inside of s2
		if (l + s1.r < s2.r)
		{
			dest = s2;
			return;
		}
		//  check to see if s2 is inside of s1
		else if (l + s2.r < s1.r)
		{
			dest = s1;
			return;
		}
		else
		{
			if (l<EPSILON) l=EPSILON;
			v.x/=l; v.y/=l; v.z/=l;

			float3 c1 = s1.center + (v * s1.r);
			dest.r = (l + s1.r + s2.r) * 0.5f;
			v *= dest.r;
			dest.center = c1 - v;
		}
	}

	FORCEINLINE void UpdateSphere(const PSector root)
	{
		if (!root) return;
		if ( root->m_node )
			root->m_sphere = root->m_node->GetSphere_world();
		else
			FillSphere( root->m_sphere, root->m_left->m_sphere, root->m_right->m_sphere );
	}

	FORCEINLINE void UpdateSphereParent(const PSector root)
	{
		if (!root || !root->m_parent) return;
		UpdateSphere( root->m_parent );
		UpdateSphereParent( root->m_parent );
	}

	FORCEINLINE float distance_Point_Point_sqr(const float3& p1, const float3& p2)
	{
		float3 d( p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
		return d.x*d.x + d.y*d.y + d.z*d.z;
	}

	FORCEINLINE float distance_Sector_Point_sqr(const PSector sector, const float3& pos)
	{
		if ( !sector ) return math::MAX;
		float3 d( sector->m_sphere.x - pos.x, sector->m_sphere.y - pos.y, sector->m_sphere.z - pos.z );
		return d.x*d.x + d.y*d.y + d.z*d.z;
	}

	FORCEINLINE float distance_Sector_Sector_sqr(const PSector s1, const PSector s2)
	{
		if ( !s1 || !s2) return math::MAX;
		float3 d( s1->m_sphere.x - s2->m_sphere.x, s1->m_sphere.y - s2->m_sphere.y, s1->m_sphere.z - s2->m_sphere.z );
		return d.x*d.x + d.y*d.y + d.z*d.z;
	}

	FORCEINLINE void FindNearestSectorTo(const PSector root, const Sphere& sphere, IN_OUT PSector& result)
	{
		sx_callstack_push(FindNearestSectorTo());

		if ( !root ) return;

		if ( !sphere.Intersect(root->m_sphere, NULL) || root->m_node)
		{
			if ( distance_Point_Point_sqr(root->m_sphere.center, sphere.center) < distance_Point_Point_sqr(result->m_sphere.center, sphere.center) )
				result = root;
			return;
		}

		if (root->m_left && root->m_left->m_node && root->m_right->m_node)
		{
			float dis_left = distance_Sector_Point_sqr( root->m_left, sphere.center);
			float dis_right = distance_Sector_Point_sqr( root->m_right, sphere.center);
			float dis_left_right = distance_Sector_Sector_sqr(root->m_left, root->m_right);
			PSector res = NULL;

			if (root->m_left && dis_left<dis_left_right && dis_left<dis_right)
				res = root->m_left;
			else if (root->m_right && dis_right<dis_left_right && dis_right<dis_left)
				res = root->m_right;
			else
				res = root;

			if ( distance_Point_Point_sqr(res->m_sphere.center, sphere.center) < distance_Point_Point_sqr(result->m_sphere.center, sphere.center) )
				result = res;
		}
		else
		{
			FindNearestSectorTo( root->m_left, sphere, result );
			FindNearestSectorTo( root->m_right, sphere, result );
		}
	}

	FORCEINLINE void InsertNode(PSector& root, PSector sector, PNode pNode)
	{
		sx_callstack_push(InsertNode());

		if (sector->m_node)
		{
			sector->m_left = Sector::Create();
			sector->m_left->m_parent = sector;
			SECTOR_SET_NODE(sector->m_left, sector->m_node);
			sector->m_left->m_sphere = sector->m_node->GetSphere_world();

			sector->m_right = Sector::Create();
			sector->m_right->m_parent = sector;
			SECTOR_SET_NODE(sector->m_right, pNode);
			sector->m_right->m_sphere = pNode->GetSphere_world();

			sector->m_node = NULL;

			UpdateSphere(sector);
			UpdateSphereParent(sector);
		}
		else
		{
			//  save old parent
			PSector parent = sector->m_parent;

			//  create new sector for this new node
			PSector newSector = Sector::Create();
			SECTOR_SET_NODE(newSector, pNode);
			newSector->m_sphere = pNode->GetSphere_world();

			//  create new parent for these sectors
			PSector newParent = Sector::Create();
			newParent->m_parent = parent;

			sector->m_parent = newParent;
			newSector->m_parent = newParent;
			newParent->m_left = sector;
			newParent->m_right = newSector;

			//  update old parent's childes
			if (parent)
			{
				if (parent->m_left == sector)
					parent->m_left = newParent;
				else if (parent->m_right == sector)
					parent->m_right = newParent;

				UpdateSphere(parent->m_left);
				UpdateSphere(parent->m_right);
				UpdateSphere(parent);
				UpdateSphereParent(parent);
			}
			else
			{
				root = newParent;
				UpdateSphere(sector);
				UpdateSphereParent(sector);
			}
		}
	}

	FORCEINLINE void DeleteSector(PSector& sector)
	{
		sx_callstack_push(DeleteSector(%.8x), (uint)sector);

		if (!sector) return;
		DeleteSector(sector->m_left);
		DeleteSector(sector->m_right);
		Sector::Free(sector);
	}

	FORCEINLINE void _GetAllNodes(const PSector sector, ArrayPNode_abs& nodeList)
	{
		sx_callstack_push(_GetAllNodes());

		if (!sector) return;

		if (sector->m_node)
		{
			nodeList.PushBack( sector->m_node );
		}
		else
		{
			_GetAllNodes( sector->m_left, nodeList );
			_GetAllNodes( sector->m_right, nodeList );
		}
	}

	FORCEINLINE void _GetNodesByFrustum(const PSector sector, Frustum& frustom, ArrayPNode_abs& nodeList, DWORD nmType)
	{
		if ( !sector ) return;

		//  return if sphere is fully out side of the frustum
		for (int i=0; i<6; i++)
		{
			if ( frustom.p[i].Distance( sector->m_sphere.center ) < -sector->m_sphere.r ) 
				return;
		}

		if (sector->m_node)
		{
			if ( nmType==NMT_ALL || sector->m_node->MemberTypeExist(nmType) )
			{
				// check the visibility via intersect between box and frustum
				if ( sector->m_node->GetBox_world().IntersectFrustum(frustom) )
					nodeList.PushBack( sector->m_node );
			}
		}
		else
		{
			_GetNodesByFrustum( sector->m_left, frustom, nodeList, nmType );
			_GetNodesByFrustum( sector->m_right, frustom, nodeList, nmType );
		}
	}

	FORCEINLINE void _GetNodesBySphere(const PSector sector, const Sphere& sphere, ArrayPNode_abs& nodeList, DWORD nmType, DWORD nodeTag)
	{
		if ( !sector  || !sphere.Intersect(sector->m_sphere, NULL) ) return;

		if (sector->m_node)
		{
			if ( !nodeTag || nodeTag & sector->m_node->GetUserTag() )
			{
				if ( nmType==NMT_ALL || sector->m_node->MemberTypeExist(nmType) )
				{
					nodeList.PushBack( sector->m_node );
				}
			}
		}
		else
		{
			_GetNodesBySphere( sector->m_left, sphere, nodeList, nmType, nodeTag  );
			_GetNodesBySphere( sector->m_right, sphere, nodeList, nmType, nodeTag );
		}
	}

	FORCEINLINE void _GetNodeByRay(const PSector sector, const Frustum& frustom, msg_IntersectRay& msgRay)
	{
		if ( !sector ) return;

		//  return if sphere is fully out side of the frustum
		for (int i=0; i<6; i++)
		{
			if ( frustom.p[i].Distance( sector->m_sphere.center ) < -sector->m_sphere.r ) 
				return;
		}

		if (sector->m_node)
		{
			// check the visibility via intersect between box and frustum
			bool  vis = true;
			OBBox box = sector->m_node->GetBox_world();

			for (int i=0; i<6 && vis; i++)
			{
				vis = false;
				for (int j=0; j<8; j++)
				{
					if ( frustom.p[i].Distance( box.v[j] ) >= 0 )
					{
						vis = true;
						i++;
						break;
					}
				}
			}

			//  now intersect ray width members
			sector->m_node->MsgProc(MT_INTERSECT_RAY, &msgRay);
		}
		else
		{
			_GetNodeByRay( sector->m_left , frustom, msgRay );
			_GetNodeByRay( sector->m_right, frustom, msgRay );
		}

	}

	void _GetNodesByName(const PSector sector, const String& name, ArrayPNode_abs& nodeList)
	{
		if ( !sector ) return;

		if (sector->m_node)
		{
			if ( name == sector->m_node->GetName() )
			{
				nodeList.PushBack( sector->m_node );
			}
		}
		else
		{
			_GetNodesByName(sector->m_left, name, nodeList);
			_GetNodesByName(sector->m_right, name, nodeList);
		}
	}

	void DrawSector(const PSector sector, DWORD flag)
	{
		if ( !sector || !d3d::Device3D::IsReady() ) return;

		if (sector->m_node)
		{
			d3d::Device3D::Matrix_World_Set(math::MTRX_IDENTICAL);
			sector->m_node->Draw(flag);
		}
		
		if (flag & SX_DRAW_BOUNDING_SPHERE)
		{
			d3d::Device3D::Matrix_World_Set(math::MTRX_IDENTICAL);
			d3d::UI3D::DrawWiredSphere( sector->m_sphere, sector->m_parent==NULL ? 0xffffff00 : 0xffaa3333);
		}
	}

	void DrawAllSectors(const PSector root, const Sphere& sphere, DWORD flag)
	{
		if (!root || !sphere.Intersect(root->m_sphere, NULL) ) return;

		DrawAllSectors(root->m_left, sphere, flag);
		DrawAllSectors(root->m_right, sphere, flag);
		DrawSector(root, flag);
	}


	//////////////////////////////////////////////////////////////////////////
	//  SCENE MANAGER - SPHERICAL BOUNDING VOLUME HIERARCHY
	//////////////////////////////////////////////////////////////////////////
	SceneManager_SBVH::SceneManager_SBVH( void ) : SceneManager(), m_root(NULL), m_count(0)
	{

	}

	SceneManager_SBVH::~SceneManager_SBVH( void )
	{
		Clear();
	}

	void SceneManager_SBVH::Clear( void )
	{
		sx_callstack_push(SceneManager_SBVH::Clear());

		DeleteSector(m_root);
		m_count = 0;
	}

	int SceneManager_SBVH::Count( void )
	{
		return m_count;
	}

	void SceneManager_SBVH::Add( const PNode pNode )
	{
		sx_callstack_push(SceneManager_SBVH::Add());

		sx_assert(pNode);
		sx_assert(!pNode->m_Sector);

		//  this is the first node ?
		if (m_count == 0)
		{
			m_root = Sector::Create();
			SECTOR_SET_NODE(m_root, pNode);
			m_root->m_sphere = pNode->GetSphere_world();
			m_count++;
			return;
		}
		m_count++;

		//  find the nearest sector
		Sector sec;
		sec.m_sphere.Set( float3(math::MAX, math::MAX, math::MAX), 0.0 );
		PSector sector = &sec;
		FindNearestSectorTo( m_root, pNode->GetSphere_world(), sector );
		InsertNode(m_root, sector, pNode);		
	}

	void SceneManager_SBVH::Remove( const PNode pNode )
	{
		sx_callstack_push(SceneManager_SBVH::Remove());

		sx_assert(pNode);

		//  this is the final node ?
		if ( m_count == 1 && m_root->m_node == pNode )
		{
			pNode->m_Sector = NULL;
			Sector::Free(m_root);
			m_count--;
			return;
		}

		//  find the nearest sector
		PSector sector = pNode->m_Sector;
		if ( sector )
		{
			m_count--;

			PSector parent	= sector->m_parent;
			PSector neighbor = parent->m_left == sector ? parent->m_right : parent->m_left;
			
			if ( parent->m_parent )
			{
				PSector grandpa	= parent->m_parent;
				if ( grandpa->m_left == parent )
					grandpa->m_left = neighbor;
				else
					grandpa->m_right = neighbor;
				neighbor->m_parent = grandpa;
			}
			else 
			{
				m_root = neighbor;
				m_root->m_parent = NULL;
			}

			Sector::Free(sector);
			Sector::Free(parent);

			UpdateSphere( neighbor );
			UpdateSphereParent( neighbor );
		}

		pNode->m_Sector = NULL;
	}

	void SceneManager_SBVH::GetAllNodes( IN_OUT ArrayPNode_abs& nodeList )
	{
		sx_callstack_push(SceneManager_SBVH::GetAllNodes());

		if (m_count)
			_GetAllNodes( m_root, nodeList );
	}

	void SceneManager_SBVH::GetNodesByFrustum( const Frustum& frustum, IN_OUT ArrayPNode_abs& nodeList, DWORD nmType /*= NMT_ALL*/ )
	{
		Frustum fr;
		for (int i=0; i<6; i++)
			fr.p[i].Normalize( frustum.p[i] );

		_GetNodesByFrustum( m_root, fr, nodeList, nmType );
		
	}

	void SceneManager_SBVH::GetNodesByArea( const float3& position, const float threshold, IN_OUT ArrayPNode_abs& nodeList, DWORD nmType, DWORD nodeTag )
	{
		Sphere sphere( position, threshold );
		_GetNodesBySphere( m_root, sphere, nodeList, nmType, nodeTag );
	}

	void SceneManager_SBVH::GetNodesByName( const WCHAR* nodeName, IN_OUT ArrayPNode_abs& nodeList )
	{
		String name = nodeName;
		_GetNodesByName(m_root, name, nodeList);
	}

	void SceneManager_SBVH::GetNodeByRay( IN_OUT msg_IntersectRay& msgRay )
	{
		Camera cam;
		cam.Eye = msgRay.ray.pos;
		cam.At = msgRay.ray.pos + msgRay.ray.dir;
		//cam.Up.Set( 1.0f-msgRay.ray.dir.x, 1.0f-msgRay.ray.dir.y, 1.0f-msgRay.ray.dir.z );
		//cam.Up.Normalize( cam.Up );
		//cam.Up.Cross( msgRay.ray.dir, cam.Up );
		cam.Up.Set(0, 1, 0);
		cam.Aspect = 1.0f;
		cam.FOV = 0.01f;
		cam.Far = 5000.0f;

		Frustum frustum;
		cam.GetFrustum(frustum);
		frustum.Normalize(frustum);
		_GetNodeByRay(m_root, frustum, msgRay);
	}

	void SceneManager_SBVH::UpdateNode( const PNode pNode )
	{
		PSector sector = pNode->m_Sector;
		if ( !sector ) return;

		if ( sector->m_parent)
		{
			PSector neighbor = sector->m_parent->m_left == sector ? sector->m_parent->m_right : sector->m_parent->m_left;
			
			// we update node in the tree if node scat from his neighbor with specified threshold
			// currently I use radius of bounding sphere of the node as the threshold
			if ( distance_Sector_Sector_sqr(neighbor, sector) < distance_Sector_Point_sqr(neighbor, pNode->GetPosition_local()) )
			{
				if ( distance_Point_Point_sqr(sector->m_sphere.center, pNode->GetPosition_local()) > (pNode->GetSphere_local().r) )
				{
					Remove( pNode );
					Add( pNode );
				}
				else
				{
					sector->m_sphere.Cover( pNode->GetSphere_world() );
					UpdateSphereParent(sector);
				}
			}
			else
			{
				sector->m_sphere = pNode->GetSphere_world();
				UpdateSphereParent(sector);
			}
		}
		else 
		{
			sector->m_sphere = pNode->GetSphere_world();
			UpdateSphereParent(sector);
		}
	}

	void SceneManager_SBVH::Debug_Draw( const float3& position, const float threshold, DWORD flag )
	{
		d3d::Device3D::RS_ZWritable(false);
		Sphere sph(position, threshold);
		d3d::Device3D::Matrix_World_Set( math::MTRX_IDENTICAL );
		d3d::UI3D::DrawWiredSphere( sph, 0xffffffff );
		DrawAllSectors(m_root, sph, flag);
		d3d::Device3D::RS_ZWritable(true);
	}

}} // namespace sx { namespace core {