#include "Mechanic_Music.h"
#include "Game.h"
#include "Player.h"
#include "Scripter.h"
#include "Entity.h"
#include "GameGUI.h"
#include "EntityManager.h"


namespace GM
{

	Mechanic_Music::Mechanic_Music( void ) : Mechanic()
	{
	}

	Mechanic_Music::~Mechanic_Music( void )
	{
	}

	void Mechanic_Music::Initialize( void )
	{

	}

	void Mechanic_Music::Finalize( void )
	{

	}

	void Mechanic_Music::ProcessInput( bool& inputHandled, float elpsTime )
	{

	}

	void Mechanic_Music::Update( float elpsTime )
	{

	}

	void Mechanic_Music::MsgProc( UINT recieverID, UINT msg, void* data )
	{
		switch (msg)
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

		case GMT_GAME_RESET:		/////////////////////////////////////////////////    RESET GAME
			{						//////////////////////////////////////////////////////////////////////////

			}
			break;	//	GMT_GAME_RESET
		}
	}

} // namespace GM
