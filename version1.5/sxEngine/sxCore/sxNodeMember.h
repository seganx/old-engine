/********************************************************************
	created:	2010/11/05
	filename: 	sxNodeMember.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the class of Node's Member. this 
				class will be the base of all other features that will
				attached to the node of the engine.
*********************************************************************/
#ifndef GUARD_sxNodeMember_HEADER_FILE
#define GUARD_sxNodeMember_HEADER_FILE

#include "sxCore_def.h"
#include "../sxSystem/sxSystem.h"

namespace sx { namespace core {

	//  forward declaration
	typedef class SEGAN_API Node *PNode;

	/*
	this class is the base of other features that will attach to the node of the engine.
	*/
	class SEGAN_API NodeMember
	{	
		SEGAN_STERILE_CLASS(NodeMember);
	
	public:
		NodeMember(void);
		virtual ~NodeMember(void);

		//! set a new name
		virtual void SetName(const WCHAR* pwcName);

		//! return the name of the member
		const WCHAR* GetName(void);

		//! return the type of the member
		NodeMemberType GetType(void);

		//! add a new option
		virtual void AddOption(DWORD option);

		//! remove option
		virtual void RemOption(DWORD option);

		//! return true if option exist
		bool HasOption(DWORD option);

		//! set an owner for this member. this will send SetOwner message to the MsgProc
		virtual void SetOwner(PNode pOwner);

		//! get the current owner
		PNode GetOwner(void);

		//!	this will call when the parent's "Update" function called
		virtual void Update(float elpsTime);

		//! draw this member depend on entry flag
		virtual void Draw(DWORD flag);

		//! draw with LOD depend on view parameter. ignore draw for so far objects
		virtual void DrawByViewParam(DWORD flag, float viewParam);

		//! use to communicate by this member
		virtual UINT MsgProc(UINT msgType, void* data);

		//! save this member to the stream
		virtual void Save(Stream& stream);

		//! load data of this member from the stream
		virtual void Load(Stream& stream);

	public:
		String		m_Name;		//  the name of the node member

	protected:
		NodeMemberType		m_Type;		//  type of this node
		PNode				m_Owner;	//  address of the owner of this member
		DWORD				m_Option;	//  use DWORD option to control the behavior

		friend class Node;

	public:
		//! type constructor use to construct node members by type
		static NodeMember* Create(NodeMemberType type);
	
	};
	typedef NodeMember *PNodeMember;
	typedef Array<PNodeMember>	ArrayPNodeMember;


} } // namespace sx { namespace core 

#endif	//	GUARD_sxNodeMember_HEADER_FILE