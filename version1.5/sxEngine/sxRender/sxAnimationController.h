/********************************************************************
	created:	2011/04/24
	filename: 	sxAnimationController.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain class of animation controller to control
				animations, switch and blend between animations and etc.
*********************************************************************/
#ifndef GUARD_sxAnimationController_HEADER_FILE
#define GUARD_sxAnimationController_HEADER_FILE

#include "sxAnimation.h"

#define SX_ANIMATION_
#define SX_ANIMATION_PLAY			0x00000001		//! controller play the animation
#define SX_ANIMATION_LOOP			0x00000002		//! controller replay the animation at the end of animation

// forward declaration
namespace sx { namespace d3d {
	typedef class AnimationController *PAnimationController;
}} // namespace sx { namespace d3d {

typedef void (*CallbackAnimControl)(void* Userdata, sx::d3d::PAnimationController Controller, float curTime, float MaxTime);

namespace sx { namespace d3d {

	/*!
	this class hold	control, switch and blend between animations
	*/
	class SEGAN_API AnimationController
	{
		SEGAN_STERILE_CLASS(AnimationController);

	public:
		AnimationController(void);
		~AnimationController(void);

		//! clear all animations
		void ClearAnimations(void);

		//! return the number of animations
		int AnimationCount(void);

		//! add animation to animation controller by given animation source address and return animation index
		int AddAnimation(const WCHAR* src);

		//! remove animation by given index
		void RemoveAnimation(int index);

		//! return pointer to animation by given index. return NULL for invalid index
		PAnimation GetAnimation(int index);

		//! return index of animation by given source name. return -1 for invalid source name
		int GetIndexOfAnimation(const WCHAR* src);

		//! set current animation by index.
		void SetCurrentAnimationByIndex(int index);

		//! set current animation by src. if no one found which match the source, add animation and switch to that
		void SetCurrentAnimationBySrc(const WCHAR* src);

		//! return current animation index. return -1 if no animation set
		int GetCurrentAnimationIndex(void);

		//! set current animation speed
		void SetCurrentSpeed(float speed);

		//! return current animation speed
		float GetCurrentSpeed(void);

		//! set current blending time in seconds
		void SetCurrentBlendTime(float time);

		//! get current blending time in seconds
		float GetCurrentBlendTime(void);

		//! set current time of animation in seconds
		void SetCurrentAnimationTime(float curTime);

		//! return current time of animation in seconds
		float GetCurrentAnimationTime(void);

		//! return maximum time of current animation in seconds
		float GetCurrentMaxTime(void);
		
		//! update animation time and bones
		void Update(float elpsTime, DWORD flag);

		//! set animation callback to call when animation running
		void SetCallback(CallbackAnimControl AnimCallback, void* userdata);

		//! return number of bones
		int GetBoneCount(void);

		//! return the name of specified joint. return NULL for invalid index
		const WCHAR* GetJointName(int jointIndex);

		//! return index of parent of the joint which specified by index. return NULL if no parent found
		int GetJointParent(int jointIndex);

		//! return blended key frame data for specified joint and given time. return NULL for invalid index and invalid time
		PAnimationKeyFrame GetJointFrameByTime(int jointIndex, float animTime);

		//! return blended key frame data for specified joint. return NULL for invalid index
		PAnimationKeyFrame GetJointFrame(int jointIndex);

		//! return base Key frame of specified joint. return NULL in invalid index
		const PAnimationKeyFrame GetJointBase(int boneIndex);

		//! save to the stream
		void Save(Stream& stream);

		//! load from stream
		void Load(Stream& stream);

		//! draw debug mode
		void Debug_Draw(DWORD flag);

	public:
		int								m_lastIndex;		// index of last animation
		int								m_Index;			// index of current animation
		float							m_lastTime;			// time of last animation
		float							m_Time;				// time of animation play back
		float							m_Speed;			// speed of animation play back
		float							m_SpeedScale;		// scale speed of animation play back
		float							m_blendTime;		// time of blending between animations
		float							m_curBlend;			// current blending time
		Array<PAnimation>			m_Anims;			// array of animations
		Array<AnimationKeyFrame>	m_Joints;			// array of joints

		DWORD							m_Option;			//  very useful as controller options
		CallbackAnimControl				m_Callback;			//  animation callback;
		void*							m_Userdata;			//  user data used in animation callback
	};


} }	//  namespace sx { namespace d3d {


#endif	//	GUARD_sxAnimationController_HEADER_FILE

