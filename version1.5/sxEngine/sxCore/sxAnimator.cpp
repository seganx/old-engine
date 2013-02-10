#include "sxAnimator.h"

static const UINT animatorFileID = MAKEFOURCC('A', 'N', 'I', 'M');

static int	s_numCPUCores = 0;

namespace sx { namespace core {


	Animator::Animator( void ) : NodeMember(), m_matBase(NULL), m_matAnim(NULL), m_updateBox(0)
	{
		m_Type = NMT_ANIMATOR;
		m_Name = L"Animator";
		m_Option = SX_ANIMATOR_PLAY | SX_ANIMATOR_LOOP;

		if ( !s_numCPUCores )
		{
			SystemInfo sysInfo;
			sys::GetSystemInfo(sysInfo);
			s_numCPUCores = sysInfo.CPU.ProcessorCount;
		}
	}

	Animator::~Animator( void )
	{
		sx_callstack_push(Animator::~Animator());
		ClearAnimations();
	}

	FORCEINLINE void Animator::ClearAnimations( void )
	{
		m_Controller.ClearAnimations();
		FitNodesToJoints();
	}

	FORCEINLINE int Animator::AnimationCount( void )
	{
		return m_Controller.AnimationCount();
	}

	FORCEINLINE int Animator::AddAnimation( const WCHAR* src )
	{
		int index = m_Controller.AddAnimation(src);
		FitNodesToJoints();
		return index;
	}

	FORCEINLINE void Animator::RemoveAnimation( int index )
	{
		m_Controller.RemoveAnimation(index);
		FitNodesToJoints();
	}

	FORCEINLINE sx::d3d::PAnimation Animator::GetAnimation( int index )
	{
		return m_Controller.GetAnimation(index);
	}

	FORCEINLINE int Animator::GetIndexOfAnimation( const WCHAR* src )
	{
		return m_Controller.GetIndexOfAnimation(src);
	}

	FORCEINLINE void Animator::SetAnimationByIndex( int index )
	{
		m_Controller.SetCurrentAnimationByIndex(index);
	}

	FORCEINLINE void Animator::SetAnimationBySrc( const WCHAR* src )
	{
		m_Controller.SetCurrentAnimationBySrc(src);
		FitNodesToJoints();
	}

	FORCEINLINE int Animator::GetAnimationIndex( void )
	{
		return m_Controller.GetCurrentAnimationIndex();
	}

	FORCEINLINE void Animator::SetSpeed( float speed )
	{
		m_Controller.SetCurrentSpeed(speed);
	}

	FORCEINLINE float Animator::GetSpeed( void )
	{
		return m_Controller.GetCurrentSpeed();
	}

	FORCEINLINE void Animator::SetBlendTime( float time )
	{
		m_Controller.SetCurrentBlendTime(time);
	}

	FORCEINLINE float Animator::GetBlendTime( void )
	{
		return m_Controller.GetCurrentBlendTime();
	}

	FORCEINLINE void Animator::SetAnimationTime( float curTime )
	{
		m_Controller.SetCurrentAnimationTime(curTime);
	}

	FORCEINLINE float Animator::GetAnimationTime( void )
	{
		return m_Controller.GetCurrentAnimationTime();
	}

	FORCEINLINE float Animator::GetMaxTime( void )
	{
		return m_Controller.GetCurrentMaxTime();
	}

	FORCEINLINE void Animator::SetCallback( CallbackAnimControl AnimCallback, void* userdata )
	{
		m_Controller.SetCallback(AnimCallback, userdata);
	}

	FORCEINLINE int Animator::GetJointCount( void )
	{
		return m_Controller.GetBoneCount();
	}

	void Animator::Update( float elpsTime )
	{
		m_Controller.Update(elpsTime, m_Option);

		PAnimationKeyFrame pBone = NULL;
		for (int i=0; i<m_Nodes.Count(); i++)
		{
			pBone = m_Controller.GetJointFrame(i);
			m_Nodes[i]->SetTransformQ_offset( pBone->position , pBone->rotation );
		}

		for (int i=0; i<m_Nodes.Count(); i++)
		{
			m_matAnim[i].Multiply( m_matBase[i], m_Nodes[i]->GetMatrix_world() );
		}

		//  update parent box
		m_updateBox++;
		if (m_Owner && m_updateBox>10 ) 
		{
			m_updateBox = cmn::Random(5);
			m_Owner->UpdateBoundingVolumes();
		}

	}

	void Animator::Draw( DWORD flag )
	{
		if ( flag & SX_DRAW_MESH )
			d3d::ShaderPool::SetWorldMatrices( m_matAnim, m_Nodes.Count() );

		if ( flag & SX_DRAW_DEBUG )
		{
			sx::d3d::Device3D::RS_Alpha( 0 );

			for (int i=0; i<m_Nodes.Count(); i++)
			{
				PNode pnode = m_Nodes[i];

				if ( pnode->GetParent() && pnode->GetParent() != m_Owner )
				{
					d3d::Device3D::RS_ZEnabled(false);
					d3d::Device3D::Matrix_World_Set(math::MTRX_IDENTICAL);
					d3d::UI3D::ReadyToDebug(0xffffffff);
					d3d::UI3D::DrawLine( pnode->GetPosition_world(), pnode->GetParent()->GetPosition_world() );
					d3d::Device3D::RS_ZEnabled(true);
				}
			}
		}
	}

	UINT Animator::MsgProc( UINT msgType, void* data )
	{
		switch (msgType)
		{
		case MT_COVERBOX:
			{
				if ( m_Owner )
				{
					OBBox b;
					Matrix m;
					Matrix matInv; 
					matInv.Inverse( m_Owner->GetMatrix_world() );

					PAABox pBox = static_cast<PAABox>(data);
					for (int i=0; i<m_Nodes.Count(); i++)
					{
						const AABox&	box = m_Nodes[i]->GetBox_local();
						const Matrix&	mat = m_Nodes[i]->GetMatrix_world();
						m.Multiply(mat, matInv);
						b.Transform(box, m);
						pBox->CoverOB( b );
					}
				}
			}
			break;

		case MT_INTERSECT_RAY:
			{
				for (int i=0; i<m_Nodes.Count(); i++)
				{
					m_Nodes[i]->MsgProc(msgType, data);
				}
			}
			break;

		case MT_ANIMATOR:
			if ( data )
			{
				msg_Animator *msgAnim = static_cast<msg_Animator*>(data);

				if ( msgAnim->name && m_Name != msgAnim->name )	return msgType;

				if ( msgAnim->remOption & SX_ANIMATOR_LOOP )	RemOption( SX_ANIMATOR_LOOP );
				if ( msgAnim->remOption & SX_ANIMATOR_PLAY)		RemOption( SX_ANIMATOR_PLAY );

				if ( msgAnim->addOption & SX_ANIMATOR_LOOP)		AddOption( SX_ANIMATOR_LOOP );
				if ( msgAnim->addOption & SX_ANIMATOR_PLAY)		AddOption( SX_ANIMATOR_PLAY );
				
				if ( msgAnim->animIndex > -1 )
					SetAnimationByIndex( msgAnim->animIndex );
				else
					msgAnim->animIndex = GetAnimationIndex();

				if ( msgAnim->animTime > -1	 )
					SetAnimationTime( msgAnim->animTime );
				else
					msgAnim->animTime = GetAnimationTime();

				if ( msgAnim->animSpeed > -1 )
					m_Controller.m_Speed = msgAnim->animSpeed;
				else
					msgAnim->animSpeed = m_Controller.m_Speed;

				if ( msgAnim->animSpeedScale > -1 )
					m_Controller.m_SpeedScale = msgAnim->animSpeedScale;
				else
					msgAnim->animSpeedScale = m_Controller.m_SpeedScale;

			}
			break;

		case MT_ANIMATOR_COUNT:
			{
				msg_Animator_Count *msgAnim = static_cast<msg_Animator_Count*>(data);
				msgAnim->animators[msgAnim->numAnimator] = this;
				msgAnim->numAnimator++;
			}
			break;

		case MT_MEMBER_ATTACHED:
			{
				//if (data != this)
				//{
				//	PNodeMember member = static_cast<PNodeMember>(data);

				//	//  animator like to placed before any mesh in the node's member list
				//	if (member->GetType() == NMT_MESH)
				//	{
				//		//  verify that new mesh placed before this animator
				//		int curIndex = 1000, meshIndex = 1000;
				//		for (int i=0; i<m_Owner->m_Member.Count() && (curIndex==1000 || meshIndex==1000); i++)
				//		{
				//			if (m_Owner->m_Member[i] == this)	curIndex = i;
				//			if (m_Owner->m_Member[i] == member)	meshIndex = i;
				//		}

				//		//  insert animator to place that is before mesh
				//		if (curIndex > meshIndex)
				//		{
				//			m_Owner->m_Member.Remove(this);
				//			m_Owner->m_Member.Insert(meshIndex, this);
				//		}
				//	}
				//}
			}
			break;
		}

		return msgType;
	}

	void Animator::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, animatorFileID);

		int ver = 2;
		SEGAN_STREAM_WRITE(stream, ver);

		NodeMember::Save( stream );

		m_Controller.Save(stream);

		//  reset nodes properties
		int n = m_Nodes.Count();
		for (int i=0; i<n; i++)
		{
			PNode node = m_Nodes[i];
			node->SetTransformQ_offset(math::VEC3_ZERO, math::QUAT_IDENTICAL);
			node->RemProperty( SX_NODE_ABSOLUTE );
			node->RemProperty( SX_NODE_LOYAL	);
			//node->AddProperty( SX_NODE_SOLITARY	);		//  no need this
			node->SetParent(NULL);
		}

		// write nodes as bones to the stream.
		SEGAN_STREAM_WRITE(stream, n);
		for (int i=0; i<n; i++)
		{
			m_Nodes[i]->Save(stream);
		}

		//  arrange them again depend on bones
		for (int i=0; i<n; i++)
		{
			int ip = m_Controller.GetJointParent(i);
			if (ip>=0) m_Nodes[i]->SetParent(m_Nodes[ip]);
		}

		//  reset nodes properties
		for (int i=0; i<n; i++)
		{
			PNode node = m_Nodes[i];
			if ( node->GetParent() == NULL )
				node->SetParent(m_Owner);

			node->AddProperty( SX_NODE_ABSOLUTE );
			node->AddProperty( SX_NODE_LOYAL	);
			//node->AddProperty( SX_NODE_SOLITARY	);		//  no need this
		}
	}

	void Animator::Load( Stream& stream )
	{
		ClearAnimations();

		int id = 0;
		SEGAN_STREAM_READ(stream, id);
		if (id != animatorFileID)
		{
			sxLog::Log(L"Incompatible file format for loading animator !");
			return;
		}

		int ver = 0;
		SEGAN_STREAM_READ(stream, ver);
		if (ver == 1)
		{
			m_Controller.Load(stream);

			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n; i++)
			{
				PNode node = sx_new( Node );
				node->Load(stream);
				m_Nodes.PushBack(node);
			}
		}
		else if ( ver == 2 )
		{
			NodeMember::Load( stream );

			m_Controller.Load(stream);

			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n; i++)
			{
				PNode node = sx_new( Node );
				node->Load(stream);
				m_Nodes.PushBack(node);
			}
		}

		FitNodesToJoints();

		Update(0);
	}

	void Animator::FitNodesToJoints( void )
	{
		int n = m_Nodes.Count();
		int b = m_Controller.GetBoneCount();

		//  reset nodes properties
		for (int i=0; i<n; i++)
		{
			PNode node = m_Nodes[i];
			node->SetTransformQ_offset(math::VEC3_ZERO, math::QUAT_IDENTICAL);
			node->RemProperty( SX_NODE_ABSOLUTE );
			node->RemProperty( SX_NODE_LOYAL	);
			//node->AddProperty( SX_NODE_SOLITARY	);		//  no need this
			node->SetParent(NULL);
		}
// 		if ( m_matAnim && m_matBase )
// 		{
// 			memcpy(m_matAnim, m_matBase, n * sizeof(Matrix));
// 			d3d::ShaderPool::SetWorldMatrices( m_matAnim, n );
// 		}

		//  remove redundancy nodes
		for (int i=b; i<n; i++)
		{
			 sx_delete_and_null( m_Nodes[b] );
			 m_Nodes.RemoveByIndex(b);
		}

		//  add new nodes to list
		for (int i=n; i<b; i++)
		{
			PNode node = sx_new(Node);
			node->AddProperty( SX_NODE_SOLITARY );
			m_Nodes.PushBack( node );
		}

		//  arrange them again depend on bones
		for (int i=0; i<b; i++)
		{
			int ip = m_Controller.GetJointParent(i);
			if (ip>=0) m_Nodes[i]->SetParent( m_Nodes[ip] );
		}

		//  reset nodes properties
		for (int i=0; i<b; i++)
		{
			PNode node = m_Nodes[i];
			node->SetName( m_Controller.GetJointName(i) );
			
			node->AddProperty( SX_NODE_ABSOLUTE );
			node->AddProperty( SX_NODE_LOYAL	);
			
			//  TEST : just to show the nodes as a box
			PAABox box = (PAABox)&node->GetBox_local();
			box->Min = float3(-0.2f, -0.2f, -0.2f);
			box->Max = float3(0.2f, 0.2f, 0.2f);
			PSphere sphere = (PSphere)&node->GetSphere_local();
			sphere->ComputeByAABox(*box);

			//  update node matrix to prepare base matrix in the next step
			PAnimationKeyFrame pBase = m_Controller.GetJointBase(i);
			node->SetTransformQ_offset( pBase->position, pBase->rotation );
		}

		//  realloc matrices as number of nodes
		if (b)
		{
			sx_mem_realloc( m_matBase, b * sizeof(Matrix) );
			sx_mem_realloc( m_matAnim, b * sizeof(Matrix) );
		}
		else
		{
			sx_mem_free(m_matBase); m_matBase = null;
			sx_mem_free(m_matAnim); m_matAnim = null;
		}

		//  update base matrices
		for (int i=0; i<b; i++)
		{
			m_matBase[i].Inverse( m_Nodes[i]->GetMatrix_world() );
		}

		//  cow connect all nodes to the owner
		for (int i=0; i<b; i++)
		{
			PNode node = m_Nodes[i];
			if ( node->GetParent() == NULL )
			{
				node->SetParent(m_Owner);
			}
		}
	}


}}	//	namespace sx { namespace core {