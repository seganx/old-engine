#include "sxNode.h"
#include "sxNodeMember.h"
#include "sxScene.h"


#define USERTAG_DELETE	12345

static const UINT nodeFileID = MAKEFOURCC('N', 'O', 'D', 'E');
static sint	s_nodeCount = 0;

namespace sx { namespace core {

	//	use to decrease array allocation
	class ArrayStore
	{
	public:
		ArrayStore(void)
		{
			for ( int i=0; i<128; i++ )
				m_pool.Push( sx_new(ArrayPNode(512) ) );
		}

		~ArrayStore()
		{
			for (;;)
			{
				ArrayPNode* nodearray = null;
				if ( m_pool.Pop( nodearray ) )
				{
					sx_delete( nodearray );
				}
				else
				{
					break;
				}
			}
		}

		ArrayPNode& Pop( void )
		{
			sx_assert(m_pool.Count());
			ArrayPNode* tmp = m_pool.Top();
			m_pool.Pop();
			tmp->Clear();
			return *tmp;
		}

		void Push( ArrayPNode& nodeArray )
		{
			m_pool.Push( &nodeArray );
		}

		Stack<ArrayPNode*>	m_pool;
	};
	static ArrayStore* s_arrayStore = null;

	static float3 camDir(0,0,0);
	int CompareNodeDistance(const PNode& n1, const PNode& n2)
	{
		const float3& p1 = n1->GetPosition_world();
		const float3& p2 = n2->GetPosition_world();
		const float d1 = camDir.x*p1.x + camDir.y*p1.y + camDir.z*p1.z;
		const float d2 = camDir.x*p2.x + camDir.y*p2.y + camDir.z*p2.z;
		return (d1 < d2) ? -1 : (d1 > d2) ? 1 : 0;
	}

	Node::Node()
		: m_reserved(0)
		, m_drawOrder(0)
		, m_Sector(NULL)
		, m_Parent(NULL)
		, m_Option(SX_NODE_SELECTABLE)
		, m_RegTypes(0)
		, m_Pos(0, 0, 0)
		, m_Quat(0, 0, 0, 1.0f)
		, m_Matrix_offset(math::MTRX_IDENTICAL)
		, m_Matrix_local(math::MTRX_IDENTICAL)
		, m_Matrix_world(math::MTRX_IDENTICAL)
		, m_UserTag(0)
		, m_UserData(0)
	{
		m_aaBox.Min.Set(-0.1f, -0.1f, -0.1f);
		m_aaBox.Max.Set( 0.1f,  0.1f,  0.1f);
		m_Sphere.ComputeByAABox( m_aaBox );

		if ( !s_nodeCount )
			s_arrayStore = sx_new( ArrayStore );
		s_nodeCount++;
	}

	Node::~Node()
	{
		if ( m_Sector )
			Scene::RemoveNode(this);

		//  use this trick to prevent some useless operation 
		m_UserTag = USERTAG_DELETE;

		//  clear himself
		Clear();

		//  detach from parent
		m_Option=0;
		if (m_Parent)
		{
			m_Parent->m_Child.Remove(this);
			if ( m_Parent->m_UserTag != USERTAG_DELETE )
				m_Parent->UpdateBoundingVolumes();
		}

		s_nodeCount--;
		if ( !s_nodeCount )
			sx_delete_and_null( s_arrayStore );
	}

	void Node::Clear( void )
	{
		sx_callstack();

		//  members will detach themselves on destruction. so we just delete them straight forward
		for ( int i=0; i<m_Member.Count(); i++ )
		{
			PNodeMember member = m_Member[ i ];
			member->m_Owner = null;
			sx_delete( member );
		}
		m_Member.Clear();

		//  nodes will detach themselves on destruction. so we just delete them straight forward 
		while ( m_Child.Count() )
		{
			Node* node = m_Child[ m_Child.Count() - 1 ];

			//  verify that the node is not absolute. absolute nodes will not delete and will just remove simply
			if ( node->HasProperty(SX_NODE_ABSOLUTE) )
			{
				m_Child.Remove(node);
			}
			else
			{
				sx_delete_and_null( node );
			}
		}
		m_Child.Clear();

		m_Name.Clear();
	}

	FORCEINLINE void Node::SetName( const WCHAR* pwcName )
	{
		m_Name = pwcName;
	}

	FORCEINLINE const WCHAR* Node::GetName( void )
	{
		return m_Name;
	}

	FORCEINLINE void Node::SetUserTag( UINT tag )
	{
		m_UserTag = tag;
	}

	FORCEINLINE UINT Node::GetUserTag( void )
	{
		return m_UserTag;
	}

	FORCEINLINE void Node::SetUserData( void* data )
	{
		m_UserData = data;
	}

	void* Node::GetUserData( void )
	{
		return m_UserData;
	}

	FORCEINLINE void Node::AddProperty( DWORD prop )
	{
		SEGAN_SET_ADD(m_Option, prop);
	}

	FORCEINLINE void Node::RemProperty( DWORD prop )
	{
		SEGAN_SET_REM(m_Option, prop);
	}

	FORCEINLINE bool Node::HasProperty( DWORD prop )
	{
		return (m_Option & prop)!=0;
	}

	void Node::SetParent( const PNode pParent )
	{
		if ( pParent==this || (m_Parent && (m_Option & SX_NODE_LOYAL)) ) return;

		//  detach from current parent
		if (m_Parent)
		{
			m_Parent->m_Child.Remove(this);
			if ( !SEGAN_SET_HAS(m_Option, SX_NODE_SOLITARY) ) 
				m_Parent->UpdateBoundingVolumes();
		}

		m_Parent = pParent;

		//  attach to the new parent
		if (m_Parent)
		{
			m_Parent->m_Child.PushBack(this);
			if ( !SEGAN_SET_HAS(m_Option, SX_NODE_SOLITARY) ) 
				m_Parent->UpdateBoundingVolumes();
		}
		
		//  update matrices to current state
		UpdateMatrix();
	}

	FORCEINLINE PNode Node::GetParent( void )
	{
		return m_Parent;
	}

	FORCEINLINE sx::core::PNode Node::GetChildByIndex( int index )
	{
		if (index<0 || index>=m_Child.Count()) return NULL;
		return m_Child[index];
	}

	bool Node::GetChildByName( const WCHAR* pwcName, OUT PNode& pNode, bool traversChildren /*= true*/ )
	{
		pNode = NULL;

		PNode node = NULL;
		str512 str = pwcName;
		for (int i=0; i<m_Child.Count(); i++)
		{
			node = m_Child[i];
			if ( str == node->GetName() )
			{
				pNode = node;
				return true;
			}

			if (traversChildren)
			{
				node->GetChildByName(pwcName, pNode, traversChildren);
				if ( pNode )
					return true;
			}
		}

		return false;
	}

	void Node::GetChildren( ArrayPNode_abs& nodeList, bool traverseChildes )
	{
		sx_callstack();

		for (int i=0; i<m_Child.Count(); i++)
		{
			nodeList.PushBack( m_Child[i] );
			if (traverseChildes)
				m_Child[i]->GetChildren(nodeList, traverseChildes);
		}
	}

	FORCEINLINE int Node::GetChildCount( void )
	{
		return m_Child.Count();
	}

	FORCEINLINE sx::core::PNodeMember Node::GetMemberByIndex( int index )
	{
		if (index<0 || index>=m_Member.Count()) return NULL;
		return m_Member[index];
	}

	int Node::GetMemberByName( const WCHAR* pwcName, OUT PNodeMember& pMember )
	{
		String str = pwcName;
		for (int i=0; i<m_Member.Count(); i++)
		{
			pMember = m_Member[i];
			if ( str == pMember->GetName() ) return i;
		}
		pMember = NULL;
		return -1;
	}

	void Node::GetMembersByType( DWORD NMT_ nmType, ArrayPNodeMember& memberList, bool traverseChildes )
	{
		//  verify that required member is exist in this node
		if (m_RegTypes & nmType)
		{
			for (int i=0; i<m_Member.Count(); i++)
			{
				if ( m_Member[i]->GetType() & nmType )
				{
					memberList.PushBack( m_Member[i] );
				}
			}
		}

		//  traverse childes ?
		if ( traverseChildes )
		{
			for (int i=0; i<m_Child.Count(); i++)
			{
				m_Child[i]->GetMembersByType(nmType, memberList, traverseChildes);
			}
		}
	}

	FORCEINLINE int Node::GetMemberCount( void )
	{
		return m_Member.Count();
	}

	FORCEINLINE void Node::SwapMembers( int index1, int index2 )
	{
		if (index1<0 || index1>=m_Member.Count() || index2<0 || index2>=m_Member.Count()) return;
		m_Member.Swap(index1, index2);
	}

	FORCEINLINE bool Node::MemberTypeExist( DWORD NMT_ nmType )
	{
		return (m_RegTypes & nmType)!=0;
	}

	void Node::SetPosition( const float3& vPosition )
	{
		m_Pos = vPosition;
		m_Matrix_local._41 = m_Pos.x;
		m_Matrix_local._42 = m_Pos.y;
		m_Matrix_local._43 = m_Pos.z;

		UpdateMatrix();
	}

	FORCEINLINE const Vector3& Node::GetPosition_local( void )
	{
		return m_Pos;
	}

	FORCEINLINE const float3& Node::GetPosition_world( void )
	{
		return (float3&)m_Matrix_world._41;
	}

	FORCEINLINE void Node::SetDirection_local( const float3& dir )
	{
		//  RISK: performance penalty to compute a simple direction

		float3 d = dir;
 		if ( d.x == 0.0f )	d.x = 0.001f;
 		if ( d.z == 0.0f )	d.z = 0.001f;

		m_Matrix_local.LookAtRH( sx::math::VEC3_ZERO, -d, sx::math::VEC3_Y );
		m_Matrix_local.Inverse( m_Matrix_local );
		m_Quat.SetRotationMatrix( m_Matrix_local );
		m_Matrix_local._41 = m_Pos.x;
		m_Matrix_local._42 = m_Pos.y;
		m_Matrix_local._43 = m_Pos.z;

		UpdateMatrix();
	}

	FORCEINLINE void Node::SetDirection_world( const float3& dir )
	{
		if ( !m_Parent )
		{
			SetDirection_local(dir);
			return;
		}

		//  RISK: performance penalty to compute a simple direction
		Matrix m;
		float3 d;

		m.Inverse( m_Parent->GetMatrix_world() );
		d.Transform_Norm( dir, m );

		if ( d.x == 0.0f )	d.x = 0.001f;
		if ( d.z == 0.0f )	d.z = 0.001f;

		m_Matrix_local.LookAtRH(sx::math::VEC3_ZERO, -d, sx::math::VEC3_Y);
		m_Matrix_local.Inverse( m_Matrix_local );
		m_Quat.SetRotationMatrix(m_Matrix_local);

		m_Matrix_local._41 = m_Pos.x;
		m_Matrix_local._42 = m_Pos.y;
		m_Matrix_local._43 = m_Pos.z;

		UpdateMatrix();
	}

	FORCEINLINE void Node::SetQuaternion( floatQ& qQuaternion )
	{
		m_Quat = qQuaternion;

		floatQ q;
		q.Normalize( m_Quat );
		q.GetRotationMatrix(m_Matrix_local);
		m_Matrix_local._41 = m_Pos.x;
		m_Matrix_local._42 = m_Pos.y;
		m_Matrix_local._43 = m_Pos.z;

		UpdateMatrix();
	}

	FORCEINLINE void Node::SetRotation( const float rx, const float ry, const float rz )
	{
		//  RISK: performance penalty to compute quaternion
		m_Matrix_local.RotationXYZ(rx, ry, rz);

		m_Quat.SetRotationMatrix(m_Matrix_local);

		m_Matrix_local._41 = m_Pos.x;
		m_Matrix_local._42 = m_Pos.y;
		m_Matrix_local._43 = m_Pos.z;

		UpdateMatrix();
	}

	FORCEINLINE const floatQ& Node::GetQuaternion( void )
	{
		return m_Quat;
	}

	FORCEINLINE void Node::SetTransformQ( const float3& vPosition, const floatQ& qQuaternion )
	{
		sx_callstack();

		m_Pos = vPosition;
		m_Quat = qQuaternion;

		floatQ q;
		q.Normalize( m_Quat );
		q.GetRotationMatrix(m_Matrix_local);
		m_Matrix_local._41 = m_Pos.x;
		m_Matrix_local._42 = m_Pos.y;
		m_Matrix_local._43 = m_Pos.z;

		UpdateMatrix();

	}

	FORCEINLINE void Node::SetTransformQ_offset( const float3& vPosition, const floatQ& qQuaternion )
	{
		qQuaternion.GetRotationMatrix( m_Matrix_offset );
		m_Matrix_offset._41 = vPosition.x;
		m_Matrix_offset._42 = vPosition.y;
		m_Matrix_offset._43 = vPosition.z;

		UpdateMatrix();

	}
	
	void Node::UpdateBoundingVolumes( void )
	{
		sx_callstack();

		//  this trick says me that this node is going to delete
		if ( m_UserTag == USERTAG_DELETE ) return;

		OBBox b;
		Matrix m;
		Matrix matInv; 
		matInv.Inverse(m_Matrix_world);

		m_aaBox.Max = -math::VEC3_MAX;
		m_aaBox.Min =  math::VEC3_MAX;
		m_Sphere.center = math::VEC3_ZERO;
		m_Sphere.radius = 0;

		//  just nodes with no parent can cover all their children
		if ( !m_Parent )
		{
			ArrayPNode& nodeList = s_arrayStore->Pop();
			GetChildren( nodeList, true );
			for (int i=0; i<nodeList.Count(); i++)
			{
				PNode node = nodeList[i];
				//if ( node->HasProperty( SX_NODE_SOLITARY ) ) continue;

				const AABox&	box = node->GetBox_local();
				const Matrix&	mat = node->GetMatrix_world();
				m.Multiply(mat, matInv);
				b.Transform(box, m);
				m_aaBox.CoverOB( b );
			}
			s_arrayStore->Push( nodeList );
		}
		//  nodes with parent should cover none solitary nodes
		else
		{
			for (int i=0; i<m_Child.Count(); i++)
			{
				PNode node = m_Child[i];
				if ( node->HasProperty( SX_NODE_SOLITARY ) ) continue;

				const AABox&	box = node->GetBox_local();
				const Matrix&	mat = node->GetMatrix_world();
				m.Multiply(mat, matInv);
				b.Transform(box, m);
				m_aaBox.CoverOB( b );
			}
		}

		for (int i=0; i<m_Member.Count(); i++)
			m_Member[i]->MsgProc(MT_COVERBOX, &m_aaBox);

		//  verify that box has been updated
		if (sx_abs_f(m_aaBox.Min.x - math::MAX) < 1.0f || sx_abs_f(m_aaBox.Min.y - math::MAX) < 1.0f || sx_abs_f(m_aaBox.Min.z - math::MAX) < 1.0f ||
            sx_abs_f(m_aaBox.Max.x - math::MAX) < 1.0f || sx_abs_f(m_aaBox.Max.y - math::MAX) < 1.0f || sx_abs_f(m_aaBox.Max.z - math::MAX) < 1.0f )
		{
			m_aaBox.Min.Set(-0.1f, -0.1f, -0.1f);
			m_aaBox.Max.Set( 0.1f,  0.1f,  0.1f);
		}

		//  compute bounding sphere
		m_Sphere.ComputeByAABox(m_aaBox);

		//  compute oriented bounding box
		m_obBox.Transform(m_aaBox, m_Matrix_world);

		if ( m_Parent && !SEGAN_SET_HAS(m_Option, SX_NODE_SOLITARY) ) 
			m_Parent->UpdateBoundingVolumes();
		else if ( !m_Parent && m_Sector )
			Scene::UpdateNode( this );

	}

	FORCEINLINE const Matrix& Node::GetMatrix_local( void )
	{
		return m_Matrix_local;
	}

	FORCEINLINE const Matrix& Node::GetMatrix_world( void )
	{
		return m_Matrix_world;
	}

	FORCEINLINE const AABox& Node::GetBox_local( void )
	{
		return m_aaBox;
	}

	FORCEINLINE OBBox& Node::GetBox_world( void )
	{
		return m_obBox;
	}

	FORCEINLINE const Sphere& Node::GetSphere_local( void )
	{
		return m_Sphere;
	}

	FORCEINLINE Sphere& Node::GetSphere_world( void )
	{
		static Sphere sphere;
		sphere.center.Transform(m_Sphere.center, m_Matrix_world);
		sphere.radius = m_Sphere.radius;
		return sphere;
	}

	FORCEINLINE void Node::Update( float elpsTime )
	{
		for (int i=0; i<m_Member.Count(); i++)
			m_Member[i]->Update( elpsTime );

		for (int i=0; i<m_Child.Count(); i++)
			m_Child[i]->Update( elpsTime );
	}

	void Node::Draw( DWORD flag )
	{
		for (int i=0; i<m_Member.Count(); i++)
			m_Member[i]->Draw(flag);

		if ( flag & SX_DRAW_ALPHA )
		{
			ArrayPNode& sortedList = s_arrayStore->Pop();

			for (int i=0; i<m_Child.Count(); i++)
				sortedList.PushBack( m_Child[i] );

			Matrix matView, matViewInv;
			d3d::Device3D::Matrix_View_Get(matView);
			matViewInv.Inverse( matView );
			camDir.Set( matViewInv._41, matViewInv._42, matViewInv._43);

			sortedList.Sort(&CompareNodeDistance);
			for (int i=0; i<sortedList.Count(); i++)
				sortedList[i]->Draw(flag);

			s_arrayStore->Push( sortedList );
		}
		else
		{
			for (int i=0; i<m_Child.Count(); i++) 
				m_Child[i]->Draw(flag);
		}

		//  additional draw
		if ( flag & SX_DRAW_DEBUG )
		{
			sx::d3d::Device3D::RS_ZEnabled(true);
			sx::d3d::Device3D::RS_Alpha( 0 );

			if ( flag & SX_DRAW_BOUNDING_BOX )
			{
				d3d::Device3D::Matrix_World_Set(m_Matrix_world);

				if ( flag & SX_DRAW_WIRED)
				{
					d3d::UI3D::DrawWiredAABox( m_aaBox, (flag & SX_DRAW_SELECTED) ? 0xffffffff : 0xffffcece );
				}
				else
				{
					d3d::UI3D::DrawAABox( m_aaBox, 0xffffffff * ((flag & SX_DRAW_SELECTED) != 0) );
				}
			}

			if ( flag & SX_DRAW_BOUNDING_SPHERE )
			{
				if ( flag & SX_DRAW_WIRED)
				{
					if ( m_Option & SX_NODE_LOYAL )
					{
						d3d::UI3D::DrawWiredSphere( m_Sphere, 0xffffffff * ((flag & SX_DRAW_SELECTED) != 0) );
					}
					else
					{
						Matrix matView;
						d3d::Device3D::Matrix_View_Get( matView );
						matView.Inverse( matView );
						Sphere worldSphere = GetSphere_world();
						matView.SetTranslation(worldSphere.x, worldSphere.y, worldSphere.z );
						d3d::UI3D::DrawCircle( matView, m_Sphere.r, 0xFFFF0000 );
					}
				}
				else
				{
					d3d::Device3D::Matrix_World_Set(m_Matrix_world);
					d3d::UI3D::DrawSphere( m_Sphere, 0xffffffff * ((flag & SX_DRAW_SELECTED) != 0) );
				}
			}
		}
	}

	UINT Node::MsgProc( UINT msgType, void* data )
	{
		sx_callstack_param(Node::MsgProc(msgType=%d), msgType);

		switch (msgType)
		{
		case MT_INTERSECT_RAY:
			{
				//	test ray intersection and return quickly if test failed
				msg_IntersectRay* ray = static_cast<msg_IntersectRay*>(data);

				//  to select nodes that represented as joints we can check the type of ray and node
				//  in this case we can select joint if type of ray be NMT_ANIMATOR and property of 
				//	node contain SX_NODE_ANIMATION. it's useful for editor :)
				if ( ray->type == NMT_ANIMATOR && (m_Option & SX_NODE_ABSOLUTE) && (m_Option & SX_NODE_LOYAL) && (m_Option & SX_NODE_SOLITARY))
				{
					AABox box = m_aaBox;
					
					//  verify joint is empty ?
					if (sx_abs_f(box.Max.x - box.Min.x)<EPSILON && sx_abs_f(box.Max.y - box.Min.y)<EPSILON )
					{
						box.Max.Set( 0.1f,  0.1f,  0.1f);
						box.Min.Set(-0.1f, -0.1f, -0.1f);
					}
					
					//  test intersection width current node matrix
					float3 hitPoint, hitNorm;
					OBBox oBox; oBox.Transform(box, m_Matrix_world);
					if ( ray->ray.Intersect_OBBox( oBox, &hitPoint, &hitNorm) )
					{	
						// intersection happened
						float dis = hitPoint.Distance( ray->ray.pos );
						ray->pushToResults(dis, hitPoint, hitNorm, this, NULL);
					}
					else return msgType;
				}// if ( ray->type == NMT_ANIMATOR && (m_Option & SX_NODE_ANIMATION) )

				//  select root node ?
				if ( ray->type == NMT_UNKNOWN && m_Option & SX_NODE_SELECTABLE )
				{
					if ( ray->name && m_Name != ray->name )	return msgType;

					float3 hitPoint, hitNorm;
					if ( !ray->ray.Intersect_Sphere( GetSphere_world(), &hitPoint, &hitNorm ) ) return msgType;
					if ( ray->depthLevel == msg_IntersectRay::SPHERE )
					{
						float dis = hitPoint.Distance( ray->ray.pos );
						ray->pushToResults(dis, hitPoint, hitNorm, this, NULL);
						return msgType;
					}

					if ( !ray->ray.Intersect_OBBox( GetBox_world(), &hitPoint, &hitNorm ) )
						return msgType;

					//if ( ray->depthLevel == msg_IntersectRay::BOX )
					{
						float dis = hitPoint.Distance( ray->ray.pos );
						ray->pushToResults(dis, hitPoint, hitNorm, this, NULL);
						return msgType;
					}

					return msgType;
				}

				//  regular intersection test
				if ( m_Option & SX_NODE_SELECTABLE && ray->ray.Intersect_Sphere( GetSphere_world() ))
				{
					if ( !ray->ray.Intersect_OBBox( GetBox_world() ) )
						return msgType;
				}
				else return msgType;
			}
			break;
		}

		//  continue sending message to members and children
		for (int i=0; i<m_Member.Count(); i++)
			m_Member[i]->MsgProc(msgType, data);

		for (int i=0; i<m_Child.Count(); i++) 
			m_Child[i]->MsgProc(msgType, data);

		return msgType;
	}

	void Node::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, nodeFileID);

		int ver = 2;
		SEGAN_STREAM_WRITE(stream, ver);

		cmn::String_Save(m_Name, &stream);

		SEGAN_STREAM_WRITE(stream, m_drawOrder);
		SEGAN_STREAM_WRITE(stream, m_Option);
		SEGAN_STREAM_WRITE(stream, m_RegTypes);

		SEGAN_STREAM_WRITE(stream, m_Pos);
		SEGAN_STREAM_WRITE(stream, m_Quat);

		SEGAN_STREAM_WRITE(stream, m_aaBox);
		SEGAN_STREAM_WRITE(stream, m_Sphere);

		int n = m_Member.Count();
		SEGAN_STREAM_WRITE(stream, n);
		for (int i=0; i<n; i++)
		{
			NodeMemberType mType = m_Member[i]->GetType();
			SEGAN_STREAM_WRITE(stream, mType);

			m_Member[i]->Save(stream);
		}

		//  write number of childes without absolute childes
		n = m_Child.Count();
		for (int i=0; i<m_Child.Count(); i++) n -= m_Child[i]->HasProperty(SX_NODE_ABSOLUTE) ? 1 : 0;
		SEGAN_STREAM_WRITE(stream, n);

		//  write all childes except absolute
		for (int i=0; i<m_Child.Count(); i++)
		{
			if ( !m_Child[i]->HasProperty(SX_NODE_ABSOLUTE) )
				m_Child[i]->Save(stream);
		}
	}

	void Node::Load( Stream& stream )
	{
		sx_callstack();

		Clear();

		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if (id != nodeFileID)
		{
			sxLog::Log(L"Incompatible file format for loading node !");
			return;
		}

		// write version
		int version = 0;
		SEGAN_STREAM_READ(stream, version);

		if (version == 1)
		{
			cmn::String_Load(m_Name, &stream);

			SEGAN_STREAM_READ(stream, m_Option);
			SEGAN_STREAM_READ(stream, m_RegTypes);

			SEGAN_STREAM_READ(stream, m_Pos);
			SEGAN_STREAM_READ(stream, m_Quat);

			SEGAN_STREAM_READ(stream, m_aaBox);
			SEGAN_STREAM_READ(stream, m_Sphere);

			SetTransformQ( m_Pos, m_Quat );

			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n; i++)
			{
				NodeMemberType mType = NMT_NULL;
				SEGAN_STREAM_READ(stream, mType);

				PNodeMember nodeMember = NodeMember::Create(mType);
				if (nodeMember)
				{
					nodeMember->SetOwner(this);
					nodeMember->Load(stream);
				}
			}

			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n && n<10000; i++)
			{
				PNode node = sx_new( Node );
				node->SetParent(this);
				node->Load(stream);
			}

			UpdateBoundingVolumes();
		}
		else if (version == 2)
		{
			cmn::String_Load(m_Name, &stream);

			SEGAN_STREAM_READ(stream, m_drawOrder);
			SEGAN_STREAM_READ(stream, m_Option);
			SEGAN_STREAM_READ(stream, m_RegTypes);

			SEGAN_STREAM_READ(stream, m_Pos);
			SEGAN_STREAM_READ(stream, m_Quat);

			SEGAN_STREAM_READ(stream, m_aaBox);
			SEGAN_STREAM_READ(stream, m_Sphere);

			SetTransformQ( m_Pos, m_Quat );

			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n; i++)
			{
				NodeMemberType mType = NMT_NULL;
				SEGAN_STREAM_READ(stream, mType);

				PNodeMember nodeMember = NodeMember::Create(mType);
				if (nodeMember)
				{
					nodeMember->SetOwner(this);
					nodeMember->Load(stream);
				}
			}

			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n && n<10000; i++)
			{
				PNode node = sx_new( Node );
				node->SetParent(this);
				node->Load(stream);
			}

			UpdateBoundingVolumes();
		}

	}

	PNode Node::Clone( void )
	{
		MemoryStream stm;
		stm.Clear();

		Save(stm);
		stm.SetPos(0);

		PNode node = sx_new( Node );
		node->Load(stm);

		node->m_UserTag = m_UserTag;
		node->m_UserData = m_UserData;

		return node;
	}

	FORCEINLINE void Node::UpdateMatrix( void )
	{
		Matrix matLocal;
		//matLocal.Multiply( m_Matrix_offset, m_Matrix_local );
		matLocal.Multiply( m_Matrix_local, m_Matrix_offset );

		if (m_Parent)
			m_Matrix_world.Multiply(matLocal, m_Parent->m_Matrix_world);
		else 
			m_Matrix_world = matLocal;

		// now update childes matrices
		for (int i=0; i<m_Child.Count(); i++)
			m_Child[i]->UpdateMatrix();

		// update bounding volumes for none solitary nodes
//		if ( !(m_Option & SX_NODE_SOLITARY) && m_Parent ) 
//			UpdateBoundingVolumes();

		//  compute oriented bounding box
		m_obBox.Transform(m_aaBox, m_Matrix_world);

		//  update bounding volumes of parent
//		if ( !(m_Option & SX_NODE_SOLITARY) && m_Parent )
//			m_Parent->UpdateBoundingVolumes();

		// finally update node in the scene
		if ( !m_Parent && m_Sector )
			Scene::UpdateNode(this);
	}



}} // namespace sx { namespace core {