/********************************************************************
	created:	2012/01/31
	filename: 	sxSound.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class to contorl the sounds as a
				node member. this help us to attach a sound to an object.
*********************************************************************/
#ifndef GUARD_sxSound_HEADER_FILE
#define GUARD_sxSound_HEADER_FILE

#include "sxNodeMember.h"
#include "../sxSound/sxSoundPlayer.h"

namespace sx { namespace core {


	/*
	Sound class use to play sounds in scene
	*/
	class SEGAN_API Sound : public NodeMember
	{	

		SEGAN_STERILE_CLASS(Sound);
	
	public:
		Sound(void);
		virtual ~Sound(void);

		//! add a new option
		void AddOption(DWORD option);

		//! remove option
		void RemOption(DWORD option);

		//!	this will call when the parent's "Update" function called
		void Update(float elpsTime);

		//! draw this member depend on entry flag
		void Draw(DWORD flag);

		//! draw with LOD depend on view parameter. ignore draw for so far objects
		void DrawByViewParam(DWORD flag, float viewParam);

		//! use to communicate by this member
		UINT MsgProc(UINT msgType, void* data);

		//! save this member to the stream
		void Save(Stream& stream);

		//! load data of this member from the stream
		void Load(Stream& stream);

		//! return the current status
		SoundStatus GetStatus(void);

		//! return the description of sound player
		const PSoundPlayerDesc GetDesc(void);

		//! set a new description of sound
		void SetDesc(SoundPlayerDesc& desc);

		//! return th current playing time position in second
		float GetTimePosition(void);

		//! set a new time position is second
		void SetTimePosition(float posTime);

		//! play the sound. if reset then the sound will restart from beginning
		void Play( bool reset, bool force = false );

		//! pause the sound
		void Pause(void);

		//! stop the sound
		void Stop(void);

	//private:

		//! commit description to player
		void CommitDesc(void);

	public:

		sx::cmn::StringList		m_resources;
		SoundPlayerDesc			m_desc;
		snd::SoundPlayer*		m_player;

		float					m_time;
		float					m_timeGetPlayer;
		float					m_repeatTime;
		float					m_chance;
		int						m_index;			//	index of sound resource in the list

	};
	typedef Sound *PSound;


} } // namespace sx { namespace core 

#endif	//	GUARD_sxSound_HEADER_FILE
