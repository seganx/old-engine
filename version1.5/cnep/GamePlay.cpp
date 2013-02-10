#include "GamePlay.h"
#include "Game.h"
#include "Entity.h"
#include "Player.h"
#include "EntityManager.h"
#include "com_weapon_MachineGun.h"
#include "ProjectileManager.h"
#include "Mechanic_EnemyWave.h"


GamePlay::GamePlay( void )
{
	m_Mechanics.PushBack( sx_new( GM::Mechanic_EnemyWaves ) );
}

GamePlay::~GamePlay( void )
{
	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		sx_delete_and_null( m_Mechanics[i] );
	}
	m_Mechanics.Clear();
}

void GamePlay::Initialize( void )
{
	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		m_Mechanics[i]->Initialize();
	}
}

void GamePlay::Finalize( void )
{
	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		m_Mechanics[i]->Finalize();
	}
}

void GamePlay::ProcessInput( bool& inputHandled, float elpsTime )
{
	sx_callstack_push(GamePlay::ProcessInput());

	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		m_Mechanics[i]->ProcessInput(inputHandled, elpsTime);
	}
}

void GamePlay::Update( float elpsTime )
{
	sx_callstack_push(GamePlay::Update());

	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		m_Mechanics[i]->Update(elpsTime);
	}
}

void GamePlay::MsgProc( UINT recieverID, UINT msg, void* data )
{
	sx_callstack_push(GamePlay::MsgProc(recieverID=%d, msg=%d), recieverID, msg);

	for (int i=0; i<m_Mechanics.Count(); i++)
	{
		m_Mechanics[i]->MsgProc(recieverID, msg, data);
	}	
}