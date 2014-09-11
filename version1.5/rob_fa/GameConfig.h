/********************************************************************
	created:	2011/07/10
	filename: 	GameConfig.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain an static class to manage game 
				configuration
*********************************************************************/
#ifndef GUARD_GameConfig_HEADER_FILE
#define GUARD_GameConfig_HEADER_FILE

#include "ImportEngine.h"

class Config
{
public:
	typedef struct GameConfig
	{
		DWORD		device_CreationFlag;		//  direct3d device creation flag
		int2		display_Size;				//  size of display
		int			display_Debug;				//  draw in debug mode
		float		game_speed;					//  game speed. default = 1
		int			shaderLevel;
		int			shadowLevel;
		int			reflectionLevel;
		float		musicVolume;
		float		soundVolume;
		float		mouseSpeed;
		//wchar		language[128];


		GameConfig(void);
	}
	*PGameConfig;

public:

	//! save game configuration to 'game.config'
	static void LoadConfig(void);

	//! load game configuration from 'game.config'
	static void SaveConfig(void);

	//! get config data
	static GameConfig* GetData(void);
};


#endif	//	GUARD_GameConfig_HEADER_FILE