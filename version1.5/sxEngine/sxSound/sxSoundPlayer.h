/********************************************************************
	created:	2012/01/29
	filename: 	sxSoundPlayer.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a playable sound class to play sounds
				from sound data stored in SoundData class
*********************************************************************/
#ifndef GUARD_sxSoundPlayer_HEADER_FILE
#define GUARD_sxSoundPlayer_HEADER_FILE

#include "sxTypesSound.h"

namespace sx { namespace snd {

	//  forward declaration
	typedef class SoundData *PSoundData;

	//! use sound player to play a sound data
	class SEGAN_API SoundPlayer
	{

		SEGAN_STERILE_CLASS(SoundPlayer);

	public:
		SoundPlayer(void);
		~SoundPlayer(void);

		SoundStatus GetStatus(void);

		void SetSoundData(const WCHAR* source);

		const WCHAR* GetSoundData(void);

		const PSoundPlayerDesc GetDesc(void);

		void SetDesc(SoundPlayerDesc& desc);

		void Set3DPosition(const float3& inPos);

		void Get3DPosition(float3& outPos);

		float GetTimePosition(void);

		void SetTimePosition(float posTime);

		void SetVolumeCoefficient(float volumeCoefficient);

		void Play(bool reset);

		void Pause(void);

		void Stop(void);

		void Fadeout( float fadetime );

	public:

		float3			m_Position;		//  position of the sound
		float			m_timePos;		//	time position
		float			m_volCeoff;		//	volume coefficient
		DWORD			m_Handle;		//  handle of the sound
		SoundPlayerDesc	m_Desc;			//	description of the sound
		PSoundData		m_SoundData;	//  sound data

		DWORD			m_flag;			//  useful flag
	};
	typedef SoundPlayer *PSoundPlayer;
	typedef Array<PSoundPlayer>	arrayPSoundPlayer;

} } // namespace sx { namespace snd {

#endif	//	GUARD_sxSoundPlay_HEADER_FILE
