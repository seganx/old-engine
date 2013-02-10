/********************************************************************
	created:	2011/11/28
	filename: 	Component.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the abstract layer of components 
				will used ny entities
*********************************************************************/
#ifndef GUARD_Component_HEADER_FILE
#define GUARD_Component_HEADER_FILE

#include "ImportEngine.h"

class Entity;


//! use this abstract class as grandpa of other game components
class Component
{
public:

	Component(void);
	virtual ~Component(void);

	//! set owner of this state
	void SetOwner(Entity* owner);

	//! return owner
	Entity* GetOwner(void);

	//! initialize Component will called after the entity initialized
	virtual void Initialize(void);

	//! finalize Component will called before entity finalized
	virtual void Finalize(void);

	//! update component
	virtual void Update(float elpsTime);

	//! handle the incoming message
	virtual void MsgProc(UINT msg, void* data);

	//! clone this component and return new one
	virtual Component* Clone(void) = 0;

public:
	str512		m_name;			//	name of the component
	UINT		m_tag;			//	user tag
	bool		m_deleteMe;		//	notify to delete this component in the next update

protected:
	Entity*		m_owner;		//  owner of this object

	friend class Entity;
};
typedef Component *PComponent;
typedef Array<Component*> arrayPComponent;

#endif	//	GUARD_Component_HEADER_FILE
