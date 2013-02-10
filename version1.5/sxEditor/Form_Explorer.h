/********************************************************************
created:	2011/02/24
filename: 	form_Explorer.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain explorer form to explore the computer and
			open/save files and folders in the system
*********************************************************************/
#ifndef SX_form_Explorer_HEADER_FILE
#define SX_form_Explorer_HEADER_FILE

#include "BaseForm.h"
#include "sxListBox.h"

/*!
explorer form to explore the computer and open/save files and folders in the system
*/
class Form_Explorer: public BaseForm
{

	class FolderList
	{
	public:
		FolderList( sx::gui::Control* parent )
		{
			pListBox = sx_new( sx::gui::ListBox );
			pListBox->SetParent( parent );
			pListBox->GetBack()->RemProperty(SX_GUI_PROPERTY_VISIBLE);
		}
		~FolderList()
		{
			pListBox->SetParent( null );
			sx_delete( pListBox );
		}

		FileInfoArray		aFiles;		//  array of files in the list
		String				sPath;		//  path of the folder
		sx::gui::ListBox*	pListBox;	//  list of files

	};
	typedef Array<FolderList*> listboxArray;

public:
	Form_Explorer(void);
	virtual ~Form_Explorer(void);

	//! on resize event when size of editor has been changed
	void OnResize(int EditorWidth, int EditorHeight);

	//! process input should be call before update
	void ProcessInput(bool& inputHandled);

	//! update the form explorer
	void Update(float elpsTime);

	//! return the path of the explorer
	const WCHAR* GetPath(void);

	//!  set title of the form
	void SetTitle(const WCHAR* title);

	//! show the form to open files and folders. set callback function for click on ok
	void Open(sx::gui::PForm pForm, GUICallbackEvent OnOkClick);

	//! show the form to save a file. set callback function for click on ok
	void Save(sx::gui::PForm pForm, GUICallbackEvent OnOkClick);

	//! set user data information
	void SetUserData(void* userData);

	//! return user data information
	void* GetUserData(void);

	//! set extension file formats to show
	void SetExtension(const WCHAR* extName);

	//! set a new path to the explorer
	bool SetPath(const WCHAR* newPath);

private:
	void UpdateDrivesList(void);
	void UpdateFilesList(int index, const WCHAR* folderPath);
	void OnChangeItemIndex(sx::gui::PControl Sender);
	void OnButtonClick(sx::gui::PControl Sender);

	sx::gui::PLabel			m_pTitle;			// Title of the form
	sx::gui::PPanel			m_pHardware;		// system icon
	sx::gui::PPanel			m_pClipper;			// use this panel to clip lists
	sx::gui::PTrackBar		m_pScroll;			// the lateral scroll let me to traverse between the other lists
	sx::gui::PTextEdit		m_pLabelPath;		// the text edit to display the path
	sx::gui::PButton		m_btnOK;			// ok button of the form

	sx::gui::PButton		m_btnProject;		//  goto project folder
	sx::gui::PButton		m_btnDesktop;		//  goto desktop folder
	sx::gui::PButton		m_btnMyDocs;		//	goto My Document folder
	sx::gui::PButton		m_btnLast;			//  goto LastFolder

	DriveInfoArray			m_driveArray;		// array of drives in the computer
	sx::gui::ListBox		m_driveList;		// list of drives in the computer
	listboxArray			m_ListArray;		// array of list boxes to show folders

	Stack<String*>			m_lastFolders;		// hold last opened folder
	String					m_Extension;		// hold the extensions of the files to show
	String					m_Path;				// hold the path of explorer
	float2					m_Size;				// size of the list box;

	void*					m_UserData;			// user data information
	GUICallbackFuncPtr		m_OnOkClick;		// pointer to click function

	//////////////////////////////////////////////////////////////////////////
	//  some additional values
	int		lastListIndex;		// index of the last list that is visible
	float	listScrolling;		// value of scrolling
	float	updateListTime;		// time of list of files update
	int		updateListIndex;	// index of update list
};
typedef Form_Explorer *PForm_Explorer;

#endif // SX_form_Explorer_HEADER_FILE
