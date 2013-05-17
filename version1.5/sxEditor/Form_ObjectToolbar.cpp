#include "Form_ObjectToolbar.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "EditorObject.h"
#include "EditorUI.h"
#include "EditorSettings.h"

#include "ImportMS3D.h"
#include "ImportTexture.h"
#include "ImportX.h"
#include "ImportOBJ.h"
#include "ImportFont.h"

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


Form_ObjectToolbar::Form_ObjectToolbar( void ): BaseForm()
{
	//  stop moving and resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);

	m_MinSize.x = 100.0f;	
	m_MinSize.y = 50.0f;
	SetSize( (float)EditorSettings::GetInitWindowSize().x, FORM_OBJECT_TOOLBAR_SIZE_H );


	ADD_BUTTON(m_btnNew,	L"New");				SEGAN_GUI_SET_ONCLICK( m_btnNew,	Form_ObjectToolbar::OnButtonClick );
	ADD_BUTTON(m_btnImport, L"ObjectImportFile");	SEGAN_GUI_SET_ONCLICK( m_btnImport, Form_ObjectToolbar::OnModelImportClick );
	ADD_BUTTON(m_btnExport, L"ObjectExportFile");	SEGAN_GUI_SET_ONCLICK( m_btnExport, Form_ObjectToolbar::OnModelExportClick );
	ADD_BUTTON(m_btnAdd,		L"ObjectAdd");		SEGAN_GUI_SET_ONCLICK( m_btnAdd, Form_ObjectToolbar::OnButtonClick );
	ADD_BUTTON(m_btnRemove,		L"ObjectRemove");	SEGAN_GUI_SET_ONCLICK( m_btnRemove, Form_ObjectToolbar::OnButtonClick );
	ADD_BUTTON(m_btnEditMember, L"EditMember");		SEGAN_GUI_SET_ONCLICK( m_btnEditMember, Form_ObjectToolbar::OnButtonClick );
	ADD_BUTTON(m_btnEditNode,	L"EditNode");		SEGAN_GUI_SET_ONCLICK( m_btnEditNode, Form_ObjectToolbar::OnButtonClick );
	ADD_BUTTON(m_btnSelect,		L"ObjectSelect");	SEGAN_GUI_SET_ONCLICK( m_btnSelect, Form_ObjectToolbar::OnButtonClick );
	ADD_BUTTON(m_btnMove,		L"ObjectMove");		SEGAN_GUI_SET_ONCLICK( m_btnMove,	Form_ObjectToolbar::OnButtonClick );
	ADD_BUTTON(m_btnRotate,		L"ObjectRotate");	SEGAN_GUI_SET_ONCLICK( m_btnRotate, Form_ObjectToolbar::OnButtonClick );
	ADD_BUTTON(m_btnView,		L"View");			SEGAN_GUI_SET_ONCLICK( m_btnView, Form_ObjectToolbar::OnButtonClick );
	ADD_BUTTON(m_btnBack,		L"Back");			SEGAN_GUI_SET_ONCLICK( m_btnBack, Form_ObjectToolbar::OnButtonClick );

	m_btnNew->SetHint(L"NEW - CTRL + N\n Clear editor.");
	m_btnImport->SetHint(L"OPEN - CRTL + O\n Import *.x/*.node/*.obj/*.ms3d file to the editor.\n NOTE: Imported node will be attached to the selected node. ");
	m_btnExport->SetHint(L"SAVE - CTRL + S\n Save object a *.node file. ");
	m_btnAdd->SetHint(L"ADD\n Add a new object to the selected node. ");
	m_btnRemove->SetHint(L"REMOVE\n Remove selected node/member. ");
	m_btnEditMember->SetHint(L"EDIT MEMBER - E\n Show member editor for selected member. ");
	m_btnEditNode->SetHint(L"EDIT NODE - N\n Edit selected node. ");
	m_btnSelect->SetHint(L"SELECT\n Change selection type to select a node/member. ");
	m_btnMove->SetHint(L"MOVE\n	Show pivot of the selected node to move. ");
	m_btnRotate->SetHint(L"ROTATE\n Show rotation tool to rotate selected node. ");
	m_btnView->SetHint(L"VIEW MODE\n Change view mode. ");
	m_btnBack->SetHint(L"BACK\n	Back to the main scene to select editor. ");

	m_btnBack->SetSize( float2(48.0f, 28.0f) );

	m_popNewObject.GetBack()->SetParent(m_pBack);
	m_popNewObject.SetSize( 100.0f, 80.0f );
	m_popNewObject.Listbox().Add( L"Node",		EditorUI::GetMemberTexture(NMT_UNKNOWN) );
	m_popNewObject.Listbox().Add( L"Mesh",		EditorUI::GetMemberTexture(NMT_MESH) );
	m_popNewObject.Listbox().Add( L"Particle",	EditorUI::GetMemberTexture(NMT_PARTICLE) );
	m_popNewObject.Listbox().Add( L"Sound",		EditorUI::GetMemberTexture(NMT_SOUND) );
	m_popNewObject.SetOnSelect( this, (GUICallbackEvent)&Form_ObjectToolbar::OnNewObjectListClick );
}

Form_ObjectToolbar::~Form_ObjectToolbar( void )
{
	//  for other things the parent will do everything :)
}

void Form_ObjectToolbar::SetSize( float width, float height )
{
	BaseForm::SetSize(width, height);
}

void Form_ObjectToolbar::ProcessInput( bool& result, int playerID /*= 0*/ )
{
	if (result) return;
	
	sx::gui::PControl focused = sx::gui::Control::GetFocusedControl();
	GUIControlType guiType = focused ? focused->GetType() : GUI_NONE;
	if ( (guiType!=GUI_TEXTEDIT) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_DELETE) )
	{
		OnButtonClick(m_btnRemove);
		result = true;
	}

	if ( SEGAN_KEYHOLD(0, SX_INPUT_KEY_LCONTROL) && SEGAN_KEYDOWN(0, SX_INPUT_KEY_S) )
	{
		if ( m_path_Save.Text() )
		{
			sx::sys::FileStream file;
			if ( file.Open(m_path_Save, FM_CREATE) )
			{
				EditorObject::node_Root->Save(file);
				file.Close();
			}

			str1024 tmp = m_path_Save;
			tmp.ExtractFileName();
			str1024 str = L"'";
			str << tmp << L"' saved .";
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

void Form_ObjectToolbar::Update( float elpsTime )
{
	m_popNewObject.Update(elpsTime);
	BaseForm::Update(elpsTime);
}

void Form_ObjectToolbar::OnResize( int EditorWidth, int EditorHeight )
{
	float offsetW = (float)(EditorWidth  % 2);
	float offsetH = (float)(EditorHeight  % 2);
	float width = (float)EditorWidth + offsetW;
	float height = (float)EditorHeight + offsetH;
	float top = height*0.5f - FORM_OBJECT_TOOLBAR_SIZE_H + 40.0f;
	SetSize(width-30.0f, FORM_OBJECT_TOOLBAR_SIZE_H);

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

void Form_ObjectToolbar::OnButtonClick( sx::gui::PControl Sender )
{
	if (!Sender) return;

	if ( Sender == m_btnNew )
	{
		switch ( Sender->GetUserTag() )
		{
		case 0:
			if ( EditorObject::node_Root )
			{
				Editor::frm_Ask->Show( L" New Object Editor", L" Are you sure to renew the object editor ? ",
					ASK_YES | ASK_NO,  Sender,
					this, (GUICallbackEvent)&Form_ObjectToolbar::OnButtonClick);
				break;
			}

		case ASK_YES:
			{
				sx_delete( EditorObject::node_Root );
				EditorObject::node_Root = NULL;
				EditorObject::selectedNode = NULL;
				EditorObject::selectedMember = NULL;
				EditorObject::ResetCamera();
				m_path_Save.Clear();
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
		if ( EditorObject::selectedNode )
		{
			if ( EditorObject::selectedMember && (EditorObject::selectedNode->GetMemberCount() || EditorObject::selectedNode->GetChildCount()) )
			{
				//  just remove member
				sx_delete_and_null( EditorObject::selectedMember );
			}
			else if ( !EditorObject::selectedNode->HasProperty(SX_NODE_ABSOLUTE ) )
			{
				//  remove node
				if ( EditorObject::selectedNode == EditorObject::node_Root )
					EditorObject::node_Root = NULL;
				sx_delete_and_null( EditorObject::selectedNode );
				EditorObject::selectedMember = NULL;
			}
		}
	}
	else if ( Sender == m_btnEditMember )
	{
		if ( EditorObject::selectedMember )
		{
			if ( EditorObject::selectedMember->GetType() == NMT_MESH )
				EditorObject::frm_EditMesh->Show();

			if ( EditorObject::selectedMember->GetType() == NMT_PARTICLE )
				EditorObject::frm_EditParticle->Show();

			if ( EditorObject::selectedMember->GetType() == NMT_SOUND )
				EditorObject::frm_EditSound->Show();

			if ( EditorObject::selectedMember->GetType() == NMT_ANIMATOR )
				EditorObject::frm_EditAnimator->Show();
		}
	}
	else if ( Sender == m_btnEditNode )
	{
		if ( EditorObject::node_Root )
		{
			EditorObject::frm_EditNode->Show();
		}
	}
	else if ( Sender == m_btnSelect )
		EditorObject::mouse_state = EDITOR_MOUSESTATE_SELECT;
	else if ( Sender == m_btnMove )
		EditorObject::mouse_state = EDITOR_MOUSESTATE_MOVE;
	else if ( Sender == m_btnRotate )
		EditorObject::mouse_state = EDITOR_MOUSESTATE_ROTATE;
	else if ( Sender == m_btnView )
	{
		static int wmode = 1;
		switch (wmode)
		{
		case 0:		EditorObject::draw_Option = SX_DRAW_DEBUG | SX_DRAW_BOUNDING_BOX;	wmode++;	break;
		case 1:		EditorObject::draw_Option |= SX_DRAW_BOUNDING_SPHERE;				wmode++;	break;
		case 2:		EditorObject::draw_Option = SX_DRAW_WIRED;							wmode++;	break;
		case 3:		EditorObject::draw_Option = 0;										wmode=0;	break;
		}
	}
	else if ( Sender == m_btnBack )
		Editor::SetSpace( EWS_NULL );
}


void Form_ObjectToolbar::OnModelImportClick( gui::PControl Sender )
{
	Editor::frm_Explorer->SetTitle(L"Import objects to editor");
	Editor::frm_Explorer->Open( this, (GUICallbackEvent)&Form_ObjectToolbar::ImportModel );
}

void Form_ObjectToolbar::OnModelExportClick( sx::gui::PControl Sender )
{
	if (!EditorObject::node_Root) return;
	Editor::frm_Explorer->SetTitle(L"Save model as node");
	Editor::frm_Explorer->Save( this, (GUICallbackEvent)&Form_ObjectToolbar::ExportModel );
}

void Form_ObjectToolbar::ImportModel( sx::gui::PControl Sender )
{
#define CALL_MODEL_IMPORTER		1

	float timeElapsed = sx::sys::GetSysTime();
	core::PNode node = NULL;
	String		 FileName;

	
	if ( Sender->GetUserTag() == CALL_MODEL_IMPORTER )
	{
		FileName = Editor::frm_importModel->GetFileName();

		sx::core::ArrayPNode nodeList;
		ImportOBJFile( FileName, nodeList, Editor::frm_importModel->GetOptions( IMPORT_MODEL_ONE_NODE ) );
		if ( nodeList.Count() )
		{
			node = nodeList[0];

			for (int i=1; i<nodeList.Count(); i++)
			{
				sx_delete( nodeList[i] );
			}

			nodeList.Clear();
		}
	}
	else
	{
		FileName = Editor::frm_Explorer->GetPath();

		str1024 str = FileName;
		str.ExtractFileExtension();
		str.MakeLower();
		if (str == L"x")
		{
			ImportXToLibrary(FileName, node);
		}
		else if (str == L"ms3d")
		{
			ImportMS3DFile(FileName, IMPORT_MS3D_ANIMATION | IMPORT_MS3D_MODEL, node);
		}
		else if (str == L"obj")
		{
			Editor::frm_importModel->Import( FileName, this, (GUICallbackEvent)&Form_ObjectToolbar::ImportModel, CALL_MODEL_IMPORTER );
		}
		else if (str == L"node")
		{
			node = sx_new( core::Node );
			sys::FileStream file;
			file.Open(FileName, FM_OPEN_READ);
			node->Load(file);
			file.Close();

			if ( !m_path_Save.Text() )
				m_path_Save = FileName;
		}

		//////////////////////////////////////////////////////////////////////////
		//  load other things
		else if (str == L"fnt")
		{
			if ( ImportFontToLibrary( FileName ) )
			{
				str1024 tmp = FileName; tmp.ExtractFileName();
				str1024 str = L"'";
				str << tmp << L"' imported to project .";
				Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);
			}
			else
			{
				str1024 tmp = FileName; tmp.ExtractFileName();
				str1024 str = L"It seems that loading '";
				str << tmp << L"' has been failed !";
				Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);
			}
			return;
		}

		else if (str == L"snd")
		{
			str = FileName;
			str.ExtractFileName();

			core::Sound* sound = sx_new( core::Sound );
			sound->m_resources.PushBack( str );
			sound->SetName( str.ExcludeFileExtension() );


			SoundPlayerDesc playerDesc;
			playerDesc.volume = 100;
			playerDesc.minDistance = 1;
			playerDesc.maxDistance = 10;
			sound->SetDesc( playerDesc );

			if ( EditorObject::selectedNode )
				sound->SetOwner( EditorObject::selectedNode );
			else if ( EditorObject::node_Root )
				sound->SetOwner( EditorObject::node_Root );
			else
			{
				node = sx_new( core::Node );
				node->SetName( str );
				sound->SetOwner( node );
				EditorObject::node_Root = node;
			}

			EditorObject::selectedMember = sound;
			return;
		}
		else if ( str == L"bmp" || str == L"jpg" || str == L"tga" || str == L"dds" || str == L"png" || str == L"hdr" )
		{
			//  import texture to library
			str = Editor::frm_Explorer->GetPath();
			str.ExtractFileName();
			str.ExcludeFileExtension();
			str << L".txr";

			String texPath;
			if ( sx::sys::FileManager::File_GetPath( str, SEGAN_PACKAGENAME_TEXTURE, texPath ) )
			{
				ConvertTexture( Editor::frm_Explorer->GetPath(), texPath );

				str1024 tmp = FileName; tmp.ExtractFileName();
				str1024 str = L"'";
				str << tmp << L"' imported to project .";
				Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);
			}

			return;
		}
	}
	


	//////////////////////////////////////////////////////////////////////////
	//	process defaults
	if (node)
	{
		if ( EditorObject::selectedNode )
			node->SetParent(EditorObject::selectedNode);
		else if ( EditorObject::node_Root )
			node->SetParent( EditorObject::node_Root );
		else
			EditorObject::node_Root = node;

		// validate node
		float f = 0.0f;
		node->MsgProc(MT_ACTIVATE, &f);

		str1024 tmp = FileName; tmp.ExtractFileName();
		str1024 str = L"'";
		str << tmp << L"' loaded to editor .";
		Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);

		EditorObject::selectedNode = node;
		if ( !EditorObject::node_Root )
			EditorObject::node_Root = node;
	}
	else
	{
		str1024 tmp = FileName; tmp.ExtractFileName();
		str1024 str = L"It seems that loading '";
		str << tmp << L"' has been failed !";
		Editor::SetLabelTips(str, sx::sys::GetSysTime() - timeElapsed + 10000.0f);
	}
	// if (node)

	Editor::frm_Explorer->Close();
}

void Form_ObjectToolbar::ExportModel( sx::gui::PControl Sender )
{
	if (!EditorObject::node_Root) return;

	str1024 str = Editor::frm_Explorer->GetPath();

	switch ( Sender->GetUserTag() )
	{
	case 0:
		if ( sx::sys::FileExist( str ) )
		{
			Editor::frm_Ask->Show( L" Save Node " , L" The file is already exist. Do you want to replace it ? ",
				ASK_YES | ASK_NO, Sender,
				this, (GUICallbackEvent)&Form_ObjectToolbar::ExportModel );
			break;
		}

	case ASK_YES:
		{
			//  check extension
			str1024 ext = str;
			ext.ExtractFileExtension();
			ext.MakeLower();
			if ( ext != "node" )
				str << L".node";

			sx::sys::FileStream file;
			if ( file.Open(str, FM_CREATE) )
			{
				EditorObject::node_Root->Save(file);
				file.Close();

				m_path_Save = str;
				str1024 tmp = m_path_Save; tmp.ExtractFileName();
				str1024 str = L"'";
				str << tmp << L"' saved .";
				Editor::SetLabelTips(str, 10000.0f);
			}

			Editor::frm_Explorer->Close();
		}
	}
}

void Form_ObjectToolbar::OnNewObjectListClick( sx::gui::PControl Sender )
{
	switch ( m_popNewObject.Listbox().GetItemIndex() )
	{
	case 0:	// Node
		{
			sx::core::PNode node = sx::core::Scene::CreateNode();
			Editor::frm_SetName->UpdateEditorData( node, NULL );
			Editor::frm_SetName->Show();
		}
		break;

	case 1:	// Mesh
		{

		}
		break;

	case 2:	// Particle
		{
			//  describe new particle
			ParticleDesc desc;
			desc.quadCount = 20;
			desc.lifeTime = 1.0f;
			desc.scaleBaseX = 0.5f;
			desc.scaleBaseY = 0.5f;
			desc.scaleSpeed = 0.0f;
			desc.initSpeed.Set(0.0f, 0.5f, 0.0f);
			desc.initRange.Set(0.0f, 0.0f, 0.0f);

			//  create new particle
			sx::core::PParticle par = sx_new( sx::core::Particle );
			par->SetDesc(&desc);
			par->Validate(0);

			EditorObject::selectedMember = par;
			Editor::frm_SetName->UpdateEditorData( EditorObject::selectedNode, EditorObject::selectedMember );
			Editor::frm_SetName->Show();
		}
		break;

	case 3: // Sound
		{
			//  create new sound
			sx::core::PSound sound = sx_new( sx::core::Sound );

			EditorObject::selectedMember = sound;
			Editor::frm_SetName->UpdateEditorData( EditorObject::selectedNode, EditorObject::selectedMember );
			Editor::frm_SetName->Show();
		}
		break;

	case 4: // ?
		{

		}
		break;
	}

	if ( sx::gui::Control::GetFocusedControl() )
		sx::gui::Control::GetFocusedControl()->SetFocused(false);
}
