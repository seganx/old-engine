/********************************************************************
	created:	2011/05/01
	filename: 	Form_ObjectToolbar.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain class toolbar form which contain some
				buttons used in object editor
*********************************************************************/
#ifndef GUARD_Form_ObjectToolbar_HEADER_FILE
#define GUARD_Form_ObjectToolbar_HEADER_FILE

#include "BaseForm.h"
#include "PopupMenu.h"

#define FORM_OBJECT_TOOLBAR_SIZE_H	50.0f		//  size of the toolbar form

/*!
tool bar form will use in upper part of windows application to manage editor
*/
class Form_ObjectToolbar: public BaseForm
{

public:
	Form_ObjectToolbar(void);
	virtual ~Form_ObjectToolbar(void);

	//! set size of the form
	void SetSize(float width, float height);

	//! process input should be call before update
	void ProcessInput(bool& result, int playerID = 0);

	//! update the form explorer
	void Update(float elpsTime);

	//! will call by editor to set the size of the from depend of size of the editor
	void OnResize(int EditorWidth, int EditorHeight);

public:

	void OnButtonClick(sx::gui::PControl Sender);			//  do some conventional operations
	void OnNewObjectListClick(sx::gui::PControl Sender);	//  select type of new object

	void OnModelImportClick(sx::gui::PControl Sender);
	void OnModelExportClick(sx::gui::PControl Sender);

	void ImportModel(sx::gui::PControl Sender);
	void ExportModel(sx::gui::PControl Sender);

public:

	String				m_path_Save;		//  last save file. use for ctrl + s

	sx::gui::PButton	m_btnNew;			//  clear current scene and new one
	sx::gui::PButton	m_btnImport;		//  import button
	sx::gui::PButton	m_btnExport;		//  export button
	sx::gui::PButton	m_btnAdd;			//  add button
	sx::gui::PButton	m_btnRemove;		//  remove button
	sx::gui::PButton	m_btnEditMember;	//  show member editor
	sx::gui::PButton	m_btnEditNode;		//  show node editor
	sx::gui::PButton	m_btnSelect;		//  select button
	sx::gui::PButton	m_btnMove;			//  move button
	sx::gui::PButton	m_btnRotate;		//  rotate button
	sx::gui::PButton	m_btnView;			//  view button
	sx::gui::PButton	m_btnBack;			//  back to man scene

	PopupMenu			m_popNewObject;		//  popup menu for new objects

};
typedef Form_ObjectToolbar *PForm_ObjectToolbar;

#endif	//	GUARD_Form_ObjectToolbar_HEADER_FILE