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
#include "gameup_import.h"
#include "Config.h"


class Player;
class GamePlay;
class GameGUI;
class GuideText;

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
#if USE_STEAM_SDK
	Steam					m_steam;					//	using steam API
#else
	Achievement				m_achievements[Achievement_Count];	// achievements
#endif
	MouseState				m_mouseMode;				// state mode of mouse

	float					m_difficultyValue;			//	difficulty value
	int						m_difficultyLevel;			//	difficulty level 0 = norm , 1 = hard , 2 = insane
	int						m_game_currentLevel;		//  index of current level. 0 = GUI mode
	int						m_game_nextLevel;			//  index of current level. 0 = GUI mode
	int						m_game_restarting;			//	restart the game
	bool					m_game_paused;				//  game paused
	bool					m_app_Paused;				//  application paused
	bool					m_app_Closing;				//  application is closing
	int						m_app_Loading;				//  application is loading resources. value is number of objects to load
	bool					m_miniGame;					//	verify to load mini game map
	sx::gui::PPanel			m_panel_Cursor;				//  cursor of the game
	sx::gui::PPanelEx		m_panel_Loading;			//  loading panel

	sx::sys::Window*		m_window;					//	rendering context

	Array<GuideText*>		m_guides;					//	array of strings for the guide panels
};

//	global game pointer
extern Game* g_game;

#if USE_GAMEUP
extern GameUp* g_gameup;

void gameup_add_score( const uint reason );
#endif

#endif	//	GUARD_Game_HEADER_FILE