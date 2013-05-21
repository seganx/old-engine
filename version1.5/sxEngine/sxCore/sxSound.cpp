#include "sxSound.h"
#include "sxNode.h"
#include "sxRenderer.h"
#include "../sxSound/sxSoundData.h"


#define MAX_SOUNDS	24


struct SoundPlayers
{
	sx::core::Sound*		owner;
	sx::snd::SoundPlayer	player;
	SoundPlayers(void):		owner(0) {};
};

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES
//////////////////////////////////////////////////////////////////////////
static const UINT soundFileID = MAKEFOURCC('S', 'O', 'N', 'D');

static int						s_soundCount			= 0;
static sx::math::Sphere			s_soundSphere			= Sphere( sx::math::VEC3_ZERO, 0.5f );
static sx::math::AABox			s_soundBox				= AABox( float3(-0.15f, -0.15f, -0.15f), float3(0.15f, 0.15f, 0.15f) );
static SoundPlayers*			s_players[MAX_SOUNDS];

sx::snd::PSoundPlayer GetSoundPlayer( sx::core::PSound sound, bool force )
{
	float3& cameraPos	= sx::core::Renderer::GetCamera()->Eye;
	float cameraDisSqr	= cameraPos.Distance_sqr( sound->GetOwner()->GetPosition_world() );

	//	check distance
	float soundDis = sound->m_desc.maxDistance;
	if ( !force && sound->m_desc.is3D && cameraDisSqr > soundDis * soundDis )
	{
		//sound->m_time = 0;//99999.9f;
		return NULL;
	}

	//	search for free player
	for ( int i=0; i<MAX_SOUNDS; i++ )
	{
		SoundPlayers* curplayer = s_players[i];

		if ( !curplayer->owner )
		{
			curplayer->owner = sound;
			return &curplayer->player;
		}
		else if ( !curplayer->player.m_Desc.is3D && curplayer->player.GetStatus() != SS_PLAYING )
		{
			curplayer->owner->m_player = NULL;
			curplayer->owner = sound;
			return &curplayer->player;
		}
	}

	//  if no free sound exist, search for minimum volume
	int index = -1;
	float minVol = FLT_MAX;
	for ( int i=0; i<MAX_SOUNDS; i++ )
	{
		SoundPlayers* curplayer = s_players[i];
		if ( curplayer->player.m_volCeoff < minVol )
		{
			minVol = curplayer->player.m_volCeoff;
			index = i;
		}
	}

	//  if sound is not 3d or force to play just replace player
	if ( !sound->m_desc.is3D || force )
	{
		SoundPlayers* curplayer = s_players[index];
		curplayer->owner->m_player = NULL;
		curplayer->owner = sound;
		return &curplayer->player;
	}
	else	//	verify that volume of this sound is more that which we find
	{
		SoundPlayers* curplayer = s_players[index];

		if ( curplayer->player.GetStatus() != SS_PLAYING )
		{
			curplayer->owner->m_player = NULL;
			curplayer->owner = sound;
			return &curplayer->player;
		}

		if ( curplayer->player.m_Desc.is3D )
		{
			float minDis = sound->m_desc.minDistance;
			float maxDis = sound->m_desc.maxDistance - minDis;
			float distnc = sound->GetOwner()->GetPosition_world().Distance( sx::core::Renderer::GetCamera()->Eye ) - minDis;
			float soundVolume = 1.0f - (distnc / maxDis);

			if ( soundVolume > curplayer->player.m_volCeoff )
			{
				curplayer->owner->m_player = NULL;
				curplayer->owner = sound;
				return &curplayer->player;
			}
		}
	}

	return NULL;
}

void ReleaseSoundPlayer( sx::core::PSound sound )
{
	for ( int i=0; i<MAX_SOUNDS; i++ )
	{
		SoundPlayers* curplayer = s_players[i];
		if ( curplayer->owner == sound )
		{
			sound->m_player = NULL;
			curplayer->player.Pause();
			curplayer->player.Stop();
			curplayer->player.SetSoundData( NULL );
			curplayer->owner = NULL;
			return;
		}
	}
}

namespace sx { namespace core {


	Sound::Sound( void )
		: NodeMember()
		, m_player(0)
		, m_time(0)
		, m_repeatTime(0)
		, m_chance(1)
		, m_index(0)
		, m_timeGetPlayer(0)
	{
		m_Type = NMT_SOUND;

		if ( !s_soundCount )
		{
			for ( int i=0; i<MAX_SOUNDS; i++ )
			{
				s_players[i] = sx_new( SoundPlayers );
			}
		}
		s_soundCount++;
	}

	Sound::~Sound( void )
	{
		sx_callstack();

		ReleaseSoundPlayer( this );

		s_soundCount--;
		if ( !s_soundCount )
		{
			for ( int i=0; i<MAX_SOUNDS; i++ )
			{
				sx_delete_and_null( s_players[i] );
			}

			sx::snd::SoundData::Manager::ClearAll();
		}

		m_resources.Clear();
	}

	void Sound::AddOption( DWORD option )
	{
		NodeMember::AddOption( option );
		m_desc.loop = HasOption( SX_SOUND_LOOP );
		CommitDesc();
	}

	void Sound::RemOption( DWORD option )
	{
		NodeMember::RemOption( option );
		m_desc.loop = HasOption( SX_SOUND_LOOP );
		CommitDesc();
	}

	void Sound::Update( float elpsTime )
	{
		if ( !m_player )
		{
			if ( m_desc.loop && ( m_Option & SX_SOUND_PLAYONLOAD ) /*&& ( m_Option & SX_SOUND_PLAYING )*/ )
			{
				if ( m_repeatTime > 0.1f )
				{
					m_time += elpsTime * 0.001f;
					if ( m_time > m_repeatTime )
					{
						m_time = 0.0f;

						float chance = sx::cmn::Random( 1.0f );
						if ( chance <= m_chance )
						{
							Play( true );
						}
					}
				}
				else
				{
					m_timeGetPlayer += elpsTime;
					if ( m_timeGetPlayer > 200 )
					{
						m_timeGetPlayer = sx::cmn::Random( 64.0f );
						Play( false );
					}
				}
			}

			return;
		}

		if ( elpsTime > 0 && m_player->GetStatus() != SS_PLAYING )
		{
			ReleaseSoundPlayer( this );
			SEGAN_SET_REM( m_Option, SX_SOUND_PLAYING );
			return;
		}

		if ( m_desc.is3D )
		{
			if ( m_player->m_Position != m_Owner->GetPosition_world() )
				m_player->Set3DPosition( m_Owner->GetPosition_world() );


			float minDis = m_desc.minDistance;
			float maxDis = m_desc.maxDistance - minDis;
			float distnc = m_player->m_Position.Distance( Renderer::GetCamera()->Eye ) - minDis;

			m_player->SetVolumeCoefficient( 1.0f - (distnc / maxDis) );
		}
		else m_player->SetVolumeCoefficient( 1.0f );
	}

	void Sound::Draw( DWORD flag )
	{
		if ( flag & SX_DRAW_DEBUG && m_Owner )
		{
			sx::d3d::Device3D::RS_Alpha( 0 );

			Matrix matView;
			d3d::Device3D::Matrix_View_Get( matView );
			matView.Inverse( matView );
			matView.SetTranslation( m_Owner->GetPosition_world().x, m_Owner->GetPosition_world().y, m_Owner->GetPosition_world().z );
			d3d::UI3D::DrawCircle( matView, m_desc.minDistance + sinf( sx::sys::GetSysTime() * 0.001f ) * 0.05f, (flag & SX_DRAW_SELECTED) ? 0xffffff00 : 0xffff22aa );
			d3d::UI3D::DrawCircle( matView, m_desc.maxDistance + sinf( sx::sys::GetSysTime() * 0.001f ) * 0.05f, (flag & SX_DRAW_SELECTED) ? 0xffffffff : 0xffff22aa );

		}
	}

	FORCEINLINE void Sound::DrawByViewParam( DWORD flag, float viewParam )
	{
		Draw( flag );
	}

	UINT Sound::MsgProc( UINT msgType, void* data )
	{
		sx_callstack_param(Sound::MsgProc(msgType=%d), msgType);

		switch ( msgType )
		{
		case MT_ACTIVATE:
			{
				if ( m_Option & SX_SOUND_PLAYONLOAD )
					Play( false );
			}
			break;

		case MT_DEACTIVATE:
			{
				Pause();
			}
			break;

		case MT_COVERBOX:
			{
				(static_cast<PAABox>(data))->CoverAA( s_soundBox );
			}
			break;

		case MT_GETBOX_LOCAL:
			{
				*(static_cast<PAABox>(data)) = s_soundBox;
				return 0;
			}
			break;

		case MT_GETBOX_WORLD:
			{
				(static_cast<POBBox>(data))->Transform( s_soundBox, m_Owner ? m_Owner->GetMatrix_world() : math::MTRX_IDENTICAL );
				return 0;
			}
			break;

		case MT_GETSPHERE_LOCAL:
			{
				*(static_cast<PSphere>(data)) = s_soundSphere;
				return 0;
			}
			break;

		case MT_GETSPHERE_WORLD:
			{
				(static_cast<PSphere>(data))->Transform( s_soundSphere, m_Owner->GetMatrix_world() );
				return 0;
			}
			break;

		case MT_INTERSECT_RAY:
			{
				// test ray intersection and return quickly if test failed
				msg_IntersectRay* ray = static_cast<msg_IntersectRay*>(data);
				if ( !SEGAN_SET_HAS(ray->type, NMT_SOUND) ) return msgType;

				//  test sphere
				float3 hitPoint, hitNorm;
				Sphere sphere(
					m_Owner->GetPosition_world(), 
					m_desc.minDistance > 1.0f ? m_desc.minDistance : 1.0f
					);
				if ( ray->ray.Intersect_Sphere( sphere, &hitPoint, &hitNorm) )
				{
					float dis = hitPoint.Distance( ray->ray.pos );
					ray->pushToResults(dis, hitPoint, hitNorm, m_Owner, this);
				}
			}
			break;

		case MT_SOUND_PLAY:
			{
				if ( data )
				{
					msg_SoundPlay* msgSound = static_cast<msg_SoundPlay*>(data);
					if ( msgSound->option && !HasOption( msgSound->option ) ) return msgType;
					if ( msgSound->name && m_Name != msgSound->name )	return msgType;

					if ( msgSound->volume >= 0 )
					{
						m_desc.volume = msgSound->volume;
						CommitDesc();
					}
					else msgSound->volume = m_desc.volume;

					if ( msgSound->index > -1 )
					{
						if ( msgSound->index < m_resources.Count() )
							m_index = msgSound->index;
					}
					
					if ( m_desc.addFreq != msgSound->addFreq )
					{
						m_desc.addFreq = msgSound->addFreq;
						CommitDesc();
					}
 					
					Play( msgSound->restart );
				}
				else Play(false);
			}
			break;

		case MT_SOUND_STOP:
			{
				if ( data )
				{
					msg_SoundStop* msgSound = static_cast<msg_SoundStop*>(data);
					if ( msgSound->option && !HasOption( msgSound->option ) ) return msgType;
					if ( msgSound->name && m_Name != msgSound->name )	return msgType;

					if ( msgSound->fadeout > 0 )
					{
						if ( m_player )
							m_player->Fadeout( msgSound->fadeout );

					}
					else
					{
						if ( msgSound->pause )
							Pause();
						else
							Stop();
					}

				}
				else Stop();
			}
			break;
		}

		return msgType;
	}

	void Sound::Save( Stream& stream )
	{
		SEGAN_STREAM_WRITE(stream, soundFileID);

		// write version
		int version = 3;
		SEGAN_STREAM_WRITE(stream, version);

		//  save default things
		NodeMember::Save( stream );

		// write sound resource
		int n = m_resources.Count();
		SEGAN_STREAM_WRITE(stream, n);
		for ( int i=0; i<n; i++ )
			cmn::String_Save( *m_resources.At(i), &stream );

		// write player description
		SEGAN_STREAM_WRITE(stream, m_desc);

		//	write loop data
		SEGAN_STREAM_WRITE(stream, m_repeatTime);
		SEGAN_STREAM_WRITE(stream, m_chance);
		SEGAN_STREAM_WRITE(stream, m_index);
	}

	void Sound::Load( Stream& stream )
	{
		UINT id = 0;
		SEGAN_STREAM_READ(stream, id);
		if ( id != soundFileID )
		{
			sxLog::Log(L"Incompatible file format for loading sound !");
			return;
		}

		// read version
		int version = 0;
		SEGAN_STREAM_READ(stream, version);

		if (version == 1)
		{
			//  load default things
			NodeMember::Load( stream );

			// read sound resource
			m_resources.Clear();
			String resource;
			cmn::String_Load( resource, &stream );
			if ( resource.Length() )
				m_resources.PushBack( resource );

			//  load new desc
			SEGAN_STREAM_READ(stream, m_desc);

			//  apply new desc
			SetDesc( m_desc );
		}

		else if (version == 2)
		{
			//  load default things
			NodeMember::Load( stream );

			// read sound resource
			m_resources.Clear();
			String resource;
			cmn::String_Load( resource, &stream );
			if ( resource.Length() )
				m_resources.PushBack( resource );

			//  load new desc
			SEGAN_STREAM_READ(stream, m_desc);

			//  apply new desc
			SetDesc( m_desc );

			//	read loop data
			SEGAN_STREAM_READ(stream, m_repeatTime);
			SEGAN_STREAM_READ(stream, m_chance);
		}

		else if (version == 3)
		{
			//  load default things
			NodeMember::Load( stream );

			// read sound resource
			m_resources.Clear();
			int n = 0;
			SEGAN_STREAM_READ(stream, n);
			if ( n )
			{
				String resource;
				for ( int i=0; i<n; i++ )
				{
					cmn::String_Load( resource, &stream );
					m_resources.PushBack( resource );

					//	just fake load to avoid releasing sound data in game
					snd::SoundData* soundData = NULL;
					snd::SoundData::Manager::Get( soundData, resource );
				}
			}

			//  load new desc
			SEGAN_STREAM_READ(stream, m_desc);

			//  apply new desc
			SetDesc( m_desc );

			//	read loop data
			SEGAN_STREAM_READ(stream, m_repeatTime);
			SEGAN_STREAM_READ(stream, m_chance);
			SEGAN_STREAM_READ(stream, m_index);
		}


		m_time = 99999.0f;
	}

	FORCEINLINE SoundStatus Sound::GetStatus( void )
	{
		if ( m_player )
			return m_player->GetStatus();
		else
			return SS_STOPPED;
	}

	FORCEINLINE const PSoundPlayerDesc Sound::GetDesc( void )
	{
		return &m_desc;
	}

	FORCEINLINE void Sound::SetDesc( SoundPlayerDesc& desc )
	{
		m_desc = desc;
		
		if ( desc.loop )
			NodeMember::AddOption( SX_SOUND_LOOP );
		else
			NodeMember::RemOption( SX_SOUND_LOOP );

		CommitDesc();
	}

	FORCEINLINE float Sound::GetTimePosition( void )
	{
		if ( m_player )
			return m_player->GetTimePosition();
		else
			return 0;
	}

	FORCEINLINE void Sound::SetTimePosition( float posTime )
	{
		if ( m_player )
			m_player->SetTimePosition( posTime );
	}

	void Sound::Play( bool reset, bool force /*= false */ )
	{
		if ( m_index<0 || m_index >= m_resources.Count() ) return;
		String* strSrc = m_resources.At(m_index);
		if ( !strSrc ) return;

		if ( !m_player || *strSrc != m_player->GetSoundData() )
		{
			m_player = GetSoundPlayer( this, force );
			if ( m_player )
			{
				const WCHAR* resource = strSrc->Text();

				m_player->Pause();
				m_player->Stop();
				m_player->SetSoundData( resource );

				if ( !m_desc.freq && m_player->m_SoundData )
					m_desc.freq = m_player->m_SoundData->GetDesc()->freq;

				CommitDesc();
				Update(-1);
				m_player->Play( reset );

				//	just fake load to avoid releasing sound data in game
// 				sx::snd::SoundData* soundData = NULL;
// 				sx::snd::SoundData::Manager::Get( soundData, resource );
			}
		}
		else
		{
			if ( reset || m_player->GetStatus() != SS_PLAYING )
				m_player->Play( reset );
		}

		SEGAN_SET_ADD( m_Option, SX_SOUND_PLAYING );
	}

	FORCEINLINE void Sound::Pause( void )
	{
		if ( m_player )
			m_player->Pause();

		SEGAN_SET_REM( m_Option, SX_SOUND_PLAYING );
	}

	void Sound::Stop( void )
	{
		if ( m_player )
		{
			m_player->SetTimePosition(0.0f);
			ReleaseSoundPlayer( this );
		}

		SEGAN_SET_REM( m_Option, SX_SOUND_PLAYING );
	}

	void Sound::CommitDesc( void )
	{
		if ( !m_player ) return;
		if ( m_repeatTime > 0.1f )
		{
			bool loop = m_desc.loop;
			m_desc.loop = false;
			m_player->SetDesc( m_desc );
			m_desc.loop = loop;
		}
		else
		{
			m_player->SetDesc( m_desc );
		}
	}

} } // namespace sx { namespace core {