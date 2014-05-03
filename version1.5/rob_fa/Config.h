/********************************************************************
	created:	2014/03/20
	filename: 	Config.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some config of the game
*********************************************************************/
#ifndef GUARD_Config_HEADER_FILE
#define GUARD_Config_HEADER_FILE

#define VER_USERDEMO		0
#define VER_PREVIEW			0
#define VER_PREVIEW_NAME	L"Immanitas Entertainment GmbH"
#define USE_RTL				0
#define USE_GAMEUP			0
#define USE_LOCK_NAME		L"Immanitas GmbH"
#define USE_HASH_LOCK		0


#if VER_USERDEMO
#define GAME_TITLE		L"Rush for Glory - demo version"
#else
#define GAME_TITLE		L"Rush for Glory"
#endif

#if VER_PREVIEW
#undef GAME_TITLE
#define GAME_TITLE		L"Rush for Glory - Evaluation version. Not for distribution - " VER_PREVIEW_NAME
#endif


#define FONT_STATUS							L"Font_17.fnt"
#define FONT_MAP_DIFFICULTY					L"Font_20_l.fnt"
#define FONT_PROFILE						L"Font_30_l.fnt"
#define FONT_PROFILE_INFO					L"Font_20_l.fnt"
#define FONT_ACHIEVEMENTS_NAMES				L"Font_25_l.fnt"
#define FONT_ACHIEVEMENTS_DESC				L"Font_20_l.fnt"
#define FONT_INFO_INDICATOR					L"Font_30_l.fnt"
#define FONT_INFO_TITLE						L"Font_30_l.fnt"
#define FONT_INFO_TITLE_HELPER				L"Font_16_l.fnt"
#define FONT_INFO_DESC						L"Font_25_l.fnt"
#define FONT_INFO_DESC_HELPER				L"Font_16_l.fnt"
#define FONT_UPGRADE_NAME					L"Font_20_l.fnt"
#define FONT_UPGRADE_STARS					L"Font_25_l.fnt"
#define FONT_UPGRADE_DESC					L"Font_25_l.fnt"
#define FONT_VICTORY_PEOPLE					L"Font_25_l.fnt"
#define FONT_TIPS							L"Font_22_l.fnt"
#define FONT_HINT_TITLE						L"Font_25_l.fnt"
#define FONT_HINT_DESC						L"Font_20_l.fnt"
#define FONT_TOWER_PANEL_INFO				L"Font_20_l.fnt"
#define FONT_TOWER_PANEL_HEALTH				L"Font_25_l.fnt"
#define FONT_WAVE_CALL_GOLD					L"Font_30_l.fnt"
#define FONT_PREVIEW						L"Font_25_l.fnt"


#endif //GUARD_Config_HEADER_FILE
