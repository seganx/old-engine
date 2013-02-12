#include "Mechanic_MT_Machinegun.h"
#include "Game.h"
#include "Player.h"
#include "Scripter.h"
#include "Entity.h"
#include "GameGUI.h"
#include "EntityManager.h"


namespace GM
{

	Mechanic_MT_Machinegun::Mechanic_MT_Machinegun( void ) : Mechanic()
		, m_tower(null)
		, m_nodeWeapon(null)
		, m_nodePipe[4]
		, m_pipeIndex(0)
		, m_shootCount(0)
		, m_shootTime(0)
		, m_Dir(0,0,1)
		, m_DirOffset(0,0,1)
		, m_fire(0)
		, m_magazineCap(0)
		, m_bullets(0)
		, m_reloadTime(0)
	{
		sx_callstack();

		ZeroMemory( m_nodePipe, sizeof(m_nodePipe) );
	}

	Mechanic_MT_Machinegun::~Mechanic_MT_Machinegun( void )
	{
		sx_callstack();

	}

	void Mechanic_MT_Machinegun::Initialize( void )
	{
		sx_callstack();

	}

	void Mechanic_MT_Machinegun::Finalize( void )
	{
		sx_callstack();

	}

	void Mechanic_MT_Machinegun::ProcessInput( bool& inputHandled, float elpsTime )
	{
		if ( !NotInGame() ) return;
		if ( g_game->m_mouseMode != MS_Null && g_game->m_mouseMode != MS_ManualTower ) return;
		if ( g_game->m_mouseMode == MS_ManualTower && !m_tower ) return;
		sx_callstack();

		if ( g_game->m_mouseMode == MS_ManualTower )
		{

		}


	}

	void Mechanic_MT_Machinegun::Update( float elpsTime )
	{
		sx_callstack();

	}

	void Mechanic_MT_Machinegun::MsgProc( UINT recieverID, UINT msg, void* data )
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

	void Mechanic_MT_Machinegun::EnterToManula( const Entity* tower )
	{

	}

	void Mechanic_MT_Machinegun::LeaveManual( void )
	{

	}

	void Mechanic_MT_Machinegun::ShootTheBullet( const prpAttack* pAttack, const float3& dir, const bool special )
	{

	}

} // namespace GM
