#include "sxAnimation.h"
#include "../sxSystem/sxSystem.h"

typedef Map<UINT, sx::d3d::PAnimation> sxMapAnimation;
static 	sxMapAnimation AnimationMap;				//  hold the created animation objects

static const char*	AnimationTitleFile = "SeganX Animation File - sajad.b@gmail.com ";

class DerivedAnimation : public sx::d3d::Animation
{
	SEGAN_STERILE_CLASS(DerivedAnimation);

public:
	DerivedAnimation( void ): m_ID( sx::cmn::ID_Generate() ), m_RefCount(1), m_maxTime(0.0f)
	{
		m_Option = 0;
		m_speed = 1.0f;
		m_blendTime = 1.0f;
	}

	~DerivedAnimation( void )
	{
		Clear();
	}

	DWORD GetID( void )
	{
		return m_ID;
	}

	void SetSource(const WCHAR* srcAddress)
	{
		UINT id = sx::cmn::GetCRC32(srcAddress);
		if (id == m_ID) return;

		sx::d3d::PAnimation a = NULL;
		if (AnimationMap.Find(m_ID, a) && a==this)
		{
			AnimationMap.Remove(m_ID);
			AnimationMap.Insert(id, this);
		}

		PStream pFile = NULL;
		if (sx::sys::FileManager::File_Open(srcAddress, SEGAN_PACKAGENAME_ANIMATION, pFile))
		{
			Load(*pFile);
			sx::sys::FileManager::File_Close(pFile);
		}

		m_Src = srcAddress;
		m_ID = id;
	}

	const WCHAR* GetSource(void)
	{
		return m_Src;
	}

	void Clear( void )
	{
		m_maxTime = -10.0f;	// flag to update max time in next GetMaxTime() call
		for (int i=0; i<m_AnimJoints.Count(); i++)
		{
			sx_delete_and_null( m_AnimJoints[i] );
		}
		m_AnimJoints.Clear();
	}

	int AnimJointCount( void )
	{
		return m_AnimJoints.Count();
	}

	sx::d3d::PAnimationJoint AddAnimJoint( const WCHAR* name )
	{
		m_maxTime = -10.0f;	// flag to update max time in next GetMaxTime() call

		sx::d3d::PAnimationJoint PAnim = sx_new( sx::d3d::AnimationJoint );
		PAnim->m_Name = name;
		m_AnimJoints.PushBack(PAnim);
		return PAnim;
	}

	void RemoveAnimJointByIndex( UINT index )
	{
		if ( index >= (UINT)m_AnimJoints.Count() ) return;
		m_AnimJoints.RemoveByIndex((int)index);
		m_maxTime = -10.0f;	// flag to update max time in next GetMaxTime() call
	}

	sx::d3d::PAnimationJoint GetAnimJointByIndex( UINT index )
	{
		if ( index >= (UINT)m_AnimJoints.Count() ) return NULL;
		return m_AnimJoints[index];
	}

	int GetIndexOfAnimJoint( sx::d3d::PAnimationJoint pAnimData )
	{
		for (int i=0; i<m_AnimJoints.Count(); i++)
		{
			if ( pAnimData == m_AnimJoints[i] )
				return i;
		}
		return -1;
	}

	sx::d3d::PAnimationJoint GetAnimJointByName( const WCHAR* name )
	{
		if ( !name ) return NULL;

		for (int i=0; i<m_AnimJoints.Count(); i++)
		{
			sx::d3d::PAnimationJoint pAnimData = m_AnimJoints[i];
			if ( !pAnimData->m_Name.Text() ) continue;

			if ( pAnimData->m_Name == name )
				return pAnimData;
		}
		return NULL;
	}

	PAnimationKeyFrame GetKeyFrame( UINT index, float fTime )
	{
		if ( index >= (UINT)m_AnimJoints.Count() ) return NULL;
		return m_AnimJoints[index]->GetKeyFrame( fTime );
	}

	float GetMaxTime( void )
	{
		if ( m_AnimJoints.IsEmpty() ) return 0.0f;

		if ( m_maxTime<0 )
		{
			for (int i=0; i<m_AnimJoints.Count(); i++)
			{
				if ( m_maxTime < m_AnimJoints[i]->m_MaxTime ) m_maxTime = m_AnimJoints[i]->m_MaxTime;
			}
		}

		return m_maxTime;
	}

	void Save( Stream& stream )
	{
		//  write the version and title
		int ver = 2;
		stream.Write( &ver, sizeof(ver) );
		stream.Write( AnimationTitleFile, (int)strlen(AnimationTitleFile) );

		MemoryStream mem;

		SEGAN_STREAM_WRITE( mem, m_Option );
		SEGAN_STREAM_WRITE( mem, m_blendTime );

		int n = m_AnimJoints.Count();
		SEGAN_STREAM_WRITE(mem, n);
		for (int i=0; i<n; i++)
			m_AnimJoints[i]->Save(mem);

		mem.SetPos(0);
		sx::sys::ZCompressStream(mem, stream);

		m_maxTime = -10.0f;	// flag to update max time in next GetMaxTime() call
	}

	void Load( Stream& stream )
	{
		Clear();

		//  at first read the version and title
		int fileVer = 0; char ch[1024];
		stream.Read(&fileVer, sizeof(fileVer));
		stream.Read(ch, (int)strlen(AnimationTitleFile));

		if (fileVer == 1)
		{
			MemoryStream mem;
			sx::sys::ZDecompressStream(stream, mem);
			mem.SetPos(0);

			int n = 0;
			SEGAN_STREAM_READ(mem, n);
			for (int i=0; i<n; i++)
			{
				sx::d3d::PAnimationJoint tmp = sx_new( sx::d3d::AnimationJoint );
				tmp->Load(mem);
				m_AnimJoints.PushBack(tmp);
			}
		}
		else if ( fileVer == 2 )
		{
			MemoryStream mem;
			sx::sys::ZDecompressStream(stream, mem);
			mem.SetPos(0);

			SEGAN_STREAM_READ( mem, m_Option );
			SEGAN_STREAM_READ( mem, m_blendTime );

			int n = 0;
			SEGAN_STREAM_READ(mem, n);
			for (int i=0; i<n; i++)
			{
				sx::d3d::PAnimationJoint tmp = sx_new( sx::d3d::AnimationJoint );
				tmp->Load(mem);
				m_AnimJoints.PushBack(tmp);
			}
		}

		m_maxTime = -10.0f;	// flag to update max time in next GetMaxTime() call
	}

public:
	String		m_Src;								//  Address of resource file in *.anim format
	DWORD				m_ID;								//  ID of the animation use for fast search
	int					m_RefCount;							//  number of references objects
	float				m_maxTime;
	Array<sx::d3d::PAnimationJoint>	m_AnimJoints;	//  array of animation data
};

namespace sx { namespace d3d {
	bool Animation::Manager::Exist( OUT PAnimation& pAnim, const WCHAR* src )
	{
		UINT key = sx::cmn::GetCRC32(src);
		return AnimationMap.Find(key, pAnim);
	}

	bool Animation::Manager::Get( OUT PAnimation& pAnim, const WCHAR* src )
	{
		if (!src) 
		{
			pAnim = NULL;
			return false;
		}

		UINT key = sx::cmn::GetCRC32(src);

		if (AnimationMap.Find(key, pAnim))
		{
			DerivedAnimation* a = (DerivedAnimation*)pAnim;
			a->m_RefCount++;
		}
		else
		{
			if (Create(pAnim, src))
			{
				AnimationMap.Insert(key, pAnim);
			}
			else return false;
		}

		return true;
	}

	bool Animation::Manager::Create( OUT PAnimation& pAnim, const WCHAR* src )
	{
		pAnim = sx_new( DerivedAnimation );
		if (pAnim)
		{
			pAnim->SetSource(src);
			return true;
		}
		else return false;
	}

	void Animation::Manager::Release( PAnimation& pAnim )
	{
		if (!pAnim) return;
		DerivedAnimation* a = (DerivedAnimation*)pAnim;

		pAnim = NULL;
		a->m_RefCount--;
		if (a->m_RefCount>0) return;

		//  remove from animation map
		PAnimation anim = NULL;
		if (AnimationMap.Find(a->m_ID, anim) && anim==a)
			AnimationMap.Remove(a->m_ID);

		sx_delete_and_null(a);
	}

	void Animation::Manager::ClearAll( void )
	{
		if ( AnimationMap.IsEmpty() ) return;
		sx_callstack();

		for (sxMapAnimation::Iterator it = AnimationMap.First(); !it.IsLast(); it++)
		{
			DerivedAnimation* a = (DerivedAnimation*)(*it);
			sx_delete_and_null(a);
		}
		AnimationMap.Clear();
	}

	Map<UINT, sx::d3d::PAnimation>::Iterator& Animation::Manager::GetFirst( void )
	{
		static sxMapAnimation::Iterator it = AnimationMap.First();
		it = AnimationMap.First();
		return it;
	}

}} // namespace sx { namespace d3d {