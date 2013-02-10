/********************************************************************
	created:	2012/01/27
	filename: 	sxTypesSound.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some data types and enumeration types
				that used in sound system.
*********************************************************************/
#ifndef GUARD_sxTypesSound_HEADER_FILE
#define GUARD_sxTypesSound_HEADER_FILE

#include "../../sxLib/lib.h"
#include "../sxCommon/sxCommon.h"

//! use these flags to create sound device
#define SX_SND_
#define SX_SND_3D			0x00000001	//! create sound device with 3d support
#define SX_SND_SYNC			0x00000002	//! create sound device without automatic update. so the update function should be call in application
#define SX_SND_SOFTWARE		0x00000004	//! create sound device without hardware process

//! describe that the sound is a sample or stream
enum SoundType
{
	ST_NULL,		//! no sound is loaded
	ST_SAMPLE,		//! sound is a sample and whole data will be placed in memory
	ST_STREAM		//! sound is an music stream file
};


//! describe the state of sound
enum SoundStatus
{
	SS_STOPPED,		//! the sound is stopped and not playing
	SS_PLAYING,		//! the sound is playing
	SS_PAUSED		//! the sound is paused
};


//! describe the sound device driver capabilities
struct SoundDeviceInfo
{
	DWORD	initFlag;			//! initialization flags
	char	name[64];			//! description of the device
	char	driver[64];			//!	the filename of the driver
	int		free3D;				//! number of free 3D sample slots in the hardware
	bool	hasEAX;				//! device supports EAX and has it enabled
	bool	inited;				//! sound device successfully initialized

	SoundDeviceInfo(void) { ZeroMemory(this, sizeof(SoundDeviceInfo)); }
};
typedef SoundDeviceInfo *PSoundDeviceInfo;

//! properties of listener in the scene
struct SoundListener
{
	float3	position;			//! the position of the listener
	float3	front;				//! the direction that the listener's front is pointing
	float3	top;				//! the direction that the listener's top is pointing

	SoundListener(void) : position(0,0,0), front(0,0,0), top(0,0,0) {}
	SoundListener(float3& _pos, float3& _front, float3& _top) : position(_pos), front(_front), top(_top) {}
};
typedef SoundListener *PSoundListener;

//! description of a sound data
struct SoundDataDesc
{
	SoundType	type;
	DWORD		freq;			//! default sample rate.
	DWORD		channels;		//!	number of channels. 1 = mono, 2 = stereo, etc. 
	float		minDistance;	//! default volume is at maximum when the listener is within this distance
	float		maxDistance;	//! default volume stops decreasing when the listener is beyond this distance
	float		lenght;			//! lenght of sound in second(s)
	bool		is3D;			//! sample has 3D functionality enabled

	SoundDataDesc(void): type(ST_NULL), freq(0), channels(0), minDistance(0), maxDistance(0), lenght(0), is3D(false) {}
	SoundDataDesc(SoundType _type): type(_type), freq(0), channels(0), minDistance(0), maxDistance(0), lenght(0), is3D(false) {}
};
typedef SoundDataDesc *PSoundDataDesc;

//! description of sound player
struct SoundPlayerDesc
{
	DWORD		freq;			//! sample rate.
	int			addFreq;		//! additional sample rate
	float		minDistance;	//! channel's volume is at maximum when the listener is within this distance
	float		maxDistance;	//! channel's volume stops decreasing when the listener is beyond this distance
	float		volume;			//! volume 0 (silent) to 1 (full). 
	float		pan;			//! panning position... -1 (full left) to +1 (full right), 0 = center
	bool		is3D;			//! sample has 3D functionality enabled
	bool		loop;			//! sample will replay if position reached at end

	SoundPlayerDesc(void) { ZeroMemory(this, sizeof(SoundPlayerDesc)); }
};
typedef SoundPlayerDesc *PSoundPlayerDesc;

#endif	//	GUARD_sxTypesSound_HEADER_FILE