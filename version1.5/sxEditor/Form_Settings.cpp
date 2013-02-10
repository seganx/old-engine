#include "Form_Settings.h"
#include "sxItemUI.h"
#include "MainEditor.h"
#include "EditorUI.h"
#include "EditorSettings.h"
#include "EditorScene.h"

#define FORM_SETTINGS_WIDTH		500.0f		//  size of the form
#define FORM_SETTINGS_HEIGHT	500.0f		//  size of the form


bool	applySettings = false;

Form_Settings::Form_Settings( void ): BaseForm()
{
	sx::gui::PLabel lbl = NULL;

	//  title
	m_pTitle	= sx_new( sx::gui::Label );
	m_pTitle->SetParent(m_pBack);
	m_pTitle->SetAlign(GTA_CENTER);
	m_pTitle->SetFont( EditorUI::GetFormCaptionFont() );
	m_pTitle->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	m_pTitle->GetElement(1)->Color() = EditorUI::GetFormCaptionFontColor();
	m_pTitle->SetText(L"Preferences");


	//  sun light panel
	m_pSunLightPanel = sx_new( sx::gui::Panel );
	m_pSunLightPanel->SetParent(m_pBack);
	m_pSunLightPanel->GetElement(0)->Color() = EditorUI::GetListBackColor();
	m_pSunLightPanel->SetSize( float2(200.0f, 200.0f) );
	m_pSunLightPanel->Position().Set( FORM_SETTINGS_WIDTH/2 - 110.0f, FORM_SETTINGS_HEIGHT/2 - 120.0f, 0.0f );
	lbl = EditorUI::CreateLabel(m_pSunLightPanel, 190.0f, 8, L"Sun light :");
	lbl->Position().y = 100.0f;
	lbl = EditorUI::CreateLabel(m_pSunLightPanel, 190.0f, 8, L"Theta :");
	m_pSunTheta = EditorUI::CreateTrackbar(m_pSunLightPanel, 190.0f, -sx::math::PIDIV2+0.01f, sx::math::PIDIV2-0.01f);
	m_pSunTheta->SetValue( sx::core::Settings::GetSunLightPosition().x );
	lbl->Position().y = 75.0f;
	m_pSunTheta->Position().y = 60.0f;
	lbl = EditorUI::CreateLabel(m_pSunLightPanel, 190.0f, 8, L"Phi :");
	m_pSunPhi = EditorUI::CreateTrackbar(m_pSunLightPanel, 190.0f, 0.0f, sx::math::PIMUL2);
	m_pSunPhi->SetValue( sx::core::Settings::GetSunLightPosition().y );
	lbl->Position().y = 40.0f;
	m_pSunPhi->Position().y = 25.0f;

	//  shadow
	lbl = EditorUI::CreateLabel(m_pSunLightPanel, 190.0f, 8, L"Shadow size :");
	m_pShadowSize = sx_new( sx::gui::ListBox );
	m_pShadowSize->SetParent( m_pSunLightPanel );
	m_pShadowSize->SetFont( EditorUI::GetDefaultFont(8) );
	m_pShadowSize->Add( L"High : 2048 x 2048", NULL );
	m_pShadowSize->Add( L"Midd : 1024 x 1024", NULL );
	m_pShadowSize->Add( L"Low : 512 x 512", NULL );
	m_pShadowSize->Add( L"Very low : 256 x 256", NULL );
	m_pShadowSize->Add( L"OFF", NULL );
	m_pShadowSize->SetSize(190, 75.0f, 18.0f, false);
	m_pShadowSize->SetItemIndex( sx::core::Settings::GetOption_Shadow()->GetLevel() );
	m_pShadowSize->SetOnSelect( this, (GUICallbackEvent)&Form_Settings::OnShadowSizeChange );
	lbl->Position().y = -5.0f;
	m_pShadowSize->Position().y = -50.0f;

	//  reflection shader quality
	sx::gui::PPanel panel = sx_new( sx::gui::Panel );
	panel->SetParent(m_pBack);
	panel->GetElement(0)->Color() = EditorUI::GetListBackColor();
	panel->SetSize( float2(200.0f, 220.0f) );
	panel->Position().Set( -FORM_SETTINGS_WIDTH/2 + 110.0f, - FORM_SETTINGS_HEIGHT/2 + 120.0f, 0.0f );

	lbl = EditorUI::CreateLabel(panel, 190.0f, 8, L"Shader Quality :");
	m_pShaderQuality = sx_new( sx::gui::ListBox );
	m_pShaderQuality->SetParent( panel );
	m_pShaderQuality->SetFont( EditorUI::GetDefaultFont(8) );
	m_pShaderQuality->Add( L"High", NULL );
	m_pShaderQuality->Add( L"Medium", NULL );
	m_pShaderQuality->Add( L"Low", NULL );
	m_pShaderQuality->SetSize(190, 60.0f, 18.0f, false);
	m_pShaderQuality->SetItemIndex( sx::d3d::Shader::Manager::GetShaderQuality() );
	m_pShaderQuality->SetOnSelect( this, (GUICallbackEvent)&Form_Settings::OnShaderQualityChange );
	m_pShaderQuality->Position().y = 60.0f;
	lbl->Position().y = 100.0f;

	lbl = EditorUI::CreateLabel(panel, 190.0f, 8, L"Reflection Quality :");
	m_reflectQuality = sx_new( sx::gui::ListBox );
	m_reflectQuality->SetParent( panel );
	m_reflectQuality->SetFont( EditorUI::GetDefaultFont(8) );
	m_reflectQuality->Add( L"Very High", NULL );
	m_reflectQuality->Add( L"High", NULL );
	m_reflectQuality->Add( L"Medium", NULL );
	m_reflectQuality->Add( L"Low", NULL );
	m_reflectQuality->Add( L"OFF", NULL );
	m_reflectQuality->SetSize(190, 90.0f, 18.0f, false);
	m_reflectQuality->SetItemIndex( sx::core::Settings::GetOption_Reflection()->GetLevel() );
	m_reflectQuality->SetOnSelect( this, (GUICallbackEvent)&Form_Settings::OnShadowSizeChange );
	m_reflectQuality->Position().y = -50.0f;
	lbl->Position().y = 5.0f;

	//  texture LOD
	panel = sx_new( sx::gui::Panel );
	panel->SetParent(m_pBack);
	panel->GetElement(0)->Color() = EditorUI::GetListBackColor();
	panel->SetSize( float2(200.0f, 110.0f) );
	panel->Position().Set( FORM_SETTINGS_WIDTH/2 - 110.0f, - FORM_SETTINGS_HEIGHT/2 + 120.0f, 0.0f );
	lbl = EditorUI::CreateLabel(panel, 190.0f, 8, L"Texture LOD :");
	m_pTextureLOD = sx_new( sx::gui::ListBox );
	m_pTextureLOD->SetParent( panel );
	m_pTextureLOD->SetFont( EditorUI::GetDefaultFont(8) );
	m_pTextureLOD->Add( L"0 : High", NULL );
	m_pTextureLOD->Add( L"1", NULL );
	m_pTextureLOD->Add( L"2", NULL );
	m_pTextureLOD->Add( L"3", NULL );
	m_pTextureLOD->Add( L"4", NULL );
	m_pTextureLOD->Add( L"5", NULL );
	m_pTextureLOD->Add( L"6", NULL );
	m_pTextureLOD->Add( L"7", NULL );
	m_pTextureLOD->Add( L"8", NULL );
	m_pTextureLOD->Add( L"9", NULL );
	m_pTextureLOD->Add( L"10", NULL );
	m_pTextureLOD->Add( L"11", NULL );
	m_pTextureLOD->Add( L"12", NULL );
	m_pTextureLOD->SetSize(190, 75.0f, 18.0f, false);
	m_pTextureLOD->SetItemIndex( sx::core::Settings::GetOption_TextureLevel()->GetLevel() );
	m_pTextureLOD->SetOnSelect( this, (GUICallbackEvent)&Form_Settings::OnTextureLODChange );
	lbl->Position().y = 40.0f;
	m_pTextureLOD->Position().y = -10.0f;


	//  set up fog panel
	panel = sx_new( sx::gui::Panel );
	panel->SetParent(m_pBack);
	panel->GetElement(0)->Color() = EditorUI::GetListBackColor();
	panel->SetSize( float2(200.0f, 200.0f) );
	panel->Position().Set( -FORM_SETTINGS_WIDTH/2 + 110.0f, FORM_SETTINGS_HEIGHT/2 - 120.0f, 0.0f );

	float top = 100.0f;
	lbl = EditorUI::CreateLabel(panel, 190.0f, 8, L"Fog :");
	lbl->Position().y = top; top -= 20.0f;

	lbl = EditorUI::CreateLabel(panel, 190.0f, 8, L"Color :");
	lbl->Position().y = top; top -= 20.0f;

	m_fog_color = sx_new( sx::gui::Panel );
	m_fog_color->SetParent(panel);
	m_fog_color->GetElement(0)->Color() = D3DColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_fog_color->SetSize( float2(100.0f, 40.0f) );
	m_fog_color->Position().Set( 40.0f, top+15.0f, 0.0f );
	m_fog_color->AddProperty(SX_GUI_PROPERTY_ACTIVATE);
	SEGAN_GUI_SET_ONCLICK(m_fog_color, Form_Settings::OnColorClick);
	top -= 10.0f;

	lbl = EditorUI::CreateLabel(panel, 190.0f, 8, L"Fog mode :");
	lbl->Position().y = top; top -= 45.0f;
	m_fog_mode = sx_new( sx::gui::ListBox );
	m_fog_mode->SetParent( panel );
	m_fog_mode->SetFont( EditorUI::GetDefaultFont(8) );
	m_fog_mode->Add( L"OFF", NULL );
	m_fog_mode->Add( L"Linear", NULL );
	m_fog_mode->Add( L"Exponential", NULL );
	m_fog_mode->Add( L"Exponentially (dis)^2", NULL );
	m_fog_mode->SetSize(190, 75.0f, 18.0f, false);
	m_fog_mode->SetItemIndex( 0 );
	m_fog_mode->SetOnSelect( this, (GUICallbackEvent)&Form_Settings::OnFogChange );
	m_fog_mode->Position().y = top; top -= 60.0f;

	m_fog_dens = EditorUI::CreateTrackbar(panel, 190.0f, 0.0f, 0.1f);
	m_fog_dens->SetValue( 0 );
	SEGAN_GUI_SET_ONSCROLL(m_fog_dens, Form_Settings::OnFogChange);
	m_fog_dens->Position().y = top-10.0f;
	m_fog_dens->RemProperty(SX_GUI_PROPERTY_VISIBLE);
	lbl = EditorUI::CreateLabel(m_fog_dens, 190.0f, 8, L"Density :");
	lbl->Position().y = 10.0f;

	m_fog_start = EditorUI::CreateTrackbar(panel, 190.0f, 0.0f, 1000.0f);
	m_fog_start->SetValue( 0 );
	SEGAN_GUI_SET_ONSCROLL(m_fog_start, Form_Settings::OnFogChange);
	m_fog_start->Position().y = top; top -= 35.0f;
	m_fog_start->RemProperty(SX_GUI_PROPERTY_VISIBLE);
	lbl = EditorUI::CreateLabel(m_fog_start, 190.0f, 8, L"Start :");
	lbl->Position().y = 10.0f;

	m_fog_end = EditorUI::CreateTrackbar(panel, 190.0f, 0.0f, 1000.0f);
	m_fog_end->SetValue( 1000 );
	SEGAN_GUI_SET_ONSCROLL(m_fog_end, Form_Settings::OnFogChange);
	m_fog_end->Position().y = top; top -= 35.0f;
	m_fog_end->RemProperty(SX_GUI_PROPERTY_VISIBLE);
	lbl = EditorUI::CreateLabel(m_fog_end, 190.0f, 8, L"End :");
	lbl->Position().y = 10.0f;


	//  create button for clear project 
	m_CleanProject = EditorUI::CreateButton( m_pBack, 128, L"Clear Project" );
	m_CleanProject->Position().Set( 100, -50, 0 );
	SEGAN_GUI_SET_ONCLICK( m_CleanProject, Form_Settings::OnButtonClick );


	//  stop moving or resizing
	SEGAN_SET_REM(m_Option, FORM_OPTION_RESIZABLE);
	//SEGAN_SET_REM(m_Option, FORM_OPTION_MOVABLE);

	SetSize( FORM_SETTINGS_WIDTH, FORM_SETTINGS_HEIGHT);

	m_pBack->State_GetByIndex(0).Position.Set(0,0,0);
	m_pBack->State_GetByIndex(1).Position.Set(0,0,0);

}

Form_Settings::~Form_Settings( void )
{
	sx_delete( m_pShadowSize );
	sx_delete( m_pTextureLOD );
	sx_delete( m_fog_mode );
	sx_delete( m_pShaderQuality );
	sx_delete( m_reflectQuality );
	//  for other things the parent will do everything :)
}

void Form_Settings::SetSize( float width, float height )
{
	float top = height/2;
	float left = -width/2;

	m_pTitle->SetSize( float2(width - 70.0f, 40.0f) );
	m_pTitle->Position().y = top - 10.0f;


	BaseForm::SetSize( width, height );
	OnFogChange(NULL);
}

void Form_Settings::ProcessInput( bool& inputHandled )
{
	if (inputHandled) return;

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

	BaseForm::ProcessInput(inputHandled);
}

void Form_Settings::Update( float elpsTime )
{
	if ( ! IsVisible() )
	{
		BaseForm::Update(elpsTime);
		return;
	}

	m_pShadowSize->Update(elpsTime);
	m_pTextureLOD->Update(elpsTime);
	m_pShaderQuality->Update(elpsTime);
	m_reflectQuality->Update(elpsTime);

	BaseForm::Update(elpsTime);

	//  update position states
	m_pBack->State_GetByIndex(0).Position = m_pBack->State_GetByIndex(1).Position;

	//  update light position
	sx::core::Settings::SetSunLightPosition( m_pSunTheta->GetBlendingValue(), m_pSunPhi->GetBlendingValue() );

	m_ColorBox.Update(elpsTime);
	OnFogChange(NULL);
}

void Form_Settings::OnResize( int EditorWidth, int EditorHeight )
{
	//
	//m_pBack->State_GetByIndex(1).Position.Set( 0.0f, 0.0f, 0.0f );
}

void Form_Settings::OnColorClick( sx::gui::PControl Sender )
{
	m_ColorBox.GetBack()->SetParent( m_fog_color );
	m_ColorBox.Show(0, 0, &m_fog_color->GetElement(0)->Color() );
}

void Form_Settings::OnShadowSizeChange( sx::gui::PControl Sender )
{
	int index = m_pShadowSize->GetItemIndex();
	if (index>-1)
	{
		sx::core::Settings::GetOption_Shadow()->SetLevel( index );
		if ( index > 3 )
			sx::core::Settings::GetOption_Shadow()->RemPermission( OPT_BY_ARTIST );
		else
			sx::core::Settings::GetOption_Shadow()->AddPermission( OPT_BY_ARTIST );
	}

	index = m_reflectQuality->GetItemIndex();
	if ( index > -1 )
	{
		sx::core::Settings::GetOption_Reflection()->SetLevel( index );
		if ( index > 3 )
			sx::core::Settings::GetOption_Reflection()->RemPermission( OPT_BY_ARTIST );
		else
			sx::core::Settings::GetOption_Reflection()->AddPermission( OPT_BY_ARTIST );
	}
}

void Form_Settings::OnTextureLODChange( sx::gui::PControl Sender )
{
	int index = m_pTextureLOD->GetItemIndex();
	if (index>-1)
	{
		sx::core::Settings::GetOption_TextureLevel()->SetLevel( index );
		sx::d3d::Texture::Manager::SetLOD( index );
	}
}

void Form_Settings::OnShaderQualityChange( sx::gui::PControl Sender )
{
	switch ( m_pShaderQuality->GetItemIndex() )
	{
	case 0:	sx::d3d::Shader::Manager::SetShaderQuality( SQ_HIGH );		break;
	case 1:	sx::d3d::Shader::Manager::SetShaderQuality( SQ_MEDIUM );	break;
	case 2:	sx::d3d::Shader::Manager::SetShaderQuality( SQ_LOW );		break;
	}
}


void Form_Settings::OnFogChange( sx::gui::PControl Sender )
{
	if ( !applySettings ) return;

	int index = m_fog_mode->GetItemIndex();
	if ( index<0 ) return;

	switch (index)
	{
	case 1:
		{
			m_fog_dens->RemProperty(SX_GUI_PROPERTY_VISIBLE);
			m_fog_start->AddProperty(SX_GUI_PROPERTY_VISIBLE);
			m_fog_end->AddProperty(SX_GUI_PROPERTY_VISIBLE);
		}
		break;

	case 2:
	case 3:
		{
			m_fog_dens->AddProperty(SX_GUI_PROPERTY_VISIBLE);
			m_fog_start->RemProperty(SX_GUI_PROPERTY_VISIBLE);
			m_fog_end->RemProperty(SX_GUI_PROPERTY_VISIBLE);
		}
		break;

	default:
		{
			m_fog_dens->RemProperty(SX_GUI_PROPERTY_VISIBLE);
			m_fog_start->RemProperty(SX_GUI_PROPERTY_VISIBLE);
			m_fog_end->RemProperty(SX_GUI_PROPERTY_VISIBLE);
		}
		break;

	}

	FogDesc fog;
	sx::d3d::Device3D::GetFogDesc(fog);
	fog.Color = m_fog_color->GetElement(0)->Color();
	switch (index)
	{
	case 0: fog.Mode = D3DFOG_NONE;		break;
	case 1: fog.Mode = D3DFOG_LINEAR;	break;
	case 2: fog.Mode = D3DFOG_EXP;		break;
	case 3: fog.Mode = D3DFOG_EXP2;		break;
	}
	fog.Density = m_fog_dens->GetValue();
	fog.Start = m_fog_start->GetValue();
	fog.End = m_fog_end->GetValue();
	sx::d3d::Device3D::SetFogDesc(fog);
}

void Form_Settings::Show( void )
{
	applySettings = false;

	FogDesc fog;
	sx::d3d::Device3D::GetFogDesc(fog);
	switch (fog.Mode)
	{
	case D3DFOG_NONE:	m_fog_mode->SetItemIndex( 0 );		break;
	case D3DFOG_LINEAR:	m_fog_mode->SetItemIndex( 1 );		break;
	case D3DFOG_EXP:	m_fog_mode->SetItemIndex( 2 );		break;
	case D3DFOG_EXP2:	m_fog_mode->SetItemIndex( 3 );		break;
	}
	m_fog_dens->SetValue(	fog.Density );
	m_fog_start->SetValue(	fog.Start	);
	m_fog_end->SetValue(	fog.End		);
	m_fog_color->GetElement(0)->Color() = fog.Color;

	m_pSunTheta->SetValue( sx::core::Settings::GetSunLightPosition().x );
	m_pSunPhi->SetValue( sx::core::Settings::GetSunLightPosition().y );

	if ( !sx::core::Settings::GetOption_Reflection()->HasPermission() )
		m_reflectQuality->SetItemIndex( 4 );

	if ( !sx::core::Settings::GetOption_Shadow()->HasPermission() )
		m_pShadowSize->SetItemIndex( 4 );

	applySettings = true;

	BaseForm::Show();
}

void Form_Settings::OnButtonClick( sx::gui::PControl Sender )
{
	if ( Sender == m_CleanProject && sx::sys::FileManager::Project_GetDir() )
	{
		// clear the scene
		EditorScene::frm_Toolbar->OnButtonClick( EditorScene::frm_Toolbar->m_btnNew );
		sx::core::Renderer::GetCamera()->Eye.Set( 9999999, 0, 9999999 );

		sxLog::Log(L"Start to Project Cleaning!\r\nlisting scene files ...\r\n");
		FileInfoArray files;
		sx::sys::GetFilesInFolderX( sx::sys::FileManager::Project_GetDir(), L"scene", &files );
		sx::sys::GetFilesInFolderX( sx::sys::FileManager::Project_GetDir(), L"node", &files );
		for (int i=0; i<files.Count(); i++)
		{
			str1024 fileName = sx::sys::FileManager::Project_GetDir();
			fileName << files[i].name;
			
			str64 exten = files[i].type;
			if ( exten == L"scene" )
			{
				sx::sys::FileStream file;
				if ( file.Open(fileName, FM_OPEN_READ | FM_SHARE_READ) )
				{
					int version = 0;
					SEGAN_STREAM_READ(file, version);

					if ( version == 1 )
					{
						sx::core::Scene::LoadNodes( file );
					}
					else
					{
						file.SetPos(0);
						sx::core::Scene::LoadNodes( file );
					}

					file.Close();
				}
			}
			else if ( exten == L"node" )
			{
				sx::core::PNode node = sx_new( sx::core::Node );
				sx::sys::FileStream file;
				file.Open(fileName, FM_OPEN_READ | FM_SHARE_READ);
				node->Load(file);
				file.Close();

				sx::core::Scene::AddNode( node );
			}
		}

		//  clear animations
		{
			files.Clear();
			sx::sys::GetFilesInFolderX( sx::sys::FileManager::Project_GetDir(), L"anim", &files );

			//  extract exist animations to avoid deleting
			for (Map<UINT, sx::d3d::PAnimation>::Iterator& it = sx::d3d::Animation::Manager::GetFirst(); !it.IsLast(); it++)
			{
				for (int i=0; i<files.Count(); i++)
				{
					str256 tmp = files[i].name;
					tmp.ExtractFileName();
					if ( tmp == (*it)->GetSource() )
					{
						str1024 fileName = sx::sys::FileManager::Project_GetDir();
						fileName << files[i].name;
						files.RemoveByIndex(i);
						break;
					}
				}
			}

			//  remove remains animations
			for (int i=0; i<files.Count(); i++)
			{
				str1024 fileName = sx::sys::FileManager::Project_GetDir();
				fileName << files[i].name;
				sx::sys::RemoveFile( fileName );
				sxLog::Log( L"Remove [ %s ]", files[i].name );
			}
		}

		//  clear geometries
		{
			files.Clear();
			sx::sys::GetFilesInFolderX( sx::sys::FileManager::Project_GetDir(), L"gmt", &files );

			//  extract exist geometries to avoid deleting
			for (Map<UINT, sx::d3d::PGeometry>::Iterator& it = sx::d3d::Geometry::Manager::GetFirst(); !it.IsLast(); it++)
			{
				for (int i=0; i<files.Count(); i++)
				{
					str256 tmp = files[i].name;
					tmp.ExtractFileName();
					if ( tmp == (*it)->GetSource() )
					{
						str1024 fileName = sx::sys::FileManager::Project_GetDir();
						fileName << files[i].name;
						files.RemoveByIndex(i);
						break;
					}
				}
			}

			//  remove remains geometries
			for (int i=0; i<files.Count(); i++)
			{
				str1024 fileName = sx::sys::FileManager::Project_GetDir();
				fileName << files[i].name;
				sx::sys::RemoveFile( fileName );
				sxLog::Log( L"Remove [ %s ]", files[i].name );
			}
		}

		//  clear textures
		{
			files.Clear();
			sx::sys::GetFilesInFolderX( sx::sys::FileManager::Project_GetDir(), L"txr", &files );

			//  extract exist textures to avoid deleting
			for (Map<UINT, sx::d3d::PTexture>::Iterator& it = sx::d3d::Texture::Manager::GetFirst(); !it.IsLast(); it++)
			{
				for (int i=0; i<files.Count(); i++)
				{
					str256 tmp = files[i].name;
					tmp.ExtractFileName();
					if ( tmp == (*it)->GetSource() )
					{
						str1024 fileName = sx::sys::FileManager::Project_GetDir();
						fileName << files[i].name;
						files.RemoveByIndex(i);
						break;
					}
				}
			}

			//  remove remains textures
			for (int i=0; i<files.Count(); i++)
			{
				FileInfo* pfileInfo = &files[i];

				str256 strName = pfileInfo->name;
				strName.ExtractFileName();
				strName.MakeLower();
				const WCHAR* wc = strName.Text();
				if (
					strName.Length() > 4 &&
					wc[0] == 'g' && 
					wc[1] == 'u' && 
					wc[2] == 'i' && 
					wc[3] == '_' 
					)
					continue;
				if (
					strName.Length() > 5 &&
					wc[0] == 'f' && 
					wc[1] == 'o' && 
					wc[2] == 'n' && 
					wc[3] == 't' && 
					wc[4] == '_' 
					)
					continue;

				str1024 fileName = sx::sys::FileManager::Project_GetDir();
				fileName << pfileInfo->name;
				sx::sys::RemoveFile( fileName );
				sxLog::Log( L"Remove [ %s ]", pfileInfo->name );
			}
		}

		sxLog::Log(L"\r\nProject cleaned successfully. \r\n");

		//  reset any things
		EditorScene::frm_Toolbar->OnButtonClick( EditorScene::frm_Toolbar->m_btnNew );

	}
}

