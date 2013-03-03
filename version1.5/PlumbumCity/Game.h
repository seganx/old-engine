/********************************************************************
	created:	2011/07/10
	filename: 	Game.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the the main class of game
*********************************************************************/
#ifndef GUARD_Game_HEADER_FILE
#define GUARD_Game_HEADER_FILE

#include "ImportEngine.h"
#include "GameTypes.h"

#define GAME_TITLE	L"PlumbumCity ver 0.1"


class Player;
class GamePlay;
class GameGUI;

//////////////////////////////////////////////////////////////////////////
//  main class of the game
class Game
{
	Game(void);
	~Game(void);

public:	//  static functions

	//! return a unique ID
	static UINT	GetNewID(void);

	//! create game instance
	static void Initialize( sx::sys::Window* win );

	//! destroy game instance
	static void Finalize(void);

	//! return path folder of current level. return null if level = 0
	static const WCHAR* GetLevelPath(void);

public:	//  member functions

	//! use folder name to load a level
	void LoadLevel(void);

	//! clear level
	void ClearLevel(void);

	//! reset the game
	void Reset(void);

	//! main game loop
	void Update(float elpsTime);

	//! main render function
	void Render( DWORD flag );

	//! post message to the game
	void PostMessage(UINT RecieverID, UINT msg, void* data);

public:

	GamePlay*				m_gamePlay;					// game play
	Player*					m_player;					// player of the game
	GameGUI*				m_gui;						// GUI in the game
	Upgrades				m_upgrades;					// upgrade structure
	Achievement				m_achievements[15];			// achievements
	MouseState				m_mouseMode;				// state mode of mouse

	float					m_difficultyValue;			//	difficulty value
	int						m_difficultyMode;			//	difficulty level
	int						m_currentLevel;				//  index of current level. 0 = GUI mode
	int						m_nextLevel;				//  index of current level. 0 = GUI mode
	int						m_gameMode;					//  game mode : 0=default, 1=warrior, 2=legend
	bool					m_gamePaused;				//  game paused
	bool					m_wavesComming;				//	waves are coming
	bool					m_app_Paused;				//  application paused
	bool					m_app_Closing;				//  application is closing
	int						m_app_Loading;				//  application is loading resources. value is number of objects to load
	bool					m_miniGame;					//	verify to load mini game map
	sx::gui::PPanel			m_panel_Cursor;				//  cursor of the game
	sx::gui::PPanelEx		m_panel_Loading;			//  loading panel

	sx::sys::Window*		m_window;					//	rendering context
};

//	global game pointer
extern Game* g_game;

#endif	//	GUARD_Game_HEADER_FILE