/********************************************************************
	created:	2010/11/23
	filename: 	sxElement.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the element class and base control 
				class of GUI. element class will use in other GUI controls 
				as part of them. for instance a tack bar will have 2 element. 
				one of them	for background and the other for scroll.
*********************************************************************/
#ifndef GUARD_sxElement_HEADER_FILE
#define GUARD_sxElement_HEADER_FILE

#include "sxTypesGUI.h"

namespace sx { namespace gui {

	//  forward declaration
	class Control;

	/*!
	this class will use in other GUI controls as element part of them.
	this contain any properties for drawing a textured rectangle
	*/
	typedef class SEGAN_API Element
	{
		SEGAN_STERILE_CLASS(Element);
	public:
		Element(Control* mOwner = NULL);
		~Element(void);

		//! save this element to the stream ( Texture src and Color )
		void Save(Stream& S);

		//! load this element from stream	( Texture src and Color )
		void Load(Stream& S);

		//! set a rectangle of the gui
		void SetRect(math::RectF& rc);

		//! set a rectangle and texture coordinate of the gui
		void SetRect(math::RectF& rc, float2& uv1, float2& uv2, float2& uv3, float2& uv4);

		//! return the rectangle of the gui
		math::RectF GetRect(void);

		//! reference to color of this element
		D3DColor& Color(void);

		//! set a new texture to element
		void SetTexture(d3d::PTexture pTxur);

		//! return the pointer to the texture currently using
		const d3d::PTexture GetTexture(void);

		//! set texture resource
		void SetTextureSrc(const WCHAR* src);

		//! return texture resource
		const WCHAR* GetTextureSrc(void);

		//! return the reference to the pointer of texture
		d3d::PTexture& GetTextureRes(void);

		//! set the element's texture to the device
		void SetTextureToDevice(UINT usage);

		//! draw this element
		void Draw(DWORD option);

		//! draw a line around this element
		void DrawOutline(float offset);

		//! return true if mouse cursor capture this element
		bool CheckCursor(float absX, float absY, math::Matrix& matView, math::Matrix& matProjection);

		//! return true if mouse cursor capture this element and return the position of intersected point in [-0.5 .. 0.5] range
		bool CheckCursor(float absX, float absY, math::Matrix& matView, math::Matrix& matProjection, math::Vector2& vOut);

		//! reference to index of this element in the control
		int& Index(void);

		//! reference to the world matrix of this element
		math::Matrix& Matrix(void);

		//! begin to use this element as clipping space.
		void BeginAsClipSpace(void);

		//! end to use this element as clipping space
		void EndAsClipSpace(void);

	private:
		int						m_Index;		//	index of this element in the control
		math::Matrix			m_Matrix;		//	world matrix of this element
		math::Vector3			m_Vert[4];		//  position vertex of this element
		PDirect3DVertexBuffer	m_VB0;			//  hold the position vertex buffer
		PDirect3DVertexBuffer	m_VB1;			//  hold the other vertex buffer information
		d3d::PTexture			m_TX;			//	this is the GUI texture.
		D3DColor				m_Color;		//	hold color of this gui

		Control*				m_pOwner;		//	pointer to the owner ot this element

		math::Plane				m_Planes[4];	//	array of planes that will be used in clipping
		DWORD					m_OldPlanes;	//  hold the activated planes in bits

		// some additional static members
		static float			s_AlphaPercent;
		static int				s_number_of_draw;

	public:
		static float&	AlphaPercent(void);
		static int&		NumberOfDraw(void);
	}
	*PElement;
	typedef Array<PElement> PElementArray;



} } // namespace sx { namespace gui {

#endif	//	GUARD_sxElement_HEADER_FILE