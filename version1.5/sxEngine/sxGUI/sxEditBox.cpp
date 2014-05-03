#include "sxEditBox.h"
#include "sxFont.h"
#include "../sxInput/sxInput.h"


#define IncreaseCaretX( p )									\
	{														\
		p.x++;												\
		if (p.x>m_Lines[p.y]->text.Length())				\
		{													\
			p.y += (m_Align == GTA_RIGHT) ?	-1 : 1;			\
			if (p.y>=0 && p.y<m_Lines.Count())				\
				p.x = 0;									\
		}													\
	}														\

#define DecreaseCaretX( p )									\
	{														\
		p.x--;												\
		if (p.x<0)											\
		{													\
			p.y += (m_Align == GTA_RIGHT) ?	1 : -1;			\
			if (p.y>=0 && p.y<m_Lines.Count())				\
				p.x = m_Lines[p.y]->text.Length();		\
		}													\
		if (p.x<0) p.x=0;									\
		if (p.y<0) p.y=0;									\
	}														\

#define sortCarets( from, to )								\
	{														\
		if ((from.y>to.y)||(from.y==to.y && from.x>to.x))	\
		{													\
			PointI tmp = from;								\
			from = to;										\
			to = tmp;										\
		}													\
	}														\

bool isInSelection(int Ln, int Col, PointI& p1, PointI& p2)
{
	return	(p1.x != p2.x || p1.y != p2.y)	&& 
			(
			(p1.y < Ln	&& Ln < p2.y)				||
			(p1.y== Ln	&& Ln < p2.y && p1.x<=Col)	||
			(p1.y < Ln	&& Ln ==p2.y && Col<p2.x)	||
			(p1.y==p2.y	&& Ln ==p2.y && p1.x<=Col && Col<p2.x)
			);
}


namespace sx { namespace gui {

	// hold all the languages codes. these code will select by indexing variable
	static const DWORD langList[] ={0x00000100, 0x00000200, 0x00000400, 0x00000800, 0x00001000, 0x00002000, 0x00004000, 0x00008000,
									0x00010000, 0x00020000, 0x00040000, 0x00080000, 0x00100000, 0x00200000, 0x00400000, 0x00800000,
									0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000,	0x80000000};

	// use this static global variable too specify which language is selected
	static DWORD s_SelectedLanguage = 0x00000100;

	//  define this exKey variable to be update on key press and used in other places
	static WORD	s_exKey = 0;

	//////////////////////////////////////////////////////////////////////////
	//	EDIITBOX
	//////////////////////////////////////////////////////////////////////////
	TextEdit::TextEdit( void ): Label(), m_CaretPos(0, 0), m_selStart(0, 0), m_selEnd(0, 0), m_Language(GIL_ENGLISH | GIL_PERSIAN)
	{
		m_Type = GUI_TEXTEDIT;

		//  create three element for background, text and cursor
		SetElementCount(3);

		SetSize(float2(150.0f, 100.0f));

		//SetFont(L"Font_Default.fnt");

		SetCaretPos(0, 0);

		AddProperty(SX_GUI_PROPERTY_PROCESSKEY | SX_GUI_PROPERTY_ACTIVATE);
	}

	TextEdit::~TextEdit( void )
	{
		
	}

	void TextEdit::SetSize( float2 S )
	{
		Label::SetSize(S);
		S *= 0.5f;
		m_Elements[0]->SetRect( RectF(-S.x, S.y + 2.0f, S.x, -S.y + 1.0f) );
		m_Elements[2]->SetRect( RectF(-1.0f, 1.0f, 0.0f, -1.0f) );
	}

	void TextEdit::Update( float elpsTime )
	{
		static float fTime = 0;
		Control::Update(elpsTime);
	
		if ( GetFocusedControl() == this )
		{
			fTime += elpsTime;
			math::Vector3 pos(m_Elements[2]->Matrix()._41, m_Elements[2]->Matrix()._42, m_Elements[2]->Matrix()._43);
			m_Elements[2]->Matrix().Scale(1.0f, 0.5f * abs(sin(fTime/100)) * (float)m_Font->GetDesc().LineHeight , 1.0f);
			m_Elements[2]->Matrix().SetTranslation(pos.x, pos.y, pos.z);
		}
	}

	void TextEdit::Draw( DWORD option )
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
			m_Elements[0]->Draw(option);
			DrawSelection(option);
			Label::DrawText(option);
			DrawCaret(option);
		}
		else
		{
			static sx::math::Matrix mat_proj_last, mat_view_last;
			if (option & SX_GUI_DRAW_SAVEMATRIX)
			{
				d3d::Device3D::Matrix_Project_Get(mat_proj_last);
				d3d::Device3D::Matrix_View_Get(mat_view_last);
			}			
			
			static math::Matrix matView = math::MTRX_IDENTICAL;
			static math::Matrix matProj(0.0f,0.0f,0.0f,0.0f,0.0f,1.9203779f,0.0f,0.0f,0.0f,0.0f,1.0001251f,1.0f,0.0f,0.0f,-0.50006253f,0.0f);

			matView._43 = Globals::Golden_FOV() * (float)d3d::Device3D::Viewport()->Height;
			matProj._11 = 1.9203779f * ((float)d3d::Device3D::Viewport()->Height / (float)d3d::Device3D::Viewport()->Width);

			d3d::Device3D::Matrix_View_Set(matView);
			d3d::Device3D::Matrix_Project_Set(matProj);

			d3d::Device3D::RS_ZEnabled(false);
			m_Elements[0]->Draw(option);
			DrawSelection(option);
			Label::DrawText(option);
			DrawCaret(option);

			if (option & SX_GUI_DRAW_SAVEMATRIX)
			{
				d3d::Device3D::Matrix_Project_Set(mat_proj_last);
				d3d::Device3D::Matrix_View_Set(mat_view_last);
			}
		}

		Control::Draw_Childes(option);
	}

	void TextEdit::DrawOutline( void )
	{
		if(	SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_) )
			return;

		//  turn alpha blend on
		d3d::Device3D::RS_Alpha( SX_MATERIAL_ALPHABLEND );

		if (SEGAN_SET_HAS(m_Option, _SX_GUI_IN_3DSPACE_))
		{
			d3d::Device3D::RS_ZEnabled(true);
			m_Elements[0]->DrawOutline(Globals::Draw_Line_Offset());
		}
		else
		{
			static sx::math::Matrix mat_proj_last, mat_view_last;
			d3d::Device3D::Matrix_Project_Get(mat_proj_last);
			d3d::Device3D::Matrix_View_Get(mat_view_last);

			d3d::Device3D::Camera_Pos(float3(0.0f, 0.0f, - Globals::Golden_FOV() * (float)d3d::Device3D::Viewport()->Height), math::VEC3_ZERO);
			d3d::Device3D::Camera_Projection(Globals::Golden_FOV());


			d3d::Device3D::RS_ZEnabled(false);
			m_Elements[0]->DrawOutline(Globals::Draw_Line_Offset());

			d3d::Device3D::Matrix_Project_Set(mat_proj_last);
			d3d::Device3D::Matrix_View_Set(mat_view_last);
		}
	}

	void TextEdit::SetText( const WCHAR* text )
	{
		Label::SetText(text);
		SetCaretPos( m_CaretPos.y, m_CaretPos.x );
	}

	void TextEdit::DrawCaret( DWORD option )
	{
		if ( GetFocusedControl() != this || !m_Font) return;

		RectF rc = GetRect();
		if (m_Elements[2]->Matrix()._41 < rc.x1	|| 
			m_Elements[2]->Matrix()._41 > rc.x2	||
			m_Elements[2]->Matrix()._42 > GetRect().y1 - (m_Font->GetDesc().LineHeight*0.5f)	||
			m_Elements[2]->Matrix()._42 < GetRect().y2 + (m_Font->GetDesc().LineHeight*0.5f)	)
		{
			return;
		}

		d3d::Device3D::Alpha_Function(D3DBLEND_INVDESTCOLOR, D3DBLEND_INVDESTCOLOR);
		m_Elements[2]->Draw(option);
		d3d::Device3D::Alpha_Reset();
	}

	void TextEdit::DrawSelection( DWORD option )
	{
		if (!m_selCount) return;

		if (!m_VB0 || !m_IB)
		{
			BurnBuffer();
			return;
		}

		d3d::Device3D::SetTexture(0, NULL);
		d3d::Device3D::SetVertexBuffer(0, m_VB0, SEGAN_SIZE_VERTEX_0);
		d3d::Device3D::SetVertexBuffer(1, m_VB1, SEGAN_SIZE_VERTEX_1);
		d3d::Device3D::SetIndexBuffer(m_IB);

		d3d::Device3D::SetMaterialColor(D3DColor(
			1 - m_Elements[1]->Color().r, 
			1 - m_Elements[1]->Color().g, 
			1 - m_Elements[1]->Color().b, 
			m_Elements[1]->Color().a));
		
		//  start drawing text with clip plane enabled
		m_Elements[0]->BeginAsClipSpace();

		//  apply text scrolling before render
		math::Matrix matScl = math::MTRX_IDENTICAL;
		matScl._41 = -(float)m_Scroll.x;
		matScl.Multiply(matScl, m_Mtrx);
		d3d::Device3D::Matrix_World_Set(matScl);
		d3d::Device3D::DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, m_numChar*4, m_selCount*4, m_numChar*6, m_selCount*2);

		m_Elements[0]->EndAsClipSpace();
	}

	void TextEdit::SetCaretPos( int Ln, int Col )
	{
		if (m_Lines.Count()) 
		{
			SEGAN_CLAMP(Ln,	 0, m_Lines.Count()-1);
			SEGAN_CLAMP(Col, 0, m_Lines[Ln]->text.Length());
		}
		else
		{
			Ln	= 0;
			Col = 0;
		}

		if (m_CaretPos.x != Col || m_CaretPos.y != Ln)
		{
			m_CaretPos.x = Col; 
			m_CaretPos.y = Ln;
			m_selStart = m_CaretPos;
		
			//  update selection position
			static PointI s_selLast = m_CaretPos;
			if (SEGAN_SET_HAS(s_exKey, SX_GUI_KEY_SHIFT) && !SEGAN_SET_HAS(s_exKey, SX_GUI_KEY_ALT))
			{
				if (m_selStart.x != s_selLast.x || m_selStart.y != s_selLast.y)
				{
					BurnBuffer();
				}
			}
			else
			{
				bool doBurn = m_selStart.x != m_selEnd.x || m_selStart.y != m_selEnd.y;
				m_selStart	= m_CaretPos;
				m_selEnd	= m_CaretPos;
				if (doBurn) BurnBuffer();
			}

			s_selLast = m_selStart;
		}

		UpdateCaretOnPixel();

		// well now update scroll system when the caret goes out side of the rect
		if (m_Font && m_CaretPos.y >= (m_Scroll.y + int(m_Size.y / (float)m_Font->GetDesc().LineHeight)))
		{
			SetScrollValue(m_Scroll.x, m_CaretPos.y - int(m_Size.y / (float)m_Font->GetDesc().LineHeight) + 1);
		} 
		else if (m_CaretPos.y < m_Scroll.y)
		{
			SetScrollValue(m_Scroll.x, m_CaretPos.y);
		}

		RectF rc = GetRect(); rc.x1 += 2.0f;
		if (m_Elements[2]->Matrix()._41 < rc.x1)
		{
			SetScrollValue(int(m_Scroll.x + m_Elements[2]->Matrix()._41 - rc.x1), m_Scroll.y);
		}
		else if (m_Elements[2]->Matrix()._41 > rc.x2)
		{
			SetScrollValue(int(m_Scroll.x + m_Elements[2]->Matrix()._41 - rc.x2), m_Scroll.y);
		}

	}

	PointI TextEdit::GetCaretPos( void )
	{
		return m_CaretPos;
	}

	void TextEdit::SetInputLanguages( GUIInputLanguage inputLang )
	{
		BYTE langindex = SEGAN_1TH_BYTEOF(m_Language);
		m_Language = inputLang;
		SEGAN_1TH_BYTEOF(m_Language) = langindex;
	}

	void TextEdit::TextInsert( PointI& p, const WCHAR* text )
	{
		if (!text) return;
		if (m_Lines.Count()<1) m_Lines.PushBack( sx_new(GUITextLine) );
		SEGAN_CLAMP(p.y, 0, m_Lines.Count()-1);

		while (*text)
		{
			PGUIFontChar pfnch = NULL;
			if (m_Font->GetChar(*text, pfnch) || *text == '\n')
			{
				//  verify multi line support
				if (*text == '\n' && !SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_MULTILINE))
				{
					text++;
					continue;
				}

				//  add character to the text editor
				int x = p.x + 1; int y = p.y;
				m_Lines[p.y]->text.Insert(*text, p.x);
				UpdateLine(p.y);

				p.x = 0;
				for (int i=0; i<x; i++)	IncreaseCaretX(p);
				if (y != p.y || *text=='\n') IncreaseCaretX(p);
			}

			text++;
		}

		BurnBuffer();
		s_exKey = 0;
		SetCaretPos(p.y, p.x);

		m_OnTextChange(this);
	}

	void TextEdit::TextDelete( PointI& p )
	{
		if (m_Lines.Count()<1) m_Lines.PushBack( sx_new(GUITextLine) );
		SEGAN_CLAMP(p.y, 0, m_Lines.Count()-1);
		int len = m_Lines[p.y]->text.Length();
		if (len < 1) return;

		int x = p.x; int y = p.y;
		if (p.x == len)
		{
			//  verify that this line terminated with return character
			if (m_Lines[p.y]->text[len-1] == '\n')
			{
				m_Lines[p.y]->text.Delete(len-1);
				x--;
			}
			else if (p.y+1 < m_Lines.Count())
				m_Lines[p.y+1]->text.Delete(0);
		}
		else
			m_Lines[p.y]->text.Delete(p.x);

		if (p.y+1 < m_Lines.Count())
		{
			m_Lines[p.y]->text << m_Lines[p.y+1]->text;
			sx_delete_and_null( m_Lines[p.y+1] );
			m_Lines.RemoveByIndex(p.y+1);
		}
		
		UpdateLine(p.y);
		BurnBuffer();

		p.x = 0;
		for (int i=0; i<x; i++)	IncreaseCaretX(p);
		if (y != p.y) IncreaseCaretX(p);

		m_OnTextChange(this);
	}

	void TextEdit::TextDelete( PointI from, PointI to )
	{
		if (m_Lines.Count()<1) m_Lines.PushBack( sx_new(GUITextLine) );
		SEGAN_CLAMP(from.y, 0, m_Lines.Count()-1);
		SEGAN_CLAMP(from.x, 0, m_Lines[from.y]->text.Length());
		SEGAN_CLAMP(to.y, 0, m_Lines.Count()-1);
		SEGAN_CLAMP(to.x, 0, m_Lines[to.y]->text.Length());

		if ( to.y < from.y || (to.y == from.y && to.x < from.x) )
		{
			PointI tmp = from;
			from = to;
			to = tmp;
		}
		
		//  remove full selected lines between these lines
		while (to.y - from.y > 1)
		{
			to.y--;
			sx_delete_and_null( m_Lines[to.y] );
			m_Lines.RemoveByIndex(to.y);
		}

		//  remove remain characters
		while (to.x != from.x || to.y != from.y)
		{
			int charIndex = to.x;
			if (to.x==0) DecreaseCaretX(to);
			DecreaseCaretX(to);
			if (to.x!=charIndex || to.y>0)
			{
				//TextDelete(to);
				int len = m_Lines[to.y]->text.Length();
				if (len < 1) break;

				int x = to.x; int y = to.y;
				if (to.x == len)
				{
					//  verify that this line terminated with return character
					if (m_Lines[to.y]->text[len-1] == '\n')
					{
						m_Lines[to.y]->text.Delete(len-1);
						x--;
					}
					else if (to.y+1 < m_Lines.Count())
						m_Lines[to.y+1]->text.Delete(0);
				}
				else
					m_Lines[to.y]->text.Delete(to.x);

				if (to.y+1 < m_Lines.Count())
				{
					m_Lines[to.y]->text << m_Lines[to.y+1]->text;
					sx_delete_and_null( m_Lines[to.y+1] );
					m_Lines.RemoveByIndex(to.y+1);
				}

				UpdateLine(to.y);

				to.x = 0;
				for (int i=0; i<x; i++)	IncreaseCaretX(to);
				if (y != to.y) IncreaseCaretX(to);
			}
		}

		//  finally burn the buffer
		SetCaretPos(to.y, to.x);
		BurnBuffer();

		m_OnTextChange(this);
	}

	int TextEdit::MouseOver( float absX, float absY )
	{
		if (m_SelectedElement != 1)
			return Control::MouseOver(absX, absY);

		Vector2 uv;
		int result = Control::MouseOver(absX, absY, uv);
		if ( GetFocusedControl() == this && result == 1 )
		{
			uv.x = uv.x + 0.5f;
			uv.y = 0.5f - uv.y;

			float w = GetRect().x2;
			float h = GetRect().y1;
			PointI p = GetCaretFromPixel(uv.x*w*2 - w, h - uv.y*h*2);
			SetCaretPos(p.y, p.x);
		}
		
		return result;
	}

	DWORD TextEdit::OnKeyDown( DWORD keyCode )
	{
		//  switch between permitted languages 
		if ((HIWORD(keyCode) == (SX_GUI_KEY_ALT | SX_GUI_KEY_SHIFT)) && 
			(LOWORD(keyCode) == VK_SHIFT || LOWORD(keyCode) == VK_MENU))
		{
			for (int i=0; i<24; i++)
			{	
				SEGAN_1TH_BYTEOF(m_Language) += 1;
				if (SEGAN_1TH_BYTEOF(m_Language) > 23) SEGAN_1TH_BYTEOF(m_Language) = 0;

				// verify that this language is permitted
				BYTE langIndex = SEGAN_1TH_BYTEOF(m_Language);
				if (langList[ langIndex ] & m_Language) break;
			}

			return 0;
		}

		GUIFontDesc desc = m_Font->GetDesc();
		PointI p = m_CaretPos;
		int charIndex = p.x;
		WCHAR pch = 0;
		switch (LOWORD(keyCode))
		{
		case VK_RETURN:
			if ( ! SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_MULTILINE) )
			{
				m_OnTextChange(this);

				return 0;
			}
			break;

		case VK_LEFT:	

			DecreaseCaretX(p);

			if (s_exKey & SX_GUI_KEY_CTRL )
			{
				pch = p.x>0 ? m_Lines[p.y]->text[p.x-1] : m_Lines[p.y]->text[p.x];
				while (p.x && (pch > 1000 || (pch!=0 && pch!=' ' && pch!='\t' && pch!='.' && pch!=',' && pch!='\r' && pch!='\n')))
				{
					DecreaseCaretX(p);
					pch = p.x>0 ? m_Lines[p.y]->text[p.x-1] : m_Lines[p.y]->text[p.x];
				}
			}

			SetCaretPos(p.y, p.x);

			return 0;

		case VK_RIGHT:	

			if (s_exKey & SX_GUI_KEY_CTRL )
			{
				pch = p.y<m_Lines.Count() ? m_Lines[p.y]->text[p.x] : 0;
				while ((pch > 1000 || (pch!=0 && pch!=' ' && pch!='\t' && pch!='.' && pch!=',' && pch!='\r' && pch!='\n')))
				{
					IncreaseCaretX(p);
					pch = p.y<m_Lines.Count() ? m_Lines[p.y]->text[p.x] : 0;
				}
			}

			IncreaseCaretX(p);
			SetCaretPos(p.y, p.x);

			return 0;

		case VK_UP:

			p = GetCaretFromPixel(m_Elements[2]->Matrix()._41, m_Elements[2]->Matrix()._42 + desc.LineHeight);
			SetCaretPos(p.y, p.x);

			return 0;

		case VK_DOWN:	

			p = GetCaretFromPixel(m_Elements[2]->Matrix()._41, m_Elements[2]->Matrix()._42 - desc.LineHeight);
			SetCaretPos(p.y, p.x);

			return 0;

		case VK_HOME:
			SetCaretPos(p.y, 0);
			return 0;

		case VK_END:
			SetCaretPos(p.y, m_Lines[p.y]->text.Length());
			return 0;

		case VK_BACK:
			if (m_Option & SX_GUI_PROPERTY_READONLY) return 0;

			if (m_selStart == m_selEnd)
			{
				if (p.x==0) DecreaseCaretX(p);
				DecreaseCaretX(p);
				if (p.x!=charIndex || p.y>0)
				{
					TextDelete(p);
					SetCaretPos(p.y, p.x);
				}
			}
			else TextDelete(m_selStart, m_selEnd);
			
			return 0;

		case VK_DELETE:
			if (m_Option & SX_GUI_PROPERTY_READONLY) return 0;

			if (m_selStart == m_selEnd)
			{
				TextDelete(p);
				SetCaretPos(p.y, p.x);
			}
			else TextDelete(m_selStart, m_selEnd);

			return 0;

		case VK_TAB:
			SetFocused(false);

			return 0;
		}

		return 1;
	}

	DWORD TextEdit::OnKeyPress( DWORD KeyCode )
	{
		if (m_Option & SX_GUI_PROPERTY_READONLY) return 0;

		WORD extnCode = HIWORD(KeyCode);
		WORD CharCode = LOWORD(KeyCode);
		if (CharCode<1 || SEGAN_SET_HAS(extnCode, SX_GUI_KEY_ALT) || SEGAN_SET_HAS(extnCode, SX_GUI_KEY_CTRL)) return 0;
	
		if (m_selStart != m_selEnd) TextDelete(m_selStart, m_selEnd);
		
		const WCHAR c[2] = {CharCode, 0};
		TextInsert(PointI(m_CaretPos.x, m_CaretPos.y), c);

		return 1;
	}

	int TextEdit::ProcessKeyboardInput( const char* pKeys, GUICallbackInputToChar InputCharFunc /*= NULL*/ )
	{
		if(	GetFocusedControl() != this || 
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_PROCESSKEY)|| !SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_VISIBLE)	||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ENABLE)	||	SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_VISIBLE_)		||
			!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_ACTIVATE)	||
			SEGAN_SET_HAS(m_Option, _SX_GUI_NOT_ENABLE_)		||
			(Globals::Editor_Mode() && SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_FREEZE)) )
			return 0;

		s_SelectedLanguage = langList[ SEGAN_1TH_BYTEOF(m_Language) ];

		s_exKey =
			BYTE(pKeys[SX_INPUT_KEY_LCONTROL]>0	|| pKeys[SX_INPUT_KEY_RCONTROL]>0)	* SX_GUI_KEY_CTRL		|
			BYTE(pKeys[SX_INPUT_KEY_LALT]>0		|| pKeys[SX_INPUT_KEY_RALT]>0	)	* SX_GUI_KEY_ALT		|
			BYTE(pKeys[SX_INPUT_KEY_LSHIFT]>0	|| pKeys[SX_INPUT_KEY_RSHIFT]>0
			||	 pKeys[SX_INPUT_KEY_MOUSE_LEFT]	== SX_INPUT_STATE_HOLD)			* SX_GUI_KEY_SHIFT;

		// process keyboard inputs by respective language
		int res = 0;
		if (s_SelectedLanguage == GIL_PERSIAN)
		{
			if (pKeys[SX_INPUT_KEY_RSHIFT]>0 || pKeys[SX_INPUT_KEY_CAPITAL]>0 || 
				pKeys[SX_INPUT_KEY_LSHIFT]>0 || pKeys[SX_INPUT_KEY_CAPSLOCK]>0 )
				res = Control::ProcessKeyboardInput(pKeys, &io::InputButtonToFarsiCodeWithShift);
			else
				res = Control::ProcessKeyboardInput(pKeys, &io::InputButtonToFarsiCode);
		}
		else
		{
			res = Control::ProcessKeyboardInput(pKeys);
		}

// 		if ( Control::GetFocusedControl() == this )
// 		{
// 			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_RETURN) && (m_Option & SX_GUI_PROPERTY_MULTILINE)==0 )
// 			{
// 				SetFocused( false );
// 			}

// 			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) )
// 			{
// 				if ( Control::GetCapturedControl() && Control::GetCapturedControl()->GetType() == GUI_TEXTEDIT )
// 					Control::GetCapturedControl()->SetFocused( true );
// 				else
// 					SetFocused( false );
// 			}
// 
// 			char* keys = (char*)sx::io::Input::GetKeys(0);//(char*)pKeys;
// 			for ( int i=0; i<0xED; i++ )
// 			{
// 				keys[i] = SX_INPUT_STATE_NORMAL;
// 			}
//		}

		return res;
	}

	void TextEdit::processInputMouse( bool& result, int playerID )
	{
		Control::processInputMouse( result, playerID );

 		if ( Control::GetFocusedControl() == this )
 		{
 			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_RETURN) && (m_Option & SX_GUI_PROPERTY_MULTILINE)==0 )
 		 	{
 		 		SetFocused( false );
 		 	}
 
 			if ( SEGAN_KEYDOWN(0, SX_INPUT_KEY_MOUSE_LEFT) )
 			{
 				if ( Control::GetCapturedControl() && Control::GetCapturedControl()->GetType() == GUI_TEXTEDIT )
 					Control::GetCapturedControl()->SetFocused( true );
 				else
 					SetFocused( false );
 			}
 		}		
	}

	void TextEdit::UpdateLine( int LineIndex )
	{
		str1024 extraText;
		PGUITextLine pline	= m_Lines[LineIndex];

		pline->width = 0;
		for (int i=0; i<pline->text.Length(); i++)
		{
			if (pline->text[i] == '\n')
			{
				pline->text.CopyTo(extraText, i+1, pline->text.Length());
				pline->text.Delete(i+1, pline->text.Length());
				pline->UpdateWidth(m_Font, false);

				//  verify that next line is exist
				LineIndex++;
				if (LineIndex < m_Lines.Count())
				{
					m_Lines[LineIndex]->text.Insert(extraText);
				}
				else
				{
					//  now add new line in the string list
					pline = sx_new( GUITextLine );
					pline->text = extraText;
					m_Lines.PushBack(pline);
				}
				UpdateLine(LineIndex);
				return;
			}

			PGUIFontChar pfnch = NULL;
			if ( !m_Font->GetChar(pline->text, i, pfnch, true) ) continue;
			pline->width += pfnch->xAdvance;
			
			if ( pline->width >= m_Size.x && (m_Option & SX_GUI_PROPERTY_WORDWRAP) && (m_Option & SX_GUI_PROPERTY_MULTILINE))
			{
				//  reverse to the previous character
				for (;i>0; i--)
				{
					WCHAR ch = pline->text[i];
					if (ch==' ' || ch=='\t' || ch=='.' || ch==',' || ch=='\r' || ch=='\n') break;
				} i++;
				pline->text.CopyTo(extraText, i, pline->text.Length());
				pline->text.Delete(i, pline->text.Length());
				pline->UpdateWidth(m_Font, false);

				//  verify that next line is exist
				LineIndex++;
				if (LineIndex < m_Lines.Count())
				{
					m_Lines[LineIndex]->text.Insert(extraText);
				}
				else
				{
					//  now add new line in the string list
					pline = sx_new( GUITextLine );
					pline->text = extraText;
					pline->UpdateWidth(m_Font, true);
					m_Lines.PushBack(pline);
				}
				UpdateLine(LineIndex);
				return;

			} // if ( pline->width >= m_Size.x )
		}
	}

	void TextEdit::SetSelection( const int2 start, const int2 end )
	{
		m_selStart = start;
		m_selEnd = end;
		BurnBuffer();
	}

	PointI TextEdit::GetCaretFromPixel( float px, float py )
	{
		if(m_Lines.Count()<1)
			return PointI(0, 0);

		GUIFontDesc desc = m_Font->GetDesc();
		int y = int( (GetRect().y1 - py + m_Scroll.y*desc.LineHeight) / (float)desc.LineHeight );
		SEGAN_CLAMP(y, 0, m_Lines.Count()-1);

		float w = m_Size.x;
		float x = GetRect().x1 - m_Scroll.x + 2.0f;
		switch (m_Align)
		{
		case GTA_CENTER:	x += (w - m_Lines[y]->width) * 0.5f;	break;
		case GTA_RIGHT:		x += (w - m_Lines[y]->width);			break;
		}
		x = (float)int(x);

		str1024 text = m_Lines[y]->text;
		ReverseRTLText(text);
		PointI result(-1, 0);
		for (int i=0; i<text.Length(); i++)
		{
			PGUIFontChar pfnch = NULL;
			if (m_Font->GetChar(text, i, pfnch, true))
			{
				if (px>=x && px<=x+pfnch->xAdvance)
				{
					if (px<=x+pfnch->width*0.45f)
						result = PointI(i, y);
					else
						result = PointI(i+1, y);
				}

				x += pfnch->xAdvance;
			}
		}

		//  caret founded. just we should try to correct RTL texts
		if (result.x>-1)
		{
			int rtlBegin=0, rtlEnd=0;
			if (CaretIsOnRTLText(result, rtlBegin, rtlEnd))
			{
				result.x = rtlEnd + rtlBegin + 1 - result.x;
			}
			return result;
		}

		if (px<x)
			return PointI(0, y);
		else
			return PointI(m_Lines[y]->text.Length(), y);
	}

	bool TextEdit::CaretIsOnRTLText( PointI& p, OUT int& fromIndex, OUT int& toIndex )
	{
		if (m_Lines.Count()<1) return false;
		SEGAN_CLAMP(p.y, 0, m_Lines.Count()-1);

		//  check that the caret is inside of RTL part of the text
		PGUITextLine pLine = m_Lines[p.y];
		for (int i=0; i<pLine->text.Length(); i++)
		{
			if (pLine->text[i]>1000)
			{
				fromIndex = i;
				for (;i<pLine->text.Length(); i++)
				{
					if (pLine->text[i]<1000 && pLine->text[i]!='\n'	&&
						pLine->text[i]!=' ' && pLine->text[i]!='.'	&& 
						pLine->text[i]!=',' && pLine->text[i]!='\t'	)
					{
						for (;i>0 && pLine->text[i]<1000; i--);
						break;
					}
				}
				toIndex = i;
				if (fromIndex < p.x && p.x <= toIndex+1) return true;
			}
		}

		return false;
	}

	void TextEdit::BurnBuffer( void )
	{
		if (!CanBurnBuffer()) 
		{
			ReleaseBuffer();
			return;
		}

		D3DColor color = m_Elements[1]->Color();
		//color.a *= Element::AlphaPercent();

		GUIFontDesc	fontDesc = m_Font->GetDesc();
		TextureDesc	txurDesc;
		m_Font->m_Texture->GetDesc(txurDesc);
		if (txurDesc.Width<2 || m_Size.y < fontDesc.LineHeight || m_Size.x < fontDesc.Size)
		{
			ReleaseBuffer();
			return;
		}

		RectF rect		= GetRect();
		int lineTop		= m_Scroll.y;
		int lineBottom	= lineTop + int(m_Size.y / (float)fontDesc.LineHeight);
		if (lineBottom > m_Lines.Count()) lineBottom = m_Lines.Count();

		//  sort caret to check the selection
		PointI p1 = m_selStart, p2 = m_selEnd;
		sortCarets(p1, p2);

		//  ignore 'space' and 'return' characters from count calculation
		int charCount=0; int selCount=0;
		for (int i=lineTop; i<lineBottom; i++)
		{
			PGUITextLine pline = m_Lines[i];
			if (pline)
			{
				for (int j=0; j<(int)pline->text.Length(); j++)
				{
					PGUIFontChar fch = NULL;
					WCHAR ch = pline->text[j];
					if (ch=='\t' || ch=='\n' || !m_Font->GetChar(ch, fch))
						continue;

					if (isInSelection(i, j, p1, p2)) selCount++;
					if (ch!=' ') charCount++;
				}
			}
		}

		//  verify that the buffer capacity is enough to contain all words
		if (charCount<=0)
		{
			ReleaseBuffer();
			return;
		}
		CreateBuffer(charCount + selCount);
		m_numChar = charCount;
		m_selCount= selCount;

		PD3DVertex0 v0;
		if ( m_VB0 && SUCCEEDED ( m_VB0->Lock(0, 0, (void**)&v0, 0) ) )
		{
			PD3DVertex1 v1;
			if ( SUCCEEDED ( m_VB1->Lock(0, 0, (void**)&v1, 0) ) )
			{
				PDWORD indx;
				if ( SUCCEEDED ( m_IB->Lock(0, 0, (void**)&indx, 0) ) )
				{
					str1024 text; int textwidth = 0, j = 0;

					//  at first burn the characters to the buffers
					for (int iline=lineTop; iline<lineBottom; iline++)
					{
						text		= m_Lines[iline]->text;
						textwidth	= m_Lines[iline]->width;
						ReverseRTLText(text);

						//  setup buffer by list of characters
						float h = m_Size.y;
						float w = m_Size.x;
						float x = rect.x1 + 2.0f;
						switch (m_Align)
						{
						case GTA_CENTER:	x += (w - textwidth) * 0.5f;	break;
						case GTA_RIGHT:		x += (w - textwidth);			break;
						}
						x = (float)int(x);
						float y = rect.y1 - fontDesc.LineHeight * (iline - lineTop);
						float z = 0.0f;
						float u, v, u2, v2;
						PGUIFontChar ch = NULL;
						int curLine = 0;

						for (int i = 0; i<(int)text.Length(); i++)
						{
							if ( !m_Font->GetChar(text, i, ch, true) ) continue;

							//  ignore space and return characters from buffer
							if (ch->ID==' ' || ch->ID=='\t' || ch->ID=='\n' || ch->ID=='\r')
							{
								x += ch->xAdvance;
								continue;
							}
							v0[j*4+0].pos.x =	x + ch->xOffset;
							v0[j*4+1].pos.x =	x + ch->xOffset + ch->width;
							v0[j*4+2].pos.x =	x + ch->xOffset;
							v0[j*4+3].pos.x =	x + ch->xOffset + ch->width;

							v0[j*4+0].pos.y =	y - ch->yOffset;
							v0[j*4+1].pos.y =	y - ch->yOffset;
							v0[j*4+2].pos.y =	y - ch->yOffset - ch->height;
							v0[j*4+3].pos.y =	y - ch->yOffset - ch->height;

							v0[j*4+0].pos.z = z;
							v0[j*4+1].pos.z = z;
							v0[j*4+2].pos.z = z;
							v0[j*4+3].pos.z = z;

							u =		((float)ch->x+0.5f) / (float)txurDesc.Width;
							v =		((float)ch->y+0.5f) / (float)txurDesc.Width;
							u2 = u + (float)ch->width   / (float)txurDesc.Width;
							v2 = v + (float)ch->height  / (float)txurDesc.Width; 
							
							v1[j*4+0].txc.x	= u;
							v1[j*4+1].txc.x	= u2;
							v1[j*4+2].txc.x	= u;
							v1[j*4+3].txc.x	= u2;

							v1[j*4+0].txc.y	= v;
							v1[j*4+1].txc.y	= v;
							v1[j*4+2].txc.y	= v2;
							v1[j*4+3].txc.y	= v2;

							v1[j*4+0].nrm.x	= 0;
							v1[j*4+1].nrm.x	= 0;
							v1[j*4+2].nrm.x	= 0;
							v1[j*4+3].nrm.x	= 0;

							v1[j*4+0].nrm.y	= 1.0f;
							v1[j*4+1].nrm.y	= 1.0f;
							v1[j*4+2].nrm.y	= 1.0f;
							v1[j*4+3].nrm.y	= 1.0f;

							v1[j*4+0].nrm.z	= 0;
							v1[j*4+1].nrm.z	= 0;
							v1[j*4+2].nrm.z	= 0;
							v1[j*4+3].nrm.z	= 0;

							v1[j*4+0].col0	= color;
							v1[j*4+1].col0	= color;
							v1[j*4+2].col0	= color;
							v1[j*4+3].col0	= color;

							v1[j*4+0].col1	= 0xFFFFFFFF;
							v1[j*4+1].col1	= 0xFFFFFFFF;
							v1[j*4+2].col1	= 0xFFFFFFFF;
							v1[j*4+3].col1	= 0xFFFFFFFF;

							indx[j*6+0] = j*4+0;
							indx[j*6+1] = j*4+1;
							indx[j*6+2] = j*4+2;
							indx[j*6+3] = j*4+1;
							indx[j*6+4] = j*4+3;
							indx[j*6+5] = j*4+2;

							x += ch->xAdvance;		
							j++;
						}
					}

					//  now burn the selected characters
					for (int iline=lineTop; m_selCount>0 && iline<lineBottom; iline++)
					{
						text		= m_Lines[iline]->text;
						textwidth	= m_Lines[iline]->width;

						//  setup buffer by list of characters
						float h = m_Size.y;
						float w = m_Size.x;
						float x = rect.x1 + 2.0f;
						switch (m_Align)
						{
						case GTA_CENTER:	x += (w - textwidth) * 0.5f;	break;
						case GTA_RIGHT:		x += (w - textwidth);			break;
						}
						x = (float)int(x);
						float y = rect.y1 - fontDesc.LineHeight * (iline - lineTop);
						float z = 0.0f;
						PGUIFontChar ch = NULL;
						int curLine = 0;
						float xOffset = x;

						for (int i = 0; i<(int)text.Length(); i++)
						{
							if ( !m_Font->GetChar(text, i, ch, false) ) continue;

							//  ignore space and return characters from buffer
							if (ch->ID=='\t' || ch->ID=='\n' || ch->ID=='\r')
							{
								x += ch->xAdvance;
								continue;
							}

							if (isInSelection(iline, i, p1, p2))
							{
								//  correct x for RTL texts
								int rtlBegin=0, rtlEnd=0;
								if (CaretIsOnRTLText(PointI(i+1, iline), rtlBegin, rtlEnd))
								{
									float rtlX = xOffset;
									for (int k=0; k<rtlBegin; k++)
									{
										PGUIFontChar pfnch = NULL;
										if (m_Font->GetChar(text, k, pfnch, false))
										{
											rtlX += pfnch->xAdvance;
										}
									}

									for (int k=rtlBegin; k<=rtlEnd; k++)
									{
										PGUIFontChar pfnch = NULL;
										if (m_Font->GetChar(text, k, pfnch, false))
										{
											rtlX += pfnch->xAdvance;
										}
									}

									for (int k=rtlBegin; k<i+1; k++)
									{
										PGUIFontChar pfnch = NULL;
										if (m_Font->GetChar(text, k, pfnch, false))
										{
											rtlX -= pfnch->xAdvance;
										}
									}

									rtlX -= 1;
									v0[j*4+0].pos.x = rtlX;
									v0[j*4+1].pos.x = rtlX + ch->xAdvance;
									v0[j*4+2].pos.x = rtlX;
									v0[j*4+3].pos.x = rtlX + ch->xAdvance;
								}
								else
								{
									v0[j*4+0].pos.x = x - 1;
									v0[j*4+1].pos.x = x + ch->xAdvance - 1;
									v0[j*4+2].pos.x = x - 1;
									v0[j*4+3].pos.x = x + ch->xAdvance - 1;
								}

								v0[j*4+0].pos.y = y;
								v0[j*4+1].pos.y = y;
								v0[j*4+2].pos.y = y - fontDesc.LineHeight;
								v0[j*4+3].pos.y = y - fontDesc.LineHeight;

								v0[j*4+0].pos.z = z;
								v0[j*4+1].pos.z = z;
								v0[j*4+2].pos.z = z;
								v0[j*4+3].pos.z = z;

								v1[j*4+0].txc.x	= 0;	v1[j*4+0].txc.y	= 0;
								v1[j*4+1].txc.x	= 0;	v1[j*4+1].txc.y	= 0;
								v1[j*4+2].txc.x	= 0;	v1[j*4+2].txc.y	= 0;
								v1[j*4+3].txc.x	= 0;	v1[j*4+3].txc.y	= 0;

								v1[j*4+0].nrm.x	= 0; v1[j*4+0].nrm.y = 1.0f; v1[j*4+0].nrm.z = 0;
								v1[j*4+1].nrm.x	= 0; v1[j*4+1].nrm.y = 1.0f; v1[j*4+1].nrm.z = 0;
								v1[j*4+2].nrm.x	= 0; v1[j*4+2].nrm.y = 1.0f; v1[j*4+2].nrm.z = 0;
								v1[j*4+3].nrm.x	= 0; v1[j*4+3].nrm.y = 1.0f; v1[j*4+3].nrm.z = 0;

								v1[j*4+0].col0	= 0xFFFFFFFF;	v1[j*4+0].col1	= 0xFFFFFFFF;
								v1[j*4+1].col0	= 0xFFFFFFFF;	v1[j*4+1].col1	= 0xFFFFFFFF;
								v1[j*4+2].col0	= 0xFFFFFFFF;	v1[j*4+2].col1	= 0xFFFFFFFF;
								v1[j*4+3].col0	= 0xFFFFFFFF;	v1[j*4+3].col1	= 0xFFFFFFFF;

								indx[j*6+0] = j*4+0;
								indx[j*6+1] = j*4+1;
								indx[j*6+2] = j*4+2;
								indx[j*6+3] = j*4+1;
								indx[j*6+4] = j*4+3;
								indx[j*6+5] = j*4+2;

								j++;
							}

							x += ch->xAdvance;		
						}
					}

					m_IB->Unlock();
				}
				m_VB1->Unlock();
			}
			m_VB0->Unlock();
		}
	}

	void TextEdit::SetScrollValue( int x, int y )
	{
		Label::SetScrollValue(x, y);

		//  update caret position
		UpdateCaretOnPixel();
		//SetCaretPos(m_CaretPos.y, m_CaretPos.x);
	}

	void TextEdit::Copy( void )
	{
		sx::sys::CopyText( GetText() );
	}

	void TextEdit::Paste( void )
	{
		String tmp = sx::sys::PasteText();
		if ( tmp.Length() )
		{
			if ( m_selCount )
				TextDelete( m_selStart, m_selEnd );

			TextInsert( GetCaretPos(), tmp );
		}
	}

	void TextEdit::UpdateCaretOnPixel( void )
	{
		if ( !m_Lines.Count() ) return;
		
		int Ln	= m_CaretPos.y;
		int Col = m_CaretPos.x;

		GUIFontDesc desc = m_Font->GetDesc();
		float w = m_Size.x;
		float x = GetRect().x1 + 2.0f;
		switch (m_Align)
		{
		case GTA_CENTER:	x += (w - m_Lines[Ln]->width) * 0.5f;		break;
		case GTA_RIGHT:		x += (w - m_Lines[Ln]->width);			break;
		}
		x = (float)int(x);

		//  correct position of cursor
		PointI p(Col, Ln);
		int rtlBegin=0, rtlEnd=0;
		if (CaretIsOnRTLText(p, rtlBegin, rtlEnd))
		{
			for (int i=0; i<rtlBegin; i++)
			{
				PGUIFontChar pfnch = NULL;
				if (m_Font->GetChar(m_Lines[Ln]->text, i, pfnch, false))
				{
					x += pfnch->xAdvance;
				}
			}

			for (int i=rtlBegin; i<=rtlEnd; i++)
			{
				PGUIFontChar pfnch = NULL;
				if (m_Font->GetChar(m_Lines[Ln]->text, i, pfnch, false))
				{
					x += pfnch->xAdvance;
				}
			}

			for (int i=rtlBegin; i<Col; i++)
			{
				PGUIFontChar pfnch = NULL;
				if (m_Font->GetChar(m_Lines[Ln]->text, i, pfnch, false))
				{
					x -= pfnch->xAdvance;
				}
			}
		}
		else
		{
			for (int i=0; i<Col; i++)
			{
				PGUIFontChar pfnch = NULL;
				if (m_Font->GetChar(m_Lines[Ln]->text, i, pfnch, false))
				{
					x += pfnch->xAdvance;
				}
			}
		}

		float y = GetRect().y1 - (desc.LineHeight*0.5f) - ( (Ln - m_Scroll.y) * desc.LineHeight);
		m_Elements[2]->Matrix().SetTranslation(x - m_Scroll.x, y, m_Elements[1]->Matrix()._43);
	}




}}	//  namespace sx { namespace gui {