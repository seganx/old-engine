#include "MainEditor.h"
#include "MainWindow.h"
#include "sxListBox.h"
#include "EditorUI.h"
#include "EditorSettings.h"
#include "EditorObject.h"
#include "EditorScene.h"
#include "EditorHUD.h"
#include "EditorGame.h"
#include "ImportX.h"
#include "ImportFont.h"
#include "ImportTexture.h"


#define CREATE_PANEL_EDITOR(panel, caption, alighY)\
{\
	panel = (sx::gui::PPanelEx)sx::gui::Create(GUI_PANELEX);\
	panel->State_Add();\
	panel->State_GetByIndex(0).Scale = float3( 1.0f, 1.0f, 1.0f );\
	panel->State_GetByIndex(1).Scale = float3( 0.3f, 0.3f, 1.0f );\
	panel->State_GetByIndex(0).Color = 0xbbffffff;\
	panel->State_GetByIndex(1).Color = 0xbbffffff;\
	panel->State_GetByIndex(0).Blender = float2( 0.1f, 0.5f );\
	panel->State_GetByIndex(1).Blender = float2( 0.1f, 0.5f );\
	panel->State_SetIndex(1);\
	panel->Shader_Set(L"return float4( tex2D( samp0, tex0 ).rgb*color.rgb, color.a);");\
	panel->Shader_Compile();\
	panel->AddProperty( SX_GUI_PROPERTY_ACTIVATE );\
	panel->SetOnMouseClick(&internal_form, (GUICallbackEvent)&Internal_Form_Scene::editorPanelClick);\
\
	{\
		sx::gui::PPanelEx panelHelper = sx_new( sx::gui::PanelEx );\
		panelHelper->SetParent( panel );\
		panelHelper->SetSize( float2(1, 1) );\
		panelHelper->State_GetByIndex(0).Align.y = alighY;\
		panelHelper->State_GetByIndex(0).Color = D3DColor(0.0f, 0.0f, 0.0f, 0.1f);\
		panelHelper->State_GetByIndex(0).Scale.Set(3.0f, 3.0f, 1.0f);\
		sx::gui::PLabel label = EditorUI::CreateLabel( panelHelper, 600.0f, 25, caption);\
		label->GetElement(1)->Color() = D3DColor(1.0f, 1.0f, 0.0, 0.7f);\
		label->SetAlign(GTA_CENTER);\
	}\
}\


//////////////////////////////////////////////////////////////////////////
//	GLOBAL VARIABLE DIFINITION
//////////////////////////////////////////////////////////////////////////
extern EditorWorkingSpace	g_CurSpace	= EWS_NULL;		//  specify the current working space
sx::gui::PPanel		g_panelBack			= NULL;			//  hold background image
sx::gui::PPanelEx	g_panelObject		= NULL;			//  hold object editor view port
sx::gui::PPanelEx	g_panelScene		= NULL;			//  hold scene space view port
sx::gui::PPanelEx	g_panelHUD			= NULL;			//  hold HUD space view port
sx::gui::PPanelEx	g_panelGame			= NULL;			//  hold game space view port
sx::gui::PPanelEx	g_panelCurSpace		= NULL;			//  hold the panel of current space to draw it last

String				g_strTips;
float				g_tipsTime = 0;
const int			g_tipsCount = 8;
const WCHAR*		g_tipsStr[g_tipsCount] = {
	L" TIPS :   Use  ' CTRL + Mouse Left '  to rotate camera.",
	L" TIPS :   Hold  ' Mouse Middle '  to pane camera.",
	L" TIPS :   Hold  ' Shift '  while selecting nodes to select more nodes.",
	L" TIPS :   Press  ' Shift + Z '  for zoom to selected node / member.",
	L" TIPS :   Press  ' L '  to lock / unlock camera. ",
	L" TIPS :   Use  ' W / S / A / D / Q / E '  to move camera.",
	L" TIPS :   For export scene as  ' OBJ '  file type  ' .obj '  after your filename. ",
	L" TIPS :   Hold  ' Shift '  while placing path nodes to connect path nodes automatically. ",
};


class Internal_Form_Scene : public sx::gui::Form
{
public:
	void editorPanelClick(sx::gui::PControl Sender)
	{
		g_panelObject->State_SetIndex(1);
		g_panelScene->State_SetIndex(1);
		g_panelHUD->State_SetIndex(1);
		g_panelGame->State_SetIndex(1);

		g_panelCurSpace = (sx::gui::PPanelEx)Sender;
		g_panelCurSpace->State_SetIndex(0);
		g_panelCurSpace->State_GetByIndex(1).Color = 0xbbFFFFFF;


		if (g_panelCurSpace == g_panelObject)	{ Editor::SetSpace(EWS_OBJECT);	return; }
		if (g_panelCurSpace == g_panelScene)	{ Editor::SetSpace(EWS_SCENE);	return; }
		if (g_panelCurSpace == g_panelHUD)		{ Editor::SetSpace(EWS_HUD);	return; }
		if (g_panelCurSpace == g_panelGame)		{ Editor::SetSpace(EWS_GAME);	return; }
	}

	void OnButtonClick(sx::gui::PControl Sender)
	{
		if (!Sender) return;

		if (Sender == Editor::btn_Settings)
		{
			Editor::frm_Settings->Show();
			return;
		}
	}
};
Internal_Form_Scene internal_form;


//////////////////////////////////////////////////////////////////////////
//	CLASS AND FUNCTION IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////
PForm_SetName			Editor::frm_SetName			= NULL;
PForm_Explorer			Editor::frm_Explorer		= NULL;
PForm_Settings			Editor::frm_Settings		= NULL;
PForm_Hint				Editor::frm_Hint			= NULL;
PForm_Ask				Editor::frm_Ask				= NULL;
PForm_ImportSound		Editor::frm_importSound		= NULL;
PForm_ImportModel		Editor::frm_importModel		= NULL;
sx::gui::PLabel			Editor::dbg_Lable			= NULL;
sx::gui::PButton		Editor::btn_Settings		= NULL;

void CreateRT(void)
{
	PD3DViewport vp = sx::d3d::Device3D::Viewport();

	g_panelObject->GetElement(0)->GetTexture()->Cleanup();
	g_panelObject->GetElement(0)->GetTexture()->CreateRenderTarget(D3DRTYPE_TEXTURE, vp->Width, vp->Height, D3DFMT_A8R8G8B8);

	g_panelScene->GetElement(0)->GetTexture()->Cleanup();
	g_panelScene->GetElement(0)->GetTexture()->CreateRenderTarget(D3DRTYPE_TEXTURE, vp->Width, vp->Height, D3DFMT_A8R8G8B8);

	g_panelHUD->GetElement(0)->GetTexture()->Cleanup();
	g_panelHUD->GetElement(0)->GetTexture()->CreateRenderTarget(D3DRTYPE_TEXTURE, vp->Width, vp->Height, D3DFMT_A8R8G8B8);

	g_panelGame->GetElement(0)->GetTexture()->Cleanup();
	g_panelGame->GetElement(0)->GetTexture()->CreateRenderTarget(D3DRTYPE_TEXTURE, vp->Width, vp->Height, D3DFMT_A8R8G8B8);

}


void Editor::Initialize( void )
{
	sx_callstack();
	
	//  verify that current editor is clear 
	Finalize();

	//	show as status bar
	dbg_Lable = sx_new( sx::gui::Label );
	dbg_Lable->SetFont( EditorUI::GetDefaultFont(8) );
	dbg_Lable->GetElement(0)->Color() = D3DColor(0.0f,0.0f,0.0f,1.0f);

	//  settings form
	btn_Settings = sx_new( sx::gui::Button );
	btn_Settings->SetParent(dbg_Lable);
	btn_Settings->GetElement(0)->SetTextureSrc( EditorUI::GetButtonTexture(L"Settings", 0) );
	btn_Settings->GetElement(1)->SetTextureSrc( EditorUI::GetButtonTexture(L"Settings", 0) );
	btn_Settings->GetElement(2)->SetTextureSrc( EditorUI::GetButtonTexture(L"Settings", 0) );
	btn_Settings->GetElement(0)->Color() = D3DColor(0.0f, 1.0f, 0.0f, 1.0f);
	btn_Settings->GetElement(1)->Color() = D3DColor(1.0f, 0.0f, 0.0f, 1.0f);
	btn_Settings->GetElement(2)->Color() = D3DColor(0.4f, 0.4f, 0.4f, 1.0f);
	btn_Settings->SetSize( float2(28.0f, 28.0f) );
	btn_Settings->SetOnMouseClick(&internal_form, (GUICallbackEvent)&Internal_Form_Scene::OnButtonClick);
	btn_Settings->SetHint(L"SETTINGS\n Show settings window to change settings of editor.");

	//	initialize other editors
	EditorUI::Initialize();
	EditorObject::Initialize();
	EditorScene::Initialize();
	EditorHUD::Initialize();
	EditorGame::Initialize();

	//  create forms
	frm_SetName		= sx_new( Form_SetName );
	frm_Explorer	= sx_new( Form_Explorer );
	frm_Settings	= sx_new( Form_Settings );
	frm_Hint		= sx_new( Form_Hint );
	frm_Ask			= sx_new( Form_Ask );
	frm_importSound	= sx_new( Form_ImportSound );
	frm_importModel	= sx_new( Form_ImportModel );

	//  create needed view port gui
	g_panelBack	= (sx::gui::PPanel)sx::gui::Create(GUI_PANEL);
	g_panelBack->GetElement(0)->SetTextureSrc( EditorUI::GetBackgroundTexture() );

	CREATE_PANEL_EDITOR(g_panelObject,	L"Object Editor", -0.57f);
	CREATE_PANEL_EDITOR(g_panelScene,	L"Scene Editor", -0.57f);
	CREATE_PANEL_EDITOR(g_panelHUD,		L"HUD Editor [ not available ]", 0.57f);
	CREATE_PANEL_EDITOR(g_panelGame,	L"Game Test [ not available ]", 0.57f);
	
	sx::d3d::PTexture pTex = NULL;

	if ( sx::d3d::Texture::Manager::Create( pTex, NULL ) )
		g_panelObject->GetElement(0)->SetTexture( pTex );

	if ( sx::d3d::Texture::Manager::Create( pTex, NULL ) )
		g_panelScene->GetElement(0)->SetTexture( pTex );

	if ( sx::d3d::Texture::Manager::Create( pTex, NULL ) )
		g_panelHUD->GetElement(0)->SetTexture( pTex );

	if ( sx::d3d::Texture::Manager::Create( pTex, NULL ) )
		g_panelGame->GetElement(0)->SetTexture( pTex );

}

void Editor::Finalize( void )
{
	sx_callstack();

	if ( g_panelObject )
	{
		sx::d3d::PTexture pTex = g_panelObject->GetElement(0)->GetTexture();
		sx::d3d::Texture::Manager::Release( pTex );
		pTex = g_panelScene->GetElement(0)->GetTexture();
		sx::d3d::Texture::Manager::Release( pTex );
		pTex = g_panelHUD->GetElement(0)->GetTexture();
		sx::d3d::Texture::Manager::Release( pTex );
		pTex = g_panelGame->GetElement(0)->GetTexture();
		sx::d3d::Texture::Manager::Release( pTex );

		g_panelObject->GetElement(0)->SetTexture(null);
		g_panelScene->GetElement(0)->SetTexture(null);
		g_panelHUD->GetElement(0)->SetTexture(null);
		g_panelGame->GetElement(0)->SetTexture(null);
	}

	//	finalize other editors
	EditorObject::Finalize();
	EditorScene::Finalize();
	EditorHUD::Finalize();
	EditorGame::Finalize();

	sx_delete( frm_importModel );
	sx_delete( frm_importSound );
	sx_delete( frm_Ask );
	sx_delete( frm_Hint );
	sx_delete( frm_Settings );
	sx_delete( frm_Explorer );
	sx_delete( frm_SetName );

	sx::gui::Destroy( g_panelBack );
	sx::gui::Destroy( g_panelObject );
	sx::gui::Destroy( g_panelScene );
	sx::gui::Destroy( g_panelHUD );
	sx::gui::Destroy( g_panelGame );

	sx_delete( dbg_Lable );

	EditorUI::Finalize();
}

void Editor::Resize( int width, int height )
{
	if (!g_panelBack) return;

	//  resize renderer
	sx::core::Renderer::SetSize(MainWin::s_Win.GetHandle(), width, height);

	//  resize input system
	int2 ioSize(width, height);
	sx::io::Input::SendSignal(0, IST_SET_SIZE, &ioSize);

	//  resize other spaces
	EditorObject::Resize(width, height);
	EditorScene::Resize(width, height);
	EditorHUD::Resize(width, height);
	EditorGame::Resize(width, height);

	float W = (float)width;
	float H = (float)height;
	float offsetX = (float)(width  % 2) * 0.5f;
	float offsetY = (float)(height % 2) * 0.5f;

	//  resize gui bases
	g_panelBack->SetSize( float2(W*1.5f, H*1.5f) );
	g_panelObject->SetSize(	float2(W, H) );//		g_panelObject->GetChild(0)->Position().y = -height*0.5f;
	g_panelScene->SetSize( float2(W, H) );
	g_panelHUD->SetSize( float2(W, H) );
	g_panelGame->SetSize( float2(W, H) );
	g_panelObject->State_GetByIndex(0).Position = float3( offsetX, offsetY, 0.0f);
	g_panelScene->State_GetByIndex(0).Position = float3( offsetX, offsetY, 0.0f);
	g_panelHUD->State_GetByIndex(0).Position = float3( offsetX, offsetY, 0.0f);
	g_panelGame->State_GetByIndex(0).Position = float3( offsetX, offsetY, 0.0f);
	g_panelObject->State_GetByIndex(1).Position = float3( -W/4, H/4, 0.0f );
	g_panelScene->State_GetByIndex(1).Position = float3( W/4, H/4, 0.0f );
	g_panelHUD->State_GetByIndex(1).Position = float3( -W/4, -H/4, 0.0f );
	g_panelGame->State_GetByIndex(1).Position = float3( W/4, -H/4, 0.0f );

	//  resize forms which needs to be resized
	frm_Settings->OnResize( width, height );
	frm_Explorer->OnResize( width, height );
	frm_SetName->OnResize( width, height );
	
	//  status bar
	dbg_Lable->SetSize( float2( (float)width, 16.0f ) );
	dbg_Lable->Position().y = -height*0.5f + 10.0f;
	str1024 str;
	str << L"Current size : " << width << L" , " << height;
	dbg_Lable->SetText(str);

	//  settings button
	btn_Settings->Position().x = width/2 - 16.0f;

	//  recreate textures
	CreateRT();

	//  finally call rendering
	Loop_Start();

}

void Editor::OpenProject( const WCHAR* ProjectDir )
{

	sx::sys::FileManager::Project_Open(ProjectDir, FMM_ARCHIVE);

// 	ImportFontToLibrary(L"D:/SeganX/Font_Arial_09.fnt");
// 	ImportFontToLibrary(L"D:/SeganX/Font_Arial_10.fnt");
// 	ImportFontToLibrary(L"D:/SeganX/Font_Arial_11.fnt");
// 	ImportFontToLibrary(L"D:/SeganX/Font_Arial_12.fnt");
// 	ImportFontToLibrary(L"D:/SeganX/Font_Form_Caption.fnt");
// 	ImportFontToLibrary(L"D:/SeganX/Font_Arial_60.fnt");
//	ImportFontToLibrary(L"D:/SeganX/Font_Arial_40.fnt");
//	ImportFontToLibrary(L"D:/SeganX/Font_Arial_30.fnt");
//	ImportFontToLibrary(L"D:/SeganX/Font_Arial_25.fnt");

	//	TEST
	{
		str1024	tmp = sx::sys::GetAppFolder();
		tmp.MakePathStyle();
		tmp << L"Media/";
		FileInfoArray fileList;
		sx::sys::GetFilesInFolder(tmp, L"*.tga", &fileList);
		for (int i=0; i<fileList.Count(); i++)
		{
			String		 str = tmp;
			str << fileList[i].name;
			ConvertTexture(str, NULL);
		}
	}
	
	//  copy default font
 	if (!sx::sys::FileManager::File_Exist(L"Font_Default.fnt", SEGAN_PACKAGENAME_COMMON))
	{
		str1024 srcFile = sx::sys::GetAppFolder();
		srcFile.MakePathStyle();
		srcFile << L"Media/Font_Arial_09.fnt";

		String destFile;
		if ( sx::sys::FileManager::File_GetPath(L"Font_Default.fnt", SEGAN_PACKAGENAME_COMMON, destFile) )
			sx::sys::CopyFile(srcFile, destFile);
		
		srcFile = sx::sys::GetAppFolder();
		srcFile.MakePathStyle();
		srcFile << L"Media/Font_Arial_09_0.txr";

		if ( sx::sys::FileManager::File_GetPath(L"Font_Arial_09_0.txr", SEGAN_PACKAGENAME_TEXTURE, destFile) )
			sx::sys::CopyFile(srcFile, destFile);
	}
}

void Editor::CloseProject( void )
{
	sx::sys::FileManager::Project_Close();
}

void Editor::Loop( float elpsTime )
{
	if ( !sx::core::Renderer::CanRender() ) return;

	//////////////////////////////////////////////////////////////////////////
	//		some debug info
	{
		str1024 lblStr; 
		lblStr.Format(L" fps: %d , gui: %d , verts: %d , tris: %d , draws: %d , stream: %d"
			,sx::d3d::Device3D::GetFPS()
			,sx::gui::Element::NumberOfDraw()
			,sx::d3d::Device3D::GetNumberOfVertices()
			,sx::d3d::Device3D::GetNumberOfTriangles()
			,sx::d3d::Device3D::GetNumberOfDrawCalls()
			,sx::sys::TaskManager::Count());
		dbg_Lable->SetText(lblStr);
	}

	//////////////////////////////////////////////////////////////////////////
	//		UPDATE EVERY THINGS
	//////////////////////////////////////////////////////////////////////////

	//  update input system	
	bool inputHandled = false;
	sx::io::Input::Update(elpsTime);
	sx::gui::Control::GetCapturedControl() = NULL;

	//  explorer will use in other scenes so should updated here
	frm_Ask->ProcessInput(inputHandled);
	frm_SetName->ProcessInput(inputHandled);
	frm_importSound->ProcessInput(inputHandled);
	frm_Explorer->ProcessInput(inputHandled);
	frm_importModel->ProcessInput(inputHandled);
	dbg_Lable->ProcessInput(inputHandled);
	frm_Settings->ProcessInput(inputHandled);

	//  update other editors
	switch (g_CurSpace)
	{
	case EWS_OBJECT:	EditorObject::Update(elpsTime, inputHandled);	break;
	case EWS_SCENE:		EditorScene::Update(elpsTime, inputHandled);	break;
	case EWS_HUD:		EditorHUD::Update(elpsTime, inputHandled);		break;
	case EWS_GAME:		EditorGame::Update(elpsTime, inputHandled);		break;

	case EWS_NULL:
		frm_Explorer->Close();

		g_panelCurSpace = NULL;
		g_panelObject->State_SetIndex(1);
		g_panelScene->State_SetIndex(1);
		g_panelHUD->State_SetIndex(1);
		g_panelGame->State_SetIndex(1);

		g_panelObject->ProcessInput(inputHandled);
		g_panelScene->ProcessInput(inputHandled);
		g_panelHUD->ProcessInput(inputHandled);
		g_panelGame->ProcessInput(inputHandled);

		//  show tips
		static int tipsIndex = 0;
		static float tipsTime = 0;
		tipsTime += elpsTime;
		if ( tipsTime > 10000 )
		{
			tipsTime = 0;

			tipsIndex++;
			if ( tipsIndex >= g_tipsCount )
				tipsIndex = 0;
		}
		dbg_Lable->SetText( g_tipsStr[tipsIndex] );
		break;
	}
	if (g_panelCurSpace) g_panelCurSpace->ProcessInput(inputHandled);

	//////////////////////////////////////////////////////////////////////////
	//  update objects in this scene
	frm_importModel->Update(elpsTime);
	frm_importSound->Update(elpsTime);
	frm_Ask->Update(elpsTime);
	frm_Hint->Update(elpsTime);
	frm_SetName->Update(elpsTime);
	frm_Settings->Update(elpsTime);
	frm_Explorer->Update(elpsTime);

	g_panelObject->Update(elpsTime);
	g_panelScene->Update(elpsTime);
	g_panelHUD->Update(elpsTime);
	g_panelGame->Update(elpsTime);
	g_panelBack->Update(elpsTime);

	//////////////////////////////////////////////////////////////////////////
	//		RENDER EVERY THINGS
	//////////////////////////////////////////////////////////////////////////
	//  start rendering
	sx::core::Renderer::Begin();
	sx::d3d::Device3D::Clear_ZBuffer();

	// count number of gui elements
	sx::gui::Element::NumberOfDraw() = 0;

	//  render other editors
	switch (g_CurSpace)
	{
	case EWS_OBJECT:
		sx::core::Renderer::SetRenderTarget( g_panelObject->GetElement(0)->GetTexture() );
		EditorObject::Render(elpsTime);
		sx::core::Renderer::SetRenderTarget(NULL);		
		break;

	case EWS_SCENE:
		sx::core::Renderer::SetRenderTarget( g_panelScene->GetElement(0)->GetTexture() );
		EditorScene::Render(elpsTime);
		sx::core::Renderer::SetRenderTarget(NULL);
		break;

	case EWS_HUD:
		sx::core::Renderer::SetRenderTarget( g_panelHUD->GetElement(0)->GetTexture() );
		EditorHUD::Render(elpsTime);
		sx::core::Renderer::SetRenderTarget(NULL);
		break;

	case EWS_GAME:
		sx::core::Renderer::SetRenderTarget( g_panelGame->GetElement(0)->GetTexture() );
		EditorGame::Render(elpsTime);
		sx::core::Renderer::SetRenderTarget(NULL);
		break;

	case EWS_NULL:
		{
			bool inputHndl = true;
			static int scener = -150;
			switch (scener)
			{
			case 15:
				//EditorObject::Update(0, inputHndl);
				sx::core::Renderer::SetRenderTarget( g_panelObject->GetElement(0)->GetTexture() );
				EditorObject::Render(0);
				sx::core::Renderer::SetRenderTarget(NULL);
				break;
		
			case 30:
				//EditorScene::Update(0, inputHndl);
				sx::core::Renderer::SetRenderTarget( g_panelScene->GetElement(0)->GetTexture() );
				EditorScene::Render(0);
				sx::core::Renderer::SetRenderTarget(NULL);
				break;

			case 45:
				//EditorGame::Update(0, inputHndl);
				sx::core::Renderer::SetRenderTarget( g_panelHUD->GetElement(0)->GetTexture() );
				EditorHUD::Render(0);
				sx::core::Renderer::SetRenderTarget(NULL);
				break;

			case 60:
				//EditorGame::Update(0, inputHndl);
				sx::core::Renderer::SetRenderTarget( g_panelGame->GetElement(0)->GetTexture() );
				EditorGame::Render(0);
				sx::core::Renderer::SetRenderTarget(NULL);
				break;
			}
			scener++;
			if ( scener > 60 ) scener=0;
		}
		break;
	}

	//  render current scene
	sx::d3d::Device3D::Viewport_SetDefault();
	sx::d3d::UI3D::ReadyToDebug(0xffffffff);

	if (g_CurSpace == EWS_NULL)
	{
		float2 cursorPos ( SEGAN_VP_WIDTH/2 - SEGAN_MOUSE_ABSX(0), SEGAN_MOUSE_ABSY(0) - SEGAN_VP_WIDTH/2 );
		g_panelBack->Position().x += (cursorPos.x/5 - g_panelBack->Position().x)*0.01f;
		g_panelBack->Position().y += (cursorPos.y/5 - g_panelBack->Position().y)*0.01f;
		g_panelBack->Draw(0);

		
		g_panelObject->Draw(0);
		g_panelScene->Draw(0);
		g_panelHUD->Draw(0);
		g_panelGame->Draw(0);
	}
	if ( g_panelCurSpace )	g_panelCurSpace->Draw(0);

	//  draw forms
	frm_Settings->Draw(0);
	frm_importModel->Draw(0);
	frm_Explorer->Draw(0);
	frm_importSound->Draw(0);
	frm_SetName->Draw(0);
	frm_Ask->Draw(0);
	frm_Hint->Draw(0);

	//frm_Hint->Show();
	//frm_Settings->Show();

	RenderViewport(elpsTime);

	//////////////////////////////////////////////////////////////////////////
	//  end rendering
	sx::core::Renderer::End();
}


void Editor::RenderViewport( float elpsTime )
{
	//  TEST
	if (g_CurSpace != EWS_NULL && g_tipsTime>0)
	{
		g_tipsTime -= elpsTime;
		str1024 str = dbg_Lable->GetText();
		str << L"    !!    " << g_strTips;
		dbg_Lable->SetText(str);
	}
	dbg_Lable->Update(elpsTime);
	dbg_Lable->Draw(0);
}

void Editor::RenderCompass( float elpsTime )
{
	//  compute compass matrix
	Matrix matView, matProj, compassView;
	sx::d3d::Device3D::Matrix_View_Get(matView);
	sx::d3d::Device3D::Matrix_Project_Get(matProj);

	compassView = matView;
	compassView._41 =  (SEGAN_VP_WIDTH/2.0f - 40.0f);
	compassView._42 = -(SEGAN_VP_HEIGHT/2.0f - 40.0f);

	//  draw compass
	Matrix compassWorld;
	compassWorld.Scale(20, 20, 20);
	sx::d3d::Device3D::Matrix_World_Set(compassWorld);
	sx::d3d::Device3D::Matrix_View_Set(compassView);
	sx::d3d::UI3D::DrawCompass();

	sx::d3d::Device3D::Matrix_View_Set(matView);
	sx::d3d::Device3D::Matrix_Project_Set(matProj);
}

void Editor::SetSpace( EditorWorkingSpace wSpace )
{
	//  operate on old space
	switch ( g_CurSpace )
	{
	case EWS_SCENE:
		{
			sx::core::ArrayPNode nodeList;
			sx::core::Scene::GetAllNodes( nodeList );
			for (int i=0; i<nodeList.Count(); i++)
			{
				msg_SoundStop msgSound(true);
				nodeList[i]->MsgProc( MT_SOUND_STOP, &msgSound );
			}
		}
		break;

	case EWS_OBJECT:
		{
			if ( EditorObject::node_Root )
			{
				msg_SoundStop msgSound(true);
				EditorObject::node_Root->MsgProc( MT_SOUND_STOP, &msgSound );
			}
		}
		break;
	}

	//  set new space
	g_CurSpace = wSpace;

	//  operate on new space
	switch ( g_CurSpace )
	{
	case EWS_SCENE:
		{
			sx::core::ArrayPNode nodeList;
			sx::core::Scene::GetAllNodes( nodeList );
			for (int i=0; i<nodeList.Count(); i++)
			{
				msg_SoundPlay msgSound(false, 0, SX_SOUND_PLAYONLOAD);
				nodeList[i]->MsgProc( MT_SOUND_PLAY, &msgSound );
			}
		}
		break;

	case EWS_OBJECT:
		{
			if ( EditorObject::node_Root )
			{
				msg_SoundPlay msgSound(false, 0, SX_SOUND_PLAYONLOAD);
				EditorObject::node_Root->MsgProc( MT_SOUND_PLAY, &msgSound );
			}
		}
		break;
	}

}

EditorWorkingSpace Editor::GetSpace( void )
{
	return g_CurSpace;
}

void Editor::SetLabelTips( const WCHAR* strTips, const float tipTime )
{
	g_strTips = strTips;
	g_tipsTime = tipTime;

	if ( !strTips || !g_client ) return;

	char msg[512] = {0};

	int i = 0;
	wchar* c = (wchar*)strTips;
	while ( *c && i<511 )
	{
		msg[i] = (char)(*c);
		c++;
		i++;
	}
	msg[i++]=0;

 	g_client->Send( msg, i, false );
 	g_client->Update( 0, NET_DELAY_TIME, NET_TIMEOUT );
}

