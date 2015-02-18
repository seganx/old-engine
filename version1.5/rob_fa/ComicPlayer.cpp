#include "ComicPlayer.h"
#include "Scripter.h"

ComicPlayer::ComicPlayer( void ): m_time(0), m_maxTime(0), m_node(0) { }

ComicPlayer::~ComicPlayer( void )
{
	sx_delete_and_null(m_node);
}

bool ComicPlayer::Load( const wchar* fileName )
{
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
		}
	}

	//	retrieve total animation time
	{
		msg_Animator msg(SX_ANIMATOR_PLAY, SX_ANIMATOR_LOOP);
		m_node->MsgProc( MT_ANIMATOR, &msg );
		m_maxTime = msg.animMaxTime * 1000.0f;
	}

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

	m_time += elpstime;

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

	//	update rendering device
	{
		SoundListener soundListener( float3(0, 0, -10), float3(0, 0, 1), float3(0, 1, 0) );
		sx::snd::Device::SetListener( soundListener );

		sx::d3d::Device3D::Camera_Pos( float3(5, 5, 5), float3(0, 0, 0) );
		sx::d3d::Device3D::Camera_Projection( sx::math::PIDIV2, SEGAN_VP_WIDTH / SEGAN_VP_HEIGHT, 0.3f, 100.0f );

		//  update sun light
		sx::d3d::ShaderPool::SetLight( sx::core::Renderer::GetSunLight() );
		sx::d3d::ShaderPool::Update( elpstime );
	}

	//	update node
	m_node->Update( elpstime );
}

void ComicPlayer::Draw( void )
{
	if ( !sx::core::Renderer::CanRender() ) return;
	sx::core::Renderer::Begin();

	sx::d3d::UI3D::ReadyToDebug( D3DColor(0,0,0,0) );
	sx::d3d::Device3D::Clear_Screen(0);

	m_node->Draw(SX_DRAW_PARTICLE | SX_DRAW_MESH);
	m_node->Draw(SX_DRAW_ALPHA | SX_DRAW_PARTICLE | SX_DRAW_MESH);

	sx::core::Renderer::End();
}

bool ComicPlayer::Playing( void )
{
	return m_time < m_maxTime;
}



