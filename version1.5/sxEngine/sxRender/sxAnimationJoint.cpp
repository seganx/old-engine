#include "sxAnimationJoint.h"


namespace sx { namespace d3d {

	//////////////////////////////////////////////////////////////////////////
	//  HELPER FUNCTIONS
	int keyCompare(AnimationKeyFrame& key1, AnimationKeyFrame& key2)
	{
		return (key1.time < key2.time) ? -1 : (key1.time == key2.time) ? 0 : 1;
	}

	
	//////////////////////////////////////////////////////////////////////////
	//	ANIMATION DATA
	AnimationJoint::AnimationJoint( void )
	{

	}

	AnimationJoint::~AnimationJoint( void )
	{
		Clear();
		m_Name.Clear();
		m_Parent.Clear();
	}

	void AnimationJoint::Clear( void )
	{
		m_Keys.Clear();
	}

	int AnimationJoint::KeyFrameCount( void )
	{
		return m_Keys.Count();
	}

	PAnimationKeyFrame AnimationJoint::AddKeyFrame( float fTime )
	{
		AnimationKeyFrame tmp;
		tmp.time = fTime;

		int n = m_Keys.Count();
		for (int i=0; i<n; i++)
		{
			if ( fTime < m_Keys[i].time )
			{
				m_Keys.Insert(i, tmp);
				return &m_Keys[i];
			}
		}

		m_Keys.PushBack( tmp );
		return &m_Keys[n];
	}

	void AnimationJoint::RemoveKeyFrameByIndex( UINT index )
	{
		if ( index >= (UINT)m_Keys.Count() ) return;
		m_Keys.RemoveByIndex((int)index);
	}

	PAnimationKeyFrame AnimationJoint::GetKeyFrameByIndex( UINT index )
	{
		if ( index >= (UINT)m_Keys.Count() ) return NULL;
		return &m_Keys[(int)index];
	}

	PAnimationKeyFrame AnimationJoint::GetKeyFrame( float fTime )
	{
		if ( !m_Keys.Count() )
		{
			m_curFrame.time = fTime;
			m_curFrame.position = m_Base.position;
			m_curFrame.rotation = m_Base.rotation;
			return &m_curFrame;
		}
		
		int lo=0, hi=m_Keys.Count()-1;
		FindKeyFrames( fTime, lo, hi );
		
		PAnimationKeyFrame pkey1 = &m_Keys[lo];
		PAnimationKeyFrame pkey2 = &m_Keys[hi];
		float t = pkey1->time!=pkey2->time ? (fTime - pkey1->time) / (pkey2->time - pkey1->time) : 0.0f;
		if ( t<=0.0f )
		{
			m_curFrame.time = fTime;
			m_curFrame.position = pkey1->position;
			m_curFrame.rotation = pkey1->rotation;
			return &m_curFrame;
		}
		if ( t>=1.0f )
		{
			m_curFrame.time = fTime;
			m_curFrame.position = pkey2->position;
			m_curFrame.rotation = pkey2->rotation;
			return &m_curFrame;
		}

		m_curFrame.time = fTime;
		m_curFrame.position.Lerp(pkey1->position, pkey2->position, t);
		m_curFrame.rotation.SLerp(pkey1->rotation, pkey2->rotation, t);
		return &m_curFrame;
	}

	void AnimationJoint::FindKeyFrames( float time, int& lo, int& hi )
	{
		int mid = (lo + hi) / 2;
		if ( time <= m_Keys[mid].time ) hi = mid;
		if ( time > m_Keys[mid].time )	lo = mid;
		if ( hi-lo > 1 ) FindKeyFrames( time, lo, hi );
	}

	void AnimationJoint::Save( Stream& stream )
	{
		//  just a simple prepare animation joints
		for (int i=1; i<m_Keys.Count(); i++)
		{
			if ( m_Keys[i].time - m_Keys[i-1].time < EPSILON )
				m_Keys.RemoveByIndex(i);
		}


		int version = 1;
		SEGAN_STREAM_WRITE(stream, version);

		cmn::String_Save(m_Name, &stream);
		cmn::String_Save(m_Parent, &stream);

		SEGAN_STREAM_WRITE(stream, m_Base);
		SEGAN_STREAM_WRITE(stream, m_MaxTime);

		int n = m_Keys.Count();
		SEGAN_STREAM_WRITE(stream, n);
		if (n>0)
			stream.Write( &m_Keys[0], sizeof(AnimationKeyFrame) * n );
	}

	void AnimationJoint::Load( Stream& stream )
	{
		Clear();

		int version = 0;
		SEGAN_STREAM_READ(stream, version);

		if (version == 1)
		{
			cmn::String_Load(m_Name, &stream);
			cmn::String_Load(m_Parent, &stream);

			SEGAN_STREAM_READ(stream, m_Base);
			SEGAN_STREAM_READ(stream, m_MaxTime);

			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			if (n>0)
			{
				m_Keys.SetCount(n);
				stream.Read( &m_Keys[0], sizeof(AnimationKeyFrame) * n );

			}
		}
	}

}} // namespace sx { namespace d3d {