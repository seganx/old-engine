#include "com_ShowRange.h"
#include "Entity.h"
#include "GameConfig.h"
#include "Game.h"
#include "GameGUI.h"
#include "Player.h"

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES
static int				s_initialCount	= 0;
static D3DColor			s_color			= 0xffffffff;
static sx::core::PNode	s_cylinderNode	= NULL;
static sx::gui::Label*	s_entityName	= null;


com_ShowRange::com_ShowRange( void ): Component()
{
	sx_callstack();

	m_name = L"Show Range";
}

com_ShowRange::~com_ShowRange( void )
{
	sx_callstack();
}

void com_ShowRange::Initialize( void )
{
	sx_callstack();

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

#if 1
		s_entityName = sx_new( sx::gui::Label );
		s_entityName->SetSize( float2(256, 64) );
		s_entityName->AddProperty( SX_GUI_PROPERTY_BILLBOARD );
		s_entityName->RemProperty( SX_GUI_PROPERTY_VISIBLE );
		s_entityName->SetFont( FONT_HUD_TITLE );
		s_entityName->SetAlign( GTA_CENTER );
		//s_entityName->AddProperty( SX_GUI_PROPERTY_3DSPACE );
		s_entityName->GetElement(0)->Color().a = 0;
		s_entityName->GetElement(1)->Color() = D3DColor(1.0f, 0.3f, 0.3f, 1.0f);

		g_game->m_gui->Add_Back( s_entityName );
#endif
	}

	s_initialCount++;
}

void com_ShowRange::Finalize( void )
{
	sx_callstack();

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


			g_game->m_gui->Remove( s_entityName );
			sx_delete_and_null( s_entityName );
		}
	}
}

void com_ShowRange::Update( float elpsTime )
{
	sx_callstack();

	Entity* pEntity = Entity::GetSelected();

	if ( !pEntity || pEntity->m_health.icur < 1 || !g_game->m_player->m_camera_RTS.m_Activate )
	{
		s_entityName->RemProperty( SX_GUI_PROPERTY_VISIBLE );
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

		bool showTitle = m_owner->m_partyCurrent == PARTY_TOWER && m_owner->m_initialized;
		if ( showTitle )
		{
			s_entityName->SetText( m_owner->m_displayName.Text() );
			s_entityName->Position() = m_owner->GetPosition();
			s_entityName->Position().y = 7.0f - 10.0f / g_game->m_player->m_camera_RTS.m_Rad;
			s_entityName->AddProperty( SX_GUI_PROPERTY_VISIBLE );
		}
		else s_entityName->RemProperty( SX_GUI_PROPERTY_VISIBLE );
	}
}

void com_ShowRange::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(com_ShowRange::MsgProc(msg=%d), msg);

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
	sx_callstack();

	return sx_new( com_ShowRange );
}

void com_ShowRange::SetColor( D3DColor color )
{
	sx_callstack();
	s_color = color;
}
