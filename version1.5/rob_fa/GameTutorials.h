/********************************************************************
	created:	2015/05/14
	filename: 	GameTutorials.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple class of tutorial manager
*********************************************************************/
#ifndef GUARD_GameTutorial_HEADER_FILE
#define GUARD_GameTutorial_HEADER_FILE

#include "ImportEngine.h"

struct GameTutorial
{
	wchar				name[32];
	uint				texts[16];
	sx::gui::Panel*		image;
	int					level;		//	show in specified level
};

class GameTutorials
{
	SEGAN_STERILE_CLASS(GameTutorials);

public:
	GameTutorials();
	~GameTutorials();

	//! clear array of all strings
	void Clear();

	//! this will load tutorial file.
	void Load( const wchar* stringsFile );

	//! return tutorial by name. return empty one of not found
	GameTutorial* GetByName( const wchar* name );

public:
	Array<GameTutorial>	m_tutor;
};

#endif	//	GUARD_GameTutorial_HEADER_FILE