/********************************************************************
	created:	2011/07/18
	filename: 	GameStrings.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple class of strings used in game
*********************************************************************/
#ifndef GUARD_GameStrings_HEADER_FILE
#define GUARD_GameStrings_HEADER_FILE

#include "ImportEngine.h"

struct GameString;
class GameStrings
{
	SEGAN_STERILE_CLASS(GameStrings);

public:
	GameStrings();
	~GameStrings();

	//! clear array of all strings
	void Clear(void);

	//! this will load an strings file.
	void Load(const wchar* stringsFile);

	GameString* Get( const uint id );
private:
	Map<uint, GameString*>	m_texts;
};

#endif	//	GUARD_Scripter_HEADER_FILE