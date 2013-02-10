#include "com_ShowRange.h"
#include "Entity.h"
#include "GameConfig.h"
#include "Game.h"

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES
static int				s_initialCount	= 0;
static D3DColor			s_color			= 0xffffffff;
static sx::core::PNode	s_cylinderNode	= NULL;


com_ShowRange::com_ShowRange( void ): Component()
{
	sx_callstack_push(com_ShowRange::com_ShowRange());

	m_name = L"Show Range";
}

com_ShowRange::~com_ShowRange( void )
{
	sx_callstack_push(com_ShowRange::~com_ShowRange());
}

void com_ShowRange::Initialize( void )
{
	sx_callstack_push(com_ShowRange::Initialize());

	if ( s_initialCount == 0 )
	{
		PStream file = NULL;
		if ( sx::sys::FileManager::File_Open(L"node_Range.node", SEGAN_PACKAGENAME_DRAFT, file) )
		{
			s_cylinderNode = sx_new( sx::core::Node );
			s_cylinderNode->Load( *file );

			float f=0;
			s_cylinderNode->MsgProc(MT_ACTIVATE, &f);
			
			sx::sys::FileManager::File_Close( file );

			if ( Config::GetData()->display_Debug == 3 )
			{
				sxLog::Log( L" cylinder node created !" );
			}
		}
	}
	s_initialCount++;
}

void com_ShowRange::Finalize( void )
{
	sx_callstack_push(com_ShowRange::Finalize());

	if ( s_initialCount )
	{
		s_initialCount--;

		if ( s_initialCount == 0 )
		{
			if ( Config::GetData()->display_Debug == 3 )
			{
				sxLog::Log( L" deleting cylinder node !" );
			}

			sx::core::Scene::DeleteNode( s_cylinderNode );
			s_cylinderNode = NULL;
		}
	}
}

void com_ShowRange::Update( float elpsTime )
{
	sx_callstack_push(com_ShowRange::Update());

	Entity* pEntity = Entity::GetSelected();

	if ( !pEntity || pEntity->m_health.icur < 1 )
	{
		sx::core::Scene::RemoveNode( s_cylinderNode );
		s_color = 0xffffffff;
	}
	else if ( pEntity && m_owner == pEntity )
	{
		sx::core::Scene::AddNode( s_cylinderNode );
		s_cylinderNode->SetPosition( m_owner->GetPosition() );
		sx::core::PMesh mesh = (sx::core::PMesh)s_cylinderNode->GetMemberByIndex(0);
		if ( mesh )
		{
			mesh->GetActiveMaterial()->SetFloat( 0 , m_owner->m_curAttack.maxRange<1 ? 1.0f : m_owner->m_curAttack.maxRange );
			mesh->GetActiveMaterial()->SetFloat( 1 , m_owner->m_curAttack.minRange );

			if ( m_owner->m_initialized )
				s_color = 0xffffffff;

			mesh->GetActiveMaterial()->SetFloat4( 0 , float4(s_color.r, s_color.g, s_color.b, s_color.a) );
		}
	}
}

void com_ShowRange::MsgProc( UINT msg, void* data )
{
	sx_callstack_push(com_ShowRange::MsgProc(msg=%d), msg);

	switch ( msg )
	{
	case GMT_GAME_RESET:
	case GMT_GAME_RESETING:
	case GMT_GAME_PAUSED:
			sx::core::Scene::RemoveNode( s_cylinderNode );
			break;
	}
}

Component* com_ShowRange::Clone( void )
{
	sx_callstack_push(com_ShowRange::Clone());

	return sx_new( com_ShowRange );
}

void com_ShowRange::SetColor( D3DColor color )
{
	sx_callstack_push(com_ShowRange::SetColor());
	s_color = color;
}
