/********************************************************************
	created:	2011/02/07
	filename: 	EditorUI.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some editor ui definitions and 
				settings contain media files path, colors, etc...				
*********************************************************************/
#ifndef GUARD_EditorUI_HEADER_FILE
#define GUARD_EditorUI_HEADER_FILE

#include "sxEditor.h"

/*
use this static class to store the setting of editor UI
*/
class EditorUI
{
public:
	//  initialize editor UI to create needed resources
	static void Initialize(void);

	//  Finalize editor UI to destroy created resources
	static void Finalize(void);

	static const WCHAR*		GetTexture(const WCHAR* name);
	static const WCHAR*		GetBackgroundTexture(void);
	static const WCHAR*		GetHardwareTexture(void);
	static const WCHAR*		GetDefaultFont(int size);
	static const D3DColor	GetDefaultFontColor(void);
	static const WCHAR*		GetFormTexture(const WCHAR* name);
	static const D3DColor	GetFormColor(void);
	static const WCHAR*		GetFormCaptionFont(void);
	static const D3DColor	GetFormCaptionFontColor(void);
	static const WCHAR*		GetScrollTexture(int index);
	static const D3DColor	GetListBackColor(void);
	static const WCHAR*		GetListSelectedTexture(void);
	static const WCHAR*		GetDiskDriveTexture(DWORD type);
	static const WCHAR*		GetFileExtTexture(const WCHAR* type);
	static const WCHAR*		GetButtonTexture(const WCHAR* name, int index);
	static const WCHAR*		GetCheckBoxTexture(int index);
	static const WCHAR*		GetEditTexture(void);
	static const WCHAR*		GetMemberTexture(NodeMemberType type);

	//! create and return a simple label
	static sx::gui::PLabel CreateLabel(sx::gui::PControl parent, const float width, const int fontSize, const WCHAR* caption = NULL);

	//! create and return a labeled button 
	static sx::gui::PButton CreateButton(sx::gui::PControl parent, const float width, const WCHAR* caption);
	static sx::gui::PButton CreateButtonEx(sx::gui::PControl parent, const float width, const float height, const WCHAR* texture);

	//! create a simple track bar
	static sx::gui::PTrackBar CreateTrackbar(sx::gui::PControl parent, const float width, const float fMin, const float fMax, const float angle = 0);

	//! create new edit box
	static sx::gui::TextEdit* CreateEditBox(sx::gui::PControl parent, const float left, const float width, const WCHAR* text = NULL);

	//! create a simple check box
	static sx::gui::PCheckBox CreateCheckBox(sx::gui::PControl parent, const float left, const float width, const WCHAR* caption = NULL);

	//! create labeled edit box. NOTE: label will bi the child of this objects
	static sx::gui::TextEdit* CreateLabeldEditBox(sx::gui::PControl parent, float left, float textWidth, float labelWidth, const WCHAR* caption = NULL, const WCHAR* text = NULL);
};

#endif	//	GUARD_EditorUI_HEADER_FILE