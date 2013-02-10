/********************************************************************
	created:	2011/04/21
	filename: 	sxAnimationData.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain AnimationData class which contain
				list of animation key frames and some functions to 
				manage them.

				note: this class is single instance and will nor have
				more that one reference pointer
*********************************************************************/
#ifndef GUARD_sxAnimationData_HEADER_FILE
#define GUARD_sxAnimationData_HEADER_FILE


#include "sxRender_def.h"

//! key frame structure holds state of bones in specified time
typedef struct AnimationKeyFrame 
{
	float		time;			//  time in second
	float3		position;		//  vector position
	floatQ		rotation;		//	quaternion rotation

	AnimationKeyFrame(): time(0.0f), position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f, 1.0f) {}
	bool operator == (AnimationKeyFrame& a) {return time==a.time && position==a.position && rotation==a.rotation;}
}
*PAnimationKeyFrame;


namespace sx { namespace d3d {

	//! this class contain an array of key frames and some functions to access and manage them
	class SEGAN_API AnimationJoint
	{
		SEGAN_STERILE_CLASS(AnimationJoint);
	public:
		AnimationJoint(void);
		~AnimationJoint(void);

		//! clear all key frames from animation data
		void Clear(void);

		//! return number of key frames in the animation data
		int	KeyFrameCount(void);

		//! add a new key frame at the specified time and return a pointer to the new key frame
		PAnimationKeyFrame AddKeyFrame(float fTime);

		//! remove key frame by given index
		void RemoveKeyFrameByIndex(UINT index);

		//! return pointer to key frame by given index. return NULL for invalid index
		PAnimationKeyFrame GetKeyFrameByIndex(UINT index);

		//! return pointer to interpolated key frame at given time.
		PAnimationKeyFrame GetKeyFrame(float fTime);

		//! save data to stream
		void Save(Stream& stream);

		//! load data from stream
		void Load(Stream& stream);

	public:
		String						m_Name;			//  name of the animation data
		String						m_Parent;		//  name of the parent
		AnimationKeyFrame				m_Base;			//  base key frame
		float							m_MaxTime;		//  maximum time of animation

	private:
		//! use newton algorithm to find key frame on time
		void FindKeyFrames(float time, int& lo, int& hi);

		AnimationKeyFrame				m_curFrame;		//  current blended key frame
		Array<AnimationKeyFrame>	m_Keys;			//  array of animation keys
	};
	typedef AnimationJoint *PAnimationJoint;


} }	//  namespace sx { namespace d3d {


#endif	//	GUARD_sxAnimationData_HEADER_FILE