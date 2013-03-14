/********************************************************************
	created:	2013/03/13
	filename: 	Element.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain basic element of a graphical user interface
*********************************************************************/
#ifndef GUARD_Element_HEADER_FILE
#define GUARD_Element_HEADER_FILE

#include "../../sxLib/Lib.h"

class SEGAN_ENG_API Element
{
	SEGAN_STERILE_CLASS(Element);
public:

	Element( void );
	~Element( void );

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


//! prepare for batching elements
SEGAN_ENG_API void sx_element_begin_batch( void );

//! add an element to the batch and return false if can't batch more elements
SEGAN_ENG_API bool sx_element_add_batch( const Element* elem );

//! end patch and append them to the end of dest element
SEGAN_ENG_API void sx_element_end_batch( Element* dest );


#endif	//	GUARD_Element_HEADER_FILE

