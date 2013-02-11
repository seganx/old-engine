#include "sxPathNode.h"
#include "sxNode.h"
#include "../sxCommon/sxCommon.h"

#define PATHNODE_INLIST_OPEN		0x00000001
#define PATHNODE_INLIST_CLOSE		0x00000002
#define PATHNODE_LAYER				SEGAN_2TH_BYTEOF(reserved)

static const UINT	pathnodeFileID = MAKEFOURCC('P', 'A', 'T', 'H');
static AABox		pathnodeBox( float3(-0.3f, -0.3f, -0.3f), float3(0.3f, 0.3f, 0.3f) );
static Sphere		pathnodeSphere( float3(0.0f, 0.0f, 0.0f), 0.3f );

static int s_pathnodeID		= 1;	//  use to find right node on loading
static int s_pathnodeLayer	= 0;	//  use to draw/select path layer


namespace sx { namespace core {

	//////////////////////////////////////////////////////////////////////////
	//  some internal functions used to path finding
	static PPathNode openlist_root=NULL, closelist_root=NULL;
	class Path_Internal
	{
	public:
		static void openlist_add(PPathNode node)
		{
			if ( node->reserved & PATHNODE_INLIST_OPEN )
				return;

			node->reserved |= PATHNODE_INLIST_OPEN;

			if (openlist_root)
			{
				PPathNode cur = openlist_root;
				while (cur)
				{
					if (node->score.z > cur->score.z)
					{
						if (cur->next)
						{
							cur = cur->next;
						}
						else
						{
							cur->next = node;
							node->prev = cur;
							node->next = NULL;
							return;
						}
					}
					else
					{
						if (cur == openlist_root)
						{
							node->prev = NULL;
							node->next = cur;
							cur->prev = node;
							openlist_root = node;
							return;
						}
						else
						{
							node->prev = cur->prev;
							node->next = cur;
							cur->prev = node;
							node->prev->next = node;
							return;
						}
					}
				}
			}
			else
			{
				node->prev = NULL;
				node->next = NULL;
				openlist_root = node;
				return;
			}
		}

		static PPathNode openlist_pop(void)
		{
			if (openlist_root)
			{
				PPathNode tmp = openlist_root;
				openlist_root = openlist_root->next;
				if (openlist_root) openlist_root->prev = NULL;
				SEGAN_SET_REM(tmp->reserved, PATHNODE_INLIST_OPEN);
				return tmp;
			}
			else return NULL;
		}

		static void openlist_clear(void)
		{
			while (openlist_root)
			{
				SEGAN_SET_REM(openlist_root->reserved, PATHNODE_INLIST_OPEN);
				openlist_root = openlist_root->next;
			}
			openlist_root = NULL;
		}

		static void ConstructPath(PPathNode end, ArrayPPathNode& pathList)
		{
			PPathNode node = end;
			while (node)
			{
				pathList.PushFront(node);
				node = node->path;
			}
		}

		static float heuristic_cost(PPathNode start, PPathNode goal)
		{
			float3 d(
				start->GetPosition().x-goal->GetPosition().x, 
				start->GetPosition().y-goal->GetPosition().y, 
				start->GetPosition().z-goal->GetPosition().z
				);

			return d.x*d.x + d.y*d.y + d.z*d.z;
		}
	};

	//  use simple array to hold created path nodes. this is useful in search path nodes by position
	static ArrayPPathNode poolPathNodes;
	PathNode::PathNode() : NodeMember(), m_ID(0), score(0,0,0), next(0), prev(0), path(0), reserved(0)
	{
		m_Type = NMT_PATHNODE;
		m_Name = L"PathNode";
		poolPathNodes.PushBack(this);
		PATHNODE_LAYER = s_pathnodeLayer;
	}

	PathNode::~PathNode()
	{
		sx_callstack();

		//  remove connections
		while ( m_Neighbor.Count() )
			DisconnectFrom( m_Neighbor[0] );			

		poolPathNodes.Remove(this);
	}


	void PathNode::Update( float elpsTime )
	{
		//float ftime	= elpsTime * 0.001f;
	}

	void PathNode::Draw( DWORD flag )
	{
		if ( flag & SX_DRAW_PATH && m_Owner )
		{
			if ( s_pathnodeLayer && s_pathnodeLayer != PATHNODE_LAYER ) return;

			sx::d3d::Device3D::RS_ZEnabled(false);
			sx::d3d::Device3D::RS_ZWritable(false);
			sx::d3d::Device3D::RS_WireFrame(false);
			sx::d3d::Device3D::RS_Alpha( 0 );

			DWORD color = 0xffff0000;
			switch ( PATHNODE_LAYER )
			{
			case 0:	color = 0xffcc0000; break;
			case 1:	color = 0xffdddd00; break;
			case 2:	color = 0xff00cc00; break;
			case 3:	color = 0xff00cccc; break;
			case 4:	color = 0xff0000cc; break;
			case 5:	color = 0xffcc00cc; break;
			case 6:	color = 0xffccaacc; break;
			case 7:	color = 0xffaaccaa; break;
			case 8:	color = 0xffaccaaa; break;
			}

			//  draw a simple sphere as node
			d3d::Device3D::Matrix_World_Set( math::MTRX_IDENTICAL );
			pathnodeSphere.center = m_Owner->GetPosition_world();
			d3d::UI3D::DrawSphere( pathnodeSphere, color );

			//  draw connections
			for (int i=0; i<m_Neighbor.Count(); i++)
			{
				PPathNode pathNode = m_Neighbor[i];
				if ( pathNode->GetOwner() )
				{
					float3 v = pathNode->GetOwner()->GetPosition_world();
					d3d::UI3D::DrawLine( pathnodeSphere.center, v );
				}
			}
		}
	}

	UINT PathNode::MsgProc( UINT msgType, void* data )
	{
		switch (msgType)
		{
		case MT_COVERBOX:
			{
				(static_cast<PAABox>(data))->CoverAA( pathnodeBox );
			}
			break;

		case MT_GETBOX_LOCAL:
			{
				*(static_cast<PAABox>(data)) = pathnodeBox;
				return 0;
			}
			break;

		case MT_GETBOX_WORLD:
			{
				(static_cast<POBBox>(data))->Transform( pathnodeBox, m_Owner->GetMatrix_world() );
				return 0;
			}
			break;

		case MT_GETSPHERE_LOCAL:
			{
				*(static_cast<PSphere>(data)) = pathnodeSphere;
				return 0;
			}
			break;

		case MT_GETSPHERE_WORLD:
			{
				(static_cast<PSphere>(data))->Set( m_Owner->GetPosition_world(), pathnodeSphere.r );
				return 0;
			}
			break;

		case MT_INTERSECT_RAY:
			{
				if ( s_pathnodeLayer && s_pathnodeLayer != PATHNODE_LAYER ) return msgType;

				// test ray intersection and return quickly if test failed
				msg_IntersectRay* ray = static_cast<msg_IntersectRay*>(data);
				if ( !SEGAN_SET_HAS(ray->type, NMT_PATHNODE) ) return msgType;

				//  test sphere
				float3 hitPoint, hitNorm;
				Sphere sphere(m_Owner->GetPosition_world(), pathnodeSphere.r);
				if ( ray->ray.Intersect_Sphere( sphere, &hitPoint, &hitNorm) )
				{
					float dis = hitPoint.Distance( ray->ray.pos );
					ray->pushToResults(dis, hitPoint, hitNorm, m_Owner, this);
				}
			}
			break;
		}

		return msgType;
	}

	void PathNode::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, pathnodeFileID);

		// write version
		int version = 3;
		SEGAN_STREAM_WRITE(stream, version);

		//  save default things
		NodeMember::Save( stream );

		//	write unique id
		m_ID = s_pathnodeID++;
		SEGAN_STREAM_WRITE(stream, m_ID);

		// write positions of neighbors
		int n = m_Neighbor.Count();
		SEGAN_STREAM_WRITE(stream, n);
		for (int i=0; i<n; i++)
		{
			PPathNode pnode = m_Neighbor[i];
			float3 pos = pnode->GetPosition();
			int id = pnode->m_ID ? pnode->m_ID : -1;

			SEGAN_STREAM_WRITE(stream, pos);
			SEGAN_STREAM_WRITE(stream, id);
		}

		//  write layer code
		BYTE layer = PATHNODE_LAYER;
		SEGAN_STREAM_WRITE(stream, layer);
	}

	void PathNode::Load( Stream& stream )
	{
		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if (id != pathnodeFileID)
		{
			sxLog::Log(L"Incompatible file format for loading path node !");
			return;
		}

		// write version
		int version = 0;
		SEGAN_STREAM_READ(stream, version);

		if (version == 1)
		{
			//  load default things
			NodeMember::Load( stream );

			//	read quads
			int n = 0; float3 pos;
			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n; i++)
			{
				SEGAN_STREAM_READ(stream, pos);
				PPathNode pathNode = FindNeighbor(pos);
				ConnectTo( pathNode );
			}
		}
		else if (version == 2)
		{
			//  load default things
			NodeMember::Load( stream );

			//	read quads
			int n = 0; float3 pos;
			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n; i++)
			{
				SEGAN_STREAM_READ(stream, pos);
				PPathNode pathNode = FindNeighbor(pos);
				ConnectTo( pathNode );
			}

			//  read layer code
			BYTE layer = 0;
			SEGAN_STREAM_READ(stream, layer);
			PATHNODE_LAYER = layer;
		}
		else if ( version == 3 )
		{
			//  load default things
			NodeMember::Load( stream );

			//	write unique id
			SEGAN_STREAM_READ(stream, m_ID);
			//m_ID = s_pathnodeID++;

			//	read neighbors data
			int n = 0; float3 pos; DWORD id;
			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n; i++)
			{
				SEGAN_STREAM_READ(stream, pos);
				SEGAN_STREAM_READ(stream, id);

				//id + s_pathnodeID - 1;

				PPathNode pathNode = FindNeighbor(pos, id);
				ConnectTo( pathNode );
			}

			//  read layer code
			BYTE layer = 0;
			SEGAN_STREAM_READ(stream, layer);
			PATHNODE_LAYER = layer;
		}

		if ( m_Owner )
			m_Owner->UpdateBoundingVolumes();
	}

	const float3& PathNode::GetPosition( void )
	{
		static float3 tmp(0,0,0);

		if (m_Owner)
			return m_Owner->GetPosition_world();
		else
			return tmp;
	}

	void PathNode::ConnectTo( PathNode* other )
	{
		if ( !other || other==this ) return;

		if ( m_Neighbor.IndexOf(other) < 0 )
		{
			m_Neighbor.PushBack(other);
			other->ConnectTo(this);
		}
	}

	void PathNode::DisconnectFrom( PathNode* other )
	{
		if (!other || other==this) return;

		if ( m_Neighbor.Remove(other) )
			other->DisconnectFrom(this);		
	}

	sx::core::PPathNode PathNode::FindNeighbor( float3& pos, DWORD id /*= 0*/ )
	{
		const float threshold = 0.25f;
		for (int i=0; i<poolPathNodes.Count(); i++)
		{
			PPathNode pnode = poolPathNodes[i];

			float3 p = pnode->GetPosition();
			if ( abs(p.x - pos.x)<threshold && abs(p.z - pos.z)<threshold && abs(p.y - pos.y)<threshold )
			{

				if ( id )
				{
					if ( pnode->m_ID == id )
						return pnode;
				}
				else return pnode;
			}
		}
		return NULL;
	}

	bool PathNode::FindPath( PPathNode start, PPathNode end, ArrayPPathNode& pathNodeList )
	{
		if (!start || !end || start == end) return true;
		ArrayPPathNode closedList;

		start->next = NULL;
		start->prev = NULL;
		start->path = NULL;
		openlist_root = start;							// clear open list and add start node
		start->score.x = 0;								// cost from start along best known path
		start->score.y = Path_Internal::heuristic_cost(start, end);	// heuristic cost from this node to the end
		start->score.z = start->score.y;				// estimated total cost from start to goal through

		while( openlist_root )
		{
			PPathNode node = Path_Internal::openlist_pop();
			if ( node == end )
			{
				Path_Internal::ConstructPath(node, pathNodeList);

				Path_Internal::openlist_clear();
				for (int i=0; i<closedList.Count(); i++)
					SEGAN_SET_REM( closedList[i]->reserved, PATHNODE_INLIST_CLOSE );
				return true;
			}

			// mark node as closed list
			node->reserved |= PATHNODE_INLIST_CLOSE;
			closedList.PushBack(node);

			for (int i=0; i<node->m_Neighbor.Count(); i++)
			{
				PPathNode pn = node->m_Neighbor[i];
				if (pn->reserved & PATHNODE_INLIST_CLOSE)
					continue;

				float tentative_g_score = node->score.x + node->GetPosition().Distance( pn->GetPosition() );

				bool tentative_is_better = false;
				if ( !SEGAN_SET_HAS(pn->reserved, PATHNODE_INLIST_OPEN) )
				{
					Path_Internal::openlist_add(pn);
					tentative_is_better = true;
				}
				else if ( tentative_g_score < pn->score.x )
				{
					tentative_is_better = true;
				}
				
				if (tentative_is_better)
				{
					pn->path = node;
					pn->score.x = tentative_g_score;
					pn->score.y = Path_Internal::heuristic_cost(pn, end);
					pn->score.z = pn->score.x + pn->score.y;
				}
			}
		}

		Path_Internal::openlist_clear();
		for (int i=0; i<closedList.Count(); i++)
			SEGAN_SET_REM( closedList[i]->reserved, PATHNODE_INLIST_CLOSE );
		return false;
	}


	//////////////////////////////////////////////////////////////////////////
	//	MANAGER
	//////////////////////////////////////////////////////////////////////////
	int& PathNode::Manager::DebugLayer( void )
	{
		return s_pathnodeLayer;
	}

}}  //  namespace sx { namespace core {