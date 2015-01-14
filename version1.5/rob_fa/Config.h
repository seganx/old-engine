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
#define USE_8_LEVELS		0
#define USE_HASH_LOCK		0
#define USE_STEAM_LINK		0
#define USE_DEV_CHEAT		0		//	development cheats
#define USE_CHEAT_CODE		0		//	player cheats

#define USE_STEAM_SDK		0

#define USE_ALAWAR_LOGO		0		//	display Alawar splash screen
#define USE_POLAND_LOGO		0		//	display logo for Polish version
#define USE_CZESH_LOGO		0		//	display logo for Czech version

#define USE_SITE_STATS		0
#define SITE_STATS_TAG		0		//	0:Steam - 1:USA/UK - 2:Germany - 3:Poland - 4:Czech

#define USE_UPDATE_1		1		//	switch to first update of the game
#define USE_ENCRPT_STR		0		//	force to use encrypted text


#if VER_USERDEMO
#define GAME_TITLE		L"Rush for Glory - demo version"
#else
#define GAME_TITLE		L"Rush for Glory"
#endif

#if VER_PREVIEW
#undef GAME_TITLE
#define GAME_TITLE		L"Rush for Glory - Evaluation version. Not for distribution - " VER_PREVIEW_NAME
#endif


#if USE_STEAM_SDK
//#define STEAM_APP_ID	k_uAppIdInvalid
#define STEAM_APP_ID	303470
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
#define FONT_HUD_TITLE						L"Font_25_l.fnt"
#define FONT_45_OUTLINE_SHADOWED			L"font_45_l_sh.fnt"
#define FONT_35_OUTLINE_SHADOWED			L"font_35_l_sh.fnt"
#define FONT_30_OUTLINE						L"Font_30_l.fnt"
#define FONT_25_OUTLINE						L"Font_25_l.fnt"
#define FONT_20_OUTLINE						L"Font_20_l.fnt"


#define GUIDE_CALLWAVE						0
#define GUIDE_UPGRADE						1
#define GUIDE_GOLDFORPEOPLE					2
#define GUIDE_DIFFICULTY					3
#define GUIDE_GAMESPEED						4
#define GUIDE_USESTARS						5

#if USE_STEAM_SDK
#define GUIDE_EFFICIENCY					6
#endif

#endif //GUARD_Config_HEADER_FILE
