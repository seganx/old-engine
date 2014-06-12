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


class GameStrings
{
	SEGAN_STERILE_CLASS(GameStrings);

public:
	GameStrings();
	~GameStrings();

	//! this will load an strings file.
	void Load(const wchar* stringsfile);

	//! return the strings by index
	const wchar* Get( const int index );

private:
	wchar				m_tmp[32];
	sx::cmn::StringList m_strlist;
};

#endif	//	GUARD_Scripter_HEADER_FILE