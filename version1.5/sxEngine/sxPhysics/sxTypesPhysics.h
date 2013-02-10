/********************************************************************
	created:	2011/02/06
	filename: 	sxTypesPhysics.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some data types and enumeration types
				that used in physics.
*********************************************************************/
#ifndef GUARD_sxTypesPhysics_HEADER_FILE
#define GUARD_sxTypesPhysics_HEADER_FILE

#include "sxPhysics_def.h"

/*!
describe type of a rigid body
*/
enum PhysicsRigidType {
	PRT_BOX,
	PRT_SPHERE,
	PRT_CYLINDER,
	PRT_CAPSUL,
	PRT_CONE,
	PRT_CONVEX,			//  convex hull rigid body
	PRT_COMPOUND,		//  compound of some convex collision types
	PRT_MESH			//  static mesh rigid
};
#define PRT_

//! these flags used as gui properties
#define SX_PHX_PROPERTY_FREEZE			0x00000001		//  the rigid state goes to freeze
#define SX_PHX_PROPERTY_DESABLE			0x00000002		//  disable collision detection in the physics world

#endif	//	GUARD_sxTypesPhysics_HEADER_FILE