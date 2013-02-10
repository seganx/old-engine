/********************************************************************
	created:	2010/10/01
	filename: 	MainEditor.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain main editor static class that control
				the other parts and integrate them
*********************************************************************/
#ifndef SX_MainEditor_HEADER_FILE
#define SX_MainEditor_HEADER_FILE

#include "sxEditor.h"
#include "EditorTypes.h"
#include "Form_SetName.h"
#include "Form_Toolbar.h"
#include "Form_Explorer.h"
#include "Form_EditMesh.h"
#include "Form_EditNode.h"
#include "Form_Settings.h"
#include "Form_Hint.h"
#include "Form_Ask.h"
#include "Form_ImportSound.h"
#include "Form_ImportModel.h"

class Editor
{

public:
	//! initialize the editor will called from editor.cpp
	static void Initialize(void);

	//! finalize the editor will called from editor.cpp
	static void Finalize(void);

	//! start the editor application
	static void StartEditor(void);

	//! close the editor application
	static void Close(void);

	//! Show 'Choose Game Project' form to open or create new project directory
	static void NewProject(void);

	//! Open new project directory and create new files
	static void OpenProject(const WCHAR* ProjectDir);

	//! close current project directory
	static void CloseProject(void);

	//! Show 'Choose Level Of Game' form to open or create new level of game
	static void NewLevel(void);

	//! open a level to edit
	static void OpenLevel(const WCHAR* LevelFileName);

	//! close a level
	static void CloseLevel(void);

	//! will called when window size has been changed to recreate necessary resources
	static void Resize(int width, int height);

	//! update/render the editor and specified scene
	static void Loop(float elpsTime);

	//! render objects and information of view port
	static void RenderViewport(float elpsTime);

	//! render compass in the corner of screen
	static void RenderCompass(float elpsTime);

	//! set the current working space
	static void SetSpace(EditorWorkingSpace wSpace);

	//! get the current working space
	static EditorWorkingSpace GetSpace(void);

	//! set tip note in debug label for specified times in milliseconds
	static void SetLabelTips(const WCHAR* strTips, const float tipTime);

public:

	static PForm_SetName			frm_SetName;		//! use to assign the fist name of an object after creation
	static PForm_Explorer			frm_Explorer;		//! use explorer form to open/save files and folders
	static PForm_Settings			frm_Settings;		//! use to change settings of the editor and engine
	static PForm_Hint				frm_Hint;			//! use to show hint of objects in the scene
	static PForm_Ask				frm_Ask;			//! use to verify user confirmation on some operations
	static PForm_ImportSound		frm_importSound;	//! use to import sound file to the engine library
	static PForm_ImportModel		frm_importModel;	//! use to import model file to the engine library

	static sx::gui::PLabel			dbg_Lable;			//  use some debug
	static sx::gui::PButton			btn_Settings;		//  show settings form
};

#endif