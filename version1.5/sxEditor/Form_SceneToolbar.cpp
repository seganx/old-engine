#include "Form_SceneToolbar.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "EditorScene.h"
#include "EditorUI.h"
#include "EditorSettings.h"
#include "Form_EditPath.h"

#include "ImportMS3D.h"
#include "ImportTexture.h"
#include "ImportX.h"
#include "ImportOBJ.h"

using namespace sx;


#define ADD_BUTTON(btn, txname){\
	btn = sx_new( sx::gui::Button );\
	btn->SetParent(m_pBack);\
	btn->SetSize( float2(32.0f, 32.0f) );\
	btn->RemProperty(SX_GUI_PROPERTY_BLENDCHILDS);\
	btn->GetElement(0)->Color() = 0xFFcccccc;\
	btn->GetElement(1)->Color() = 0xFFffffff;\
	btn->GetElement(2)->Color() = 0xFF999999;\
	btn->GetElement(0)->SetTextureSrc( EditorUI::GetButtonTexture(txname, 0) );\
	btn->GetElement(1)->SetTextureSrc( EditorUI::GetButtonTexture(txname, 0) );\
	btn->GetElement(2)->SetTextureSrc( EditorUI::GetButtonTexture(txname, 0) );\
}\


static sx::cmn::StringList s_importedFiles;

Form_SceneToolbar::Form_SceneToolbar( void ): BaseForm()
{
	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);

	m_MinSize.x = 100.0f;	
	m_MinSize.y = 50.0f;
	SetSize( (float)EditorSettings::GetInitWindowSize().x, FORM_SCENE_TOOLBAR_SIZE_H );

	ADD_BUTTON(m_btnNew,	L"New");				SEGAN_GUI_SET_ONCLICK( m_btnNew, Form_SceneToolbar::OnButtonClick );
	ADD_BUTTON(m_btnImport, L"ObjectImportFile");	SEGAN_GUI_SET_ONCLICK( m_btnImport, Form_SceneToolbar::OnModelImportClick );
	ADD_BUTTON(m_btnExport, L"ObjectExportFile");	SEGAN_GUI_SET_ONCLICK( m_btnExport, Form_SceneToolbar::OnModelExportClick );
	ADD_BUTTON(m_btnAdd, L"ObjectAdd");				SEGAN_GUI_SET_ONCLICK( m_btnAdd, Form_SceneToolbar::OnButtonClick );
	ADD_BUTTON(m_btnRemove, L"ObjectRemove");		SEGAN_GUI_SET_ONCLICK( m_btnRemove, Form_SceneToolbar::OnButtonClick );
	ADD_BUTTON(m_btnEditMember, L"EditMember");		SEGAN_GUI_SET_ONCLICK( m_btnEditMember, Form_SceneToolbar::OnButtonClick );
	ADD_BUTTON(m_btnEditNode, L"EditNode");			SEGAN_GUI_SET_ONCLICK( m_btnEditNode, Form_SceneToolbar::OnButtonClick );
	ADD_BUTTON(m_btnSelect, L"ObjectSelect");		SEGAN_GUI_SET_ONCLICK( m_btnSelect, Form_SceneToolbar::OnButtonClick );
	ADD_BUTTON(m_btnMove, L"ObjectMove");			SEGAN_GUI_SET_ONCLICK( m_btnMove,	Form_SceneToolbar::OnButtonClick );
	ADD_BUTTON(m_btnRotate, L"ObjectRotate");		SEGAN_GUI_SET_ONCLICK( m_btnRotate, Form_SceneToolbar::OnButtonClick );
	ADD_BUTTON(m_btnView, L"View");					SEGAN_GUI_SET_ONCLICK( m_btnView, Form_SceneToolbar::OnButtonClick );
	ADD_BUTTON(m_btnBack, L"Back");					SEGAN_GUI_SET_ONCLICK( m_btnBack, Form_SceneToolbar::OnButtonClick );
	m_btnBack->SetSize( float2(48.0f, 28.0f) );

	m_btnNew->SetHint(L"NEW - CTRL + N\n Clear the scene and reset camera.");
	m_btnImport->SetHint(L"IMPORT - CRTL + O\n Import *.scene/*.node file to the scene. ");
	m_btnExport->SetHint(L"EXPORT - CTRL + S\n Export current scene to a *.scene/*.obj file. \n NOTE: selected nodes will export for *.obj file. ");
	m_btnAdd->SetHint(L"ADD\n Add a new object to the scene. ");
	m_btnRemove->SetHint(L"REMOVE\n Remove selected node from the scene. ");
	m_btnEditMember->SetHint(L"EDIT MEMBER - E\n Edit selected member. ");
	m_btnEditNode->SetHint(L"EDIT NODE - N\n Edit selected node in the scene. ");
	m_btnSelect->SetHint(L"SELECT\n Change selection type to select a node in the scene. ");
	m_btnMove->SetHint(L"MOVE\n	Show pivot of the selected nodes to move them. ");
	m_btnRotate->SetHint(L"ROTATE\n Show rotation tool to rotate selected nodes. ");
	m_btnView->SetHint(L"VIEW MODE\n Change view mode. ");
	m_btnBack->SetHint(L"BACK\n	Back to the main scene to select editor. ");

	//  create popup for add new object
	m_popNewObject.GetBack()->SetParent(m_pBack);
	m_popNewObject.SetSize( 100.0f, 60.0f );
	m_popNewObject.Listbox().Add( L"Path node",		EditorUI::GetMemberTexture(NMT_PATHNODE) );
	m_popNewObject.Listbox().Add( L"Terrain",		EditorUI::GetMemberTexture(NMT_TERRAIN) );
	m_popNewObject.Listbox().Add( L"nothing",		EditorUI::GetMemberTexture(NMT_NULL) );
	m_popNewObject.SetOnSelect( this, (GUICallbackEvent)&Form_SceneToolbar::OnNewObjectListClick );

	//  create popup for select type
	m_popSelectType.GetBack()->SetParent(m_pBack);
	m_popSelectType.SetSize( 100.0f, 110.0f );
	m_popSelectType.Listbox().Add( L"Node",		EditorUI::GetMemberTexture(NMT_UNKNOWN) );
	m_popSelectType.Listbox().Add( L"Mesh",		EditorUI::GetMemberTexture(NMT_MESH) );
	m_popSelectType.Listbox().Add( L"Path",		EditorUI::GetMemberTexture(NMT_PATHNODE) );
	m_popSelectType.Listbox().Add( L"Particle",	EditorUI::GetMemberTexture(NMT_PARTICLE) );
	m_popSelectType.Listbox().Add( L"Sound",	EditorUI::GetMemberTexture(NMT_SOUND) );
	m_popSelectType.Listbox().Add( L"All Type",	EditorUI::GetMemberTexture(NMT_ALL) );
	m_popSelectType.SetOnSelect( this, (GUICallbackEvent)&Form_SceneToolbar::OnPopupSelectType );
}

Form_SceneToolbar::~Form_SceneToolbar( void )
{
	//  for other things the parent will do everything :)
}

void Form_SceneToolbar::SetSize( float width, float height )
{
	BaseForm::SetSize(width, height);
}

void Form_SceneToolbar::ProcessInput( bool& result, int playerID /*= 0*/ )
{
	if (result) return;

	if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_DELETE) )
	{
		if ( !sx::gui::Control::GetFocusedControl() || sx::gui::Control::GetFocusedControl()->GetType() != GUI_TEXTEDIT )
			OnButtonClick(m_btnRemove);
		result = true;
	}

	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_N) )
	{
		OnButtonClick(m_btnNew);
		result = true;
	}

	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_S) )
	{
		if ( m_path_Save.Text() )
		{
			SaveScene(m_path_Save);
			result = true;

			str1024 tmp = m_path_Save; tmp.ExtractFileName();
			str1024 str = L"'";
			str << m_path_Save << L"' saved .";
			Editor::SetLabelTips(str, 10000.0f);
		}
		else
		{
			OnModelExportClick(m_btnExport);
			result = true;
		}
	}

	BaseForm::ProcessInput(result);
}

void Form_SceneToolbar::Update( float elpsTime )
{
	if ( !IsVisible() ) return;

	m_popNewObject.Update(elpsTime);
	m_popSelectType.Update(elpsTime);
	BaseForm::Update(elpsTime);

	static float updateTime = 0;
	if ( m_popSelectType.IsVisible() )
	{
		updateTime += elpsTime;
		if ( updateTime > 300 )
		{
			updateTime = -10000;
			switch (EditorScene::mouse_select_type)
			{
			case NMT_UNKNOWN :	m_popSelectType.Listbox().SetItemIndex(0);	break;
			case NMT_MESH :		m_popSelectType.Listbox().SetItemIndex(1);	break;
			case NMT_PATHNODE :	m_popSelectType.Listbox().SetItemIndex(2);	break;
			case NMT_PARTICLE : m_popSelectType.Listbox().SetItemIndex(3);	break;
			case NMT_SOUND :	m_popSelectType.Listbox().SetItemIndex(4);	break;
			case NMT_ALL :		m_popSelectType.Listbox().SetItemIndex(5);	break;
			}
		}
	}
	else updateTime = 0;
	
}

void Form_SceneToolbar::OnResize( int EditorWidth, int EditorHeight )
{
	float offsetW = (float)(EditorWidth  % 2);
	float offsetH = (float)(EditorHeight  % 2);
	float width = (float)EditorWidth + offsetW;
	float height = (float)EditorHeight + offsetH;
	float top = height*0.5f - FORM_SCENE_TOOLBAR_SIZE_H + 40.0f;
	SetSize(width-30.0f, FORM_SCENE_TOOLBAR_SIZE_H);

	//  set new states
	m_pBack->State_GetByIndex(0).Center		= float3(0.0f, 0.0f, 0.0f);
	m_pBack->State_GetByIndex(0).Position	= float3(0.0f, top, 0.0f);
	m_pBack->State_GetByIndex(0).Scale		= float3(1.0f, 1.0f, 1.0f);
	m_pBack->State_GetByIndex(0).Color.w	= 0.0f;

	m_pBack->State_GetByIndex(1).Center		= float3(0.0f, 0.0f, 0.0f);
	m_pBack->State_GetByIndex(1).Position	= float3(0.0f, top, 0.0f);
	m_pBack->State_GetByIndex(1).Scale		= float3(1.0f, 1.0f, 1.0f);

	//  arrange buttons
	float t = -14.0f;
	float l = -width/2 + 32.0f;

	m_btnNew->Position().Set(		l, t, 0.0f	);	l += 36.0f;
	m_btnImport->Position().Set(	l, t, 0.0f	);	l += 36.0f;
	m_btnExport->Position().Set(	l, t, 0.0f	);	l += 50.0f;

	m_btnAdd->Position().Set(		l, t, 0.0f	);	l += 36.0f;
	m_btnRemove->Position().Set(	l, t, 0.0f	);	l += 36.0f;
	m_btnEditMember->Position().Set(l, t, 0.0f	);	l += 50.0f;

	m_btnEditNode->Position().Set(	l, t, 0.0f	);	l += 36.0f;
	m_btnSelect->Position().Set(	l, t, 0.0f	);	l += 36.0f;
	m_btnMove->Position().Set(		l, t, 0.0f	);	l += 36.0f;
	m_btnRotate->Position().Set(	l, t, 0.0f	);	l += 50.0f;

	m_btnView->Position().Set(		l, t, 0.0f	);	l += 36.0f;

	m_btnBack->Position().Set( width/2 - 42.0f, t, 0.0f	);
}

void Form_SceneToolbar::OnButtonClick( sx::gui::PControl Sender )
{
	if (!Sender) return;

	if ( Sender == m_btnNew )
	{
		switch ( Sender->GetUserTag() )
		{
		case 0:
			if ( sx::core::Scene::Count() )
			{
				Editor::frm_Ask->Show( L" New Scene ", L" Are you sure to renew the scene ? ",
					ASK_YES | ASK_NO,  Sender,
					this, (GUICallbackEvent)&Form_SceneToolbar::OnButtonClick);
				break;
			}

		case ASK_YES:
			{
				core::Scene::Cleanup();
				EditorScene::selectedNodes.Clear();
				EditorScene::ResetCamera();
				m_path_Save.Clear();
				EditorScene::mng_UndoManager.Clear();

				s_importedFiles.Clear();
			}
			break;
		}

	}
	else if ( Sender == m_btnAdd )
	{
		m_popNewObject.Popup(m_btnAdd->Position().x, m_btnAdd->Position().y-70.0f);
	}
	else if ( Sender == m_btnRemove )
	{
		if ( EditorScene::selectedNodes.Count() )
			EditorScene::mng_UndoManager.DeleteNode( &EditorScene::selectedNodes[0], EditorScene::selectedNodes.Count() );
		EditorScene::selectedNodes.Clear();
		EditorScene::selectedMember = NULL;
	}
	else if ( Sender == m_btnEditMember )
	{
		if ( EditorScene::selectedMember )
		{
			if ( EditorScene::selectedMember->GetType() == NMT_MESH )
				EditorScene::frm_EditMesh->Show();
			else if ( EditorScene::selectedMember->GetType() == NMT_PATHNODE )
				EditorScene::frm_EditPath->Show();
			else if ( EditorScene::selectedMember->GetType() == NMT_SOUND )
				EditorScene::frm_EditSound->Show();
			else if ( EditorScene::selectedMember->GetType() == NMT_PARTICLE )
				EditorScene::frm_EditParticle->Show();
		}
	}
	else if ( Sender == m_btnEditNode )
	{
		if ( EditorScene::selectedNodes.Count()==1 )
		{
			EditorScene::frm_EditNode->Show();
		}
	}
	else if ( Sender == m_btnSelect )
	{
		EditorScene::mouse_state = EDITOR_MOUSESTATE_SELECT;
		m_popSelectType.Popup(m_btnSelect->Position().x, m_btnSelect->Position().y-95.0f);
	}
	else if ( Sender == m_btnMove )
	{
		EditorScene::mouse_state = EDITOR_MOUSESTATE_MOVE;
	}
	else if ( Sender == m_btnRotate )
	{
		EditorScene::mouse_state = EDITOR_MOUSESTATE_ROTATE;
	}
	else if ( Sender == m_btnView )
	{
		static int wmode = 0;
		switch (wmode)
		{
		case 0:		EditorScene::draw_Option = SX_DRAW_DEBUG | SX_DRAW_BOUNDING_BOX;	wmode++;	break;
		case 1:		EditorScene::draw_Option |= SX_DRAW_BOUNDING_SPHERE;				wmode++;	break;
		case 2:		EditorScene::draw_Option = SX_DRAW_WIRED;							wmode++;	break;
		case 3:		EditorScene::draw_Option = 0;										wmode=0;	break;
		}
	}
	else if ( Sender == m_btnBack )
		Editor::SetSpace( EWS_NULL );
}


void Form_SceneToolbar::OnModelImportClick( gui::PControl Sender )
{
	Editor::frm_Explorer->SetTitle(L"Import node to scene");
	Editor::frm_Explorer->Open( this, (GUICallbackEvent)&Form_SceneToolbar::ImportModel );
}

void Form_SceneToolbar::OnModelExportClick( sx::gui::PControl Sender )
{
	Editor::frm_Explorer->SetTitle(L"Save scene");
	Editor::frm_Explorer->Save( this, (GUICallbackEvent)&Form_SceneToolbar::ExportModel );
}

void Form_SceneToolbar::ImportModel( sx::gui::PControl Sender )
{
#define CALL_MODEL_IMPORTER		6489

	float timeElapsed = sx::sys::GetSysTime();
	core::PNode node = NULL;
	String		 FileName;

	if ( Sender->GetUserTag() == CALL_MODEL_IMPORTER )
	{
		FileName = Editor::frm_importModel->GetFileName();

		sx::core::ArrayPNode nodeList;
		ImportOBJFile(FileName, nodeList, Editor::frm_importModel->GetOptions() );
		if ( nodeList.Count() )
		{
			for (int i=0; i<nodeList.Count(); i++)
			{
				core::Scene::AddNode( nodeList[i] );
			}
			EditorScene::mng_UndoManager.AddAction( UAT_CREATE, &nodeList[0], nodeList.Count() );

			nodeList.Clear();
			str1024 tmp = Editor::frm_Explorer->GetPath(); tmp.ExtractFileName();
			str1024 str = L"'";
			str << tmp << L"' loaded to editor .";
			Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);
		}
	}
	else
	{
		if ( !sx::sys::FileExist( Editor::frm_Explorer->GetPath() ) )
		{
			Editor::SetLabelTips( L" File is not exist !" , 5000.0f );
			return;
		}
		FileName = Editor::frm_Explorer->GetPath();

		str1024 str = FileName;
		str.ExtractFileExtension();
		str.MakeLower();
		if (str == L"node")
		{
			node = sx_new( core::Node );
			sys::FileStream file;
			file.Open( FileName, FM_OPEN_READ);
			node->Load(file);
			file.Close();

			core::Scene::AddNode( node );
			EditorScene::selectedNodes.Clear();
			EditorScene::selectedNodes.PushBack( node );
			EditorScene::selectedMember = NULL;
			EditorScene::mng_UndoManager.AddAction( UAT_CREATE, &node, 1);

			float3 pos = node->GetPosition_local();
			pos.x = EditorScene::GetCurrentCamera().At.x;
			pos.z = EditorScene::GetCurrentCamera().At.z;
			node->SetPosition(pos);

			str1024 tmp = FileName; tmp.ExtractFileName();
			str1024 str = L"'";
			str << tmp << L"' loaded to editor .";
			Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);

		}
		else if (str == L"x")
		{
			if ( ImportXToLibrary( FileName, node ) )
			{
				core::Scene::AddNode( node );
				EditorScene::selectedNodes.Clear();
				EditorScene::selectedNodes.PushBack( node );
				EditorScene::selectedMember = NULL;
				EditorScene::mng_UndoManager.AddAction( UAT_CREATE, &node, 1);

				str1024 tmp = FileName; tmp.ExtractFileName();
				str1024 str = L"'";
				str << tmp << L"' loaded to editor .";
				Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);
			}
			else
			{
				str1024 tmp = Editor::frm_Explorer->GetPath(); tmp.ExtractFileName();
				str1024 str = L"It seems that loading '";
				str << tmp << L"' has been failed !";
				Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);
			}			
		}
		else if (str == L"obj")
		{
			Editor::frm_importModel->Import( FileName, this, (GUICallbackEvent)&Form_SceneToolbar::ImportModel, CALL_MODEL_IMPORTER );
		}
		else if (str == L"scene")
		{
			switch ( Sender->GetUserTag() )
			{
			case 0:
				if ( s_importedFiles.IndexOf( Editor::frm_Explorer->GetPath() ) >= 0 )
				{
					str.Format( L" %s\nwas imported to the scene. Do you want to import that again ? ", Editor::frm_Explorer->GetPath() );
					Editor::frm_Ask->Show( L" Import Scene File ", str,	
						ASK_YES | ASK_NO, Sender,
						this, (GUICallbackEvent)&Form_SceneToolbar::ImportModel);
					break;
				}

			case ASK_YES:
				{
					LoadScene( Editor::frm_Explorer->GetPath() );
					s_importedFiles.PushBack( Editor::frm_Explorer->GetPath() );

					str1024 tmp = Editor::frm_Explorer->GetPath(); tmp.ExtractFileName();
					str1024 str = L"'";
					str << tmp << L"' loaded to editor .";
					Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);
				}
			}
		}
		else
		{
			str1024 tmp = Editor::frm_Explorer->GetPath(); tmp.ExtractFileName();
			str1024 str = L"It seems that loading '";
			str << tmp << L"' has been failed !";
			Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);
		}
	}
	Editor::frm_Explorer->Close();
}

void Form_SceneToolbar::ExportModel( sx::gui::PControl Sender )
{
	str1024 str = Editor::frm_Explorer->GetPath();

	//  check extension
	str1024 ext = str;
	ext.ExtractFileExtension();
	ext.MakeLower();

	if ( ext == "obj" )
	{
		switch ( Sender->GetUserTag() )
		{
		case 0:
			if ( sx::sys::FileExist( str ) )
			{
				Editor::frm_Ask->Show( L" Save Scene " , L" The file is already exist. Do you want to replace it ? ",
					ASK_YES | ASK_NO, Sender,
					this, (GUICallbackEvent)&Form_SceneToolbar::ExportModel );
				break;
			}

		case ASK_YES:
			{
				if ( EditorScene::selectedNodes.IsEmpty() )
				{
					sx::core::ArrayPNode nodeList;
					sx::core::Scene::GetAllNodes(nodeList);
					ExportOBJFile(str, nodeList);
				}
				else
				{
					ExportOBJFile(str, EditorScene::selectedNodes);
				}
				Editor::frm_Explorer->Close();
			}
			break;
		}
	}
	else
	{
		if ( ext != L"scene" )	str << L".scene";

		switch ( Sender->GetUserTag() )
		{
		case 0:
			if ( sx::sys::FileExist( str ) )
			{
				Editor::frm_Ask->Show( L" Save Scene " , L" The file is already exist. Do you want to replace it ? ",
					ASK_YES | ASK_NO, Sender,
					this, (GUICallbackEvent)&Form_SceneToolbar::ExportModel );
				break;
			}

		case ASK_YES:
			{
				SaveScene( str );
				Editor::frm_Explorer->Close();
			}
			break;
		}
	}
}

void Form_SceneToolbar::OnNewObjectListClick( sx::gui::PControl Sender )
{
	switch ( m_popNewObject.Listbox().GetItemIndex() )
	{
	case 0:	// path node
		{
			EditorScene::frm_EditPath->Show();
		}
		break;

	case 1: // edit terrain
		{
			EditorScene::frm_EditTrrn->Show();
		}
		break;
	}

	if ( sx::gui::Control::GetFocusedControl() )
		sx::gui::Control::GetFocusedControl()->SetFocused(false);
}

void Form_SceneToolbar::OnPopupSelectType( sx::gui::PControl Sender )
{
	switch ( m_popSelectType.Listbox().GetItemIndex() )
	{
	case 0:	EditorScene::mouse_select_type = NMT_UNKNOWN;	break;
	case 1:	EditorScene::mouse_select_type = NMT_MESH;		break;
	case 2:	EditorScene::mouse_select_type = NMT_PATHNODE;	break;
	case 3:	EditorScene::mouse_select_type = NMT_PARTICLE;	break;
	case 4:	EditorScene::mouse_select_type = NMT_SOUND;		break;
	case 5:	EditorScene::mouse_select_type = NMT_ALL;		break;
	}

	m_popSelectType.Close();
	if ( sx::gui::Control::GetFocusedControl() )
		sx::gui::Control::GetFocusedControl()->SetFocused(false);
}

void Form_SceneToolbar::SaveScene( const WCHAR* fileName )
{
	sx::core::Settings::GetOption_Reflection()->RemPermission( OPT_BY_SYSTEM );
	sx::core::Settings::GetOption_Reflection()->RemPermission( OPT_BY_USER );
	sx::core::Settings::GetOption_Shadow()->RemPermission( OPT_BY_SYSTEM );
	sx::core::Settings::GetOption_Shadow()->RemPermission( OPT_BY_USER );

	sx::sys::FileStream file;
	if ( file.Open(fileName, FM_CREATE) )
	{
		m_path_Save = fileName;
		
		int version = 1;
		SEGAN_STREAM_WRITE(file, version);

		core::Scene::SaveNodes( file );
		core::Renderer::Save( file );

		file.Close();
	}

	sx::core::Settings::GetOption_Reflection()->AddPermission( OPT_BY_SYSTEM );
	sx::core::Settings::GetOption_Reflection()->AddPermission( OPT_BY_USER );
	sx::core::Settings::GetOption_Shadow()->AddPermission( OPT_BY_SYSTEM );
	sx::core::Settings::GetOption_Shadow()->AddPermission( OPT_BY_USER );
}

void Form_SceneToolbar::LoadScene( const WCHAR* fileName )
{
	sys::FileStream file;
	if ( file.Open(fileName, FM_OPEN_READ) )
	{
		if ( !m_path_Save.Text() )
			m_path_Save = fileName;

		int version = 0;
		SEGAN_STREAM_READ(file, version);

		if ( version == 1 )
		{
			core::Scene::LoadNodes( file );
			core::Renderer::Load( file );
			EditorScene::ReloadCamera();
		}
		else
		{
			file.SetPos(0);
			core::Scene::LoadNodes( file );
		}

		file.Close();
	}

	sx::core::Settings::GetOption_Reflection()->AddPermission( OPT_BY_SYSTEM );
	sx::core::Settings::GetOption_Reflection()->AddPermission( OPT_BY_USER );
	sx::core::Settings::GetOption_Shadow()->AddPermission( OPT_BY_SYSTEM );
	sx::core::Settings::GetOption_Shadow()->AddPermission( OPT_BY_USER );
}

