/********************************************************************
	created:	2011/02/06
	filename: 	sxRigid.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of physics rigid body. this
				class will implemented in a node member to enter to
				the scene. also this may be implemented in other physics
				objects as rigid body.
*********************************************************************/
#ifndef GUARD_sxRigid_HEADER_FILE
#define GUARD_sxRigid_HEADER_FILE

#include "sxTypesPhysics.h"

namespace sx { namespace phx {


	//  use this class to create/destroy and modify a physics rigid body.
	class SEGAN_API Rigid
	{
		SEGAN_STERILE_CLASS(Rigid);

	public:
		Rigid(void);
		~Rigid(void);

		//! create a simple rigid body
		void CreateSimpleRigid(PhysicsRigidType PRT_ type, Vector3& size, Matrix& offset);

		//! create a convex hull rigid body from the given geometry
		void CreateConvexHullRigid(PVector3 vertices, int count, Matrix& offset);

		//! create an static mesh rigid body from given geometry
		void CreateStaticMeshRigid(PVector3 vertices, PDWORD indices, int faceCount, Matrix& offset);

		//! destroy created rigid body from physics engine
		void DestroyRigid(void);

		//! return type of rigid body
		PhysicsRigidType GetType(void);

		//! return the size of simple rigid. return zero for convex hull and static mesh
		Vector3 GetSize(void);

		//! set a new mass to the rigid
		void SetMass(float mass);

		//! return the mass of rigid
		float GetMass(void);

		//! set force to the rigid. it's not effect on static meshes
		void SetForce(Vector3& value);

		//! return the current force applying on rigid
		Vector3 GetForce(void);

		//! set torque to the rigid. it's not effect on static mesh
		void SetTorque(Vector3& value);

		//! return the current torque applying on rigid
		Vector3 GetTorque(void);

		//! set impulse to the rigid. it's not effect on static mesh
		void SetImpulse(Vector3& value);

		//! return the current impulse applying on rigid
		Vector3 GetImpulse(void);

		//! set user data to the rigid
		void SetUserData(void* value);

		//! return current user data
		void* GetUserData(void);

		//! add a new property 'SX_PHX_PROPERTY_'
		void AddProperty(DWORD prop);

		//! remove a property from current property set 'SX_PHX_PROPERTY_'
		void RemProperty(DWORD prop);

		//! return true if this control has specified property 'SX_PHX_PROPERTY_'
		bool HasProperty(DWORD prop);

		//! save the current rigid to the stream ( all EXCEPT UserData )
		void Save( Stream& S );

		//! load rigid properties from stream	( all EXCEPT UserData )
		void Load( Stream& S );

		//! create and return a rigid body just like this
		Rigid* Clone(void);
	};


}} // namespace sx { namespace phx {

#endif	//	GUARD_sxRigid_HEADER_FILE