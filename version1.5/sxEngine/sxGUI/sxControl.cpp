#include "sxControl.h"
#include "../sxCommon/sxCommon.h"
#include "../sxInput/sxInput.h"


namespace sx { namespace gui {

	//////////////////////////////////////////////////////////////////////////
	//	STATIC PARAMETERS
	//////////////////////////////////////////////////////////////////////////
	static	Control*	s_FocusedContorl	= NULL;	//  internal system of focused control
	static	Control*	s_CapturedContorl	= NULL;	//  internal system of captured control


	//////////////////////////////////////////////////////////////////////////
	//	CONTROL
	//////////////////////////////////////////////////////////////////////////
	Control::Control( void ):
		m_Type(GUI_NONE),
		m_Option(SX_GUI_PROPERTY_VISIBLE | SX_GUI_PROPERTY_ENABLE),
		m_Size(100.0f, 80.0f),
		m_SelectedElement(-1),
		m_Pos(math::VEC2_ZERO),
		m_Rot(math::VEC2_ZERO),
		m_PosOffset(math::VEC2_ZERO),
		m_RotOffset(math::VEC2_ZERO),
		m_SclOffset(1.0f, 1.0f, 1.0f),
		m_Mtrx(math::MTRX_IDENTICAL),
		m_UserData(NULL),
		m_UserTag(0),
		m_Parent(NULL),
		m_LastMouseMessage(0),
		m_OnClick(NULL, NULL),
		m_OnEnter(NULL, NULL),
		m_OnExit(NULL, NULL),
		m_OnMove(NULL, NULL),
		m_OnWheel(NULL, NULL),
		m_OnKeyDown(NULL, NULL)
	{

	}

	Control::~Control( void )
	{
		if ( s_FocusedContorl	== this )		s_FocusedContorl	= NULL;
		if ( s_CapturedContorl	== this )		s_CapturedContorl	= NULL;

		//  clear current childes
		while(m_Child.Count())
		{
			PControl child = m_Child[m_Child.Count()-1];
			sx_delete_and_null(child);
		}

		//  clear current elements
		for (int i=0; i<m_Elements.Count(); i++)
		{
			sx_delete_and_null(m_Elements[i]);
		}
		m_Elements.Clear();

		SetParent(NULL);
	}

	FORCEINLINE void Control::SetName( const WCHAR* pwcName )
	{
		m_Name = pwcName;
	}

	FORCEINLINE const WCHAR* Control::GetName( void )
	{
		return m_Name;
	}

	FORCEINLINE void Control::SetHint( const WCHAR* pwcName )
	{
		m_Hint = pwcName;
	}

	FORCEINLINE const WCHAR* Control::GetHint( void )
	{
		return m_Hint;
	}

	FORCEINLINE GUIControlType Control::GetType( void )
	{
		return m_Type;
	}

	void Control::Save( Stream& S )
	{
		//  version 2 : add Hint

		int version = 2;
		SEGAN_STREAM_WRITE(S, version);

		//  save itself
		sx::cmn::String_Save(m_Name, &S);
		SEGAN_STREAM_WRITE(S, m_Option);
		SEGAN_STREAM_WRITE(S, m_Size);
		SEGAN_STREAM_WRITE(S, m_Pos);
		SEGAN_STREAM_WRITE(S, m_Rot);

		//  save each element
		for (int i=0; i<m_Elements.Count(); i++)
		{
			m_Elements[i]->Save(S);
		}

		//  version 2 : add hint
		sx::cmn::String_Save(m_Hint, &S);
	}

	void Control::Load( Stream& S )
	{
		int version = 0;
		SEGAN_STREAM_READ(S, version);

		if (version >= 1)
		{
			sx::cmn::String_Load(m_Name, &S);
			SEGAN_STREAM_READ(S, m_Option);
			SEGAN_STREAM_READ(S, m_Size);
			SEGAN_STREAM_READ(S, m_Pos);
			SEGAN_STREAM_READ(S, m_Rot);

			for (int i=0; i<m_Elements.Count(); i++)
			{
				m_Elements[i]->Load(S);
			}

			//  apply loaded data
			SetSize(m_Size);
		}

		if (version >= 2)
		{
			sx::cmn::String_Load(m_Hint, &S);
		}
	}

	FORCEINLINE float2 Control::GetSize( void )
	{
		return m_Size;
	}

	void Control::SetSize( float2 S )
	{
		//  align to pixel space
		if (SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_PIXELALIGN))
		{
			S.x = floor(S.x/2) * 2.0f;
			S.y = floor(S.y/2) * 2.0f;
		}
		m_Size = S;
	}

	FORCEINLINE RectF Control::GetRect( void )
	{
		return RectF(-m_Size.x*0.5f, m_Size.y*0.5f, m_Size.x*0.5f, -m_Size.y*0.5f);
	}

	FORCEINLINE void Control::AddProperty( DWORD prop )
	{
		SEGAN_SET_ADD(m_Option, prop);
	}

	FORCEINLINE void Control::RemProperty( DWORD prop )
	{
		SEGAN_SET_REM(m_Option, prop);
	}

	FORCEINLINE bool Control::HasProperty( DWORD prop )
	{
		return SEGAN_SET_HAS(m_Option, prop);
	}

	FORCEINLINE void* Control::GetUserData( void )
	{
		return m_UserData;
	}

	FORCEINLINE void Control::SetUserData( void* userData )
	{
		m_UserData = userData;
	}

	FORCEINLINE DWORD Control::GetUserTag( void )
	{
		return m_UserTag;
	}

	FORCEINLINE void Control::SetUserTag( DWORD userTag )
	{
		m_UserTag = userTag;
	}

	bool Control::GetFocused( void )
	{
		if (s_FocusedContorl == this)
		{
			return true;
		}
		else
		{
			for (int i=0; i<m_Child.Count(); i++)
			{
				if ( m_Child[i]->GetFocused() )
					return true;				
			}

			return false;
		}
	}

	void Control::SetFocused( bool val )
	{
		if ( val && 
			m_Option & SX_GUI_PROPERTY_ACTIVATE	&&
			m_Option & SX_GUI_PROPERTY_ENABLE	&&
			m_Option & SX_GUI_PROPERTY_VISIBLE	&&
			!(m_Option & _SX_GUI_NOT_ENABLE_)	&&
			!(m_Option & _SX_GUI_NOT_VISIBLE_)
			)
			s_FocusedContorl = this;
		else
		{
//			if ( s_FocusedContorl == this )
				s_FocusedContorl = NULL;
		}
	}

	FORCEINLINE Control* Control::GetParent( void )
	{
		return m_Parent;
	}

	void Control::SetParent( Control* p )
	{
		if (p == m_Parent || p == this) return;

		//  remove from last parent
		if (m_Parent) m_Parent->m_Child.Remove(this);

		m_Parent = p;

		//  add to the new parent
		if (m_Parent) m_Parent->m_Child.PushBack(this);
	}

	FORCEINLINE sx::gui::PControl Control::GetChild( int index )
	{
		if (index<0 || index>=m_Child.Count()) return NULL;

		return m_Child[index];
	}

	int Control::GetChild( const WCHAR* pwcName, OUT PControl& pControl )
	{
		String name = pwcName;

		for (int i=0; i<m_Child.Count(); i++)
		{
			if ( name == m_Child[i]->GetName() )
			{
				pControl = m_Child[i];
				return i;
			}
		}

		pControl = NULL;
		return -1;
	}

	void Control::GetChildren( ArrayPControl& OUT controlList )
	{
		for (int i=0; i<m_Child.Count(); i++)
		{
			controlList.PushBack( m_Child[i] );
			m_Child[i]->GetChildren( controlList );
		}
	}

	FORCEINLINE int Control::GetChildCount( void )
	{
		return m_Child.Count();
	}

	FORCEINLINE int Control::GetElementCount( void )
	{
		return m_Elements.Count();
	}

	FORCEINLINE sx::gui::PElement Control::GetElement( int index )
	{
		sx_callstack_push(Control::GetElement(index=%d), index);

		if (index<0 || index>=m_Elements.Count())
			return NULL;
		else
			return m_Elements[index];
	}

	FORCEINLINE sx::gui::PElement Control::GetSelectedElement( void )
	{
		return GetElement(m_SelectedElement);
	}

	FORCEINLINE Vector3& Control::Position( void )
	{
		return m_Pos;
	}

	FORCEINLINE Vector3& Control::Rotation( void )
	{
		return m_Rot;
	}

	FORCEINLINE Vector3& Control::Scale( void )
	{
		static math::Vector3 s_scale(1.0f, 1.0f, 1.0f);
		return s_scale;
	}

	FORCEINLINE Vector3& Control::PositionOffset( void )
	{
		return m_PosOffset;
	}

	FORCEINLINE Vector3& Control::RotationOffset( void )
	{
		return m_RotOffset;
	}

	FORCEINLINE Vector3& Control::ScaleOffset( void )
	{
		return m_SclOffset;
	}

	Matrix& Control::GetMatrix( void )
	{
		return m_Mtrx;
	}

	void Control::Update( float elpsTime )
	{
		if ( !SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ENABLE) )
		{
			SEGAN_SET_ADD(m_Option, _SX_GUI_NOT_ENABLE_);
		} else {
			SEGAN_SET_REM(m_Option, _SX_GUI_NOT_ENABLE_);
		}

		if ( !SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_VISIBLE) )
		{
			SEGAN_SET_ADD(m_Option, _SX_GUI_NOT_VISIBLE_);
			return;
		} else {
			SEGAN_SET_REM(m_Option, _SX_GUI_NOT_VISIBLE_);
		}

		if ( SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_3DSPACE) )
		{
			SEGAN_SET_ADD(m_Option, _SX_GUI_IN_3DSPACE_);
		} else {
			SEGAN_SET_REM(m_Option, _SX_GUI_IN_3DSPACE_);
		}

		//  start building matrix
		BuildMatrix();

		if (m_Parent)
		{	
			if (SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_BINDTOPARENT))
			{
				Vector3 v(m_Mtrx._41, m_Mtrx._42, m_Mtrx._43);
				Vector3 s(1.0f, 1.0f, 1.0f);
				RectF	r(	-m_Size.x*0.5f + v.x + m_Parent->m_Size.x*0.5f,
							 m_Size.y*0.5f + v.y - m_Parent->m_Size.y*0.5f,
							 m_Size.x*0.5f + v.x - m_Parent->m_Size.x*0.5f,
							-m_Size.y*0.5f + v.y + m_Parent->m_Size.y*0.5f );
				
				float w = m_Size.x;
				float h = m_Size.y;

				if (r.x1<0)
				{
					s.x = s.x + r.x1 / w;
					v.x = v.x - r.x1 / 2;
				}
				if (r.y1>0)
				{
					s.y = s.y - r.y1 / h;
					v.y = v.y - r.y1 / 2;
				}
				if (r.x2>0)
				{
					s.x = s.x - r.x2 / w;
					v.x = v.x - r.x2 / 2;
				}
				if (r.y2<0)
				{
					s.y = s.y + r.y2 / h;
					v.y = v.y - r.y2 / 2;
				}

				if (s.x<SX_GUI_MINIMUM_SCALE || s.y<SX_GUI_MINIMUM_SCALE)
				{
					SEGAN_SET_ADD(m_Option, _SX_GUI_NOT_VISIBLE_);
					return;
				}

				math::Matrix matP;
				matP.Scale(s.x, s.y, s.z);
				m_Mtrx._41 = 0.0f;
				m_Mtrx._42 = 0.0f;
				m_Mtrx._43 = 0.0f;

				m_Mtrx.Multiply(m_Mtrx, matP);
				m_Mtrx._41 = v.x;
				m_Mtrx._42 = v.y;
				m_Mtrx._43 = v.z;
			}

			m_Mtrx.Multiply(m_Mtrx,  m_Parent->m_Mtrx);

			if ( m_Parent->m_Option & _SX_GUI_NOT_ENABLE_ )
			{
				SEGAN_SET_ADD(m_Option, _SX_GUI_NOT_ENABLE_);
			}

			//  check control in 3d space
			if ( m_Parent->m_Option & _SX_GUI_IN_3DSPACE_ )
			{
				SEGAN_SET_ADD(m_Option, _SX_GUI_IN_3DSPACE_);
			} else {
				SEGAN_SET_REM(m_Option, _SX_GUI_IN_3DSPACE_);
			}

		}	//	if (m_Parent)

		// after that all let's update the children
		for (int i=m_Child.Count()-1; i>-1; i--)
		{
			if (m_Child[i]->m_Option & _SX_GUI_IN_3DSPACE_)
			{
				m_Child[i]->m_Pos.z += SX_GUI_Z_BIAS;
				m_Child[i]->Update(elpsTime);
				m_Child[i]->m_Pos.z -= SX_GUI_Z_BIAS;
			}
			else m_Child[i]->Update(elpsTime);
		}

		//  now prepare matrix to show a gui
		if (!SEGAN_SET_HAS(m_Option, _SX_GUI_IN_3DSPACE_))
		{
			//  align to pixel space
			if ( SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_PIXELALIGN) )
			{
				m_Mtrx._41 = floor(m_Mtrx._41) - 0.5f * (d3d::Device3D::Viewport()->Width  % 2);
				m_Mtrx._42 = floor(m_Mtrx._42) + 0.5f * (d3d::Device3D::Viewport()->Height % 2);
			}
			else
			{
				m_Mtrx._41 -= 0.5f * !(d3d::Device3D::Viewport()->Width  % 2);
				m_Mtrx._42 += 0.5f * !(d3d::Device3D::Viewport()->Height % 2);
			}
		}
	}

	void Control::ProcessInput( bool& result, int playerID )
	{
		if (
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_VISIBLE)	||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ENABLE)	||
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_)		|| 
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_ENABLE_)
			)	
			return;

		//  keyboard process happened only by focused object
		processInputKeyboard(result, playerID);

		//  process mouse inputs
		processInputMouse(result, playerID);

	}

	void Control::Draw( DWORD option )
	{
		if(	SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_) )
			return;

		bool inSpace3D = ( m_Option & _SX_GUI_IN_3DSPACE_ ) != 0;
		bool just3D = ( option & SX_GUI_DRAW_JUST3D ) != 0;
		bool just2D = ( option & SX_GUI_DRAW_JUST2D ) != 0;
		if ( just2D || just3D )
		{
			if ( just3D != inSpace3D )
				return;
		}

		//  turn alpha blend on
		d3d::Device3D::RS_Alpha( SX_MATERIAL_ALPHABLEND );

		if ( inSpace3D )
		{
			d3d::Device3D::RS_ZEnabled(true);
			for (int i=0; i<m_Elements.Count(); i++) m_Elements[i]->Draw(m_Option | option);
		}
		else
		{
			static sx::math::Matrix mat_proj_last, mat_view_last;
			if (option & SX_GUI_DRAW_SAVEMATRIX)
			{
				d3d::Device3D::Matrix_Project_Get(mat_proj_last);
				d3d::Device3D::Matrix_View_Get(mat_view_last);
			}

			math::Matrix matView = math::MTRX_IDENTICAL;
			math::Matrix matProj(0.0f,0.0f,0.0f,0.0f,0.0f,1.9203779f,0.0f,0.0f,0.0f,0.0f,1.0001251f,1.0f,0.0f,0.0f,-0.50006253f,0.0f);
			matView._43 = Globals::Golden_FOV() * (float)d3d::Device3D::Viewport()->Height;
			matProj._11 = 1.9203779f * ((float)d3d::Device3D::Viewport()->Height / (float)d3d::Device3D::Viewport()->Width);

			d3d::Device3D::Matrix_View_Set(matView);
			d3d::Device3D::Matrix_Project_Set(matProj);
			
			d3d::Device3D::RS_ZEnabled(false);
			for (int i=0; i<m_Elements.Count(); i++) m_Elements[i]->Draw(m_Option | option);

			if (option & SX_GUI_DRAW_SAVEMATRIX)
			{
				d3d::Device3D::Matrix_Project_Set(mat_proj_last);
				d3d::Device3D::Matrix_View_Set(mat_view_last);
			}
		}
	}

	void Control::DrawOutline( void )
	{
		if(	SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_) )
			return;

		//  turn alpha blend on
		d3d::Device3D::RS_Alpha( SX_MATERIAL_ALPHABLEND );

		if (SEGAN_SET_HAS(m_Option, _SX_GUI_IN_3DSPACE_))
		{
			d3d::Device3D::RS_ZEnabled(true);
			for (int i=0; i<m_Elements.Count(); i++) m_Elements[i]->DrawOutline(Globals::Draw_Line_Offset());	
		}
		else
		{
			static sx::math::Matrix mat_proj_last;
			d3d::Device3D::Matrix_Project_Get(mat_proj_last);
			d3d::Device3D::Camera_Projection(Globals::Golden_FOV());

			d3d::Device3D::RS_ZEnabled(false);
			for (int i=0; i<m_Elements.Count(); i++) m_Elements[i]->DrawOutline(Globals::Draw_Line_Offset());

			d3d::Device3D::Matrix_Project_Set(mat_proj_last);
		}

	}

	UINT Control::MsgProc( UINT msgType, void* data )
	{
		return msgType;
	}

	Control* Control::Clone( void )
	{
		Control* pgui = Create( m_Type );
		if (pgui)
		{
			//  serialize current control to memory
			MemoryStream stream;
			Save( stream );

			//  load new control from serialized memory
			stream.SetPos(0);
			pgui->Load( stream );
		}
		return pgui;
	}

	FORCEINLINE Control* Control::GetFocusedControl( void )
	{
		return s_FocusedContorl;
	}

	FORCEINLINE Control*& Control::GetCapturedControl( void )
	{
		return s_CapturedContorl;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	PROTECTED 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Control::BuildMatrix( void )
	{
		//  start building matrix
		if ( m_Option & _SX_GUI_IN_3DSPACE_ )
		{
			math::Matrix matScl; matScl.Scale(0.01f, 0.01f, 0.01f);
			
			m_Mtrx.RotationPYR( m_Rot.x + m_RotOffset.x, m_Rot.y + m_RotOffset.y, m_Rot.z + m_RotOffset.z );

			//  verify that this gui should be display as billboard
			if ( m_Option & SX_GUI_PROPERTY_BILLBOARD && !m_Parent )
			{
				Matrix matView, matViewInv;
				d3d::Device3D::Matrix_View_Get( matView );
				matViewInv.Inverse( matView );

				m_Mtrx.Multiply( m_Mtrx, matViewInv );
			}

			m_Mtrx.Multiply(m_Mtrx, matScl);
			m_Mtrx._41 = m_Pos.x + m_PosOffset.x;
			m_Mtrx._42 = m_Pos.y + m_PosOffset.y;
			m_Mtrx._43 = m_Pos.z + m_PosOffset.z;
		}
		else
		{
			m_Mtrx.RotationPYR( m_Rot.x + m_RotOffset.x, m_Rot.y + m_RotOffset.y, m_Rot.z + m_RotOffset.z );
			
			//  verify that this gui should be display as billboard
			if ( m_Option & SX_GUI_PROPERTY_BILLBOARD && !m_Parent )
			{
				Matrix matView, matProj;
				d3d::Device3D::Matrix_View_Get( matView );
				d3d::Device3D::Matrix_Project_Get( matProj );

				D3DViewport VP = *d3d::Device3D::Viewport();

				float3 viewPos = m_Pos + m_PosOffset;
				viewPos.ProjectToScreen(viewPos, math::MTRX_IDENTICAL, matView, matProj, VP);

				m_Mtrx._41 = viewPos.x - VP.Width*0.5f;
				m_Mtrx._42 = - viewPos.y + VP.Height*0.5f;;
				m_Mtrx._43 = m_Pos.z + m_PosOffset.z;
			}
			else
			{
				m_Mtrx._41 = m_Pos.x + m_PosOffset.x;
				m_Mtrx._42 = m_Pos.y + m_PosOffset.y;
				m_Mtrx._43 = m_Pos.z + m_PosOffset.z;
			}
		}
	}

	int Control::MouseOver( float absX, float absY )
	{
		if(	!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ACTIVATE)	||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_VISIBLE)	||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ENABLE)	||
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_)		|| 
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_ENABLE_)		||
			(Globals::Editor_Mode() && SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_FREEZE)) )
			return -1;
		int result = -1;		

		static math::Matrix matView, matProjection;
		if (SEGAN_SET_HAS(m_Option, _SX_GUI_IN_3DSPACE_))
		{
			d3d::Device3D::Matrix_View_Get(matView);
			d3d::Device3D::Matrix_Project_Get(matProjection);

			for (int i=m_Elements.Count()-1; i>-1; i--)
			{
				if (m_Elements[i]->CheckCursor(absX, absY, matView, matProjection))
				{
					result = i;
					break;
				}
			}	
		}
		else
		{
			matView.LookAtLH(float3(0.0f, 0.0f, - Globals::Golden_FOV() * d3d::Device3D::Viewport()->Height), math::VEC3_ZERO, float3(0.0f, 1.0f, 0.0f));
			matProjection.PerspectiveFovLH(	
				Globals::Golden_FOV(), 
				(float)d3d::Device3D::Viewport()->Width / (float)d3d::Device3D::Viewport()->Height, 
				0.1f, 
				1000.0f);

			for (int i=m_Elements.Count()-1; i>-1; i--)
			{
				if (m_Elements[i]->CheckCursor(absX, absY, matView, matProjection))
				{
					result = i;
					break;
				}
			}
		}

		if (result != m_LastMouseMessage)
		{
			if ( result >- 1 )			m_OnEnter(this);	//  Mouse Entered ...
			else if ( result == -1 )	m_OnExit(this);		//  Mouse Exited ...

			m_LastMouseMessage = result;
		}

		if (result>-1 || m_SelectedElement>-1)
		{
			s_CapturedContorl = this;
			m_OnMove(this);
		}
		else
		{
			//s_CapturedContorl = NULL;
		}

		return result;
	}

	int Control::MouseOver( float absX, float absY, OUT math::Vector2& uv, int element /*= -1 */ )
	{
		if(	!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ACTIVATE)	||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_VISIBLE)	||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ENABLE)	||
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_)		|| 
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_ENABLE_)		||
			(Globals::Editor_Mode() && SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_FREEZE)) )
			return -1;

		static math::Matrix matView, matProjection;
		if (SEGAN_SET_HAS(m_Option, _SX_GUI_IN_3DSPACE_))
		{
			d3d::Device3D::Matrix_View_Get(matView);
			d3d::Device3D::Matrix_Project_Get(matProjection);

			if ( element == -1 )
			{
				for (int i=m_Elements.Count()-1; i>-1; i--)
				{
					if (m_Elements[i]->CheckCursor(absX, absY, matView, matProjection, uv))
						return i;
				}
			}
			else
			{
				if ( m_Elements[element]->CheckCursor(absX, absY, matView, matProjection, uv) )
					return element;
			}			
		}
		else
		{
			matView.LookAtLH(float3(0.0f, 0.0f, - Globals::Golden_FOV() * d3d::Device3D::Viewport()->Height), math::VEC3_ZERO, float3(0.0f, 1.0f, 0.0f));
			matProjection.PerspectiveFovLH(	Globals::Golden_FOV(), 
				(float)d3d::Device3D::Viewport()->Width / (float)d3d::Device3D::Viewport()->Height, 
				0.1f, 1000.0f);

			if ( element == -1 )
			{
				for (int i=m_Elements.Count()-1; i>-1; i--)
				{
					if (m_Elements[i]->CheckCursor(absX, absY, matView, matProjection, uv))
						return i;
				}
			}
			else
			{
				if ( m_Elements[element]->CheckCursor(absX, absY, matView, matProjection, uv) )
					return element;
			}
		}

// 		if (result>-1)
// 		{
// 			s_CapturedContorl = this;
// 			m_OnMove(this);
// 		}
		return -1;
	}

	int Control::MouseDown( float absX, float absY )
	{
		if (m_SelectedElement<0)
		{
			m_SelectedElement = MouseOver(absX, absY);
		}

		if (m_SelectedElement>-1)
			SetFocused( true );

		return m_SelectedElement;
	}

	int Control::MouseUp( float absX, float absY )
	{
		if (m_SelectedElement>-1)
		{
			if (MouseOver(absX, absY)>-1)
				m_OnClick(this);
		}

		int result = m_SelectedElement;
		m_SelectedElement = -1;
		return result;
	}

	DWORD Control::OnKeyDown( DWORD keyCode )
	{
		return keyCode;
	}

	DWORD Control::OnKeyPress( DWORD KeyCode )
	{
		return KeyCode;
	}

	int Control::ProcessKeyboardInput( const char* pKeys, GUICallbackInputToChar InputCharFunc )
	{
		if(	s_FocusedContorl != this || 
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_PROCESSKEY)||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ACTIVATE)	||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_VISIBLE)	||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ENABLE)	||
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_)		|| 
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_ENABLE_)		||
			(Globals::Editor_Mode() && SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_FREEZE)) )
			return 0;

		static float elapedTime	= 0;
		static float lastTime	= sys::GetSysTime();
		elapedTime += sys::GetSysTime() - lastTime;
		lastTime = sys::GetSysTime();

		WORD exKeys =	
			BYTE(pKeys[SX_INPUT_KEY_LSHIFT]>0	|| pKeys[SX_INPUT_KEY_RSHIFT]>0	)	* SX_GUI_KEY_SHIFT		|
			BYTE(pKeys[SX_INPUT_KEY_LCONTROL]>0	|| pKeys[SX_INPUT_KEY_RCONTROL]>0)	* SX_GUI_KEY_CTRL		|
			BYTE(pKeys[SX_INPUT_KEY_LALT]>0		|| pKeys[SX_INPUT_KEY_RALT]>0	)	* SX_GUI_KEY_ALT		|
			BYTE(pKeys[SX_INPUT_KEY_CAPITAL]>0	|| pKeys[SX_INPUT_KEY_CAPITAL]>0	)	* SX_GUI_KEY_CAPITAL	|
			BYTE(pKeys[SX_INPUT_KEY_CAPSLOCK]>0	|| pKeys[SX_INPUT_KEY_CAPSLOCK]>0)	* SX_GUI_KEY_CAPITAL	;

		DWORD KeyDownCode		= MAKELONG(0, exKeys);
		DWORD KeyPressCode		= 0;
		static int CurKey		= 0;
		static float waitTime	= 0;

		for (int i=1; i<0xED; i++)
		{
			if (pKeys[i] == SX_INPUT_STATE_DOWN || (CurKey == i && pKeys[i] == SX_INPUT_STATE_HOLD))
			{
				if (CurKey == i && elapedTime<waitTime) continue;
				waitTime	= CurKey == i ? 50.0f : 600.0f;
				CurKey		= i;
				elapedTime	= 0;

				KeyDownCode = MAKELONG(io::InputButtonToVirtualKey(i), exKeys);

				WORD _KeyPressCode = 0;
				if (InputCharFunc)
				{
					_KeyPressCode = InputCharFunc(i);
				}
				else
				{
					if (pKeys[SX_INPUT_KEY_LSHIFT]>0		||
						pKeys[SX_INPUT_KEY_RSHIFT]>0		||
						pKeys[SX_INPUT_KEY_CAPSLOCK]>0	||
						pKeys[SX_INPUT_KEY_CAPITAL]>0 
						)
						_KeyPressCode = io::InputButtonToUpperACCI(i);
					else
						_KeyPressCode = io::InputButtonToLowerACCI(i);
				}
				if (_KeyPressCode=='\r') _KeyPressCode = '\n';
				KeyPressCode = MAKELONG(_KeyPressCode, exKeys);

				m_OnKeyDown(this);
				break;
			}
			else if (pKeys[i] == SX_INPUT_STATE_UP)
			{
				CurKey = 0;
			}

		}

		bool please_handle_key = true;
		if (KeyDownCode)
		{
			please_handle_key = OnKeyDown(KeyDownCode) > 0;
		}

		if (KeyPressCode && please_handle_key)
		{
			OnKeyPress(KeyPressCode);
		}

		return 1;
	}

	void Control::SetOnMouseClick( PForm pForm, GUICallbackEvent pFunc )
	{
		m_OnClick.m_pForm = pForm;
		m_OnClick.m_pFunc = pFunc;
	}

	void Control::SetOnMouseEnter( PForm pForm, GUICallbackEvent pFunc )
	{
		m_OnEnter.m_pForm = pForm;
		m_OnEnter.m_pFunc = pFunc;
	}

	void Control::SetOnMouseExit( PForm pForm, GUICallbackEvent pFunc )
	{
		m_OnExit.m_pForm = pForm;
		m_OnExit.m_pFunc = pFunc;
	}

	void Control::SetOnMouseMove( PForm pForm, GUICallbackEvent pFunc )
	{
		m_OnMove.m_pForm = pForm;
		m_OnMove.m_pFunc = pFunc;
	}

	void Control::SetOnMouseWheel( PForm pForm, GUICallbackEvent pFunc )
	{
		m_OnWheel.m_pForm = pForm;
		m_OnWheel.m_pFunc = pFunc;
	}

	void Control::SetOnKeyDown( PForm pForm, GUICallbackEvent pFunc )
	{
		m_OnKeyDown.m_pForm = pForm;
		m_OnKeyDown.m_pFunc = pFunc;
	}

	void Control::SetElementCount( int count )
	{
		//  clear last elements
		for (int i=0; i<m_Elements.Count(); i++)
		{
			sx_delete_and_null(m_Elements[i]);
		}
		m_Elements.Clear();

		//  create new elements
		for (int i=0; i<count; i++)
		{
			m_Elements.PushBack( sx_new( Element(this) ) );
		}
	}

	void Control::Draw_Childes( DWORD Option )
	{
		if(	SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_) )
			return;

		if (m_Option & SX_GUI_PROPERTY_CLIPCHILDS)
		{
			for (int i=0; i<m_Child.Count(); i++) 
			{
				m_Elements[0]->BeginAsClipSpace();
				m_Child[i]->Draw(Option);
				m_Elements[0]->EndAsClipSpace();
			}
		}
		else
		{
			for (int i=0; i<m_Child.Count(); i++) 
			{
				m_Child[i]->Draw(Option);
			}
		}
	}

	void Control::DrawLine_Childes( void )
	{
		for (int i=0; i<m_Child.Count(); i++)
			m_Child[i]->DrawOutline();

	}

	void Control::Save_Childes( Stream& S )
	{
		//  save the childes
		int n = m_Child.Count();
		SEGAN_STREAM_WRITE(S, n);
		for (int i=0; i<n; i++)
		{
			GUIControlType type = m_Child[i]->m_Type;
			SEGAN_STREAM_WRITE(S, type);
			m_Child[i]->Save(S);
		}
	}

	void Control::Load_Childes( Stream& S )
	{
		//  clear current childes
		for (int i=0; i<m_Child.Count(); i++)
		{
			sx_delete_and_null(m_Child[i]);
		}
		m_Child.Clear();

		//  load new childes
		int n = 0;
		SEGAN_STREAM_READ(S, n);
		for (int i=0; i<n; i++)
		{
			GUIControlType type = GUI_NONE;
			SEGAN_STREAM_READ(S, type);

			if (type != GUI_NONE)
			{
				PControl tmp = gui::Create(type);
				tmp->Load(S);
				tmp->SetParent(this);
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//  additional functions to better control of input
	void Control::processInputMouse( bool& result, int playerID )
	{
		if(	!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_VISIBLE)	||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ENABLE)	||
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_)		|| 
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_ENABLE_))	return;

		//  get input's cursor
		Input_State_Cursor curPos = *(sx::io::Input::GetCursor_ABS(playerID));
		const char* keys = sx::io::Input::GetKeys(playerID);

		//  check the clip space
		if ( m_Option & SX_GUI_PROPERTY_CLIPCHILDS 
			&& !SEGAN_KEYHOLD(0, SX_INPUT_KEY_MOUSE_LEFT) && !SEGAN_KEYUP(0, SX_INPUT_KEY_MOUSE_LEFT)
			)
		{
			DWORD curOption = m_Option;
			SEGAN_SET_ADD(m_Option, SX_GUI_PROPERTY_ACTIVATE);
			if (Control::MouseOver(curPos.x, curPos.y)<0)
			{
				m_Option = curOption;
				for (int i=m_Child.Count()-1; i>-1; i--)
					m_Child[i]->MouseUp(curPos.x, curPos.y);
				return;
			}
			m_Option = curOption;
		}

		// after that all let's update the children
		for (int i=m_Child.Count()-1; i>-1; i--)
			m_Child[i]->processInputMouse(result, playerID);
		
		if (result)// return;
		{
			curPos.x = -99999999.0f;
			curPos.y = -99999999.0f;
		}

		//  check mouse events
		if (keys[SX_INPUT_KEY_MOUSE_LEFT] == SX_INPUT_STATE_DOWN)
		{
			if (MouseDown(curPos.x, curPos.y) > -1) result = true;
		}
		else if (keys[SX_INPUT_KEY_MOUSE_LEFT] == SX_INPUT_STATE_UP)
		{
			if (MouseUp(curPos.x, curPos.y) > -1) result = true;
		}

		//  check mouse over
		if ( s_FocusedContorl == this || s_CapturedContorl == this || keys[SX_INPUT_KEY_MOUSE_LEFT] == SX_INPUT_STATE_NORMAL )
		{
			if ( MouseOver(curPos.x, curPos.y) > -1 || m_SelectedElement>=0 ) 
				result = true;
		} 
		else MouseOver(curPos.x, curPos.y);


		//  check mouse wheel
		if (keys[SX_INPUT_KEY_MOUSE_WHEEL] != 0)
		{
			if (MouseOver(curPos.x, curPos.y) > -1) 
			{
				m_OnWheel(this);
				result = true;
			}
		}
	}

	void Control::processInputKeyboard( bool& result, int playerID )
	{
		if(	!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_VISIBLE)	||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ENABLE)	||
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_)		|| 
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_ENABLE_))	return;

		//  keyboard will be processed only for focused object.
		if (s_FocusedContorl == this)
		{
			const char* keys = sx::io::Input::GetKeys(playerID);
			ProcessKeyboardInput(keys);
			//result = true;
		}
		else
		{
			for (int i=0; i<m_Child.Count(); i++)
				m_Child[i]->processInputKeyboard(result, playerID);
		}

	}


}} //  namespace sx { namespace gui {