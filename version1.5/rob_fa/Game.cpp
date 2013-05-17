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

//  singleton pointer
extern Game*			g_game = NULL;
static UINT				s_idCounter = 0;

//  some internal resources
static sx::d3d::PTexture	texture_scene;
static sx::d3d::PTexture	texture_post;
static sx::d3d::Shader3D	shader_present;

//  some internal variables
static float gameTime = 0;


void Draw_Loading( int count, int index, const WCHAR* state, const WCHAR* name )
{
	using namespace sx::gui;

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
}

Game::Game( void )
:	m_gamePlay(0)
,	m_player(0)
,	m_gui(0)
,	m_mouseMode(MS_Null)
,	m_difficultyValue(1.0f)
,	m_difficultyLevel(0)
,	m_game_currentLevel(-1)
,	m_game_nextLevel(0)
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

	//  load achievements information

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

					if ( i < 15 )
						m_achievements[i].Initialize( name, desc, tips, image, v );
				}
			}
		}
	}

	//	initialize upgrades
	m_upgrades.LoadDefaults();
}

Game::~Game( void )
{
	sx_delete(m_panel_Loading);
	sx_delete(m_panel_Cursor);

	for ( int i=0; i<15; i++ )
		m_achievements[i].Finalize();

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
}

void Game::Finalize( void )
{
	if ( !g_game ) return;

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

	//  finish loading
	m_app_Loading = 1;

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
	PostMessage( 0, GMT_GAME_RESETING, 0 );
	PostMessage( 0, GMT_GAME_RESET, 0 );
	m_gui->ShowTips( L" Game Restarted !", 0xffff0000 );
	m_player->SyncPlayerAndGame( true );
}

void Game::Update( float elpsTime )
{
	sx_callstack();

	if ( m_app_Closing ) return;

	if ( m_game_currentLevel != m_game_nextLevel )
	{
		m_game_currentLevel = m_game_nextLevel;
		LoadLevel();
	}

	if ( m_app_Loading == 1 )
	{
		if ( g_game->m_game_currentLevel )
			g_game->PostMessage( 0, GMT_GAME_START, 0 );
		m_app_Loading = 0;
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

	if ( m_app_Closing || m_game_currentLevel != m_game_nextLevel ) return;
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

//////////////////////////////////////////////////////////////////////////
//	achievements implementation
//////////////////////////////////////////////////////////////////////////
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
}


bool Achievement::IsUnlocked( void )
{
	return value >= range;
}


void Achievement::Finalize( void )
{

}

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
