#include "sxSoundPlayer.h"
#include "sxSoundDevice.h"
#include "sxSoundData.h"
#include "bass.h"
#include "../sxSystem/sxSystem.h"


#define SND_LOG_GET_STOP_CHANNEL	0

typedef Map<UINT, DWORD>	sxMapSoundData;

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES
//////////////////////////////////////////////////////////////////////////
static sxMapSoundData	s_mapSoundData;


namespace sx { namespace snd {


	SoundPlayer::SoundPlayer( void ) 
		: m_Handle(0)
		, m_flag(0)
		, m_Position(0,0,0)
		, m_timePos(0)
		, m_volCeoff(1)
		, m_SoundData(0)
	{

	}

	SoundPlayer::~SoundPlayer( void )
	{
		SetSoundData( NULL );
	}

	FORCEINLINE SoundStatus SoundPlayer::GetStatus( void )
	{
		if ( m_Handle )
		{
			DWORD state = BASS_ChannelIsActive( m_Handle );
			switch ( state )
			{
			case BASS_ACTIVE_STALLED:
			case BASS_ACTIVE_PLAYING:	return SS_PLAYING;
			case BASS_ACTIVE_PAUSED:	return SS_PAUSED;
			case BASS_ACTIVE_STOPPED:	return SS_STOPPED;
			}
		}
		
		return SS_STOPPED;
	}

	void SoundPlayer::SetSoundData( const WCHAR* source )
	{
		if (source && m_SoundData && (wcscmp(m_SoundData->GetSource(), source) == 0)) 
			return;

		//  stop current channel
		Stop();

		//  find new one
		PSoundData SN = NULL;
		if ( SoundData::Manager::Get(SN, source) )
		{
			m_Handle = SN->GetHandle();

#if SND_LOG_GET_STOP_CHANNEL
			if ( !m_Handle )
			{
				sxLog::Log( L"ERROR: Can't get handle for sound resource %s", source );
				SoundData::Manager::Release( SN );
				return;
			}
			else
			{
				sxLog::Log( L"INFO: Get handle %u for sound resource %s", m_Handle, source );
			}
#endif

			//  update description
			m_Desc.freq = SN->GetDesc()->freq;
			m_Desc.is3D = SN->GetDesc()->is3D;
			m_Desc.loop = false;
			m_Desc.minDistance = SN->GetDesc()->minDistance;
			m_Desc.maxDistance = SN->GetDesc()->maxDistance;
			m_Desc.pan = 0;
			m_Desc.volume = 1;
		}

		//  release last one
		SoundData::Manager::Release(m_SoundData);
		m_SoundData = SN;
	}

	FORCEINLINE const WCHAR* SoundPlayer::GetSoundData( void )
	{
		if ( m_SoundData )
			return m_SoundData->GetSource();
		else
			return NULL;
	}

	FORCEINLINE const PSoundPlayerDesc SoundPlayer::GetDesc( void )
	{
		return &m_Desc;
	}

	void SoundPlayer::SetDesc( SoundPlayerDesc& desc )
	{
		if ( desc.loop )
			BASS_ChannelFlags(m_Handle, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
		else
			BASS_ChannelFlags(m_Handle, 0, BASS_SAMPLE_LOOP);
		
		DWORD freq = desc.freq + desc.addFreq;
		BASS_ChannelSetAttribute( m_Handle, BASS_ATTRIB_FREQ, (float)freq );
		BASS_ChannelSetAttribute( m_Handle, BASS_ATTRIB_PAN, desc.pan );
		BASS_ChannelSetAttribute( m_Handle, BASS_ATTRIB_VOL, desc.volume * m_volCeoff );
		
		if ( desc.is3D )
		{
			BASS_ChannelSet3DAttributes( m_Handle, BASS_3DMODE_NORMAL, desc.minDistance, -1, -1, -1, 0 );
		}
 		else
 		{
 			BASS_ChannelSet3DAttributes( m_Handle, BASS_3DMODE_OFF, 0, 0, -1, -1, -1 );
 		}

		m_Desc = desc;
	}

	FORCEINLINE void SoundPlayer::Set3DPosition( const float3& inPos )
	{
		m_Position = inPos;
		BASS_ChannelSet3DPosition( m_Handle, (BASS_3DVECTOR*)&inPos, NULL, NULL );
	}

	FORCEINLINE void SoundPlayer::Get3DPosition( float3& outPos )
	{
		outPos = m_Position;
	}

	FORCEINLINE float SoundPlayer::GetTimePosition( void )
	{
		QWORD bytePos = BASS_ChannelGetPosition( m_Handle, BASS_POS_BYTE );
		return (float) BASS_ChannelBytes2Seconds( m_Handle, bytePos );
	}

	FORCEINLINE void SoundPlayer::SetTimePosition( float posTime )
	{
		m_timePos = posTime;
		QWORD bytePos = BASS_ChannelSeconds2Bytes( m_Handle, posTime );
		BASS_ChannelSetPosition( m_Handle, bytePos, BASS_POS_BYTE );
	}

	FORCEINLINE void SoundPlayer::SetVolumeCoefficient( float volumeCoefficient )
	{
		m_volCeoff = sx_min_f( 1.0f, sx_max_f(0, volumeCoefficient) );
		BASS_ChannelSetAttribute( m_Handle, BASS_ATTRIB_VOL, m_Desc.volume * m_volCeoff);
	}

	FORCEINLINE void SoundPlayer::Play( bool reset )
	{
		if ( ! m_SoundData ) return;

		if ( reset || !m_Handle )
		{
			//BASS_ChannelStop( m_Handle );
			m_Handle = m_SoundData->GetHandle();

#if SND_LOG_GET_STOP_CHANNEL
			if ( !m_Handle )
			{
				sxLog::Log( L"ERROR: Can't get handle for sound resource %s", m_SoundData->GetSource() );
			}
			else
			{
				sxLog::Log( L"INFO: Get handle %u for sound resource %s", m_Handle, m_SoundData->GetSource() );
			}
#endif

			SetDesc( m_Desc );
			Set3DPosition( m_Position );
			SetTimePosition( 0 );
			SetVolumeCoefficient( m_volCeoff );
		}

		BASS_ChannelPlay( m_Handle, reset );
	}

	FORCEINLINE void SoundPlayer::Pause( void )
	{
		BASS_ChannelPause( m_Handle );
	}

	void SoundPlayer::Stop( void )
	{
		if ( m_Handle )
		{
			SetTimePosition(0.0f);
#if SND_LOG_GET_STOP_CHANNEL
		if ( BASS_ChannelStop( m_Handle ) == FALSE )
			sxLog::Log( L"ERROR: The sound channel can't be stopped ! Handle : %u , Bass error code : %d", m_Handle, BASS_ErrorGetCode() );
		else
			sxLog::Log( L"INFO: Handle %u stopped successfully for %s", m_Handle, m_SoundData->GetSource() );
#else
			BASS_ChannelStop( m_Handle );
#endif

		}

		m_Handle = 0;
	}

} }	//	namespace sx { namespace snd {