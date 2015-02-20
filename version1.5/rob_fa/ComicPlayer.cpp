#include "ComicPlayer.h"
#include "Game.h"
#include "GameStrings.h"
#include "Scripter.h"
#include "GameUtils.h"

ComicPlayer::ComicPlayer( void ): m_time(0), m_maxTime(0), m_node(0), m_camera(0) { }

ComicPlayer::~ComicPlayer( void )
{
	for ( int i=0; i<m_labels.Count(); ++i )
	{
		sx::gui::Label* label = m_labels[i].label;
		sx_delete(label);
	}

	sx_delete_and_null(m_node);
}

bool ComicPlayer::Load( const wchar* fileName, void* loadingCallback )
{
	Callback_Draw_Loading drawLoading = loadingCallback ? (Callback_Draw_Loading)loadingCallback : null;

	if ( drawLoading )
		drawLoading(0, 0, L"loading comic", fileName);

	Scripter script;
	script.Load( fileName );

	for (int i=0; i<script.GetObjectCount(); i++)
	{
		str512 tmp;
		if ( script.GetString(i, L"Type", tmp) )
		{
			if ( tmp == L"Comic" )
			{
				if (script.GetString(i, L"node", tmp))
				{
					//	load main node contain animation and sounds
					m_node = sx_new( sx::core::Node );

					PStream pfile = NULL;
					if ( sx::sys::FileManager::File_Open( tmp, SEGAN_PACKAGENAME_DRAFT, pfile ) )
					{
						m_node->Load(*pfile);
						sx::sys::FileManager::File_Close(pfile);
#if 0
						for ( Map<UINT, sx::d3d::PGeometry>::Iterator& it = sx::d3d::Geometry::Manager::GetFirst(); !it.IsLast(); it++)
						{
							sx::d3d::Geometry* gm = (*it);
							if ( !gm->Validate(0) )
							{
								if ( drawLoading )
									drawLoading( 0, 0, L"loading geometry", gm->GetSource() );
								gm->Activate(0);
							}
						}

						for ( Map<UINT, sx::d3d::PTexture>::Iterator& it = sx::d3d::Texture::Manager::GetFirst(); !it.IsLast(); it++)
						{
							sx::d3d::Texture* tx = (*it);
							if ( !tx->Activated() )
							{
								if ( drawLoading )
									drawLoading( 0, 0, L"loading texture", tx->GetSource() );
								tx->Activate(0);
							}
						}
#endif
						if ( drawLoading )
							drawLoading( 0, 0, L"activating node", m_node->GetName() );

						//	activate node
						float f = 0;
						m_node->MsgProc( MT_ACTIVATE, &f );
					}
					else return false;
				}
			}
			else if ( tmp == L"Sound")
			{
				if ( !script.GetString(i, L"Name", tmp) ) continue;

				ComicSound comicSound;
				comicSound.time = tmp.ToFloat();
				if ( script.GetString(i, L"name", tmp) )
					sx_str_copy( comicSound.name, 32, tmp );
				script.GetUint(i, L"index", comicSound.index);
				m_sounds.PushBack( comicSound );
			}
			else if ( tmp == L"Particle")
			{
				if ( !script.GetString(i, L"Name", tmp) ) continue;

				ComicParticle comicParticle;
				comicParticle.time = tmp.ToFloat();
				if ( script.GetString(i, L"name", tmp) )
					sx_str_copy( comicParticle.name, 32, tmp );
				m_particles.PushBack( comicParticle );
			}
			else if ( tmp == L"Text")
			{
				if ( script.GetString( i, L"node", tmp ) )
				{
					ComicText comicText;
					if ( m_node->GetChildByName( tmp, comicText.node ) )
					{
						uint txtId = 0;
						float width = 0, height = 0;
						script.GetUint( i, L"text", txtId );
						script.GetFloat( i, L"width", width );
						script.GetFloat( i, L"height", height );

						comicText.label = create_label( null, txtId, width, height, 0, 0, 0 );
						comicText.label->AddProperty( SX_GUI_PROPERTY_3DSPACE );
						
						m_labels.PushBack( comicText );
					}
					else sxLog::Log(L"ERROR : ComicPlayer couldn't find node %s in %s", tmp.Text(), fileName);
				}
			}
		}
	}

	//	retrieve total animation time
	{
		msg_Animator msg( SX_ANIMATOR_PLAY, SX_ANIMATOR_LOOP, 0, -1, -1, -1, 0.0f );
		m_node->MsgProc( MT_ANIMATOR, &msg );
		m_maxTime = msg.animMaxTime * 1000.0f;
	}

	//	get the camera node
	m_node->GetChildByName( L"camera", m_camera );
	
	return true;
}

void ComicPlayer::Update( float elpstime )
{
	//	handle windows update
	{
		MSG msg;
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	sx::io::Input::Update( elpstime );
	if (SEGAN_KEYUP(0, SX_INPUT_KEY_SPACE) || 
		SEGAN_KEYUP(0, SX_INPUT_KEY_ESCAPE) || 
		SEGAN_KEYUP(0, SX_INPUT_KEY_RETURN) || 
		SEGAN_KEYUP(0, SX_INPUT_KEY_MOUSE_LEFT) || 
		SEGAN_KEYUP(0, SX_INPUT_KEY_MOUSE_RIGHT) )
	{
		m_time = 99999;
		return;
	}

	//	update rendering device
	{
		SoundListener soundListener( float3(0, 0, -10), float3(0, 0, 1), float3(0, 1, 0) );
		sx::snd::Device::SetListener( soundListener );

		float3 camPos = m_camera->GetPosition_world();
		float3 camDir; camDir.Transform_Norm( float3(0, 0, 1), m_camera->GetMatrix_world() );
		float3 camAt = camPos + camDir;
		float3 camUp; camUp.Transform_Norm( float3(0, 1, 0), m_camera->GetMatrix_world() );
		sx::d3d::Device3D::Camera_Pos( m_camera->GetPosition_world(), camAt, camUp );
		sx::d3d::Device3D::Camera_Projection( PI * 0.25f, SEGAN_VP_WIDTH / SEGAN_VP_HEIGHT, 0.1f, 400.0f );

		//  update sun light
		sx::d3d::ShaderPool::SetLight( sx::core::Renderer::GetSunLight() );
		sx::d3d::ShaderPool::Update( elpstime );
	}

	//	update node
	m_node->Update( elpstime );

	//	update texts
	for ( int i=0; i<m_labels.Count(); ++i )
	{
		ComicText& ct = m_labels[i];
		ct.label->Position() = ct.node->GetPosition_world();
		ct.label->Update( elpstime );
	}

	//	play sounds
	for ( int i=0; i<m_sounds.Count(); ++i )
	{
		if ( m_time > m_sounds[i].time )
		{
			msg_SoundPlay msg(false, 0, 0, m_sounds[i].name, m_sounds[0].index );
			m_node->MsgProc( MT_SOUND_PLAY, &msg );
			m_sounds.RemoveByIndex(i--);
		}
	}

	//	play particles
	for ( int i=0; i<m_particles.Count(); ++i )
	{
		if ( m_time > m_particles[i].time )
		{
			sx::core::Node* node = null;
			if ( m_node->GetChildByName( m_particles[i].name, node, true ) )
			{
				msg_Particle msg( SX_PARTICLE_SPRAY );
				node->MsgProc( MT_PARTICLE, &msg );
			}
			m_particles.RemoveByIndex(i--);
		}
	}

	m_time += elpstime;
}

void ComicPlayer::Draw( void )
{
	if ( !sx::core::Renderer::CanRender() ) return;
	sx::core::Renderer::Begin();

	sx::d3d::UI3D::ReadyToDebug( D3DColor(0,0,0,0) );
	sx::d3d::Device3D::Clear_Screen(0);

	//	draw base node
	m_node->Draw(SX_DRAW_PARTICLE | SX_DRAW_MESH);
	m_node->Draw(SX_DRAW_ALPHA | SX_DRAW_PARTICLE | SX_DRAW_MESH);

	//	draw labels
	sx::d3d::UI3D::ReadyToDebug( D3DColor(0,0,0,0) );
	for ( int i=0; i<m_labels.Count(); ++i )
	{
		m_labels[i].label->Draw(0);
	}

	sx::core::Renderer::End();
}

bool ComicPlayer::Playing( void )
{
	return m_time < m_maxTime;
}




void play_comic( const uint id, void* loadingCallback )
{
	ComicPlayer comic;
	comic.Load( g_game->m_strings->Get(id)->text, loadingCallback );

	float initTime = sx::sys::GetSysTime();
	float elpsTime = 0;
	while ( comic.Playing() )
	{
		// calculate elapsed time
		elpsTime = sx::sys::GetSysTime() - initTime;
		initTime = sx::sys::GetSysTime();
		comic.Update( elpsTime );
		comic.Draw();
	}
}
