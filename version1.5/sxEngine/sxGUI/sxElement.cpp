#include "sxElement.h"
#include "../sxCommon/sxCommon.h"
#include "sxControl.h"

namespace sx { namespace gui {

	//////////////////////////////////////////////////////////////////////////
	//	ELEMENT
	float	Element::s_AlphaPercent = 1.0f;
	int		Element::s_number_of_draw = 0;
	Element::Element( Control* mOwner )
		: m_Index(0),
		m_Matrix(math::MTRX_IDENTICAL), 
		m_VB0(NULL), 
		m_VB1(NULL), 
		m_TX(NULL), 
		m_Color(1,1,1,1),
		m_pOwner(mOwner),
		m_OldPlanes(0x00)
	{
		ZeroMemory(m_Vert, sizeof(m_Vert));
	}

	Element::~Element( void )
	{
		if (m_TX)
		{
			m_TX->Deactivate();
			d3d::Texture::Manager::Release(m_TX);
		}
		d3d::Resource3D::ReleaseVertexBuffer(m_VB0);
		d3d::Resource3D::ReleaseVertexBuffer(m_VB1);
	}

	void Element::Save( Stream& S )
	{
		//  save the texture of the element
		String textureSrc;
		if (m_TX)	textureSrc = m_TX->GetSource();
		sx::cmn::String_Save(textureSrc, &S);

		//  save the color of element
		S.Write(&m_Color, sizeof(m_Color));
	}

	void Element::Load( Stream& S )
	{
		//  load the texture of the element
		String textureSrc;
		sx::cmn::String_Load(textureSrc, &S);
		SetTextureSrc(textureSrc);

		//  load the color of element
		S.Read(&m_Color, sizeof(m_Color));
	}

	void Element::SetRect( RectF& rc )
	{
		m_Vert[0].Set(rc.x1, rc.y1, 0.0f);
		m_Vert[1].Set(rc.x2, rc.y1, 0.0f);
		m_Vert[2].Set(rc.x1, rc.y2, 0.0f);
		m_Vert[3].Set(rc.x2, rc.y2, 0.0f);

		if (!m_VB0 && !d3d::Resource3D::CreateVertexBuffer(4 * SEGAN_SIZE_VERTEX_0, m_VB0)) return;
		if (!m_VB1 && !d3d::Resource3D::CreateVertexBuffer(4 * SEGAN_SIZE_VERTEX_1, m_VB1)) return;

		//  Apply Vertices To Buffer
		PD3DVertex0 v0;
		if ( SUCCEEDED( m_VB0->Lock(0, 0, (void**)&v0, 0) ) )
		{
			v0[0].pos = m_Vert[0];
			v0[1].pos = m_Vert[1];
			v0[2].pos = m_Vert[2];
			v0[3].pos = m_Vert[3];
			m_VB0->Unlock();

			PD3DVertex1 v1;
			if ( SUCCEEDED( m_VB1->Lock(0, 0, (void**)&v1, 0) ) )
			{
				v1[0].txc	= Vector2(0.0f, 0.0f);
				v1[1].txc	= Vector2(1.0f, 0.0f);
				v1[2].txc	= Vector2(0.0f, 1.0f);
				v1[3].txc	= Vector2(1.0f, 1.0f);

				for (int i=0; i<4; i++)
				{
					v1[i].nrm	= math::VEC3_ZERO;
					v1[i].col0	= 0xFFFFFFFF;
					v1[i].col1	= 0xFFFFFFFF;
				}

				m_VB1->Unlock();
			}
		}
	}

	void Element::SetRect( math::RectF& rc, float2& uv1, float2& uv2, float2& uv3, float2& uv4 )
	{
		m_Vert[0] = Vector3(rc.x1, rc.y1, 0.0f);
		m_Vert[1] = Vector3(rc.x2, rc.y1, 0.0f);
		m_Vert[2] = Vector3(rc.x1, rc.y2, 0.0f);
		m_Vert[3] = Vector3(rc.x2, rc.y2, 0.0f);

		if (!m_VB0 && !d3d::Resource3D::CreateVertexBuffer(4 * SEGAN_SIZE_VERTEX_0, m_VB0)) return;
		if (!m_VB1 && !d3d::Resource3D::CreateVertexBuffer(4 * SEGAN_SIZE_VERTEX_1, m_VB1)) return;

		//  Apply Vertices To Buffer
		PD3DVertex0 v0;
		if ( SUCCEEDED( m_VB0->Lock(0, 0, (void**)&v0, 0) ) )
		{
			v0[0].pos = m_Vert[0];
			v0[1].pos = m_Vert[1];
			v0[2].pos = m_Vert[2];
			v0[3].pos = m_Vert[3];
			m_VB0->Unlock();

			PD3DVertex1 v1;
			if ( SUCCEEDED( m_VB1->Lock(0, 0, (void**)&v1, 0) ) )
			{
				v1[0].txc	= uv1;
				v1[1].txc	= uv2;
				v1[2].txc	= uv3;
				v1[3].txc	= uv4;

				for (int i=0; i<4; i++)
				{
					v1[i].nrm	= math::VEC3_ZERO;
					v1[i].col0	= 0xFFFFFFFF;
					v1[i].col1	= 0xFFFFFFFF;
				}

				m_VB1->Unlock();
			}
		}
	}

	FORCEINLINE RectF Element::GetRect( void )
	{
		return RectF(m_Vert[0].x, m_Vert[0].y, m_Vert[3].x, m_Vert[3].y);
	}

	FORCEINLINE D3DColor& Element::Color( void )
	{
		return m_Color;
	}

	FORCEINLINE void Element::SetTexture( d3d::PTexture pTxur )
	{
		d3d::Texture::Manager::AddRef(pTxur);
		d3d::Texture::Manager::Release(m_TX);
		m_TX = pTxur;
	}

	FORCEINLINE const d3d::PTexture Element::GetTexture( void )
	{
		return m_TX;
	}

	void Element::SetTextureSrc( const WCHAR* src )
	{
		sx_callstack();

		if (src && m_TX && (wcscmp(m_TX->GetSource(), src) == 0)) 
			return;

		d3d::PTexture TX = NULL;
		if (d3d::Texture::Manager::Get(TX, src)) TX->Activate();

		if (m_TX)
		{
			m_TX->Deactivate();
			d3d::Texture::Manager::Release(m_TX);
		}
		m_TX = TX;
	}

	FORCEINLINE const WCHAR* Element::GetTextureSrc( void )
	{
		if (!m_TX) return NULL;
		return m_TX->GetSource();
	}

	void Element::SetTextureToDevice( UINT stage )
	{
		if (m_TX)
			m_TX->SetToDevice(stage);
		else
			d3d::Device3D::SetTexture(stage, NULL);
	}

	void Element::Draw( DWORD option )
	{
		if (SEGAN_SET_HAS(option, _SX_GUI_NOT_VISIBLE_) || m_Color.a < SX_GUI_MINIMUM_ALPHA) return;

		//  verify the vertex buffers
		if (!m_VB0) SetRect( GetRect() );

		//  verify the texture
		if (m_TX)
			m_TX->SetToDevice();
		else
			d3d::Device3D::SetTexture(0, NULL);

		//  update and correct position of element in 3d space
		static math::Matrix m, mat;
		if (option & _SX_GUI_IN_3DSPACE_)
		{
			mat = m_Matrix; mat._43 += m_Index * SX_GUI_Z_BIAS;
			if (m_pOwner) //  this element has parent?
			{
				m.Multiply(mat, m_pOwner->m_Mtrx);
				d3d::Device3D::Matrix_World_Set(m);
			}
			else d3d::Device3D::Matrix_World_Set(mat);
		}
		else
		{
			if (m_pOwner) //  this element has parent?
			{
				m.Multiply(m_Matrix, m_pOwner->m_Mtrx);
				d3d::Device3D::Matrix_World_Set(m);
			}
			else d3d::Device3D::Matrix_World_Set(m_Matrix);
		}

		//  draw the element
		if (m_pOwner->m_Option & SX_GUI_PROPERTY_IGNOREBLEND)
			d3d::Device3D::SetMaterialColor( D3DColor(m_Color.r, m_Color.g, m_Color.b, m_Color.a * s_AlphaPercent * 2.0f) );
		else
			d3d::Device3D::SetMaterialColor( D3DColor(m_Color.r, m_Color.g, m_Color.b, m_Color.a * s_AlphaPercent) );

		d3d::Device3D::SetIndexBuffer(NULL);
		d3d::Device3D::SetVertexBuffer(0, m_VB0,	SEGAN_SIZE_VERTEX_0);
		d3d::Device3D::SetVertexBuffer(1, m_VB1,	SEGAN_SIZE_VERTEX_1);
		d3d::Device3D::DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		s_number_of_draw++;
	}

	void Element::DrawOutline( float offset )
	{
		d3d::Device3D::SetTexture(0, NULL);
		d3d::Device3D::SetTexture(1, NULL);
		d3d::Device3D::SetTexture(2, NULL);

		if (m_pOwner)
		{
			static math::Matrix m;
			m.Multiply(m_Matrix, m_pOwner->m_Mtrx);
			d3d::Device3D::Matrix_World_Set(m);
		}
		else
			d3d::Device3D::Matrix_World_Set(m_Matrix);

		d3d::Device3D::SetMaterialColor(Globals::Draw_Line_Color());

		D3DVertex0 lineVert[5];
		RectF rc(m_Vert[0].x, m_Vert[0].y, m_Vert[3].x, m_Vert[3].y);
		lineVert[0].pos = Vector3(rc.x2 + offset, rc.y1 + offset, 0.0f);
		lineVert[1].pos = Vector3(rc.x1 - offset, rc.y1 + offset, 0.0f);
		lineVert[2].pos = Vector3(rc.x1 - offset, rc.y2 - offset, 0.0f);
		lineVert[3].pos = Vector3(rc.x2 + offset, rc.y2 - offset, 0.0f);
		lineVert[4].pos = lineVert[0].pos;
		
		d3d::Device3D::DrawPrimitiveUP(D3DPT_LINESTRIP, 4, lineVert, SEGAN_SIZE_VERTEX_0);
	}

	bool Element::CheckCursor( float X, float Y, math::Matrix& matView, math::Matrix& matProjection )
	{
		PD3DViewport vp = d3d::Device3D::Viewport();

		static math::Matrix mat;

		if (m_pOwner)
			mat.Multiply(m_Matrix, m_pOwner->m_Mtrx);

		mat.Multiply(mat, matView);
		mat.Inverse(mat);

		// Compute the vector of the pick ray in screen space
		math::Vector3 v;
		v.x =  ( ( ( 2.0f * X ) / (float)vp->Width  ) - 1 ) / matProjection._11;
		v.y = -( ( ( 2.0f * Y ) / (float)vp->Height ) - 1 ) / matProjection._22;
		v.z =  1.0f;

		// Transform the screen space pick ray into 3D space
		math::Vector3 v_Dir;
		v_Dir.Transform_Norm(v, mat);

		Vector3 v_Orig(mat._41, mat._42, mat._43);

		return	math::IntersectTriangle(v_Orig, v_Dir, m_Vert[0], m_Vert[1], m_Vert[2], Globals::Editor_Mode())
			||	math::IntersectTriangle(v_Orig, v_Dir, m_Vert[3], m_Vert[2], m_Vert[1], Globals::Editor_Mode());
	}

	bool Element::CheckCursor( float X, float Y, math::Matrix& matView, math::Matrix& matProjection, math::Vector2& vOut )
	{
		PD3DViewport vp = d3d::Device3D::Viewport();

		static math::Matrix mat;

		if (m_pOwner)
			mat.Multiply(m_Matrix, m_pOwner->m_Mtrx);

		mat.Multiply(mat, matView);
		mat.Inverse(mat);

		// Compute the vector of the pick ray in screen space
		math::Vector3 v;
		v.x = (float)  ( ( ( 2.0 * X ) / vp->Width  ) - 1 ) / matProjection._11;
		v.y = (float) -( ( ( 2.0 * Y ) / vp->Height ) - 1 ) / matProjection._22;
		v.z =  1.0;

		// Transform the screen space pick ray into 3D space
		math::Vector3 v_Dir;
		v_Dir.Transform_Norm(v, mat);

		Vector3 v_Orig(mat._41, mat._42, mat._43);

		if (math::IntersectTriangle(v_Orig, v_Dir, m_Vert[0], m_Vert[1], m_Vert[2], vOut, Globals::Editor_Mode()))
		{
			return true;
		}
		else
		{
			if (math::IntersectTriangle(v_Orig, v_Dir, m_Vert[3], m_Vert[2], m_Vert[1], vOut, Globals::Editor_Mode()))
			{
				vOut.x = - vOut.x;
				vOut.y = - vOut.y;
				return true;
			}
		}

		return false;
	}

	int& Element::Index( void )
	{
		return m_Index;
	}

	math::Matrix& Element::Matrix( void )
	{
		return m_Matrix;

	}

	void Element::BeginAsClipSpace( void )
	{
		//  save current planes
		d3d::Device3D::GetClipPlane(0, m_Planes[0]);
		d3d::Device3D::GetClipPlane(1, m_Planes[1]);
		d3d::Device3D::GetClipPlane(2, m_Planes[2]);
		d3d::Device3D::GetClipPlane(3, m_Planes[3]);
		d3d::Device3D::RS_Get(D3DRS_CLIPPLANEENABLE, m_OldPlanes);

		//  calculate view position
		static math::Matrix mat;
		d3d::Device3D::Matrix_View_Get(mat);
		mat.Inverse(mat);
		math::Vector3 viewEye(mat._41, mat._42, mat._43);

		//  make 4 point around of quad
		math::Plane plane;
		math::Vector3 a = m_Vert[0];
		math::Vector3 b = m_Vert[1];
		math::Vector3 c = m_Vert[2];
		math::Vector3 d = m_Vert[3];

		//  this element has parent?
		if (m_pOwner)
		{
			mat.Multiply(m_Matrix, m_pOwner->m_Mtrx);
			a.Transform(a, mat);
			b.Transform(b, mat);
			c.Transform(c, mat);
			d.Transform(d, mat);
		}
		else
		{
			a.Transform(a, m_Matrix);
			b.Transform(b, m_Matrix);
			c.Transform(c, m_Matrix);
			d.Transform(d, m_Matrix);
		}

		//  now make planes to clip
		plane.Set(b, a, viewEye);
		if (m_OldPlanes==0)
			d3d::Device3D::SetClipPlane(0, plane);
		else if (plane.d < m_Planes[0].d)
			d3d::Device3D::SetClipPlane(0, plane);


		plane.Set(d, b, viewEye);
		if (m_OldPlanes==0)
			d3d::Device3D::SetClipPlane(1, plane);
		else if (plane.d < m_Planes[1].d)
			d3d::Device3D::SetClipPlane(1, plane);

		plane.Set(c, d, viewEye);
		if (m_OldPlanes==0)
			d3d::Device3D::SetClipPlane(2, plane);
		else if (plane.d < m_Planes[2].d)
			d3d::Device3D::SetClipPlane(2, plane);

		plane.Set(a, c, viewEye);
		if (m_OldPlanes==0)
			d3d::Device3D::SetClipPlane(3, plane);
		else if (plane.d < m_Planes[3].d)
			d3d::Device3D::SetClipPlane(3, plane);

		d3d::Device3D::RS_Set(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 | D3DCLIPPLANE1 | D3DCLIPPLANE2 | D3DCLIPPLANE3);
	}

	void Element::EndAsClipSpace( void )
	{
		d3d::Device3D::SetClipPlane(0, m_Planes[0]);
		d3d::Device3D::SetClipPlane(1, m_Planes[1]);
		d3d::Device3D::SetClipPlane(2, m_Planes[2]);
		d3d::Device3D::SetClipPlane(3, m_Planes[3]);
		d3d::Device3D::RS_Set(D3DRS_CLIPPLANEENABLE, m_OldPlanes);
	}

	float& Element::AlphaPercent( void )
	{
		return s_AlphaPercent;
	}

	int& Element::NumberOfDraw( void )
	{
		return s_number_of_draw;
	}


}}	//	namespace sx { namespace gui {