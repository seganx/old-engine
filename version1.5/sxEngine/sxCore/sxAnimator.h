/********************************************************************
	created:	2011/05/02
	filename: 	sxAnimator.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a node member class to control 
				animations for it's owner. Animator class can load 
				animations and create/manage/animate joints.

				Animator will send joint matrices to the main 
				shader pool. thus any mesh which rendered by animating 
				shaders can use joint matrices to animate model.
*********************************************************************/
#ifndef GUARD_sxAnimator_HEADER_FILE
#define GUARD_sxAnimator_HEADER_FILE

#include "sxNode.h"
#include "sxNodeMember.h"

namespace sx { namespace core {

	/*
	Animator control animations for it's owner. 
	this class can load animations and create/manage/animate joints.
	*/
	class SEGAN_API Animator : public NodeMember
	{
		SEGAN_STERILE_CLASS(Animator);

	public:
		Animator(void);
		~Animator(void);

		//! clear all animations
		void ClearAnimations(void);

		//! return the number of animations
		int AnimationCount(void);

		//! add animation to animation controller by given animation source address and return animation index
		int AddAnimation(const WCHAR* src);

		//! remove animation by given index
		void RemoveAnimation(int index);

		//! return pointer to animation by given index. return NULL for invalid index
		d3d::PAnimation GetAnimation(int index);

		//! return index of animation by given source name. return -1 for invalid source name
		int GetIndexOfAnimation(const WCHAR* src);

		//! set current animation by index.
		void SetAnimationByIndex(int index);

		//! set current animation by src. if no one found which match the source, add animation and switch to that. this function is slow
		void SetAnimationBySrc(const WCHAR* src);

		//! return current animation index. return -1 if no animation set
		int GetAnimationIndex(void);

		//! set current animation speed
		void SetSpeed(float speed);

		//! return current animation speed
		float GetSpeed(void);

		//! set current blending time in seconds
		void SetBlendTime(float time);

		//! get current blending time in seconds
		float GetBlendTime(void);

		//! set current time of animation in seconds
		void SetAnimationTime(float curTime);

		//! return current time of animation in seconds
		float GetAnimationTime(void);

		//! return maximum time of current animation in seconds
		float GetMaxTime(void);

		//! set animation callback to call when animation running
		void SetCallback(CallbackAnimControl AnimCallback, void* userdata);

		//! return number of joints
		int GetJointCount(void);

		//! update animation time and bones. this will call when the parent's "Update" function called
		void Update(float elpsTime);

		//! draw this member depend on entry flag
		void Draw(DWORD flag);

		//! use to communicate by this member
		UINT MsgProc(UINT msgType, void* data);

		//! save this member to the stream
		void Save(Stream& stream);

		//! load data of this member from the stream
		void Load(Stream& stream);

	public:
		//! update joints and fit number of nodes to number of joints
		void FitNodesToJoints(void);

		PMatrix							m_matBase;			//  base matrix array of animation's joint
		PMatrix							m_matAnim;			//  animated matrix array will send to shader pool
		ArrayPNode						m_Nodes;			//  nodes array created as number of joints
		sx::d3d::AnimationController	m_Controller;		//  man animation controller

		int								m_updateBox;		//  use in update bounding box
	};
	typedef Animator *PAnimator;

}}	//	namespace sx { namespace core {

#endif	//	GUARD_sxAnimator_HEADER_FILE