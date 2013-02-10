#include "sxNodeMember.h"
#include "sxNode.h"

#include "sxMesh.h"
#include "sxAnimator.h"
#include "sxParticle.h"
#include "sxPathNode.h"
#include "sxTerrain.h"
#include "sxSound.h"

namespace sx { namespace core {

	NodeMember::NodeMember( void ): m_Owner(NULL), m_Option(0), m_Type(NMT_NULL)
	{

	}

	NodeMember::~NodeMember( void )
	{
		sx_callstack_push(NodeMember::~NodeMember());
		SetOwner(NULL);
	}

	FORCEINLINE void NodeMember::SetName( const WCHAR* pwcName )
	{
		m_Name = pwcName;
	}

	FORCEINLINE const WCHAR* NodeMember::GetName( void )
	{
		return m_Name;
	}

	FORCEINLINE NodeMemberType NodeMember::GetType( void )
	{
		return m_Type;
	}

	FORCEINLINE void NodeMember::AddOption( DWORD option )
	{
		m_Option |= option;
	}

	FORCEINLINE void NodeMember::RemOption( DWORD option )
	{
		SEGAN_SET_REM(m_Option, option);
	}

	FORCEINLINE bool NodeMember::HasOption( DWORD option )
	{
		return SEGAN_SET_HAS(m_Option, option);
	}

	void NodeMember::SetOwner( PNode pOwner )
	{
		if (pOwner == m_Owner) return;

		if (m_Owner) 
		{
			//  remove this from owner's list
			m_Owner->m_Member.Remove(this);

			//  unregister this from owner's registry type
			bool noTypeExist = true;
			for (int i=0; i<m_Owner->m_Member.Count(); i++)
			{
				if (m_Owner->m_Member[i]->GetType() == m_Type)
				{
					noTypeExist = false;
					break;
				}
			}
			if (noTypeExist) SEGAN_SET_REM(m_Owner->m_RegTypes, m_Type);

			//  send message to the owner to notify that this member has been detached
			m_Owner->MsgProc(MT_MEMBER_DETACHED, this);
		}

		//  set new owner for this
		m_Owner = pOwner;

		if (m_Owner) 
		{
			//  add this to the new owner's list
			m_Owner->m_Member.PushBack(this);

			//  register this to the owner's registry type
			SEGAN_SET_ADD(m_Owner->m_RegTypes, m_Type);

			//  send message to the owner to notify that this member has been attached
			m_Owner->MsgProc(MT_MEMBER_ATTACHED, this);
		}
	}

	FORCEINLINE sx::core::PNode NodeMember::GetOwner( void )
	{
		return m_Owner;
	}

	FORCEINLINE void NodeMember::Update( float elpsTime )
	{

	}

	FORCEINLINE void NodeMember::Draw( DWORD flag )
	{

	}

	FORCEINLINE void NodeMember::DrawByViewParam( DWORD flag, float viewParam )
	{

	}

	FORCEINLINE UINT NodeMember::MsgProc( UINT msgType, void* data )
	{
		return msgType;
	}

	void NodeMember::Save( Stream& stream )
	{
		sx::cmn::String_Save(m_Name, &stream);
		stream.Write(&m_Option, sizeof(m_Option));
	}

	void NodeMember::Load( Stream& stream )
	{
		sx::cmn::String_Load(m_Name, &stream);
		stream.Read(&m_Option, sizeof(m_Option));
	}


	//////////////////////////////////////////////////////////////////////////
	//  static functions
	//////////////////////////////////////////////////////////////////////////
	NodeMember* NodeMember::Create( NodeMemberType type )
	{
		switch (type)
		{
		case NMT_MESH:				return sx_new( Mesh );
		case NMT_SOUND:				return sx_new( Sound	);
		case NMT_ANIMATOR:			return sx_new( Animator );
		case NMT_PARTICLE:			return sx_new( Particle );
		case NMT_PATHNODE:			return sx_new( PathNode );
		case NMT_TERRAIN:			return sx_new( Terrain  );
		}

		return NULL;
	}


}} // namespace sx { namespace core {