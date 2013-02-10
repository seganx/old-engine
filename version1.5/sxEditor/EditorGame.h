/********************************************************************
	created:	2011/04/18
	filename: 	EditorGame.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain game editor to display/preview/play the game
*********************************************************************/
#ifndef GUARD_EditorGame_HEADER_FILE
#define GUARD_EditorGame_HEADER_FILE

class EditorGame
{
public:
	//! initialize the editor will called from editor.cpp
	static void Initialize(void);

	//! finalize the editor will called from editor.cpp
	static void Finalize(void);

	//! will called when window size has been changed to recreate necessary resources
	static void Resize(int width, int height);

	//! update the editor 
	static void Update(float elpsTime, bool& inputResult);

	//! render the editor
	static void Render(float elpsTime);
};

#endif	//	GUARD_EditorGame_HEADER_FILE