#include "sxAnimationController.h"
#include "sxDevice3D.h"
#include "sxUI3D.h"

namespace sx { namespace d3d {


	AnimationController::AnimationController( void )
		: m_lastIndex(0), m_Index(0), m_lastTime(0), m_Time(0), m_Speed(1.0f), m_SpeedScale(1.0f), m_blendTime(1.0f), m_curBlend(0),
		m_Callback(NULL), m_Userdata(NULL)
	{

	}

	AnimationController::~AnimationController( void )
	{
		ClearAnimations();
	}

	void AnimationController::ClearAnimations( void )
	{
		for (int i=0; i<m_Anims.Count(); i++)
		{
			Animation::Manager::Release( m_Anims[i] );
		}
		m_Anims.Clear();
		m_Joints.Clear();
	}

	int AnimationController::AnimationCount( void )
	{
		return m_Anims.Count();
	}

	int AnimationController::AddAnimation( const WCHAR* src )
	{
		//int index = GetIndexOfAnimation(src);
		//if ( index>=0 ) return index;

		PAnimation pAnim = NULL;
		if ( Animation::Manager::Get(pAnim, src) )
		{
			//  add animation to list
			m_Anims.PushBack(pAnim);

			//  create new bones depend on animations data
			for (int i=m_Joints.Count(); i<pAnim->AnimJointCount(); i++)
			{
				AnimationKeyFrame bone;
				m_Joints.PushBack( bone );
			}
		}

		return m_Anims.Count() - 1;
	}

	void AnimationController::RemoveAnimation( int index )
	{
		if ( index<0 || index >= m_Anims.Count() ) return;
		m_Anims.RemoveByIndex(index);

		//  compute maximum bones exist in animations
		int n = 0;
		for (int i=0; i<m_Anims.Count(); i++)
		{
			if ( n < m_Anims[i]->AnimJointCount() )
				n = m_Anims[i]->AnimJointCount();
		}

		//  remove redundancy bones
		n = m_Joints.Count() - n;
		while (n>0)
		{
			m_Joints.RemoveByIndex( m_Joints.Count() - 1 );
			n--;
		}
	}

	sx::d3d::PAnimation AnimationController::GetAnimation( int index )
	{
		if ( index<0 || index >= m_Anims.Count() ) return NULL;
		return m_Anims[index];
	}

	int AnimationController::GetIndexOfAnimation( const WCHAR* srcName )
	{
		if ( !srcName ) return -1;

		for (int i=0; i<m_Anims.Count(); i++)
		{
			PAnimation pAnim = m_Anims[i];
			if ( pAnim->GetSource() && !wcscmp(srcName, pAnim->GetSource()) )
			{
				return i;
			}
		}

		return -1;
	}

	void AnimationController::SetCurrentAnimationByIndex( int index )
	{
		if (  index == m_Index || index<0 || index >= m_Anims.Count()) return;

		m_lastIndex = m_Index;
		m_Index = index;
		m_lastTime = m_Time;
		m_Time = 0.0f;
		m_curBlend = 0.0f;
	}

	void AnimationController::SetCurrentAnimationBySrc( const WCHAR* src )
	{
		int index = GetIndexOfAnimation(src);
		if ( index < 0 ) index = AddAnimation( src );
		SetCurrentAnimationByIndex(index);
	}

	int AnimationController::GetCurrentAnimationIndex( void )
	{
		return m_Index;
	}

	void AnimationController::SetCurrentSpeed( float speed )
	{
		m_Speed = speed;
	}

	float AnimationController::GetCurrentSpeed( void )
	{
		return m_Speed;
	}

	void AnimationController::SetCurrentBlendTime( float time )
	{
		m_blendTime = time;
	}

	float AnimationController::GetCurrentBlendTime( void )
	{
		return m_blendTime;
	}

	void AnimationController::SetCurrentAnimationTime( float curTime )
	{
		m_Time = curTime;
	}

	float AnimationController::GetCurrentAnimationTime( void )
	{
		return m_Time;
	}

	float AnimationController::GetCurrentMaxTime( void )
	{
		if ( m_Index<0 || m_Anims.IsEmpty() ) return 0.0f;
		
		Animation* anim = m_Anims[m_Index];
		float speed = anim->m_speed * m_Speed * m_SpeedScale;
		if ( speed > 0.00001f )
			return anim->GetMaxTime() / speed;
		else
			return 0;
	}

	void AnimationController::Update( float elpsTime, DWORD flag )
	{
		if ( !(flag & SX_ANIMATION_PLAY) || m_Index<0 || m_Index>=m_Anims.Count() ) return;
		PAnimation pAnim = m_Anims[m_Index];
		elpsTime *= 0.001f;

		if ( m_Time > pAnim->GetMaxTime() && (pAnim->m_Option & SX_ANIMATION_LOOP) && (flag & SX_ANIMATION_LOOP) )
			m_Time = 0.0f;
		
		float blendTime = m_blendTime * pAnim->m_blendTime;
		if ( m_curBlend < blendTime && m_lastIndex>=0 && m_lastIndex<m_Anims.Count() )
		{
			PAnimation pLastAnim = m_Anims[m_lastIndex];
			float weight = m_curBlend / blendTime;
			if ( m_lastTime > pLastAnim->GetMaxTime()  && (pLastAnim->m_Option & SX_ANIMATION_LOOP) && (flag & SX_ANIMATION_LOOP) )
				m_lastTime = 0.0f;

			for (int i=0; i<m_Joints.Count(); i++)
			{
				PAnimationKeyFrame pKey = pAnim->GetKeyFrame( static_cast<UINT>(i), m_Time );
				PAnimationKeyFrame pLastKey = pLastAnim->GetKeyFrame( static_cast<UINT>(i), m_lastTime );

				if (pKey && pLastKey)
				{
					m_Joints[i].time = m_Time;

					m_Joints[i].position.Lerp( pLastKey->position, pKey->position, weight );
					m_Joints[i].rotation.SLerp( pLastKey->rotation, pKey->rotation, weight );

				}
				else if (pKey)
				{
					m_Joints[i].time = m_Time;
					m_Joints[i].position = pKey->position;
					m_Joints[i].rotation = pKey->rotation;
				}
			}

			m_curBlend += elpsTime * m_Speed * m_SpeedScale * pAnim->m_speed;
			m_lastTime += elpsTime * m_Speed * m_SpeedScale * pAnim->m_speed;
		}
		else
		{
			for (int i=0; i<m_Joints.Count(); i++)
			{
				PAnimationKeyFrame pKey = pAnim->GetKeyFrame( static_cast<UINT>(i), m_Time );
				if (pKey)
				{
					m_Joints[i].time = m_Time;
					m_Joints[i].position = pKey->position;
					m_Joints[i].rotation = pKey->rotation;
				}
			}
		}

		m_Time += elpsTime * m_Speed * m_SpeedScale * pAnim->m_speed;
	}

	FORCEINLINE void AnimationController::SetCallback( CallbackAnimControl AnimCallback, void* userdata )
	{
		m_Callback = AnimCallback;
		m_Userdata = userdata;
	}

	FORCEINLINE int AnimationController::GetBoneCount( void )
	{
		return m_Joints.Count();
	}

	const WCHAR* AnimationController::GetJointName( int jointIndex )
	{
		for (int i=0; i<m_Anims.Count(); i++)
		{
			PAnimationJoint pJoint = m_Anims[i]->GetAnimJointByIndex(jointIndex);
			if (pJoint) return pJoint->m_Name;
		}
		return NULL;
	}

	int AnimationController::GetJointParent( int jointIndex )
	{
		PAnimationJoint pJoint = NULL;

		// find joint in the list
		int animIndex = 0;
		for ( ; animIndex<m_Anims.Count() && !pJoint; animIndex++)
		{
			pJoint = m_Anims[animIndex]->GetAnimJointByIndex(jointIndex);
		}
		animIndex--;

		// verify that this joint has parent
		if (pJoint->m_Parent)
		{
			for (int iP = 0; iP<m_Anims[animIndex]->AnimJointCount(); iP++)
			{
				if ( pJoint->m_Parent == m_Anims[animIndex]->GetAnimJointByIndex(iP)->m_Name )
				{
					return iP;
				}
			}
		}

		return -1;
	}

	FORCEINLINE PAnimationKeyFrame AnimationController::GetJointFrameByTime( int jointIndex, float animTime )
	{
		if ( m_Index<0 || m_Index>=m_Anims.Count() ) return NULL;
		return m_Anims[m_Index]->GetKeyFrame(jointIndex, animTime);
	}

	FORCEINLINE PAnimationKeyFrame AnimationController::GetJointFrame( int jointIndex )
	{
		if ( jointIndex<0 || jointIndex>m_Joints.Count() ) return NULL;
		return &m_Joints[jointIndex];
	}

	FORCEINLINE const PAnimationKeyFrame AnimationController::GetJointBase( int boneIndex )
	{
		for (int i=0; i<m_Anims.Count(); i++)
		{
			PAnimationJoint pJoint = m_Anims[i]->GetAnimJointByIndex(boneIndex);
			if (pJoint) return &pJoint->m_Base;
		}
		return NULL;
	}

	void AnimationController::Save( Stream& stream )
	{
		int ver = 999999;
		SEGAN_STREAM_WRITE(stream, ver);

		int n = m_Anims.Count();
		SEGAN_STREAM_WRITE(stream, n);
		for (int i=0; i<n; i++)
		{
			sx::d3d::PAnimation painm = m_Anims[i];

			String str = painm->GetSource();
			cmn::String_Save(str, &stream);

			SEGAN_STREAM_WRITE( stream, painm->m_speed );
			SEGAN_STREAM_WRITE( stream, painm->m_blendTime );
			SEGAN_STREAM_WRITE( stream, painm->m_Option );
		}

		SEGAN_STREAM_WRITE(stream, m_lastIndex);
		SEGAN_STREAM_WRITE(stream, m_Index);
		SEGAN_STREAM_WRITE(stream, m_lastTime);
		SEGAN_STREAM_WRITE(stream, m_Time);
		SEGAN_STREAM_WRITE(stream, m_Speed);
		SEGAN_STREAM_WRITE(stream, m_blendTime);
		SEGAN_STREAM_WRITE(stream, m_curBlend);
	}

	void AnimationController::Load( Stream& stream )
	{
		ClearAnimations();

		int ver = 0;
		SEGAN_STREAM_READ(stream, ver);

		if ( ver == 999999 )
		{
			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n; i++)
			{
				String str;
				cmn::String_Load(str, &stream);

				int index = AddAnimation( str );
				if ( index > -1 )
				{
					sx::d3d::PAnimation painm = m_Anims[index];
					SEGAN_STREAM_READ( stream, painm->m_speed );
					SEGAN_STREAM_READ( stream, painm->m_blendTime );
					SEGAN_STREAM_READ( stream, painm->m_Option );
				}
			}

			SEGAN_STREAM_READ(stream, m_lastIndex);
			SEGAN_STREAM_READ(stream, m_Index);
			SEGAN_STREAM_READ(stream, m_lastTime);
			SEGAN_STREAM_READ(stream, m_Time);
			SEGAN_STREAM_READ(stream, m_Speed);
			SEGAN_STREAM_READ(stream, m_blendTime);
			SEGAN_STREAM_READ(stream, m_curBlend);
		}
		else
		{
			stream.SetPos( stream.GetPos() - 4 );

			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			for (int i=0; i<n; i++)
			{
				String str;
				cmn::String_Load(str, &stream);
				AddAnimation(str);
			}

			SEGAN_STREAM_READ(stream, m_lastIndex);
			SEGAN_STREAM_READ(stream, m_Index);
			SEGAN_STREAM_READ(stream, m_lastTime);
			SEGAN_STREAM_READ(stream, m_Time);
			SEGAN_STREAM_READ(stream, m_Speed);
			SEGAN_STREAM_READ(stream, m_blendTime);
			SEGAN_STREAM_READ(stream, m_curBlend);
		}
	}

	void AnimationController::Debug_Draw( DWORD flag )
	{
		//static Matrix mat[200];
		//for (int i=0; i<m_Joints.Count(); i++)
		//{
		//	m_Joints[i].rotation.GetRotationMatrix( mat[i] );
		//	mat[i].TranslateIt(m_Joints[i].position.x, m_Joints[i].position.y, m_Joints[i].position.z);

		//	//  apply parent for debug
		//	PAnimationJoint pJoint = m_Anims[m_Index]->GetAnimJointByIndex(i);
		//	if (pJoint && pJoint->m_Parent)
		//	{
		//		for (int iP = 0; iP<m_Anims[m_Index]->AnimJointCount(); iP++)
		//		{
		//			if ( pJoint->m_Parent == m_Anims[m_Index]->GetAnimJointByIndex(iP)->m_Name )
		//			{
		//				mat[i].Multiply(mat[i], mat[iP]);

		//				if (flag & SX_ANIMATION_DRAW_SKELETON)
		//				{
		//					Device3D::Matrix_World_Set(math::MTRX_IDENTICAL);
		//					Device3D::SetMaterialColor(0xFFFFFFFF);
		//					UI3D::DrawLine( float3(mat[i]._41, mat[i]._42, mat[i]._43), float3(mat[iP]._41, mat[iP]._42, mat[iP]._43) );
		//				}
		//			}
		//		}
		//	}


		//	if (flag & SX_ANIMATION_DRAW_JOINTS)
		//	{
		//		Device3D::Matrix_World_Set(mat[i]);
		//		Device3D::SetMaterialColor(0xFFFF0000);
		//		UI3D::DrawLine( math::VEC3_ZERO, math::VEC3_X/4 );
		//		Device3D::SetMaterialColor(0xFF00FF00);
		//		UI3D::DrawLine( math::VEC3_ZERO, math::VEC3_Y/4 );
		//		Device3D::SetMaterialColor(0xFF0000FF);
		//		UI3D::DrawLine( math::VEC3_ZERO, math::VEC3_Z/4 );

		//		AABox box( float3(-0.1f, -0.1f, -0.1f), float3(0.1f, 0.1f, 0.1f) );
		//		UI3D::DrawWiredAABox(box, 0xFFFF0000);
		//	}
		//}
	}



}} // namespace sx { namespace d3d {