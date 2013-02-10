/********************************************************************
	created:	2012/01/29
	filename: 	sxSoundData.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of sound data to store date
				and share it in other sound players.
*********************************************************************/
#ifndef GUARD_sxSoundData_HEADER_FILE
#define GUARD_sxSoundData_HEADER_FILE

#include "sxTypesSound.h"

namespace sx { namespace snd {

	// forward declaration
	typedef class SoundData *PSoundData;

	/*!
	Sound data class represents sound Sample/Stream resource used in Sound Play object
	Sound data abstract class can't be initialized or create directly. 
	Use PSoundData type and SoundData::Manager to create or release usable Sound Data.
	*/
	class SEGAN_API SoundData
	{
	public:

		//! return the ID of this resource
		virtual DWORD GetID(void) = 0;

		//! set source file of the sound. this call will update ID by CRC32 algorithm
		virtual void SetSource(const WCHAR* srcAddress) = 0;

		//! return the source of the sound file
		virtual const WCHAR* GetSource(void) = 0;

		//! return the sound data information
		virtual const PSoundDataDesc GetDesc(void) = 0;

		//! return the sound handle
		virtual const DWORD GetHandle(void) = 0;

		//! set some new properties
		virtual void SetDesc(SoundDataDesc& desc) = 0;

		/*!
		load a sound data from file with new description.
		NOTE: use this function to preview a sound or import a new Sound.
		NOTE: calling this function without Import flag cause to disable all other functions except CleanUp(void)/GetHandle(); 
		NOTE: calling this function with Import flag case to apply new description
		*/
		virtual bool LoadFromFile(const WCHAR* fileName, SoundDataDesc& newDesc, bool Import = false) = 0;

		//! load a sound from stream
		virtual void Load(Stream& stm, PSoundDataDesc newDesc = NULL) = 0;

		//! clear current data
		virtual void CleanUp(void) = 0;

		//////////////////////////////////////////////////////////////////////////
		//  INTERNAL MANAGER
		//////////////////////////////////////////////////////////////////////////
		class SEGAN_API Manager
		{
		public:

			/*! 
			return true if sound data of the 'src' is exist in the manager and throw out founded one.
			NOTE: this function DO NOT increase the internal reference counter of the object. so releasing
			the object after the work finished may cause to destroy the object.
			*/
			static bool Exist(OUT PSoundData& pSndData, const WCHAR* src);

			/*!
			search for sound data of the src. if not exist in manager, create and return new one.
			NOTE: this function increase the internal reference counter of the object. so release
			the object after the work finished.
			*/
			static bool Get(OUT PSoundData& pSndData, const WCHAR* src);

			//! create a new sound data independent of the others.
			static bool Create(OUT PSoundData& pSndData, const WCHAR* src);

			//! decrease the internal reference counter and destroy the sound data if reference counter became zero.
			static void Release(PSoundData& pSndData);

			//! clear all sound data in the manager
			static void ClearAll(void);
		};

	};
	typedef SoundData *PSoundData;

} } // namespace sx { namespace snd {

#endif	//	GUARD_sxSoundData_HEADER_FILE
