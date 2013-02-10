/********************************************************************
	created:	2011/04/22
	filename: 	sxAnimation.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain class of animation. this class hold
				some animation data as the number of Animation's Joints.

				note: this class is single instance and reference 
				countable and will use in AnimationContoller class.
*********************************************************************/
#ifndef GUARD_sxAnimation_HEADER_FILE
#define GUARD_sxAnimation_HEADER_FILE

#include "sxAnimationJoint.h"

namespace sx { namespace d3d {

	// forward declaration
	typedef class Animation *PAnimation;

	/*!
	this class hold	some animation data as the number of Animation's Joints 
	and some functions to access and manage them.
	Animation abstract class can't be initialized or create directly. 
	Use PAnimation type and Animation::Manager to create or release usable animation.
	*/
	class SEGAN_API Animation
	{
	public:
		//! return the ID of this resource
		virtual DWORD GetID(void) = 0;

		//! set source file of the animation in *.anm format. this call will update ID by CRC32 algorithm
		virtual void SetSource(const WCHAR* srcAddress) = 0;

		//! return the source of the animation file
		virtual const WCHAR* GetSource(void) = 0;

		//! clear all animation data
		virtual void Clear(void) = 0;

		//! return number of animation joints
		virtual int	AnimJointCount(void) = 0;

		//! add a new animation joint and return a pointer to the new one
		virtual PAnimationJoint AddAnimJoint(const WCHAR* name) = 0;

		//! remove animation joint by given index
		virtual void RemoveAnimJointByIndex(UINT index) = 0;

		//! return pointer to animation joint by given index. return NULL for invalid index
		virtual PAnimationJoint GetAnimJointByIndex(UINT index) = 0;

		//! return index of animation joint. return -1 for invalid pointer
		virtual int GetIndexOfAnimJoint(PAnimationJoint pAnimData) = 0;

		//! return pointer to animation joint by given name. return NULL for invalid name
		virtual PAnimationJoint GetAnimJointByName(const WCHAR* name) = 0;

		//! return pointer to interpolated key frame at specified animation joint and given time. return NULL for invalid index.
		virtual PAnimationKeyFrame GetKeyFrame(UINT index, float fTime) = 0;

		//! return maximum time of this animation
		virtual float GetMaxTime(void) = 0;

		//! save to stream
		virtual void Save(Stream& stream) = 0;

		//! load from stream
		virtual void Load(Stream& stream) = 0;

	public:
		DWORD		m_Option;
		float		m_speed;
		float		m_blendTime;

	public:
		//////////////////////////////////////////////////////////////////////////
		//  INTERNAL MANAGER
		//////////////////////////////////////////////////////////////////////////
		class SEGAN_API Manager
		{
		public:

			/*! 
			return true if animation of the 'src' is exist in the manager and throw out founded one.
			NOTE: this function DO NOT increase the internal reference counter of the object. so releasing
			the object after the work finished may cause to destroy the object.
			*/
			static bool Exist(OUT PAnimation& pAnim, const WCHAR* src);

			/*!
			search for animation of the src. if not exist in manager, create and return new one.
			NOTE: this function increase the internal reference counter of the object. so release
			the object after the work finished.
			*/
			static bool Get(OUT PAnimation& pAnim, const WCHAR* src);

			//! create a new animation independent of the others.
			static bool Create(OUT PAnimation& pAnim, const WCHAR* src);

			//! decrease the internal reference counter and destroy the animation if reference counter became zero.
			static void Release(PAnimation& pAnim);

			//! clear all animation in the manager
			static void ClearAll(void);

			//! get first iterator of animation container
			static Map<UINT, sx::d3d::PAnimation>::Iterator& GetFirst(void);

		};
	};


} }	//  namespace sx { namespace d3d {


#endif	//	GUARD_sxAnimation_HEADER_FILE
