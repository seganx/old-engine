#include "sxSoundDevice.h"
#include "../sxSystem/sxSystem.h"

#include "bass.h"
#ifndef _SX_BASS_LOADED
#define _SX_BASS_LOADED
#pragma comment(lib, "sxSound/bass.lib")
#endif


//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABES
static  SoundDeviceInfo		s_deviceInfo;
static	float				s_musicVolume = 1;
static	float				s_fxVolume = 1;
static	bool				s_Pause = false;
static	bool				s_Mute = false;




namespace sx { namespace snd {


	void Device::Create( HWND hwndWindow, DWORD flags )
	{
		s_deviceInfo.initFlag = flags;

		// verify that the correct BASS was loaded
		if ( HIWORD( BASS_GetVersion() ) != BASSVERSION )
		{
			sxLog::Log(L"An incorrect version of BASS.DLL was loaded");
			return;
		}

		// first step of gathering sound device information
		BASS_DEVICEINFO deviceInfo;
		for (int i=1; BASS_GetDeviceInfo(i, &deviceInfo); i++)
		{
			if ( deviceInfo.flags & BASS_DEVICE_ENABLED )
			{
				if ( deviceInfo.name )
				{
					size_t len = strlen( deviceInfo.name );
					memcpy( s_deviceInfo.name, deviceInfo.name, len<63 ? len : 63 );
				}
				if ( deviceInfo.driver )
				{
					size_t len = strlen( deviceInfo.driver );
					memcpy( s_deviceInfo.driver, deviceInfo.driver, len<63 ? len : 63 );
				}
				break;
			}
		}

		// prepare flags to initialize sound device
		DWORD soundFlags = 0;
		if ( flags & SX_SND_3D ) soundFlags = BASS_DEVICE_3D;

		// initialize the default sound device depend on flag
		if ( !BASS_Init( -1, 44100, soundFlags, hwndWindow, NULL) )
		{
			switch ( BASS_ErrorGetCode() )
			{
			case BASS_ERROR_DX :		sxLog::Log(L"Initialize sound device failed du to DirectX (or ALSA) is not installed.");	return;
			case BASS_ERROR_DEVICE :	
			case BASS_ERROR_DRIVER :	sxLog::Log(L"Initialize sound device failed du to there is no available device driver. The device may already be in use or is invalid.");	return;
			case BASS_ERROR_UNKNOWN :	sxLog::Log(L"Initialize sound device failed du to mystery problem!"); return;
			case BASS_ERROR_NO3D :		sxLog::Log(L"Initialize sound device with 3D support failed! try to initialize without 3D sound.");
				
				if ( !BASS_Init( -1, 44100, 0, hwndWindow, NULL) )
				{
					sxLog::Log(L"Initialize sound device failed du to mystery problem!");
					return;
				}
			}
		}
		s_deviceInfo.inited = true;

		BASS_INFO bassInfo;
		BASS_GetInfo( &bassInfo );
		s_deviceInfo.free3D = bassInfo.free3d * BASS_Set3DFactors( 1.0f, 1.0f, 0 );
		s_deviceInfo.hasEAX = BASS_SetEAXParameters( -1, 0, -1, -1) > 0;

		if ( flags & SX_SND_SYNC )
		{
			BASS_SetConfig( BASS_CONFIG_UPDATEPERIOD, 0 );
			BASS_SetConfig( BASS_CONFIG_UPDATETHREADS, 0 );
		}

		sxLog::Log( 
			L"Sound device initialize successfully :\r\n\tName :\t\t%s\r\n\tDriver :\t%s\r\n\t3DSlot :\t%d\r\n\tEAX :\t\t%s\r\n\r\n",
			str256(s_deviceInfo.name).Text(),
			str256(s_deviceInfo.driver).Text(),
			s_deviceInfo.free3D,
			s_deviceInfo.hasEAX ? L"Yes" : L"NO"
			);

		sx::snd::Device::SetVolume( s_musicVolume, s_fxVolume );
	}

	void Device::Destroy( void )
	{
		if ( s_deviceInfo.inited )
			BASS_Free();
	}

	const PSoundDeviceInfo Device::GetInfo( void )
	{
		return &s_deviceInfo;
	}

	void Device::SetVolume( float musicVolume, float fxVolume )
	{
// 		if ( s_musicVolume == musicVolume && s_fxVolume == fxVolume ) return;

		s_musicVolume = musicVolume;
		s_fxVolume = fxVolume;

		if ( !s_deviceInfo.inited ) return;

		UINT notMute = s_Mute ? 0 : 1;

		DWORD musicVol = notMute * UINT( s_musicVolume * 10000 );
		SEGAN_BETWEEN( musicVol, 50, 9999 );
		BASS_SetConfig(	BASS_CONFIG_GVOL_STREAM, musicVol );

		DWORD soundVol = notMute * UINT( s_fxVolume * 10000 );
		SEGAN_BETWEEN( soundVol, 50, 9999 );
		BASS_SetConfig(	BASS_CONFIG_GVOL_SAMPLE, soundVol );
	}

	void Device::GetVolume( float& musicVolume, float& fxVolume )
	{
		musicVolume = s_musicVolume;
		fxVolume = s_fxVolume;
	}

	void Device::SetMute( bool mute )
	{
		if ( !s_deviceInfo.inited && mute == s_Mute ) return;
		s_Mute = mute;
		
		UINT notMute = s_Mute ? 0 : 1;

		DWORD musicVol = notMute * UINT( s_musicVolume * 10000 );
		SEGAN_BETWEEN( musicVol, 50, 9999 );
		BASS_SetConfig(	BASS_CONFIG_GVOL_STREAM, musicVol );

		DWORD soundVol = notMute * UINT( s_fxVolume * 10000 );
		SEGAN_BETWEEN( soundVol, 50, 9999 );
		BASS_SetConfig(	BASS_CONFIG_GVOL_SAMPLE, soundVol );
	}

	void Device::SetListener( SoundListener& IN listener )
	{
		if ( !s_deviceInfo.inited ) return;

		BASS_Set3DPosition( 
			(BASS_3DVECTOR*)&listener.position, NULL,
			(BASS_3DVECTOR*)&listener.front,
			(BASS_3DVECTOR*)&listener.top
			);

		BASS_Apply3D();
	}

	void Device::GetListener( SoundListener& OUT listener )
	{
		if ( !s_deviceInfo.inited ) return;

		BASS_Get3DPosition( 
			(BASS_3DVECTOR*)&listener.position, NULL,
			(BASS_3DVECTOR*)&listener.front,
			(BASS_3DVECTOR*)&listener.top
			);
	}

	void Device::Pause( bool pause )
	{
		sx_callstack();

		if ( !s_deviceInfo.inited && pause == s_Pause ) return;
		s_Pause = pause;

		if ( pause )
			BASS_Pause();
		else
			BASS_Start();
	}

	void Device::Update( float elpsTime )
	{
		if ( !s_deviceInfo.inited ) return;
	
		BASS_Update( DWORD(elpsTime) );
	}


} } // namespace sx { namespace snd {