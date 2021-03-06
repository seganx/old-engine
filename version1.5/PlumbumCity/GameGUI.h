/********************************************************************
	created:	2012/02/21
	filename: 	GameGUI.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple class to handle gui in game
*********************************************************************/
#ifndef GUARD_GameGUI_HEADER_FILE
#define GUARD_GameGUI_HEADER_FILE

#include "ImportEngine.h"
#include "GameMenus.h"

class GameHint;
class GameTips;


class GoldAndPeople : public sx::gui::Form
{
public:
	GoldAndPeople( void );
	~GoldAndPeople( void );
	void ProcessInput(bool& inputHandled, float elpsTime);
	void Update( float elpsTime );

	void Alarm( void );
	void OnClick(sx::gui::PControl sender);
	void OnEnter( sx::gui::PControl sender );
	void OnExit( sx::gui::PControl sender );


public:
	sx::gui::PPanelEx	m_back;
	sx::gui::PanelEx*	m_digit[8];
	sx::gui::PanelEx*	m_alarm;
	sx::gui::PanelEx*	m_info;

	float				m_time;
};


class GameGUI: public sx::gui::Form
{
public:
	GameGUI(void);
	~GameGUI(void);

	void Initialize(void);

	void Finalize(void);

	void ProcessInput(bool& inputHandled, float elpsTime);

	void Update(float elpsTime);

	void Draw(DWORD flag);

	void MsgProc(UINT recieverID, UINT msg, void* data);

	void Add_Front(sx::gui::PControl pGUI);

	void Add_Back(sx::gui::PControl pGUI);

	void Remove(sx::gui::PControl pGUI);

	void ShowTips(const WCHAR* tipText, const DWORD tipColor = 0xffffffff, const WCHAR* tipIcon = NULL);

	void ShowMenu( const WCHAR* name );

	void OnClick( sx::gui::Control* sender );
public:

	sx::gui::PPanelEx			m_powerAttaks;
	sx::gui::TrackBar*			m_gameSpeed;
	sx::gui::Panel*				m_gameSpeedDefault;

	GoldAndPeople*				m_goldPeople;		//	to show gold and people
	GameHint*					m_hint;				//  simple class to handle hints in the game
	Array<GameTips*>			m_tips;				//  array of game tips
	sx::gui::ArrayPControl		m_gui;				//	array of gui to display
	MenuStatus*					m_status;
	MenuMain*					m_main;
	MenuMap*					m_map;
	MenuPlayLevel*				m_playLevel;
	MenuProfile*				m_profile;
	MenuAchievements*			m_achivements;
	MenuSettings*				m_settings;
	MenuCredits*				m_credits;
	MenuConfirmExit*			m_confirmExit;
	MenuPause*					m_pause;
	MenuVictory*				m_victory;
	MenuGameOver*				m_gameOver;
	MenuCinematic*				m_cinematic;
	MenuInfo*					m_info;
	MenuUpgrade*				m_upgradePanel;

	// TEST
	sx::gui::Label*				m_energy;
	//
};

#endif	//	GUARD_GameGUI_HEADER_FILE
