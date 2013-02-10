#include "Form_Explorer.h"
#include "sxItemUI.h"
#include "EditorUI.h"
#include "EditorSettings.h"

#define PANEL_TOP_Y				20.0f
#define PANEL_BOTTOM_Y			50.0f
#define FILELIST_SIZE_W			200.0f
#define FILELIST_LEFT_OFFSET	100.0f
#define FILELIST_MAXCOUNT		20			//  20 list of files in maximum
#define FORM_EXPLORER_SIZE_H	263.0f		//  size of the explorer form

#define ADD_TO_LAST_PATH()\
{\
	str1024 tmp1 = m_Path;\
	tmp1.MakeLower();\
	str1024 tmp2;\
	if ( !m_lastFolders.IsEmpty() )\
		tmp2 = m_lastFolders.Top()->Text();\
	tmp2.MakeLower();\
	if ( tmp1 != tmp2 )\
		m_lastFolders.Push( sx_new( String(m_Path) ) );\
}\

Form_Explorer::Form_Explorer( void ): BaseForm(), lastListIndex(-1), listScrolling(0.0f), updateListTime(0.0f), updateListIndex(-1), m_UserData(0)
{
	m_pTitle	= (sx::gui::PLabel)sx::gui::Create(GUI_LABEL);
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"File Explorer");

	m_pHardware	= (sx::gui::PPanel)sx::gui::Create(GUI_PANEL);
	m_pHardware->SetSize( float2(24.0f, 24.0f) );
	m_pHardware->SetParent(m_pBack);
	m_pHardware->GetElement(0)->SetTextureSrc( EditorUI::GetHardwareTexture() );

	m_pClipper	= (sx::gui::PPanel)sx::gui::Create(GUI_PANEL);
	m_pClipper->SetParent(m_pBack);
	m_pClipper->AddProperty(SX_GUI_PROPERTY_CLIPCHILDS);
	m_pClipper->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_pScroll	= (sx::gui::PTrackBar)sx::gui::Create(GUI_TRACKBAR);
	m_pScroll->SetParent(m_pBack);
	m_pScroll->GetElement(0)->SetTextureSrc( EditorUI::GetScrollTexture(0) );
	m_pScroll->GetElement(1)->SetTextureSrc( EditorUI::GetScrollTexture(1) );
	m_pScroll->SetMax(0.0f);

	m_pLabelPath = EditorUI::CreateEditBox(m_pClipper, 10.0f, FORM_EXPLORER_SIZE_H);
	//m_pLabelPath->AddProperty(SX_GUI_PROPERTY_READONLY);
	m_pLabelPath->SetAlign(GTA_LEFT);
	SEGAN_GUI_SET_ONTEXT(m_pLabelPath, Form_Explorer::OnButtonClick);

	m_btnOK = (sx::gui::PButton)sx::gui::Create(GUI_BUTTON);
	m_btnOK->SetSize( float2(64.0f, 32.0f) );
	m_btnOK->SetParent(m_pBack);
	m_btnOK->GetElement(0)->SetTextureSrc( EditorUI::GetButtonTexture(L"OK", 0) );
	m_btnOK->GetElement(1)->SetTextureSrc( EditorUI::GetButtonTexture(L"OK", 1) );
	m_btnOK->GetElement(2)->SetTextureSrc( EditorUI::GetButtonTexture(L"OK", 2) );
	m_btnOK->AddProperty(SX_GUI_PROPERTY_IGNOREBLEND);

	m_btnProject = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"Folder_project") );
	m_btnDesktop = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"Folder_desktop") );
	m_btnMyDocs	 = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"Folder_docs") );
	m_btnLast	 = EditorUI::CreateButtonEx( m_pBack, 32, 32, EditorUI::GetTexture(L"Folder_last") );

	m_btnProject->SetHint(L"PROJECT\n Goto the project directory. ");
	m_btnDesktop->SetHint(L"DESKTOP\n Show user's desktop directory. ");
	m_btnMyDocs->SetHint(L"DOCUMENTS\n Finds user's documents. ");
	m_btnLast->SetHint(L"BACK\n Goto the previous directory. ");

	SEGAN_GUI_SET_ONCLICK(m_btnOK, Form_Explorer::OnButtonClick);
	SEGAN_GUI_SET_ONCLICK(m_btnProject, Form_Explorer::OnButtonClick);
	SEGAN_GUI_SET_ONCLICK(m_btnDesktop, Form_Explorer::OnButtonClick);
	SEGAN_GUI_SET_ONCLICK(m_btnMyDocs, Form_Explorer::OnButtonClick);
	SEGAN_GUI_SET_ONCLICK(m_btnLast, Form_Explorer::OnButtonClick);

	m_driveList.SetParent(m_pClipper);
	m_driveList.SetOnSelect(this, (GUICallbackEvent)&Form_Explorer::OnChangeItemIndex);

	for (int i=0; i<FILELIST_MAXCOUNT; i++)
	{
		FolderList* tmpList = sx_new( FolderList(m_pClipper) );
		m_ListArray.PushBack( tmpList );
	}

	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);

	m_MinSize.x = 400.0f;	
	m_MinSize.y = 200.0f;
	int2 initSize = EditorSettings::GetInitWindowSize();
	SetSize( (float)initSize.x - FILELIST_LEFT_OFFSET, FORM_EXPLORER_SIZE_H );

	//  set new states
	m_pBack->State_GetByIndex(0).Align		= float2(0.0f, -0.5f);
	m_pBack->State_GetByIndex(0).Center		= float3(0.0f, 0.0f, 0.0f);
	m_pBack->State_GetByIndex(0).Position	= float3(0.0f, 20.0f, 0.0f);
	m_pBack->State_GetByIndex(0).Scale		= float3(1.0f, 1.0f, 1.0f);

	m_pBack->State_GetByIndex(1).Align		= float2(0.0f, -0.5f);
	m_pBack->State_GetByIndex(1).Center		= float3(0.0f, 0.5f, 0.0f);
	m_pBack->State_GetByIndex(1).Position	= float3(0.0f, 20.0f, 0.0f);
	m_pBack->State_GetByIndex(1).Scale		= float3(1.0f, 1.0f, 1.0f);
}

Form_Explorer::~Form_Explorer( void )
{
	for (;;)
	{
		String* pstr = null;
		if ( m_lastFolders.Pop( pstr ) )
		{
			sx_delete( pstr );
		}
		else break;
	}
	
	for (int i=0; i<m_ListArray.Count(); i++)
		sx_delete(m_ListArray[i]);
	m_ListArray.Clear();
	//  for other things the parent will do everything :)
}

void Form_Explorer::OnResize( int EditorWidth, int EditorHeight )
{
	float offsetW = (float)(EditorWidth  % 2);
	float offsetH = (float)(EditorHeight % 2);
	float W	= (float)EditorWidth;
	float H = (float)EditorHeight;

	float width = W - FILELIST_LEFT_OFFSET + offsetW;
	float height = FORM_EXPLORER_SIZE_H + offsetH*0.5f;

	m_Size.x = width; 
	m_Size.y = height;
	
	m_pTitle->SetSize( float2(width - 70.0f, 24.0f) );
	m_pTitle->Position().y = height*0.5f - PANEL_TOP_Y*0.5f + 5.0f;

	m_pHardware->Position() = float3(- width*0.5f + 20.0f, height*0.5f - PANEL_TOP_Y*0.5f + 5.0f, 0.0f); 

	m_pClipper->SetSize( float2(width - FILELIST_LEFT_OFFSET/2, height) );
	m_pClipper->Position().x = FILELIST_LEFT_OFFSET/4;

	m_pScroll->SetSize( float2(width - FILELIST_LEFT_OFFSET/2, 16.0f) );
	m_pScroll->Position().y = - height*0.5f + PANEL_BOTTOM_Y - 8.0f;
	m_pScroll->Position().x = FILELIST_LEFT_OFFSET/4;

	m_pLabelPath->SetSize( float2(m_pClipper->GetSize().x-FILELIST_LEFT_OFFSET, 20.0f) );
	m_pLabelPath->Position() = float3(- 50.0f, - height*0.5f + 20.0f, 0.0f);

	m_btnOK->Position() = float3(width*0.5f - 50.0f, - height*0.5f + 16.0f, 0.0f);
	m_btnProject->Position() = float3(-width*0.5f + FILELIST_LEFT_OFFSET/4-5.0f, height*0.5f - 40.0f, 0.0f);
	m_btnDesktop->Position() = float3(-width*0.5f + FILELIST_LEFT_OFFSET/4-5.0f, height*0.5f - 90.0f, 0.0f);
	m_btnMyDocs->Position()  = float3(-width*0.5f + FILELIST_LEFT_OFFSET/4-5.0f, height*0.5f - 140.0f, 0.0f);
	m_btnLast->Position()	 = float3(-width*0.5f + FILELIST_LEFT_OFFSET/4-5.0f, height*0.5f - 190.0f, 0.0f);

	const float leftOffset = FILELIST_LEFT_OFFSET;

	m_driveList.SetSize(FILELIST_SIZE_W, height - PANEL_BOTTOM_Y - PANEL_TOP_Y, 48.0);
	m_driveList.Position() = float3(- m_pClipper->GetSize().x*0.5f + leftOffset, (PANEL_BOTTOM_Y - PANEL_TOP_Y) * 0.5f, 0.0f);

	for (int i=0; i<FILELIST_MAXCOUNT; i++)
	{
		if (m_ListArray[i]->pListBox->GetBack()->HasProperty(SX_GUI_PROPERTY_VISIBLE))
			m_ListArray[i]->pListBox->SetSize(FILELIST_SIZE_W, m_Size.y - PANEL_BOTTOM_Y - PANEL_TOP_Y, 16.0);
		
		m_ListArray[i]->pListBox->Position().y = (PANEL_BOTTOM_Y - PANEL_TOP_Y) * 0.5f;
		m_ListArray[i]->pListBox->Position().x = - m_pClipper->GetSize().x*0.5f + (i+1)*FILELIST_SIZE_W + leftOffset;
	}

	BaseForm::SetSize(width, height);

	//  set new states
	m_pBack->State_GetByIndex(0).Position.x	= 0.0f;
	m_pBack->State_GetByIndex(1).Position.x	= 0.0f;
	Update(0);

}

void Form_Explorer::ProcessInput( bool& inputHandled )
{
	if ( !IsVisible() || inputHandled ) return;

	BaseForm::ProcessInput(inputHandled);
	if ( m_pLabelPath->GetFocused() )
	{
		if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RCONTROL) )
		{
			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_C) || SEGAN_KEYDOWN(0, SX_INPUT_KEY_X) || SEGAN_KEYDOWN(0, SX_INPUT_KEY_INSERT) )
				m_pLabelPath->Copy();
			else if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_V) )
				m_pLabelPath->Paste();
		}
		else if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LSHIFT) || SEGAN_KEYHOLD(0, SX_INPUT_KEY_RSHIFT) )
		{
			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_INSERT) )
				m_pLabelPath->Paste();
		}


		inputHandled = true;
		return;
	}

	const char* keys = sx::io::Input::GetKeys(0);

	if (keys[SX_INPUT_KEY_ESCAPE] == SX_INPUT_STATE_UP)
	{
		sx::gui::ArrayPControl clist;
		m_pBack->GetChildren(clist);
		for (int i=0; i<clist.Count(); i++)
		{
			if ( clist[i]->GetFocused() )
			{
				Close();
				inputHandled = true;
				return;
			}
		}
	}
	else if (keys[SX_INPUT_KEY_RIGHT] == SX_INPUT_STATE_UP)
	{
		if (m_driveList.GetFocused())
		{
			 m_ListArray[0]->pListBox->SetFocused();
			 if (m_ListArray[0]->pListBox->GetFocused() && m_ListArray[0]->pListBox->GetItemIndex()<0	&& m_ListArray[0]->pListBox->Count())
				 m_ListArray[0]->pListBox->SetItemIndex(0);
			 m_pScroll->SetValue(1);
			 inputHandled = true;
		}
		else
		{
			for (int i=0; i<m_ListArray.Count()-1; i++)
			{
				if (m_ListArray[i]->pListBox->GetFocused())
				{
					m_ListArray[i+1]->pListBox->SetFocused();
					if (m_ListArray[i+1]->pListBox->GetFocused()	&& m_ListArray[i+1]->pListBox->GetItemIndex()<0 && m_ListArray[i+1]->pListBox->Count())
						m_ListArray[i+1]->pListBox->SetItemIndex(0);
					m_pScroll->SetValue((float)i+2);
					inputHandled = true;
					break;
				}
			}
		}
	}
	else if (keys[SX_INPUT_KEY_LEFT] == SX_INPUT_STATE_UP)
	{
		if (m_ListArray[0]->pListBox->GetFocused())
		{
			m_driveList.SetFocused();
			m_pScroll->SetValue(0.0f);
			inputHandled = true;
		}
		else if (!m_driveList.GetFocused())
		{
			for (int i=1; i<m_ListArray.Count(); i++)
			{
				if (m_ListArray[i]->pListBox->GetFocused())
				{
					m_ListArray[i-1]->pListBox->SetFocused();
					m_pScroll->SetValue((float)i);
					inputHandled = true;
					break;
				}
			}
		}
	}
	else if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_RETURN) )
	{
		OnButtonClick( m_btnOK );
	}
}

void Form_Explorer::Update( float elpsTime )
{
	//  update list of drives and files and folders
	updateListTime += elpsTime;
	if ( IsVisible() && updateListTime > ( 500.0f + lastListIndex * 50.0f ) )
	{
		updateListIndex++;
		if (updateListIndex>=lastListIndex) updateListIndex = -1;

		if (updateListIndex < 0)
			UpdateDrivesList();
		else
		{
			if (m_ListArray[updateListIndex]->pListBox->GetBack()->HasProperty(SX_GUI_PROPERTY_VISIBLE))
				UpdateFilesList(updateListIndex, m_ListArray[updateListIndex]->sPath);
		}
		updateListTime = 0;
	}

	int n = int(m_Size.x / FILELIST_SIZE_W);
	m_pScroll->SetMax((float)(lastListIndex - n + 2));

	//  update horizontal scroll
	if (m_pScroll->GetMax()<1.0f)
		m_pScroll->RemProperty(SX_GUI_PROPERTY_VISIBLE);
	else
		m_pScroll->AddProperty(SX_GUI_PROPERTY_VISIBLE);

	//  update position of lists by scroll
	float xOffset = - m_pScroll->GetBlendingValue() * FILELIST_SIZE_W;
	listScrolling += (xOffset - listScrolling) * 0.15f * (elpsTime * 0.06f);
	m_driveList.GetBack()->PositionOffset().x = listScrolling;
	for (int i=0; i<FILELIST_MAXCOUNT; i++)
		m_ListArray[i]->pListBox->GetBack()->PositionOffset().x = listScrolling;

	//  update other the view lists
	m_driveList.Update(elpsTime);
	for (int i=0; i<FILELIST_MAXCOUNT; i++)
		m_ListArray[i]->pListBox->Update(elpsTime);

	BaseForm::Update(elpsTime);
}

const WCHAR* Form_Explorer::GetPath( void )
{
	return m_pLabelPath->GetText();
}

void Form_Explorer::Open( sx::gui::PForm pForm, GUICallbackEvent OnOkClick )
{
	//m_pLabelPath->AddProperty(SX_GUI_PROPERTY_READONLY);
	BaseForm::Show();
	m_OnOkClick.m_pForm = pForm;
	m_OnOkClick.m_pFunc = OnOkClick;
}

void Form_Explorer::Save( sx::gui::PForm pForm, GUICallbackEvent OnOkClick )
{
	//m_pLabelPath->RemProperty(SX_GUI_PROPERTY_READONLY);
	BaseForm::Show();
	m_OnOkClick.m_pForm = pForm;
	m_OnOkClick.m_pFunc = OnOkClick;
}

void Form_Explorer::SetUserData( void* userData )
{
	m_UserData = userData;
}

void* Form_Explorer::GetUserData( void )
{
	return m_UserData;
}

void Form_Explorer::UpdateDrivesList( void )
{
	//  get list of drives
	DriveInfoArray n_driveArray;
	sx::sys::GetDrives(&n_driveArray);

	//  get current states
	int n = n_driveArray.Count();
	int m = m_driveArray.Count();
	int lastIndex = m_driveList.GetItemIndex();
	WCHAR lastDrive = (lastIndex>=0) ? m_driveArray[lastIndex].name : 0;

	//  find changes to apply
	bool changed = (m != n);
	for (int i=0; i<sx_min_i(n, m) && !changed; i++)
	{
		if (m_driveArray[i].size != n_driveArray[i].size)	changed = true;
		if (!changed && m_driveArray[i].type != n_driveArray[i].type)	changed = true;
		if (!changed && m_driveArray[i].name != n_driveArray[i].name)	changed = true;
	}

	//  hold current list to compare later
	m_driveArray.Clear();
	for (int i=0; i<n_driveArray.Count(); i++)
		m_driveArray.PushBack( n_driveArray[i] );

	if (changed)
	{
		//  apply changes
		m_driveList.SetOnSelect(NULL, NULL);

		for (int i=0; i<(m-n); i++)
			m_driveList.Remove(0);
		for (int i=0; i<(n-m); i++)
			m_driveList.Add(NULL, NULL);
		for (int i=0; i<n_driveArray.Count() && i<m_driveList.Count(); i++)
		{
			m_driveList[i].SetTexture( EditorUI::GetDiskDriveTexture(n_driveArray[i].type) );
			m_driveList[i].Text.Format( 
				L"\n  [ %c: ] %s\nFree %.2f / %.2f GB",
				n_driveArray[i].name,
				n_driveArray[i].label,
				(float)n_driveArray[i].free/1024,
				(float)n_driveArray[i].size/1024 );
		}
		m_driveList.CommitChanges();
		m_driveList.SetOnSelect(this, (GUICallbackEvent)&Form_Explorer::OnChangeItemIndex);

		//  verify that the change happens on selected item
		for (int i=0; i<m_driveArray.Count(); i++)
		{
			if (m_driveArray[i].name == lastDrive)
			{
				m_driveList.SetOnSelect(NULL, NULL);
				m_driveList.SetItemIndex(i);
				m_driveList.SetOnSelect(this, (GUICallbackEvent)&Form_Explorer::OnChangeItemIndex);
				return;
			}
		}
		int i = m_driveList.GetItemIndex();
		if ( i<0 || (i<m_driveArray.Count() && lastDrive != m_driveArray[i].name) )
		{
			//  apply item index change
			OnChangeItemIndex(m_driveList.GetBack());
		}
	}
}

void Form_Explorer::UpdateFilesList( int index, const WCHAR* folderPath )
{
	if (!folderPath || index<0 || index >= FILELIST_MAXCOUNT) return;
	
	// get current contents
	FileInfoArray& lastFiles	= m_ListArray[index]->aFiles;
	sx::gui::ListBox& fileList	= *(m_ListArray[index]->pListBox);
	m_ListArray[index]->sPath	= folderPath;
	
	//  get new list of files and folders
	m_Extension.MakeLower();
	FileInfoArray newFiles;
	sx::sys::GetFilesInFolder(folderPath, L"*.*", &newFiles);
	for (int i=0; i<newFiles.Count(); i++)
	{
		if (!(newFiles[i].flag & FILE_ATTRIBUTE_DIRECTORY)	&&
			m_Extension.Text()								&&
			m_Extension.Text()[0] != '*'					&&
			newFiles[i].type != m_Extension)
		{
			newFiles.RemoveByIndex(i);
			i--;
		}
	}
	sx::sys::SortFilesBy(SFT_BYNAME, &newFiles);

	//  get current states
	int n = newFiles.Count();
	int m = lastFiles.Count();
	int lastIndex = fileList.GetItemIndex();
	String lastSelectedName(lastIndex>=0 ? fileList.GetText(lastIndex) : NULL);

	//  find changes to apply
	bool changed = (m != n);
	for (int i=0; i<sx_min_i(n, m) && !changed; i++)
	{
		if (newFiles[i].modified.wMilliseconds != lastFiles[i].modified.wMilliseconds)	changed = true;
		if (!changed && newFiles[i].flag != lastFiles[i].flag)							changed = true;
		if (!changed && newFiles[i].size != lastFiles[i].size)							changed = true;
		if (!changed && wcscmp( newFiles[i].name, lastFiles[i].name ) )					changed = true;
		if (!changed && wcscmp( newFiles[i].type, lastFiles[i].type ) )					changed = true;
	}

	//  hold current list to compare later
	lastFiles.Clear();
	for (int i=0; i<newFiles.Count(); i++)
		lastFiles.PushBack( newFiles[i] );

	if (changed)
	{
		//  apply changes
		fileList.SetOnSelect(NULL, NULL);

		for (int i=0; i<(m-n); i++)
			fileList.Remove(0);
		for (int i=0; i<(n-m); i++)
			fileList.Add(NULL, NULL);
		for (int i=0; i<fileList.Count() && i<newFiles.Count(); i++)
		{
			fileList[i].SetTexture( EditorUI::GetFileExtTexture(newFiles[i].type) );
			fileList[i].Text = newFiles[i].name;
		}
		fileList.CommitChanges();
		fileList.SetOnSelect(this, (GUICallbackEvent)&Form_Explorer::OnChangeItemIndex);
	
		//  verify that the change happens on selected item
		for (int i=0; i<fileList.Count(); i++)
		{
			if (fileList[i].Text == lastSelectedName)
			{
				fileList.SetOnSelect(NULL, NULL);
				fileList.SetItemIndex(i);
				fileList.SetOnSelect(this, (GUICallbackEvent)&Form_Explorer::OnChangeItemIndex);
				return;
			}
		}
		int i = fileList.GetItemIndex();
		if (lastSelectedName != fileList.GetText(i))
		{
			//  find previous list
			if (index>0)
				OnChangeItemIndex(m_ListArray[index-1]->pListBox->GetBack());
			else
				OnChangeItemIndex(m_driveList.GetBack());
		}
	}
}

void Form_Explorer::OnChangeItemIndex( sx::gui::PControl Sender )
{
	//  get and validate data
	if (!Sender || !Sender->GetUserData()) return;
	sx::gui::PListBox curList = (sx::gui::PListBox)Sender->GetUserData();
	int itemIndex = curList->GetItemIndex();
	
	//  prepare necessary data
	String		 path;

	//  get correct list and path
	if (curList == &m_driveList)
	{
		lastListIndex = -1;
		if (itemIndex>=0 && m_driveList[itemIndex].Text.Length()>5)
			path << m_driveList[itemIndex].Text[5] << L":\\";
	}
	else
	{
		//  find the list
		for (int i=0; i<FILELIST_MAXCOUNT; i++)
		{
			if (curList == m_ListArray[i]->pListBox)
			{
				lastListIndex = i;
				break;
			}
		}

		if (itemIndex>=0 && lastListIndex>=0 && lastListIndex<FILELIST_MAXCOUNT)
		{
			path = m_ListArray[lastListIndex]->sPath;
			path.MakePathStyle();
			path << m_ListArray[lastListIndex]->pListBox->GetText(itemIndex);
		}
	}
	m_Path = *path;
	if ( path != m_pLabelPath->GetText() )
	{
		//  avoid stack overflow
		m_pLabelPath->SetOnTextChange(NULL, NULL);
		m_pLabelPath->SetText( path );
		SEGAN_GUI_SET_ONTEXT(m_pLabelPath, Form_Explorer::OnButtonClick);
	}

	FileInfo finfo;
	if (itemIndex>=0 && sx::sys::GetFileInfo(path, finfo) && (finfo.flag & FILE_ATTRIBUTE_DIRECTORY))
	{
		lastListIndex++;
		if (lastListIndex<FILELIST_MAXCOUNT)
		{
			m_ListArray[lastListIndex]->pListBox->SetOnSelect(NULL, NULL);
			m_ListArray[lastListIndex]->pListBox->GetBack()->AddProperty(SX_GUI_PROPERTY_VISIBLE);
			m_ListArray[lastListIndex]->pListBox->SetSize(FILELIST_SIZE_W, m_Size.y - PANEL_BOTTOM_Y - PANEL_TOP_Y, 16.0);
			m_ListArray[lastListIndex]->pListBox->SetItemIndex(-1);
			UpdateFilesList(lastListIndex, path);
			m_ListArray[lastListIndex]->pListBox->SetOnSelect(this, (GUICallbackEvent)&Form_Explorer::OnChangeItemIndex);
		}
	}

	for (int i=lastListIndex+1; i<FILELIST_MAXCOUNT; i++)
	{
		m_ListArray[i]->pListBox->GetBack()->RemProperty(SX_GUI_PROPERTY_VISIBLE);
	}

	int n = lastListIndex + int(m_Size.x / FILELIST_SIZE_W) + 2;
	m_pScroll->SetMax((float)n);
	m_pScroll->SetValue((float)n);
}

void Form_Explorer::SetTitle( const WCHAR* title )
{
	m_pTitle->SetText(title);
}

void Form_Explorer::SetExtension( const WCHAR* extName )
{
	m_Extension = extName;
}

bool Form_Explorer::SetPath( const WCHAR* newPath )
{
	if (!newPath) return false;

	if ( m_lastFolders.IsEmpty() || *(m_lastFolders.Top()) != newPath )
		ADD_TO_LAST_PATH();

	m_driveList.SetItemIndex(-1);

	int index = -1;
	WCHAR* c = (WCHAR*)newPath;
	str1024 folderName;

	while ( *c )
	{
		if ( index<0 )
		{
			//  find drive
			for (int i=0; i<m_driveArray.Count(); i++)
			{
				if ( m_driveArray[i].name == String		::UpperChar(*c) )
				{
					m_driveList.SetItemIndex(i);
					m_driveList.SetFocused();
					index++;
					break;
				}
			}
			if ( index<0 ) return false;

			c++;
			c++;
			c++;
			continue;
		}

		if ( *c == '\\' || *c == '/' )
		{
			//UpdateFilesList(index, folderName);

			folderName.MakeLower();
			str1024 tmp;
			int lastIndex = index;
			for (int i=0; i<m_ListArray[index]->pListBox->Count(); i++)
			{
				tmp = m_ListArray[index]->pListBox->GetItem(i).Text;
				tmp.MakeLower();
				if ( tmp == folderName )
				{
					m_ListArray[index]->pListBox->SetItemIndex(i);
					m_ListArray[index]->pListBox->SetFocused();
					index++;
					break;
				}
			}
			if ( lastIndex == index ) return false;

			folderName.Clear();
			c++;
			continue;
		}

		folderName.Append( *c );
		c++;
	}

	if ( folderName.Length() )
	{
		folderName.MakeLower();
		str1024 tmp;
		for (int i=0; i<m_ListArray[index]->pListBox->Count(); i++)
		{
			tmp = m_ListArray[index]->pListBox->GetItem(i).Text;
			tmp.MakeLower();
			if ( tmp == folderName )
			{
				m_ListArray[index]->pListBox->SetItemIndex(i);
				m_ListArray[index]->pListBox->SetFocused();
				index++;
				break;
			}
		}
	}

	return true;
}

void Form_Explorer::OnButtonClick( sx::gui::PControl Sender )
{
	if ( Sender == m_pLabelPath )
	{
		if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_RETURN) )
		{
			str1024 tmp = m_pLabelPath->GetText();
			SetPath( tmp );
			m_pLabelPath->SetFocused(true);
			m_pLabelPath->SetCaretPos(0, 1000);
		}
	}
	else if ( Sender == m_btnOK )
	{
		str1024 tmp1 = m_Path;
		str1024 tmp2 = m_pLabelPath->GetText();
		str1024 tmp3 = tmp2; tmp3.ExtractFileExtension();
		tmp1.MakeLower();
		tmp2.MakeLower();

		if ( tmp1 != tmp2 && ( !tmp3.Length() || sx::sys::FileExist( tmp2 ) ) )
		{
			tmp1 = m_pLabelPath->GetText();
			SetPath( tmp1 );
			m_pLabelPath->SetFocused(true);
			m_pLabelPath->SetCaretPos(0, 1000);
		}
		else
		{
			ADD_TO_LAST_PATH();
			m_OnOkClick( m_btnOK );
		}
	}
	else if ( Sender == m_btnProject )
	{
		SetPath( sx::sys::FileManager::Project_GetDir() );
	}
	else if ( Sender == m_btnDesktop )
	{
		SetPath( sx::sys::GetDesktopFolder() );
	}
	else if ( Sender == m_btnMyDocs )
	{
		SetPath( sx::sys::GetDocumentsFolder() );
	}
	else if ( Sender == m_btnLast )
	{
		str1024 tmp1 = m_Path;
		while ( !m_lastFolders.IsEmpty() )
		{
			str1024 tmp2 = m_lastFolders.Top()->Text();
			if ( tmp1 == tmp2 )
			{
				String* pstr = null;
				if ( m_lastFolders.Pop( pstr ) )
					sx_delete( pstr );
			}
			else
				break;
		}

		if ( !m_lastFolders.IsEmpty() )
		{
			SetPath( m_lastFolders.Top()->Text() );
			String* pstr = null;
			if ( m_lastFolders.Pop( pstr ) )
				sx_delete( pstr );
		}
	}

}