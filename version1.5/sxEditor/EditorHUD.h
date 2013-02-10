/********************************************************************
	created:	2011/04/18
	filename: 	EditorHUD.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain HUD editor to edit GUI and HUD
*********************************************************************/
#ifndef GUARD_EditorHUD_HEADER_FILE
#define GUARD_EditorHUD_HEADER_FILE

class EditorHUD
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

#endif	//	GUARD_EditorHUD_HEADER_FILE