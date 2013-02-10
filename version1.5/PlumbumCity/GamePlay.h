/********************************************************************
	created:	2011/11/09
	filename: 	GamePlay.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class to control the game play
*********************************************************************/
#ifndef GUARD_GamePlay_HEADER_FILE
#define GUARD_GamePlay_HEADER_FILE

#include "ImportEngine.h"
#include "Mechanic.h"


class GamePlay
{
public:
	GamePlay(void);
	~GamePlay(void);

	void Initialize(void);

	void Finalize(void);

	void ProcessInput(bool& inputHandled, float elpsTime);

	void Update(float elpsTime);

	void MsgProc(UINT recieverID, UINT msg, void* data);

public:

	GM::arrayPMechanic	m_Mechanics;	//  mechanics of the game

};

#endif	//	GUARD_GamePlay_HEADER_FILE