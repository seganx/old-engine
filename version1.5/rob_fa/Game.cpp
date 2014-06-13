#include "Game.h"
#include "Player.h"
#include "GameConfig.h"
#include "GameUtils.h"
#include "GamePlay.h"
#include "GameGUI.h"
#include "EntityManager.h"
#include "ProjectileManager.h"
#include "ComponentManager.h"
#include "Scripter.h"
#include "Entity.h"
#include "GameStrings.h"

//  singleton pointer
extern Game*			g_game = NULL;
static UINT				s_idCounter = 0;

//  some internal resources
static sx::d3d::PTexture	texture_scene;
static sx::d3d::PTexture	texture_post;
static sx::d3d::Shader3D	shader_present;

//  some internal variables
static float gameTime = 0;


#if USE_STEAM_SDK

const char* s_SteamStatName[] = 
{
	"Constructions",
	"Slaughtered",
	"Destroyed",
	"Gaining",
	"Score",
	"Efficiency_0",
	"Efficiency_1",
	"Efficiency_2",
	"Efficiency_3",
	"Efficiency_4",
	"Efficiency_5",
	"Efficiency_6",
	"Efficiency_7",
	"Efficiency_8",
	"Efficiency_9"
};

const char* s_SteamAchievementsName[Achievement_Count] = 
{
	"Ach_First_Blood",
	"Ach_Slayer",
	"Ach_Blood_lust",
	"Ach_Terminator",
	"Ach_Constructor",
	"Ach_Engineer",
	"Ach_Architect",
	"Ach_Tower_Dealer",
	"Ach_Real_Estate",
	"Ach_Death_Trap",
	"Ach_Apocalypto",
	"Ach_Wrath_Of_Battle",
	"Ach_Agile_Warrior",
	"Ach_Perfect_Battle",
	"Ach_Sniper",
	"Ach_Clever_Dealer",
	"Ach_Redeemer",
	"Ach_Normality",
	"Ach_Dignified_Killer",
	"Ach_Deadly_Serious",
	"Ach_Fisherman",
	"Ach_Stalwart",
	"Ach_Star_Collector",
	"Ach_Director",
	"Ach_Gamepa"
};

#endif

void Draw_Loading( int count, int index, const WCHAR* state, const WCHAR* name )
{
	using namespace sx::gui;

	{
		MSG msg;
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	Game* game = g_game;

	PPanel panel = (PPanel)game->m_panel_Loading->GetChild(0);
	panel->Rotation().z -= 0.05f;

	panel = (PPanel)game->m_panel_Loading->GetChild(1);
	panel->Rotation().z += 0.07f;

	panel = (PPanel)game->m_panel_Loading->GetChild(2);
	panel->Rotation().z -= 0.1f;

// 	panel = (PPanel)game->m_panel_Loading->GetChild(3);
// 	panel->Rotation().z += 0.15f;

	PLabel label = (PLabel)game->m_panel_Loading->GetChild(4);
	
	if ( state && 0 )
		label->SetText( state );
	else
		label->SetText( NULL );

	label = (PLabel)game->m_panel_Loading->GetChild(5);
	if ( name && 0 )
	{
		//str1024 str;
		//str << L"< " << name << L" >";
		label->SetText( name );
	}
	else label->SetText( NULL );

	for ( int i=0; i<10; i++ )
		game->Update( 16 );

	game->Render( 1 );

	Sleep(5);
}

Game::Game( void )
:	m_gamePlay(0)
,	m_strings(0)
,	m_player(0)
,	m_gui(0)
,	m_mouseMode(MS_Null)
,	m_difficultyValue(1.0f)
,	m_difficultyLevel(0)
,	m_game_currentLevel(-1)
,	m_game_nextLevel(0)
,	m_game_restarting(0)
,	m_game_paused(false)
,	m_app_Paused(false)
,	m_app_Loading(0)
,	m_app_Closing(false)
,	m_panel_Cursor(NULL)
,	m_panel_Loading(NULL)
,	m_miniGame(false)
{
	// set cursor to center
	SEGAN_MOUSE_ABSX(0) = Config::GetData()->display_Size.x / 2.0f;
	SEGAN_MOUSE_ABSY(0) = Config::GetData()->display_Size.y / 2.0f;

	//  create target texture and presenter shader
	shader_present.CompileShader( sx::core::ShaderStore::GetPost_Present( L"return float4( tex2D( samp0, tex0 ).rgb, 1.0f );" ), SQ_HIGH, L"scene present");

	sx::d3d::Texture::Manager::Create(texture_scene, NULL);
	texture_scene->CreateRenderTarget(
		D3DRTYPE_TEXTURE,
		Config::GetData()->display_Size.x,
		Config::GetData()->display_Size.y,
		D3DFMT_A8R8G8B8);

	sx::d3d::Texture::Manager::Create( texture_post, NULL );

	//  create and load cursor
	m_panel_Cursor = sx_new( sx::gui::Panel );
	m_panel_Cursor->SetSize( float2(64, 64) );
	m_panel_Cursor->GetElement(0)->SetTextureSrc(L"gui_Cursor_default.txr");

	//  create a panel for display loading
	m_panel_Loading = sx_new( sx::gui::PanelEx );
	m_panel_Loading->SetSize( float2( 1024, 1024 ) );

	float2 dispSize( (float)Config::GetData()->display_Size.x, (float)Config::GetData()->display_Size.y );
	float3 ringPos( - dispSize.x * 0.43f, - dispSize.y * 0.32f , 0 );
	float2 ringSize( dispSize.x * 0.1f, dispSize.x * 0.1f );
	sx::gui::PPanel panel = sx_new( sx::gui::Panel );
	panel->GetElement(0)->SetTextureSrc( L"gui_Loading_Ring04.txr" );
	panel->SetSize( ringSize );
	panel->SetParent( m_panel_Loading );
	panel->Position() = ringPos;

	panel = sx_new( sx::gui::Panel );
	panel->GetElement(0)->SetTextureSrc( L"gui_Loading_Ring03.txr" );
	panel->SetSize( ringSize );
	panel->SetParent( m_panel_Loading );
	panel->Position() = ringPos;

	panel = sx_new( sx::gui::Panel );
	panel->GetElement(0)->SetTextureSrc( L"gui_Loading_Ring02.txr" );
	panel->SetSize( ringSize );
	panel->SetParent( m_panel_Loading );
	panel->Position() = ringPos;

	panel = sx_new( sx::gui::Panel );
	panel->GetElement(0)->SetTextureSrc( L"gui_Loading_Ring01.txr" );
	panel->SetSize( ringSize );
	panel->SetParent( m_panel_Loading );
	panel->Position() = ringPos;

	sx::gui::PLabel label = sx_new( sx::gui::Label );
	label->GetElement(0)->Color().a = 0;
	label->GetElement(1)->Color() = 0xffffffff;
	label->SetSize( float2(500, 50) );
	label->SetParent( m_panel_Loading );
	label->SetAlign( GTA_LEFT );
	label->Position().x = ringPos.x + 250 + ringSize.x * 0.5f;
	label->Position().y = ringPos.y - 10;
	
	label = sx_new( sx::gui::Label );
	label->GetElement(0)->Color().a = 0;
	label->GetElement(1)->Color() = 0xffffffff;
	label->SetSize( float2(500, 50) );
	label->SetParent( m_panel_Loading );
	label->SetAlign( GTA_LEFT );
	label->Position().x = ringPos.x + 250 + ringSize.x * 0.5f;
	label->Position().y = ringPos.y - 20;

	//	strings used in game
	m_strings = sx_new( GameStrings );

	//  load achievements information
#if USE_STEAM_SDK
	m_steam.Initialize();
	{
		str1024 path = sx::sys::FileManager::Project_GetDir();
		path << "localization/";
		const char* language = SteamApps()->GetCurrentGameLanguage();
		if ( strcmp(language, "german") == 0 )
			path << L"german.txt";
		else
			path << L"english.txt";
		m_strings->Load( path );
	}
#else

#endif

}

Game::~Game( void )
{
	sx_delete(m_panel_Loading);
	sx_delete(m_panel_Cursor);

	for ( int i=0; i<m_guides.Count(); ++i )
	{
		GuideText* guide = m_guides[i];
		sx_delete(guide);
	}
	m_guides.Clear();

#if USE_STEAM_SDK
	m_steam.Finalize();
#else
	for ( int i=0; i<Achievement_Count; i++ )
		m_achievements[i].Finalize();
#endif

	sx_delete_and_null(m_strings);

	sx::d3d::Texture::Manager::Release(texture_post);
	sx::d3d::Texture::Manager::Release(texture_scene);
}

UINT Game::GetNewID( void )
{
	return s_idCounter++;
}

const WCHAR* Game::GetLevelPath( void )
{
	sx_callstack();

	static str1024 mainPath;
	mainPath = sx::sys::FileManager::Project_GetDir();
	mainPath << L"level" << g_game->m_game_currentLevel << L"/";
	return mainPath;
}

void Game::Initialize( sx::sys::Window* win )
{
	if ( g_game ) return;
	g_game = sx_new( Game );
	g_game->m_window = win;

#if USE_STEAM_SDK

#else
	{
		String str = sx::sys::FileManager::Project_GetDir();
		str << L"Achievements.txt";

		Scripter script;
		script.Load( str );

		str512 tmp, name, desc, tips, image;
		for (int i=0; i<script.GetObjectCount(); i++)
		{
			if ( script.GetString(i, L"Type", tmp) )
			{
				if ( tmp == L"Achievement" )
				{
					if ( !script.GetString( i, L"Name", name ) )
						continue;

					if ( !script.GetString( i, L"desc", desc ) )
						continue;

					if ( !script.GetString( i, L"image", image ) )
						continue;

					if ( !script.GetString( i, L"tips", tips ) )
						continue;

					int v = 0;
					if ( !script.GetInteger( i, L"value", v ) )
						continue;

					if ( i < Achievement_Count )
						m_achievements[i].Initialize( name, desc, tips, image, v );
				}
			}
		}
	}
#endif

	//	load guid strings
	{
		String str = sx::sys::FileManager::Project_GetDir();
		str << L"Strings.txt";

		Scripter script;
		script.Load( str );

		str512 tmp, tips;
		for (int i=0; i<script.GetObjectCount(); i++)
		{
			if ( script.GetString(i, L"Type", tmp) )
			{
				if ( tmp == L"Guide" )
				{
					if ( !script.GetString( i, L"text", tips ) )
						continue;

					GuideText* guide = sx_new( GuideText );
					guide->m_text.SetText( tips );
					g_game->m_guides.PushBack( guide );
				}
			}
		}
	}

	//	initialize upgrades
	g_game->m_upgrades.LoadDefaults();

	//  game play
	g_game->m_gamePlay = sx_new( GamePlay );

	//  create player
	g_game->m_player = sx_new( Player );

	//  create game gui
	g_game->m_gui = sx_new( GameGUI );

	// initialize objects
	g_game->m_gui->Initialize();
	g_game->m_gamePlay->Initialize();
	g_game->m_player->Initialize();

	//	guide the game to the main menu
	g_game->m_game_nextLevel = 0;
}

void Game::Finalize( void )
{
	if ( !g_game ) return;
	
	//  clear the game
	g_game->ClearLevel();

	//  finalize objects before closing game
	ProjectileManager::ClearTypes();
	ComponentManager::ClearTypes();
	EntityManager::ClearTypes();

	g_game->m_player->Finalize();
	g_game->m_gamePlay->Finalize();
	g_game->m_gui->Finalize();

	sx_delete_and_null( g_game->m_gui );
	sx_delete_and_null( g_game->m_player );
	sx_delete_and_null( g_game->m_gamePlay );

	sx_delete_and_null( g_game );
}

void Game::LoadLevel( void )
{
	sx_callstack();

	// be sure the level is empty
	ClearLevel();

	//  notify to loading
	m_app_Loading = 0xffffff;

	str1024 str = L"gui_loading_level_";
	str << m_game_currentLevel << L".txr";
	m_panel_Loading->GetElement(0)->SetTextureSrc( str );

	//  load the scene
	Sleep(5);
	str = GetLevelPath();
	str << L"scene.scene";
	sx::sys::FileStream file;
	if ( file.Open(str, FM_OPEN_READ) )
	{
		int version = 0;
		SEGAN_STREAM_READ(file, version);

		sx::core::Renderer::GetCamera()->Eye.Set( 100000, 0, 100000 );
		if ( version == 1 )
		{
			sx::core::Scene::LoadNodes( file /*, &Draw_Loading*/ );
			sx::core::Renderer::Load( file );
			g_game->m_player->m_camera_RTS.Reload();
		}
		else
		{
			file.SetPos(0);
			sx::core::Scene::LoadNodes( file /*, &Draw_Loading*/ );
			sx::core::Renderer::GetCamera()->Eye.Set( 10, 10, 10 );
		}

		file.Close();
	}	

	//  load game play and player
	Draw_Loading( 0, 0, L"prepare contents", NULL );
	PostMessage(0, GMT_LEVEL_LOAD, NULL);

	//  additional part
	if ( true )
	{
		for ( Map<UINT, sx::d3d::PGeometry>::Iterator& it = sx::d3d::Geometry::Manager::GetFirst(); !it.IsLast(); it++)
		{
			sx::d3d::Geometry* gm = (*it);
			if ( !gm->Validate(0) )
			{
				Draw_Loading( 0, 0, L"loading geometry", gm->GetSource() );
				gm->Activate(0);
			}
		}
		
		for ( Map<UINT, sx::d3d::PTexture>::Iterator& it = sx::d3d::Texture::Manager::GetFirst(); !it.IsLast(); it++)
		{
			sx::d3d::Texture* tx = (*it);
			if ( !tx->Activated() )
			{
				Draw_Loading( 0, 0, L"loading texture", tx->GetSource() );
				tx->Activate(0);
			}
		}

		sx::core::ArrayPNode nodes;
		sx::core::Scene::GetAllNodes( nodes );
		for (int i=0; i<nodes.Count(); i++)
		{
			float f = 0;
			nodes[i]->MsgProc( MT_ACTIVATE, &f );
		}
	}

	//	verify that entity types are loaded
	if ( EntityManager::GetTypeByIndex(0) == NULL )
	{
		Draw_Loading( 0, 0, L"loading types", NULL );

		ProjectileManager::LoadTypes();
		ComponentManager::LoadTypes();
		EntityManager::LoadTypes( &Draw_Loading );
	}

	//  load default post shader & texture
	{
		str = GetLevelPath();
		str << L"post_screen.txt";

		String postCode;
		sx::cmn::String_Load( postCode, str );
		if ( postCode.Text() )
			shader_present.CompileShader( sx::core::ShaderStore::GetPost_Present( postCode ), SQ_HIGH, L"scene present");

		
		str = L"gui_post_level";
		str << m_game_currentLevel << L".txr";
		texture_post->Cleanup();
		texture_post->SetSource( str );
		texture_post->Activate();

	}


	//	check the lock node
#ifdef USE_LOCK_NAME
	if ( !g_game->m_game_currentLevel )
	{
		sx::core::PNode node = null;
		sx::core::ArrayPNode locknode;
		sx::core::Scene::GetNodesByName( USE_LOCK_NAME, locknode );
		if ( locknode.Count() < 1 )
		{
			node = locknode.At(0);
			float f = 0.0f;
			node->MsgProc( MT_ACTIVATE, &f );
		}
	}
#endif

	//  finish loading
	m_app_Loading = 1;

	PostMessage(0, GMT_LEVEL_LOADED, NULL);
}

void Game::ClearLevel( void )
{
	sx_callstack();

	m_gui->m_gameSpeed->SetValue( 1.0f );
	ProjectileManager::ClearProjectiles();
	EntityManager::ClearEntities();
	PostMessage(0, GMT_LEVEL_CLEAR, NULL);
	sx::core::Scene::Cleanup();
	//sx::snd::SoundData::Manager::ClearAll();

	shader_present.CompileShader( sx::core::ShaderStore::GetPost_Present(L"return float4( tex2D( samp0, tex0 ).rgb, 1.0f );"), SQ_HIGH );
}

void Game::Reset( void )
{
	m_game_restarting = 20;
	PostMessage( 0, GMT_GAME_RESETING, 0 );
	PostMessage( 0, GMT_GAME_RESET, 0 );
	//m_gui->ShowTips( L" Game Restarted !", 0xffff0000 );
	m_player->SyncPlayerAndGame( true );
}

void Game::Update( float elpsTime )
{
	sx_callstack();

	if ( m_app_Closing ) return;

	if ( m_game_currentLevel != m_game_nextLevel )
	{

#if VER_USERDEMO
		//	exhibition version only shows 1, 4, 5, 8 levels
		if ( g_game->m_game_nextLevel == 2 || g_game->m_game_nextLevel == 3 )
			g_game->m_game_nextLevel = 4;
		else if ( g_game->m_game_nextLevel == 6 || g_game->m_game_nextLevel == 7 )
			g_game->m_game_nextLevel = 8;
		else if ( g_game->m_game_nextLevel == 9 || g_game->m_game_nextLevel == 10 )
			g_game->m_game_nextLevel = 1;
#endif
// #if VER_PREVIEW
// 		//	preview version contain all levels except in 'insane' difficulty mode
// 		if ( g_game->m_difficultyLevel == 2 )
// 		{
// 			g_game->m_difficultyLevel = 1;
// 		}
// #endif
		m_game_currentLevel = m_game_nextLevel;
		LoadLevel();
	}

	if ( m_app_Loading == 1 )
	{
		if ( g_game->m_game_currentLevel )
			g_game->PostMessage( 0, GMT_GAME_START, 0 );
		m_app_Loading = 0;
	}
	if ( m_game_restarting )
	{
		--m_game_restarting;
	}
	
// 	if ( m_app_Loading && SEGAN_KEYDOWN( 0, SX_INPUT_KEY_ESCAPE ) )
// 	{
// 		sx::sys::Application::Terminate();
// 		g_game->m_app_Closing = true;
// 		return;
// 	}

	gameTime += elpsTime;

	bool inputHandled = false;
	if ( !m_app_Loading )
	{
		m_gui->ProcessInput( inputHandled, elpsTime );

		// update inputs
		m_gamePlay->ProcessInput(inputHandled, elpsTime);
		m_player->ProcessInput(inputHandled, elpsTime);

		if ( !m_game_paused )
		{
			//  update elements in the scene
			ProjectileManager::Update( elpsTime * Config::GetData()->game_speed );
			EntityManager::Update( elpsTime * Config::GetData()->game_speed );
		}

		//  update game play
		m_gamePlay->Update( elpsTime * Config::GetData()->game_speed );

		//  update player
		m_player->Update(elpsTime);

	}

	//  update rendering system
	if ( m_game_paused )
	{
		sx::core::Renderer::Update( 0 );
	}
	else
	{
		sx::core::Renderer::Update( elpsTime * Config::GetData()->game_speed );
	}

	//  update GUI
	m_gui->Update( elpsTime );
	
	//////////////////////////////////////////////////////////////////////////
	//  update additional objects
	m_panel_Cursor->Position().Set( SEGAN_MOUSE_ABSX(0) - SEGAN_VP_WIDTH/2, SEGAN_VP_HEIGHT/2 - SEGAN_MOUSE_ABSY(0), 0.0f );
	m_panel_Cursor->Update( elpsTime );

	if ( m_app_Loading )
		m_panel_Loading->Update( elpsTime );


	sx::core::PCamera pCamera = sx::core::Renderer::GetCamera();
	SoundListener soundListener( pCamera->Eye, pCamera->GetDirection(), pCamera->Up );
	sx::snd::Device::SetListener( soundListener );
	sx::snd::Device::SetVolume( m_game_paused ? 0 : Config::GetData()->musicVolume, Config::GetData()->soundVolume );
	sx::snd::Device::Pause( m_app_Paused );
}

void Game::Render( DWORD flag )
{
	sx_callstack();

	if ( m_game_restarting || m_app_Closing || m_game_currentLevel != m_game_nextLevel ) return;
	if ( !sx::core::Renderer::CanRender() ) return;

	sx::core::Renderer::Begin();
	sx::core::Renderer::SetRenderTarget( texture_scene );

	D3DViewport vp = *sx::d3d::Device3D::Viewport();
	vp.Width = Config::GetData()->display_Size.x;
	vp.Height = Config::GetData()->display_Size.y;
	sx::d3d::Device3D::Viewport_Set(&vp);

	FogDesc fog;
	sx::d3d::Device3D::GetFogDesc(fog);
	sx::d3d::Device3D::Clear_Screen( (m_game_currentLevel==0 || flag || m_app_Loading ) ? 0xff000000 : fog.Color );

	//  render the meshes in the scene
	if ( !m_app_Loading )
	{
		switch ( Config::GetData()->display_Debug )
		{
		case 0:
			sx::core::Renderer::RenderScene( SX_DRAW_PARTICLE | SX_DRAW_MESH | SX_SHADER_SHADOW | SX_DRAW_TERRAIN );
			break;

		case 1:
			sx::core::Renderer::RenderScene( SX_DRAW_PARTICLE | SX_DRAW_MESH | SX_SHADER_SHADOW | SX_DRAW_TERRAIN );
			sx::d3d::Device3D::RS_AntialiasLine( true );
			sx::core::Renderer::RenderScene( SX_DRAW_WIRED | SX_DRAW_DEBUG | SX_DRAW_BOUNDING_BOX );
			sx::d3d::Device3D::RS_AntialiasLine( false );
			break;

		case 2:
			sx::d3d::Device3D::RS_AntialiasLine( false );
			sx::core::Renderer::RenderScene( SX_DRAW_WIRED | SX_DRAW_PARTICLE | SX_DRAW_MESH | SX_DRAW_TERRAIN );
			sx::d3d::Device3D::RS_WireFrame( false );
			break;

		default:
			sx::core::Renderer::RenderScene( SX_DRAW_PARTICLE | SX_DRAW_MESH | SX_SHADER_SHADOW | SX_DRAW_TERRAIN );

		}
	}

	sx::d3d::UI3D::ReadyToDebug( D3DColor(0,0,0,0) );
	sx::d3d::Device3D::RS_Alpha(0);

	if ( !m_app_Loading && !m_game_paused )
		m_gui->Draw( SX_GUI_DRAW_JUST3D );

	if ( m_app_Loading )
		m_panel_Loading->Draw( 0 );

	//  flush the scene texture to the screen
	sx::d3d::Device3D::SetRenderTarget( 0, NULL );
	if ( Config::GetData()->device_CreationFlag & SX_D3D_FULLSCREEN )
	{
		D3DViewport vp = *sx::d3d::Device3D::Viewport();
		vp.Width = sx::sys::GetDesktopWidth();
		vp.Height = sx::sys::GetDesktopHeight();
		sx::d3d::Device3D::Viewport_Set(&vp);
	}
	else
	{
		D3DViewport vp = *sx::d3d::Device3D::Viewport();
		vp.Width = Config::GetData()->display_Size.x;
		vp.Height = Config::GetData()->display_Size.y;
		sx::d3d::Device3D::Viewport_Set(&vp);
	}


	//  fit flush matrix to pixels
	if ( !(Config::GetData()->device_CreationFlag & SX_D3D_FULLSCREEN && Config::GetData()->display_Size.x < (int)sx::sys::GetDesktopWidth()) )
	{
		Matrix matW(1, 0, 0, 0,	0, 1, 0, 0,	0, 0, 0, 0,
			-1.0f/(float)Config::GetData()->display_Size.x,
			1.0f/(float)Config::GetData()->display_Size.y,
			0, 1);
		shader_present.SetValue( shader_present.GetParameter(0), &matW, sizeof(Matrix));
	}

	shader_present.SetToDevice();
	texture_scene->SetToDevice();
	texture_post->SetToDevice(1);
	sx::d3d::UI3D::DrawQuad();

	sx::d3d::UI3D::ReadyToDebug( D3DColor(0,0,0,0) );

	if ( !m_app_Loading )
	{
		m_gui->Draw( SX_GUI_DRAW_JUST2D );
		m_panel_Cursor->Draw(0);
	}

	sx::core::Renderer::End();

	vp = *sx::d3d::Device3D::Viewport();
	vp.Width = Config::GetData()->display_Size.x;
	vp.Height = Config::GetData()->display_Size.y;
	sx::d3d::Device3D::Viewport_Set(&vp);

}

void Game::PostMessage( UINT RecieverID, UINT msg, void* data )
{
	sx_callstack_param(Game::PostMessage(RecieverID=%d, msg=%d), RecieverID, msg);

	switch ( msg )
	{
	case GMT_SCREEN_RESIZED:
		{
			sx::core::Renderer::SetSize( 
				m_window->GetHandle(),
				Config::GetData()->display_Size.x,
				Config::GetData()->display_Size.y,
				Config::GetData()->device_CreationFlag
				);

			if ( (Config::GetData()->device_CreationFlag & SX_D3D_FULLSCREEN) == 0 )
			{
				m_window->SetTopMostEnable( false );
				m_window->SetBorder( WBT_ORDINARY );
				WindowRect wr;
				wr.Width = Config::GetData()->display_Size.x;
				wr.Height = Config::GetData()->display_Size.y;
				wr.Left = (sx::sys::GetDesktopWidth() - wr.Width) / 2;
				wr.Top = (sx::sys::GetDesktopHeight() - wr.Height) / 4;
				m_window->SetRect(wr);
				
			}


			texture_scene->CreateRenderTarget(
				D3DRTYPE_TEXTURE,
				Config::GetData()->display_Size.x,
				Config::GetData()->display_Size.y,
				D3DFMT_A8R8G8B8);


			float2 dispSize( (float)Config::GetData()->display_Size.x, (float)Config::GetData()->display_Size.y );
			float3 ringPos( - dispSize.x * 0.43f, - dispSize.y * 0.32f , 0 );
			float2 ringSize( dispSize.x * 0.1f, dispSize.x * 0.1f );

			sx::gui::Control* control = (sx::gui::Control*)m_panel_Loading->GetChild( 0 );
			control->SetSize( ringSize );
			control->Position() = ringPos;

			control = (sx::gui::Control*)m_panel_Loading->GetChild( 1 );
			control->SetSize( ringSize );
			control->Position() = ringPos;

			control = (sx::gui::Control*)m_panel_Loading->GetChild( 2 );
			control->SetSize( ringSize );
			control->Position() = ringPos;

			control = (sx::gui::Control*)m_panel_Loading->GetChild( 3 );
			control->SetSize( ringSize );
			control->Position() = ringPos;

			control = (sx::gui::Control*)m_panel_Loading->GetChild( 4 );
			control->Position().x = ringPos.x + 250 + ringSize.x * 0.5f;
			control->Position().y = ringPos.y - 10;

			control = (sx::gui::Control*)m_panel_Loading->GetChild( 5 );
			control->Position().x = ringPos.x + 250 + ringSize.x * 0.5f;
			control->Position().y = ringPos.y - 20;
		}
		break;

	case GMT_LEVEL_LOAD:
	case GMT_GAME_RESET:
		{
			if ( Config::GetData()->display_Debug == 3 )
			{
				g_game->m_upgrades.LoadDefaults();
			}

		}
	case GMT_GAME_START:
#if USE_GAMEUP
			g_gameup->begin_score();
#endif
			break;
	}


	ProjectileManager::MsgProc(RecieverID, msg, data);
	ComponentManager::MsgProc(msg, data);
	EntityManager::MsgProc(RecieverID, msg, data);
	g_game->m_gui->MsgProc(RecieverID, msg, data);
	g_game->m_gamePlay->MsgProc(RecieverID, msg, data);
	g_game->m_player->MsgProc(RecieverID, msg, data);
}

void textcopy( WCHAR* dest, const WCHAR* src )
{
	if ( !dest || !src ) return;

	WCHAR* c = (WCHAR*)src;
	while ( *c )
	{
		*dest = *c;
		c++;
		dest++;
	}
}

#if USE_GAMEUP
void gameup_add_score( const uint reason )
{
	switch ( g_game->m_difficultyLevel )
	{
	case 0: g_gameup->add_score( reason, GAME_SCORE_EASY ); break;
	case 1: g_gameup->add_score( reason, GAME_SCORE_NORM ); break;
	case 2: g_gameup->add_score( reason, GAME_SCORE_HARD ); break;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
//	achievements implementation
//////////////////////////////////////////////////////////////////////////
#if USE_STEAM_SDK

Steam::Steam()
: m_CallbackUserStatsReceived(this, &Steam::OnUserStatsReceived)
, m_CallbackUserStatsStored(this, &Steam::OnUserStatsStored)
, m_CallbackAchievementStored(this, &Steam::OnAchievementStored)
, m_GameID(0)
, m_pSteamUser(NULL)
, m_pSteamUserStats(NULL)
, m_bStatsValid(false)
, m_hEfficiencyLeaderboard(0)
, m_hScoreLeaderboard(0)
, m_real_estate(0)
, m_death_trap(0)
, m_apocalypto(0)
, m_agile(0)
, m_perfect(0)
, m_sniper(0)
, m_redeemer(0)
, m_levels(0)
, m_minilevels(0)
, m_tower_dealer_temp(0)
, m_wrath_temp(0)
, m_clever_temp(false)
{
	memset(m_achievements, 0, sizeof(m_achievements));
	memset(m_stats_helper, 0, sizeof(m_stats_helper));
	memset(m_stats, 0, sizeof(m_stats));
	memset(m_stat_efficiency, 0, sizeof(m_stat_efficiency));
}

void Steam::Initialize( void )
{
	if (SteamUtils())
	{
		m_GameID = CGameID(SteamUtils()->GetAppID()).ToUint64();
	}

	m_pSteamUser = SteamUser();
	m_pSteamUserStats = SteamUserStats();

	// Is Steam loaded? If not we can't get stats.
	if (NULL == m_pSteamUser || NULL == m_pSteamUserStats)
	{
		return;
	}

	// Is the user logged on?  If not we can't get stats.
	if (!m_pSteamUser->BLoggedOn())
	{
		return;
	}

	//m_pSteamUserStats->ResetAllStats(true);

	// Request user stats.
	m_pSteamUserStats->RequestCurrentStats();

	FindLeaderboards();
}

void Steam::Finalize( void )
{
	//if (m_pSteamUserStats && m_bStatsValid)
	//{
		//m_pSteamUserStats->StoreStats();
	//}
}

void Steam::CallAchievement( const int type, const SteamCallState state )
{
	if (!m_bStatsValid)
	{
		return;
	}

	switch ( type )
	{
	case EAT_First_Blood:// kill first enemy in the game 
		{
			if (!m_achievements[EAT_First_Blood])
			{
				m_achievements[EAT_First_Blood] = true;
				m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_First_Blood]);
				m_pSteamUserStats->StoreStats();
			}

			if (m_stats[EST_Slaughtered] >= 1000)
			{
				if (!m_achievements[EAT_Slayer])
				{
					m_achievements[EAT_Slayer] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Slayer]);
					m_pSteamUserStats->StoreStats();
				}
			}

			if (m_stats[EST_Slaughtered] >= 2500)
			{
				if (!m_achievements[EAT_Blood_lust])
				{
					m_achievements[EAT_Blood_lust] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Blood_lust]);
					m_pSteamUserStats->StoreStats();
				}
			}

			if (m_stats[EST_Slaughtered] >= 3500)
			{
				if (!m_achievements[EAT_Terminator])
				{
					m_achievements[EAT_Terminator] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Terminator]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	//case EAT_Slayer:// kill 1000 enemies through the game
	//	break;
	//case EAT_Blood_lust:// kill 2500 enemies through the game
	//	break;
	//case EAT_Terminator:// kill 3500 enemies through the game
	//	break;

	case EAT_Constructor:// create 30 towers through the game
		{
			if (m_stats[EST_Constructions] >= 30)
			{
				if (!m_achievements[EAT_Constructor])
				{
					m_achievements[EAT_Constructor] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Constructor]);
					m_pSteamUserStats->StoreStats();
				}
			}

			if (m_stats[EST_Constructions] >= 100)
			{
				if (!m_achievements[EAT_Engineer])
				{
					m_achievements[EAT_Engineer] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Engineer]);
					m_pSteamUserStats->StoreStats();
				}
			}

			if (m_stats[EST_Constructions] >= 250)
			{
				if (!m_achievements[EAT_Architect])
				{
					m_achievements[EAT_Architect] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Architect]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	//case EAT_Engineer:// create 100 towers through the game
	//	break;
	//case EAT_Architect:// build 250 towers through the game
	//	break;

	case EAT_Tower_Dealer:// sell 10 towers in one level
		
		if (!m_achievements[EAT_Tower_Dealer])// verify that the achievement is already given or not
		{
			if ( state == ESC_InPlay )
			{
				m_tower_dealer_temp += 1;
				if ( m_tower_dealer_temp == 10 )
				{
					m_achievements[EAT_Tower_Dealer] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Tower_Dealer]);
					m_pSteamUserStats->StoreStats();
				}
			}
			else
			{
				if ( m_tower_dealer_temp < 10 )
					m_tower_dealer_temp = 0;
			}
		}
		break;

	case EAT_Real_Estate:// sell 100 towers through the game
		{
			m_real_estate += 1;
			m_pSteamUserStats->SetStat("real_estate", m_real_estate);
			m_pSteamUserStats->StoreStats();

			if (!m_achievements[EAT_Real_Estate])
			{
				if (m_real_estate >= 100)
				{
					m_achievements[EAT_Real_Estate] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Real_Estate]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	case EAT_Death_Trap:// kill 100 enemies by trap through the game
		{
			m_death_trap += 1;
			m_pSteamUserStats->SetStat("death_trap", m_death_trap);
			m_pSteamUserStats->StoreStats();

			if (!m_achievements[EAT_Death_Trap])
			{
				if (m_death_trap >= 100)
				{
					m_achievements[EAT_Death_Trap] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Death_Trap]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	case EAT_Apocalypto:// kill 1000 enemies by death rain through the game
		{
			m_apocalypto += 1;
			m_pSteamUserStats->SetStat("apocalypto", m_apocalypto);
			m_pSteamUserStats->StoreStats();

			if (!m_achievements[EAT_Apocalypto])
			{
				if (m_apocalypto >= 1000)
				{
					m_achievements[EAT_Apocalypto] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Apocalypto]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	case EAT_Wrath_Of_Battle:// use death rain 10 times in the single level
		
		if (!m_achievements[EAT_Wrath_Of_Battle])// verify that the achievement is already given or not
		{
			if ( state == ESC_InPlay )
			{
				m_wrath_temp += 1;
				if ( m_wrath_temp == 10 )
				{
					m_achievements[EAT_Wrath_Of_Battle] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Wrath_Of_Battle]);
					m_pSteamUserStats->StoreStats();
				}
			}
			else
			{
				if ( m_wrath_temp < 10 )
					m_wrath_temp = 0;
			}
		}
		break;

	case EAT_Agile_Warrior:// call 100 waves earlier through the game
		{
			m_agile += 1;
			m_pSteamUserStats->SetStat("agile", m_agile);
			m_pSteamUserStats->StoreStats();

			if (!m_achievements[EAT_Agile_Warrior])
			{
				if (m_agile >= 100)
				{
					m_achievements[EAT_Agile_Warrior] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Agile_Warrior]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	case EAT_Perfect_Battle:// make 50 tower full upgrade with full ability through the game
		{
			m_perfect += 1;
			m_pSteamUserStats->SetStat("perfect", m_perfect);
			m_pSteamUserStats->StoreStats();

			if (!m_achievements[EAT_Perfect_Battle])
			{
				if (m_perfect >= 50)
				{
					m_achievements[EAT_Perfect_Battle] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Perfect_Battle]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	case EAT_Sniper:// kill 100 enemies by sniper of ground lava through the game
		{
			m_sniper += 1;
			m_pSteamUserStats->SetStat("sniper", m_sniper);
			m_pSteamUserStats->StoreStats();

			if (!m_achievements[EAT_Sniper])
			{
				if (m_sniper >= 100)
				{
					m_achievements[EAT_Sniper] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Sniper]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	case EAT_Clever_Dealer:// sell all towers after final wave
		
		if (!m_achievements[EAT_Clever_Dealer])// verify that the achievement is already given or not
		{
			if ( state == ESC_OnStart )
			{
				m_clever_temp = true;
			}
			else if ( state == ESC_InPlay )
			{
				if ( m_clever_temp )
				{
					bool allTowers = true;
					for ( uint i=0; i<EntityManager::GetEntityCount(); ++i )
					{
						Entity* entity = EntityManager::GetEntityByIndex( i );
						if ( entity->m_partyCurrent == PARTY_TOWER && entity->m_health.icur > 0 )
						{
							allTowers = false;
							break;
						}
					}
					if ( allTowers )
					{
						m_achievements[EAT_Clever_Dealer] = true;
						m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Clever_Dealer]);
						m_pSteamUserStats->StoreStats();
					}
				}
			}
			else m_clever_temp = false;			
		}
		break;

	case EAT_Redeemer:// release people to reach 300 people
		{
			m_redeemer += 1;
			m_pSteamUserStats->SetStat("redeemer", m_redeemer);
			m_pSteamUserStats->StoreStats();

			if (!m_achievements[EAT_Redeemer])
			{
				if (m_redeemer >= 300)
				{
					m_achievements[EAT_Redeemer] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Redeemer]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	case EAT_Normality:// play all levels in NORM difficulty mode
		{
			if (!m_achievements[EAT_Normality])
			{
				m_levels |= (1 << (g_game->m_game_currentLevel - 1));
				m_pSteamUserStats->SetStat("levels", m_levels);
				m_pSteamUserStats->StoreStats();

				if ((m_levels & 0x3ff) == 0x3ff)
				{
					m_achievements[EAT_Normality] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Normality]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	case EAT_Dignified_Killer:// play all levels in HARD difficulty mode
		{
			if (!m_achievements[EAT_Dignified_Killer])
			{
				m_levels |= (1 << (g_game->m_game_currentLevel - 1 + 10));
				m_pSteamUserStats->SetStat("levels", m_levels);
				m_pSteamUserStats->StoreStats();

				if ((m_levels & 0xffc00) == 0xffc00)
				{
					m_achievements[EAT_Dignified_Killer] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Dignified_Killer]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	case EAT_Deadly_Serious:// play all levels in INSANE difficulty mode
		{
			if (!m_achievements[EAT_Deadly_Serious])
			{
				m_levels |= (1 << (g_game->m_game_currentLevel - 1 + 20));
				m_pSteamUserStats->SetStat("levels", m_levels);
				m_pSteamUserStats->StoreStats();

				if ((m_levels & 0x3ff00000) == 0x3ff00000)
				{
					m_achievements[EAT_Deadly_Serious] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Deadly_Serious]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;

	case EAT_Fisherman:// get 10 stars through the game
		
		if (!m_achievements[EAT_Fisherman])// verify that the achievement is already given or not
		{
			int numStars = g_game->m_player->m_profile.GetNumStars();
			if ( numStars >= 10 )
			{
				m_achievements[EAT_Fisherman] = true;
				m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Fisherman]);
				m_pSteamUserStats->StoreStats();
			}
		}
		break;

	case EAT_Stalwart:// get 20 stars through the game

		if (!m_achievements[EAT_Stalwart])// verify that the achievement is already given or not
		{
			int numStars = g_game->m_player->m_profile.GetNumStars();
			if ( numStars >= 20 )
			{
				m_achievements[EAT_Stalwart] = true;
				m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Stalwart]);
				m_pSteamUserStats->StoreStats();
			}
		}
		break;

	case EAT_Star_Collector:// get all 30 stars through the game

		if (!m_achievements[EAT_Star_Collector])// verify that the achievement is already given or not
		{
			int numStars = g_game->m_player->m_profile.GetNumStars();
			if ( numStars >= 30 )
			{
				m_achievements[EAT_Star_Collector] = true;
				m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Star_Collector]);
				m_pSteamUserStats->StoreStats();
			}
		}
		break;

	case EAT_Director:// use cinematic camera for the first time
		{
			if (!m_achievements[EAT_Director])
			{
				m_achievements[EAT_Director] = true;
				m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Director]);
				m_pSteamUserStats->StoreStats();
			}
		}
		break;

	case EAT_Gamepa:// play all mini games
		{
			if (!m_achievements[EAT_Gamepa])
			{
				m_minilevels |= (1 << (g_game->m_game_currentLevel - 1));
				m_pSteamUserStats->SetStat("minilevels", m_minilevels);
				m_pSteamUserStats->StoreStats();

				if (m_minilevels == 0x1ff)
				{
					m_achievements[EAT_Gamepa] = true;
					m_pSteamUserStats->SetAchievement(s_SteamAchievementsName[EAT_Gamepa]);
					m_pSteamUserStats->StoreStats();
				}
			}
		}
		break;
	}
}

void Steam::CallStat( const StatType type, const SteamCallState state, const float value /*= 0.0f*/ )
{
	if (!m_bStatsValid)
	{
		return;
	}

	switch ( type )
	{
	case EST_Constructions:// Number of towers have been constructed
		{
			++m_stats_helper[EST_Constructions];

			++m_stats[EST_Constructions];

			m_pSteamUserStats->SetStat(s_SteamStatName[EST_Constructions], m_stats[EST_Constructions]);
			m_pSteamUserStats->StoreStats();
		}
		break;

	case EST_Slaughtered:// Number of enemies have been dead
		{
			++m_stats_helper[EST_Slaughtered];

			++m_stats[EST_Slaughtered];

			m_pSteamUserStats->SetStat(s_SteamStatName[EST_Slaughtered], m_stats[EST_Slaughtered]);
			m_pSteamUserStats->StoreStats();
		}
		break;

	case EST_Destroyed:// Number of towers have been destroyed by enemies
		{
			++m_stats_helper[EST_Destroyed];

			++m_stats[EST_Destroyed];

			m_pSteamUserStats->SetStat(s_SteamStatName[EST_Destroyed], m_stats[EST_Destroyed]);
			m_pSteamUserStats->StoreStats();
		}
		break;

	case EST_Gaining:// Total gold have been earned through the game
		{
			if ( state == ESC_InPlay )
			{
				int goldIncome = int(value);
				m_stats_helper[EST_Gaining] += goldIncome;

				m_stats[EST_Gaining] += goldIncome;

				m_pSteamUserStats->SetStat(s_SteamStatName[EST_Gaining], m_stats[EST_Gaining]);
				m_pSteamUserStats->StoreStats();
			}
		}
		break;

	case EST_Efficiency:// Average of all levels efficiency 
		{
			if ( state == ESC_OnStart )
			{
				m_stats_helper[EST_Constructions] = 0;
				m_stats_helper[EST_Slaughtered] = 0;
				m_stats_helper[EST_Destroyed] = 0;
				m_stats_helper[EST_Gaining] = 0;
				m_stats_helper[EST_Helper_Enemies] = 0;
			}
			else if ( state == ESC_OnVictory )
			{
				//	compute efficiency for this level
				{
					float towers =		(float)m_stats_helper[EST_Constructions];
					float destroyed =	(float)m_stats_helper[EST_Destroyed];
					float killed =		(float)m_stats_helper[EST_Slaughtered];
					float enemies =		(float)m_stats_helper[EST_Helper_Enemies];
					float addgold =		(float)m_stats_helper[EST_Gaining];
					float curgold =		(float)g_game->m_player->m_gold;
					float addpeople =	curgold / 500.0f;
					float deadpeople =	value;

					float factor;
					switch ( g_game->m_difficultyLevel )
					{
					case 0: factor = 500.0f; break;
					case 1: factor = 750.0f; break;
					case 2: factor = 1000.0f; break;
					}

					//	compute the efficiency on this level
					float towervalue = ( 1.0f - ( destroyed / towers ) ) * factor;
					float enemyvalue = ( killed / enemies ) * factor;
					float goldvalue = ( curgold / addgold ) * factor;

					float peoplevalu = 0.0f;
					if ( addpeople > deadpeople )
						peoplevalu = ( 1.0f - ( deadpeople / addpeople ) ) *  factor;
					else if ( addpeople < deadpeople )
						peoplevalu = - ( 1.0f - ( addpeople / deadpeople ) ) * factor;

					const float efficiency = ( towervalue + enemyvalue + goldvalue + peoplevalu ) / 4.0f;

					//	store efficiency for this level
					const int efficiencyID = g_game->m_game_currentLevel - 1;
					if (efficiency > m_stat_efficiency[efficiencyID])
					{
						m_stat_efficiency[efficiencyID] = efficiency;
						m_pSteamUserStats->SetStat(s_SteamStatName[EST_Efficiency + efficiencyID], m_stat_efficiency[efficiencyID]);
						m_pSteamUserStats->StoreStats();
					}
				}
				
				//	store efficiency average
				{
					float avr_efficiency = 0;
					for ( int i=0; i<10; ++i )
					{
						avr_efficiency += m_stat_efficiency[i];
					}
					avr_efficiency /= 10;

					m_pSteamUserStats->SetStat("Efficiency", avr_efficiency);
					m_pSteamUserStats->StoreStats();

					if (m_hEfficiencyLeaderboard)
					{
						SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(
							m_hEfficiencyLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, static_cast<int>(avr_efficiency), NULL, 0);

						m_SteamCallResultUploadScore.Set(hSteamAPICall, this, &Steam::OnUploadScore);
					}
				}
			}
		}
		break;

	case EST_Score:// Total score of the player - computed by Tower Experiences
		{
			if ( state == ESC_InPlay )
			{
				float addScore = value * 10;
				m_stats[EST_Score] += int(addScore);
			}
			else if ( state == ESC_OnEnd )
			{
				m_pSteamUserStats->SetStat(s_SteamStatName[EST_Score], m_stats[EST_Score]);
				m_pSteamUserStats->StoreStats();

				if (m_hScoreLeaderboard)
				{
					SteamAPICall_t hSteamAPICall = SteamUserStats()->UploadLeaderboardScore(
						m_hScoreLeaderboard, k_ELeaderboardUploadScoreMethodKeepBest, m_stats[EST_Score], NULL, 0);

					m_SteamCallResultUploadScore.Set(hSteamAPICall, this, &Steam::OnUploadScore);
				}
			}
		}
		break;

	case EST_Helper_Enemies:// count the number of enemies have been created
		{
			++m_stats_helper[EST_Helper_Enemies];
		}
		break;
	}
}

void Steam::FindLeaderboards()
{
	if (!m_hEfficiencyLeaderboard)
	{
		SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard("Level Efficiency");
		m_SteamCallResultCreateLeaderboard.Set(hSteamAPICall, this, &Steam::OnFindLeaderboard);
	}
}

void Steam::OnUserStatsReceived(UserStatsReceived_t *pCallback)
{
	if (!m_pSteamUserStats)
	{
		return;
	}

	// we may get callbacks for other games' stats arriving, ignore them
	if (m_GameID == pCallback->m_nGameID)
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			//log_file << "Received stats and achievements from Steam\n";

			m_bStatsValid = true;

			// load achievements
			for (int i = 0; i < Achievement_Count; ++i)
			{
				m_pSteamUserStats->GetAchievement(s_SteamAchievementsName[i], &m_achievements[i]);
			}

			// load stats
			m_pSteamUserStats->GetStat(s_SteamStatName[EST_Constructions], &m_stats[EST_Constructions]);
			m_pSteamUserStats->GetStat(s_SteamStatName[EST_Slaughtered], &m_stats[EST_Slaughtered]);
			m_pSteamUserStats->GetStat(s_SteamStatName[EST_Destroyed], &m_stats[EST_Destroyed]);
			m_pSteamUserStats->GetStat(s_SteamStatName[EST_Gaining], &m_stats[EST_Gaining]);
			m_pSteamUserStats->GetStat(s_SteamStatName[EST_Score], &m_stats[EST_Score]);

			for ( int i=0; i<10; ++i )
				m_pSteamUserStats->GetStat(s_SteamStatName[EST_Efficiency + i], &m_stat_efficiency[i]);

			m_pSteamUserStats->GetStat("real_estate", &m_real_estate);
			m_pSteamUserStats->GetStat("death_trap", &m_death_trap);
			m_pSteamUserStats->GetStat("apocalypto", &m_apocalypto);
			m_pSteamUserStats->GetStat("agile", &m_agile);
			m_pSteamUserStats->GetStat("perfect", &m_perfect);
			m_pSteamUserStats->GetStat("sniper", &m_sniper);
			m_pSteamUserStats->GetStat("redeemer", &m_redeemer);
			m_pSteamUserStats->GetStat("levels", &m_levels);
			m_pSteamUserStats->GetStat("minilevels", &m_minilevels);
		}
		else
		{
			//char buffer[64] = {0};
			//sprintf_s(buffer, "RequestStats - failed, %d\n", pCallback->m_eResult);
			//log_file << buffer;
		}
	}
}

void Steam::OnUserStatsStored(UserStatsStored_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_GameID == pCallback->m_nGameID)
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			//log_file << "StoreStats - success\n";
		}
		else if ( k_EResultInvalidParam == pCallback->m_eResult )
		{
			// One or more stats we set broke a constraint. They've been reverted,
			// and we should re-iterate the values now to keep in sync.
			//log_file << "StoreStats - some failed to validate\n";

			// Fake up a callback here so that we re-load the values.
			UserStatsReceived_t callback;
			callback.m_eResult = k_EResultOK;
			callback.m_nGameID = m_GameID;
			OnUserStatsReceived(&callback);
		}
		else
		{
			//char buffer[64] = {0};
			//sprintf_s(buffer, "StoreStats - failed, %d\n", pCallback->m_eResult);
			//log_file << buffer;
		}
	}
}

void Steam::OnAchievementStored(UserAchievementStored_t *pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (m_GameID == pCallback->m_nGameID)
	{
		if (0 == pCallback->m_nMaxProgress)
		{
			//char buffer[128] = {0};
			//sprintf_s(buffer, "Achievement '%s' unlocked!\n", pCallback->m_rgchAchievementName);
			//log_file << buffer;
		}
		else
		{
			//char buffer[128] = {0};
			//sprintf_s(buffer, "Achievement '%s' progress callback, (%d,%d)\n", 
			//	pCallback->m_rgchAchievementName, pCallback->m_nCurProgress, pCallback->m_nMaxProgress);
		}
	}
}

void Steam::OnFindLeaderboard( LeaderboardFindResult_t *pFindLeaderboardResult, bool bIOFailure )
{
	// see if we encountered an error during the call
	if (!pFindLeaderboardResult->m_bLeaderboardFound || bIOFailure)
	{
		return;
	}

	// check to see which leader board handle we just retrieved
	const char *pchName = SteamUserStats()->GetLeaderboardName(pFindLeaderboardResult->m_hSteamLeaderboard);
	if (strcmp(pchName, "Level Efficiency") == 0)
	{
		m_hEfficiencyLeaderboard = pFindLeaderboardResult->m_hSteamLeaderboard;

		if (!m_hScoreLeaderboard)
		{
			SteamAPICall_t hSteamAPICall = SteamUserStats()->FindLeaderboard("Player_Score");
			m_SteamCallResultCreateLeaderboard.Set(hSteamAPICall, this, &Steam::OnFindLeaderboard);
		}
	}
	else if (strcmp( pchName, "Player_Score" ) == 0)
	{
		m_hScoreLeaderboard = pFindLeaderboardResult->m_hSteamLeaderboard;
	}
}

void Steam::OnUploadScore(LeaderboardScoreUploaded_t *pFindLeaderboardResult, bool bIOFailure)
{
	if (!pFindLeaderboardResult->m_bSuccess || bIOFailure)
	{
		//OutputDebugString( "Score could not be uploaded to Steam\n" );
		return;
	}
	else
	{

	}
}

#else

void Achievement::Initialize( const WCHAR* cname, const WCHAR* cdesc, const WCHAR* ctips, const WCHAR* cicon, int irange )
{
	if ( !cname || !cdesc || !cicon || !ctips )
	{
		sxLog::Log( L"Warning! Achievement has been ignored ! check the 'Achievements.txt' file." );
		return;
	}

	textcopy( name, cname );
	textcopy( desc, cdesc );
	textcopy( tips, ctips );
	textcopy( icon, cicon );

	range = irange;
}

void Achievement::AddValue( int val /*= 1 */ )
{
	if ( value >= range ) return;
	value += val;
	if ( value > range ) value = range;

	if ( value != range ) return;
	//str1024 str;
	//str.Format( L" Achievement unlocked : \n %s ", name );
	//g_game->m_gui->ShowTips( tips, 0xffffa352, icon );
	g_game->m_gui->ShowTips( tips, 0xffff0319, icon );

	msg_SoundPlay msg( true, 0, 0, L"achievement" );
	g_game->m_gui->m_main->m_soundNode->MsgProc( MT_SOUND_PLAY, &msg );

	//	copy achievements values
	for ( int i=0; i<Achievement_Count; i++ )
		g_game->m_player->m_profile.achievements[i] = g_game->m_achievements[i].value;

#if USE_GAMEUP
	gameup_add_score( GAME_SCORE_ACHIV );
#endif
}


bool Achievement::IsUnlocked( void )
{
	return value >= range;
}


void Achievement::Finalize( void )
{

}
#endif

//////////////////////////////////////////////////////////////////////////
//	upgrades implementation
//////////////////////////////////////////////////////////////////////////
void Upgrades::LoadDefaults( void )
{
	sx_callstack();

	//  load some level configuration
	String str = sx::sys::FileManager::Project_GetDir();
	str << L"upgrades.txt";

	Scripter script;
	script.Load( str );

	for (int i=0; i<script.GetObjectCount(); i++)
	{
		str512 tmpStr;
		if ( script.GetString(i, L"Type", tmpStr) && tmpStr == L"Upgrades" )
		{
			if ( !script.GetString(i, L"Name", tmpStr) )
				continue;

			for ( int j=0; j<44; j++ )
			{
				tmpStr.Format( L"%d_value", j );
				script.GetFloat(i, tmpStr, defaults[j] );

				tmpStr.Format( L"%d_name", j );
				if ( script.GetString(i, tmpStr, tmpStr ) )
					String::Copy( name[j], 512, tmpStr );

				tmpStr.Format( L"%d_desc", j );
				if ( script.GetString(i, tmpStr, tmpStr ) )
					String::Copy( desc[j], 512, tmpStr );

				tmpStr.Format( L"%d_unlock", j );
				script.GetInteger(i, tmpStr, unlock[j] );
			}
		}
	}
}

void Upgrades::Reset( void )
{
	for ( int i=0; i<28; i++ )
		value[i] = 1.0f;

	for ( int i=28; i<44; i++ )
		value[i] = 0.0f;
}
