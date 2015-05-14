/********************************************************************
	created:	2012/05/12
	filename: 	GameMenus.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some class of menus in the game
*********************************************************************/
#ifndef GUARD_GameMenus_HEADER_FILE
#define GUARD_GameMenus_HEADER_FILE

#include "ImportEngine.h"
#include "GameUtils.h"
#include "GameTypes.h"
#include "Config.h"

class GameGuide;

class Menu: public sx::gui::Form
{
public:
	Menu();
	virtual ~Menu();
	virtual void Initialize(void);
	virtual void Finalize(void);
	virtual void ProcessInput(bool& inputHandled, float elpsTime);
	virtual void Update(float elpsTime);
	virtual void Draw(DWORD flag);
	virtual void MsgProc(UINT recieverID, UINT msg, void* data);
	virtual void Show(void);
	virtual void Hide(void);
	virtual bool IsVisible(void);
	void OnEnter( sx::gui::PControl sender );
	void OnExit( sx::gui::PControl sender );
public:
	sx::core::Node*		m_soundNode;
	sx::gui::PPanelEx	m_back;
};

class MenuConfirmProfile: public Menu
{
public:
	virtual void Initialize(void);
	virtual void Finalize(void);
	virtual void ProcessInput(bool& inputHandled, float elpsTime);

	virtual void Show(void);
	virtual void Hide(void);

	void OnClick(sx::gui::PControl sender);

public:
	sx::gui::PanelEx*	m_form;
	sx::gui::Button*	m_yes;
	sx::gui::Button*	m_no;
};

class MenuConfirmMenu: public Menu
{
public:
	virtual void Initialize(void);
	virtual void Finalize(void);
	virtual void ProcessInput(bool& inputHandled, float elpsTime);

	virtual void Show(void);
	virtual void Hide(void);

	void OnClick(sx::gui::PControl sender);

public:
	sx::gui::PanelEx*	m_form;
	sx::gui::Button*	m_yes;
	sx::gui::Button*	m_no;
};

class MenuConfirmExit: public Menu
{
public:
	virtual void Initialize(void);
	virtual void Finalize(void);
	virtual void ProcessInput(bool& inputHandled, float elpsTime);

	virtual void Show( bool cancelPause = true );
	virtual void Hide(void);

	void OnClick(sx::gui::PControl sender);

public:
	bool				m_cancelPause;
	sx::gui::PanelEx*	m_form;
	sx::gui::Button*	m_yes;
	sx::gui::Button*	m_no;
};

class MenuMain: public Menu
{
public:
	void Initialize(void);
	void Finalize(void);
	void ProcessInput(bool& inputHandled, float elpsTime);
	void Update(float elpsTime);
	void Draw(DWORD flag);
	void MsgProc(UINT recieverID, UINT msg, void* data);

	void OnClick(sx::gui::PControl sender);
	void OnEnter( sx::gui::PControl sender );
	void OnExit( sx::gui::PControl sender );

	void Show(void);
	void Hide(void);

public:
	sx::gui::PanelEx*		m_mainBack;
	sx::gui::PanelEx*		m_mainSparks;
	sx::gui::PanelEx*		m_slantBack;
	sx::gui::PanelEx*		m_btn[6];
	float					m_time;
	class MenuInfo*			m_info;
};

class MenuUpgrade: public Menu
{
public:
	void Initialize(void);
	void Finalize(void);
	void ProcessInput(bool& inputHandled, float elpsTime);
	void MsgProc(UINT recieverID, UINT msg, void* data);
	void Update(float elpsTime);
	void Show(void);
	void Hide(void);
	void OnClick(sx::gui::PControl sender);
	void OnCheckEnter(sx::gui::PControl sender);
	void OnCheckExit(sx::gui::PControl sender);
	void SetData( const int level, const int points, const int* upgrades );
	void GetData( int* upgrades );

public:
	int					m_points;
	sx::gui::PLabel		m_stars;
	sx::gui::PButton	m_goback;
	sx::gui::PPanel		m_border;
	sx::gui::CheckBox*	m_checks[44];
	sx::gui::Label*		m_labels[44];
	sx::gui::Label*		m_desc;
};

class MenuMap: public Menu
{
public:
	void Initialize(void);
	void Finalize(void);
	void ProcessInput(bool& inputHandled, float elpsTime);
	void Update(float elpsTime);
	void Draw(DWORD flag);
	void Show(void);
	void Hide(void);
	void OnClick(sx::gui::PControl sender);
	void OnScroll(sx::gui::PControl sender);

	void OnEnter_( sx::gui::PControl sender );
	void OnExit_( sx::gui::PControl sender );

public:

	struct Level_GUI
	{
		sx::gui::PanelEx*	m_road;
		sx::gui::PanelEx*	m_area;
		sx::gui::PanelEx*	m_flag;
		sx::gui::PanelEx*	m_star[3];
		sx::gui::Button*	m_button;
	};
	Level_GUI			m_levels[10];
	sx::gui::PanelEx*	m_playGame;
	sx::gui::PanelEx*	m_miniGame;
	sx::gui::PanelEx*	m_upgrade;
	sx::gui::Button*	m_goback;
	sx::gui::PanelEx*	m_chooser;
	sx::gui::TrackBar*	m_diff_scroll;
	sx::gui::Label*		m_diff_label;
	int					m_frame;
	int					m_selectedLevel;

	GameGuide*			m_guide;
};

class MenuProfile: public Menu
{
public:
	void Initialize(void);
	void Finalize(void);
	void ProcessInput(bool& inputHandled, float elpsTime);
	void MsgProc(UINT recieverID, UINT msg, void* data);
	void Update(float elpsTime);
	void Draw(DWORD flag);
	void Show(void);
	void Hide(void);

	void OnClick(sx::gui::PControl sender);
	void OnEnter( sx::gui::PControl sender );
	void OnExit( sx::gui::PControl sender );
	void OnKey(sx::gui::PControl sender);
	void OnDeleteProfile(void);

	void SyncAchievements( void );
	void SyncProfileAndPlayer( bool profileToPlayer );
	void SaveProfile(void);

public:
	MenuConfirmProfile	m_confirm;

	sx::gui::TextEdit*	m_profileName;
	sx::gui::Button*	m_rename;
	sx::gui::Button*	m_delete;
	sx::gui::Button*	m_goback;

	sx::gui::PanelEx*	m_profPanel[4];
	PlayerProfile		m_profiles[4];
	int					m_profIndex;
};

#if USE_STEAM_SDK
#else
class MenuAchievements: public Menu
{
public:
	void Initialize( void );
	void ProcessInput(bool& inputHandled, float elpsTime);
	void OnClick(sx::gui::PControl sender);
	void OnMouseEnter(sx::gui::Control* sender);
	void OnMouseExit(sx::gui::Control* sender);
	void Show(void);

public:
	sx::gui::Button*	m_goback;
	sx::gui::Panel*		m_icon[15];
	sx::gui::Panel*		m_image;
	sx::gui::Label*		m_name;
	sx::gui::Label*		m_desc;
};
#endif

class MenuSettings: public Menu
{
public:
	void Initialize(void);
	void ProcessInput(bool& inputHandled, float elpsTime);
	void Show(void);

	void OnClick(sx::gui::PControl sender);
	void OnScroll(sx::gui::PControl sender);

public:
	sx::gui::CheckBox*	m_fullscreen;
	sx::gui::TrackBar*	m_shader;
	sx::gui::TrackBar*	m_shadow;
	sx::gui::TrackBar*	m_reflection;
	sx::gui::TrackBar*	m_music;
	sx::gui::TrackBar*	m_sound;
	sx::gui::TrackBar*	m_mouse;
	sx::gui::TrackBar*	m_language;

	sx::gui::Button*	m_goback;
	bool				m_applyChange;

	struct Languages
	{
		wchar	file[64];
		wchar	font[128];
		wchar	name[64];
		float	x, y;
	};
	Array<Languages>	m_langs;	//	list of available languages
};

class MenuCredits: public Menu
{
public:
	virtual void Initialize(void);
	virtual void Finalize(void);
	virtual void ProcessInput(bool& inputHandled, float elpsTime);

	void OnClick(sx::gui::PControl sender);
};

class MenuStatus: public Menu
{
public:
	virtual void Initialize(void);
	void Update(float elpsTime);
	void OnClick(sx::gui::PControl sender);

public:
	sx::gui::Label*			m_playerName;
	sx::gui::ProgressBar*	m_battery;
	sx::gui::Button*		m_exit;
};

class MenuPause: public Menu
{
public:
	void Initialize(void);
	void Finalize(void);
	void ProcessInput(bool& inputHandled, float elpsTime);
	void Update(float elpsTime);
	void Draw(DWORD flag);
	void MsgProc(UINT recieverID, UINT msg, void* data);
	void Show(void);
	void Hide(void);
	void OnClick(sx::gui::PControl sender);

public:
	MenuConfirmMenu		m_confirmMenu;
};

class MenuVictory: public Menu
{
public:
	void Initialize(void);
	void Finalize(void);
	void ProcessInput(bool& inputHandled, float elpsTime);
	void Update(float elpsTime);
	void MsgProc(UINT recieverID, UINT msg, void* data);
	void Show(void);
	void Hide(void);

	void ApplyChangesToProfile(void);
	void OnClick(sx::gui::PControl sender);

public:
	int					m_starAdd;
	int					m_starCount;
	sx::gui::PanelEx*	m_stars[3];
	sx::gui::Panel*		m_stars_back[2];
	sx::gui::Button*	m_upgrade;
	bool				m_upgrade_exist;

	int					m_goldCounter;
	int					m_golds;
	sx::gui::Label*		m_goldLabel;

	int					m_peoples;
	sx::gui::Label*		m_peopleLabel;

	float				m_time;
	float				m_shakeTime;
	float				m_starTime;
	float				m_goldTime;
	sx::gui::Button*	m_nextLevel;
	GameGuide*			m_guide;

	struct APL
	{
		sx::gui::Label*		label;
		float				age;
	};
	Array<APL*> m_apl;

};

class MenuGameOver: public Menu
{
public:
	void Initialize(void);
	void ProcessInput(bool& inputHandled, float elpsTime);
	void Show(void);
	void OnClick(sx::gui::PControl sender);
};

class MenuCinematic: public Menu
{
public:
	void Initialize(void);
	void Update(float elpsTime);
};

struct GameTutorial;
class MenuInfo: public Menu
{
public:
	void Initialize(void);
	void Finalize(void);
	void ProcessInput(bool& inputHandled, float elpsTime);
	void MsgProc(UINT recieverID, UINT msg, void* data);
	void Update(float elpsTime);
	void Draw(DWORD flag);
	void Show( bool gamepaused = true );
	void Hide(void);
	void OnClick(sx::gui::PControl sender);

	//	add a tutorial to display list and return the index of that
	int AddTutorial(GameTutorial* tutor);
	void ClearTutorial(void);
	void PrepareTutorial(const GameTutorial* tutor);
	void ShowTutorial(int index, float displayDelayTime);
	void ShowInfo(int index);

public:

	Array<GameTutorial*>	m_tutorial;
	int						m_index;
	
	sx::gui::Panel*			m_holder;		//	place holder for tutorial objects
	sx::gui::Label*			m_indicator;
	sx::gui::Button*		m_next;
	sx::gui::Button*		m_prev;
	float					m_infoButtonTime;
	float					m_tutorialDelayTime;
	bool					m_go_to_menu;


	struct Helper
	{
		sx::gui::PLabel		title;
		sx::gui::PLabel		desc;
		sx::gui::PPanel		image;
		sx::gui::PPanelEx	back;
		float				showTime;
	};
	Helper				m_helper;


};



#endif	//	GUARD_GameMenus_HEADER_FILE