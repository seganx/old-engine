/********************************************************************
	created:	2010/11/17
	filename: 	EditorSettings.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain all settings of the editor. use in
				all other units.
*********************************************************************/
#ifndef GUARD_EditorSettings_HEADER_FILE
#define GUARD_EditorSettings_HEADER_FILE

#include "sxEditor.h"

//!  this static class contain all settings. it saves and loads settings automatically
class EditorSettings
{
public:

	//! return initialization window size
	static int2 GetInitWindowSize(void);

	//! return sleep time in milliseconds
	static UINT GetSleepTime(void);

	//! return true if grid should be draw
	static bool GetGridVisible(void);

	//! return grid size
	static int GetGridSize(void);

	//! return color of grid
	static D3DColor GetGridColor(void);

};

#endif	//	GUARD_EditorSettings_HEADER_FILE