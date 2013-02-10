#include "Mechanic.h"
#include "GameTypes.h"
#include "Game.h"


//////////////////////////////////////////////////////////////////////////
//  some internal variable
static GM::Mechanic* s_Focused = NULL;

GM::Mechanic::Mechanic( void )
{
	//	get sound node
	m_soundNode = sx_new( sx::core::Node );

	PStream pfile = NULL;
	if ( sx::sys::FileManager::File_Open( L"gui_sounds.node", SEGAN_PACKAGENAME_DRAFT, pfile ) )
	{
		m_soundNode->Load(*pfile);
		sx::sys::FileManager::File_Close(pfile);
	}
}

GM::Mechanic::~Mechanic( void )
{
	sx::core::Scene::DeleteNode( m_soundNode );
}

void GM::Mechanic::MsgProc( UINT recieverID, UINT msg, void* data )
{
	switch ( msg )
	{
	case GMT_LEVEL_LOAD:		/////////////////////////////////////////////////    LOAD LEVEL
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_LEVEL_LOAD

	case GMT_LEVEL_CLEAR:		/////////////////////////////////////////////////    CLEAR LEVEL
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_LEVEL_CLEAR

	case GMT_GAME_START:		/////////////////////////////////////////////////    START GAME
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_GAME_START

	case GMT_GAME_END:			/////////////////////////////////////////////////    END GAME
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_GAME_END

	case GMT_GAME_RESETING:		/////////////////////////////////////////////////    GOING TO RESET GAME
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_GAME_RESETING

	case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
		{						//////////////////////////////////////////////////////////////////////////

		}
		break;	//	GMT_GAME_RESET
	}
}

FORCEINLINE bool GM::Mechanic::IsFocused( void )
{
	return s_Focused == this;
}

void GM::Mechanic::SetFocused( bool focused )
{
	if ( focused )
	{
		if ( s_Focused != this )
			s_Focused = this;
	}
	else
	{
		if ( s_Focused == this )
			s_Focused = NULL;
	}

}
FORCEINLINE GM::Mechanic* GM::Mechanic::GetFocused( void )
{
	return s_Focused;
}

bool GM::Mechanic::NotInGame( void )
{
	return !g_game->m_game_currentLevel || g_game->m_game_paused;
}

