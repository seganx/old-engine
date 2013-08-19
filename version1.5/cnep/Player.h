/********************************************************************
	created:	2011/07/10
	filename: 	Player.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain player object
*********************************************************************/
#ifndef GUARD_Player_HEADER_FILE
#define GUARD_Player_HEADER_FILE

#include "ImportEngine.h"
#include "Mechanic.h"
#include "GameUtils.h"
#include "GameTypes.h"

class Entity;

class Player
{
public:
	Player(void);
	~Player(void);

	void Initialize(void);

	void Finalize(void);

	void ProcessInput(bool& inputHandled, float elpsTime);

	void Update(float elpsTime);

	void MsgProc(UINT recieverID, UINT msg, void* data);

	void SyncPlayerAndGame( bool playerToGame );

	void ClearMechanincs( void );
public:
	
	GU::Camera_Mobile			m_camera_MBL;
	GU::Camera_RTS				m_camera_RTS;
	GU::Camera_RTS				m_camera_Pause;
	GM::arrayPMechanic			m_Mechanics;

public:

	String				m_name;				//	name of player
	int							m_gold;				//  gold
	int							m_people;			//	current people

	PlayerProfile				m_profile;
};

#endif	//	GUARD_Player_HEADER_FILE
