/********************************************************************
	created:	2012/01/28
	filename: 	sxSoundDevice.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the main class of sound system
*********************************************************************/
#ifndef GUARD_sxSoundDevice_HEADER_FILE
#define GUARD_sxSoundDevice_HEADER_FILE

#include "sxTypesSound.h"

namespace sx { namespace snd {

	class SEGAN_API Device
	{
	public:

		static void Create(HWND hwndWindow, DWORD flags);

		static void Destroy(void);

		static const PSoundDeviceInfo GetInfo(void);

		static void SetVolume(float musicVolume, float fxVolume);

		static void GetVolume(float& musicVolume, float& fxVolume);

		static void SetMute(bool mute);

		static void SetListener(SoundListener& IN listener);

		static void GetListener(SoundListener& OUT listener);

		static void Pause(bool pause);

		static void Update(float elpsTime);
	};

} } // namespace sx { namespace snd {

#endif	//	GUARD_sxSoundDevice_HEADER_FILE