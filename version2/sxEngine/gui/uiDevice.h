/********************************************************************
	created:	2013/03/15
	filename: 	uiDevice.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain GUI manager for manage GUI system
*********************************************************************/
#ifndef GUARD_uiDevice_HEADER_FILE
#define GUARD_uiDevice_HEADER_FILE


#include "../../sxLib/Lib.h"


//!	basic element of a graphical user interface
class SEGAN_ENG_API uiElement
{
	SEGAN_STERILE_CLASS(uiElement);
public:

	uiElement( void );
	~uiElement( void );

	//! create vertices
	void CreateVertices( const uint count );

	//! clear all vertices
	void ClearVertives( void );

public:

	uint		m_numVertices;
	float3*		m_pos;
	float2*		m_uv;
	Color*		m_color;
};

class SEGAN_ENG_API uiDevice
{
	SEGAN_STERILE_CLASS(uiDevice);

public:
	uiDevice( void );
	~uiDevice( void );


	//! prepare for batching elements
	void BeginBatchElements( const dword flag );

	//! add an element to the batch and return false if can't batch given element with another
	bool AddBatchElements( const uiElement* elem );

	//! end patch and append them to the end of dest element
	void EndBatchElements( uiElement* dest );

public:

	Array<uielements
};

#endif	//	GUARD_uiDevice_HEADER_FILE