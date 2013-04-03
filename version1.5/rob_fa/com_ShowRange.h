/********************************************************************
	created:	2011/11/30
	filename: 	com_ShowRange.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple component to show tower range
*********************************************************************/
#ifndef GUARD_ShowRange_HEADER_FILE
#define GUARD_ShowRange_HEADER_FILE

#include "ImportEngine.h"
#include "Component.h"

class Entity;

//! a simple component to show tower range
class com_ShowRange : public Component
{
public:

	com_ShowRange(void);
	virtual ~com_ShowRange(void);

	//! initialize Component will called after the entity initialized
	void Initialize(void);

	//! finalize Component will called before entity finalized
	void Finalize(void);

	//! update component
	void Update(float elpsTime);

	//! handle the incoming message
	void MsgProc(UINT msg, void* data);

	//! clone this component and return new one
	Component* Clone(void);

public:

	//! set the color value of show range
	void SetColor(D3DColor color);

};

#endif	//	GUARD_ShowRange_HEADER_FILE
