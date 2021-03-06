#include "sxLabel.h"
#include "sxFont.h"
#include "../sxCommon/sxCommon.h"

#define TMP_SIZE 16384				//  size of temp string use to parse text to the separate lines
static WCHAR s_tmp[ TMP_SIZE ];		//  this is a temp string ! :D

namespace sx { namespace gui {

	//////////////////////////////////////////////////////////////////////////
	//	GUI TEXT LINE CLASS
	//////////////////////////////////////////////////////////////////////////
	GUITextLine::GUITextLine(): text(), width(0)
	{

	}

	GUITextLine::~GUITextLine()
	{

	}

	WCHAR GUITextLine::GetChar( int index )
	{
		if (index<0 || index>text.Length()) return 0;
		return text[index];
	}

	bool GUITextLine::IsColorCode( const WCHAR* str )
	{
		if ( !str ) return false;
		return
			( str[0] == '`'	) &&
			( str[1] == '0'	) &&
			( str[2] == 'x' || str[2] == 'X' ) &&
			( str[3] != 0	) &&
			( str[4] != 0	) &&
			( str[5] != 0	) &&
			( str[6] != 0	) &&
			( str[7] != 0	) &&
			( str[8] != 0	) &&
			( str[9] == '`'	);

	}

	int GUITextLine::UpdateWidth( Font* textFont, bool reversed )
	{
		width = 0;
		if (!textFont) return 0;

		int len = text.Length();
		for ( int i=0; i<len; i++ )
		{
			if ( IsColorCode( &text[i] ) )
			{
				i += 10;
			}

			PGUIFontChar pfnch = NULL;
			if (textFont->GetChar(text, i, pfnch, reversed))
			{
				if ( i == 0 || i == len )
					width += pfnch->width;

				width += pfnch->xAdvance;
			}
		}

		return width;
	}

	int GUITextLine::GetNumCharToBuffer( Font* textFont )
	{
		int cc = 0;
		for (int i=0; i<text.Length(); i++)
		{
			if ( IsColorCode( &text[i] ) )
			{
				i += 10;
			}

			PGUIFontChar fch = NULL;
			WCHAR ch = text[i];
			if ( ch==' ' || ch=='\t' || ch=='\n' || !textFont->GetChar(ch, fch) )
				continue;
			cc++;
		}
		return cc;
	}

	//////////////////////////////////////////////////////////////////////////
	//	LABEL
	//////////////////////////////////////////////////////////////////////////
	Label::Label( void ): Control(), 
		m_font(NULL),
		m_align(GTA_LEFT),
		m_scroll(0, 0),
		m_onTextChange(NULL, NULL),
		m_onScrollChange(NULL, NULL),
		m_numChar(0),
		m_vertexBuffer0(NULL),
		m_vertexBuffer1(NULL),
		m_indexBuffer(NULL)
	{
		m_Type = GUI_LABEL;

		//  align control to the pixel space to show perfect strings
		AddProperty(SX_GUI_PROPERTY_PIXELALIGN);

		//  create one element
		SetElementCount(2);

		SetSize(float2(150.0f, 75.0f));

#if 1
		if ( sys::FileManager::File_Exist(L"Font_Default.fnt", SEGAN_PACKAGENAME_COMMON) )
			SetFont(L"Font_Default.fnt");
		else
			Font::Manager::Get( m_font, L"Font_Default.fnt");
#endif
	}

	Label::~Label( void )
	{
		Font::Manager::Release(m_font);
		d3d::Resource3D::ReleaseVertexBuffer( m_vertexBuffer0 );
		d3d::Resource3D::ReleaseVertexBuffer( m_vertexBuffer1 );
		d3d::Resource3D::ReleaseIndexBuffer( m_indexBuffer );

		for (int i=0; i<m_lines.Count(); i++)
		{
			sx_delete(m_lines[i]);
		}
		m_lines.Clear();
	}

	void Label::SetSize( float2 sz )
	{
		if ( sx_abs_f(m_Size.x - sz.x)<5 && sx_abs_f(m_Size.y - sz.y)<5 ) return;

		Control::SetSize(sz);
		RectF rc = Control::GetRect();
		m_elements[0]->SetRect(rc);
		m_elements[1]->SetRect(rc);

		CreateLines();
		BurnBuffer();
	}

	void Label::AddProperty( DWORD prop )
	{
		SEGAN_SET_ADD(m_Option, prop);

		//  word wrap can be activate when the control be multi line support
		if (!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_MULTILINE))
			SEGAN_SET_REM(m_Option, SX_GUI_PROPERTY_WORDWRAP);

		if ( prop & SX_GUI_PROPERTY_PIXELALIGN || prop & SX_GUI_PROPERTY_MULTILINE || prop & SX_GUI_PROPERTY_WORDWRAP || prop & SX_GUI_PROPERTY_AUTOSIZE )
		{
			CreateLines();
			BurnBuffer();
		}
	}

	void Label::RemProperty( DWORD prop )
	{
		SEGAN_SET_REM(m_Option, prop);

		//  word wrap can be activate when the control be multi line support
		if (!SEGAN_SET_HAS(m_Option, SX_GUI_PROPERTY_MULTILINE))
			SEGAN_SET_REM(m_Option, SX_GUI_PROPERTY_WORDWRAP);

		if ( prop & SX_GUI_PROPERTY_PIXELALIGN || prop & SX_GUI_PROPERTY_MULTILINE || prop & SX_GUI_PROPERTY_WORDWRAP )
		{
			CreateLines();
			BurnBuffer();
		}
	}

	void Label::Save( Stream& stream )
	{
		Control::Save(stream);

		//  save the text alignment
		SEGAN_STREAM_WRITE(stream, m_align);

		//  save the font source
		String fntSrc;
		if (m_font) fntSrc = m_font->GetSource();
		cmn::String_Save(fntSrc, &stream);

		//  save the text
		GetText();
		cmn::String_Save(m_text, &stream);

		Save_Childes(stream);
	}

	void Label::Load( Stream& stream )
	{
		Control::Load(stream);

		//  read the text alignment
		SEGAN_STREAM_READ(stream, m_align);

		//  read the font source
		String fntSrc;
		cmn::String_Load(fntSrc, &stream);
		SetFont(fntSrc);

		//  read the text
		cmn::String_Load(m_text, &stream);
		SetText(m_text);

		Load_Childes(stream);
	}

	void Label::Draw( DWORD option )
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
			d3d::Device3D::RS_ZWritable(false);
			m_elements[0]->Draw(option);
			DrawText(option);
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
			m_elements[0]->Draw(option);
			DrawText(option);

			if (option & SX_GUI_DRAW_SAVEMATRIX)
			{
				d3d::Device3D::Matrix_Project_Set(mat_proj_last);
				d3d::Device3D::Matrix_View_Set(mat_view_last);
			}
		}

		Control::Draw_Childes(option);
	}

	void Label::DrawText( DWORD option )
	{
		if ( !m_vertexBuffer0 || !m_indexBuffer )
		{
			BurnBuffer();
			return;
		}

		//  apply text scrolling before render
		math::Matrix matScl = math::MTRX_IDENTICAL;
		matScl._41 = -(float)m_scroll.x;
		matScl.Multiply(matScl, m_Mtrx);
		d3d::Device3D::Matrix_World_Set(matScl);

		//  prepare API to draw text
		m_font->m_texture->SetToDevice(0);
		d3d::Device3D::SetVertexBuffer(0, m_vertexBuffer0, SEGAN_SIZE_VERTEX_0);
		d3d::Device3D::SetVertexBuffer(1, m_vertexBuffer1, SEGAN_SIZE_VERTEX_1);
		d3d::Device3D::SetIndexBuffer(m_indexBuffer);

		d3d::Device3D::RS_VertexColor( true );

		D3DColor color = m_elements[1]->Color();
		color.a *= ( m_Option & SX_GUI_PROPERTY_IGNOREBLEND ) ? Element::AlphaPercent() * 2 : Element::AlphaPercent();
		d3d::Device3D::SetMaterialColor( color );

		//  start drawing text with clip plane enabled
		m_elements[0]->BeginAsClipSpace();

		//  check texture state to correct aberrations caused by perspective matrix
		float4 q;
		m_Mtrx.GetRotationQ(q.x, q.y, q.z, q.w);
		if (m_Option & SX_GUI_PROPERTY_PIXELALIGN && !SEGAN_SET_HAS(m_Option, _SX_GUI_IN_3DSPACE_) && 
			sx_abs_f(q.x)<0.01f && sx_abs_f(q.y)<0.01f && sx_abs_f(q.z)<0.01f)
		{
			d3d::Device3D::RS_TextureFilter(false);
			d3d::Device3D::DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_numChar*4, 0, m_numChar*2);
			d3d::Device3D::RS_TextureFilter(true);
		}
		else d3d::Device3D::DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_numChar*4, 0, m_numChar*2);

		d3d::Device3D::RS_VertexColor( false );

		m_elements[0]->EndAsClipSpace();
	}

	void Label::SetText( const WCHAR* text )
	{
		if ( m_text == text ) return;

		m_text = text;

		CreateLines();
		BurnBuffer();

		//  update text by changes
		GetText();

		m_onTextChange(this);
	}

	const WCHAR* Label::GetText( void )
	{
		m_text.Clear();
		for (int i=0; i<m_lines.Count(); i++)
		{
			m_text << m_lines[i]->text;

			//if (m_text.Length()>0 && m_text[m_text.Length()-1] == '\n') 
			//	m_text.Insert('\r', m_text.Length()-1);
		}
		return m_text;
	}

	sx::gui::PGUITextLine Label::GetLine( const int Index )
	{
		if(Index<0 || Index>=m_lines.Count()) return NULL;
		return m_lines[Index];
	}

	void Label::SetOnTextChange( PForm pForm, GUICallbackEvent pFunc )
	{
		m_onTextChange.m_pForm = pForm;
		m_onTextChange.m_pFunc = pFunc;

	}

	void Label::SetOnScrollChanged( PForm pForm, GUICallbackEvent pFunc )
	{
		m_onScrollChange.m_pForm = pForm;
		m_onScrollChange.m_pFunc = pFunc;
	}

	void Label::SetFont( const WCHAR* fontSrc )
	{
		Font* pFont = NULL;
		if (Font::Manager::Get(pFont, fontSrc))
		{
			Font::Manager::Release(m_font);
			m_font = pFont;
		}
		
		SetAlign(m_align);
	}

	const WCHAR* Label::GetFontSource( void )
	{
		if (m_font)
			return m_font->GetSource();
		else
			return NULL;
	}

	GUITextAlign Label::GetAlign( void )
	{
		return m_align;
	}

	void Label::SetAlign( GUITextAlign align )
	{
		m_align = align;
		SetScrollValue(m_scroll.x, m_scroll.y);
		BurnBuffer();
	}

	void Label::SetScrollValue( int x, int y )
	{
		PointI maxscroll = GetScrollMaxValue();

		SEGAN_CLAMP(x, 0, maxscroll.x);
		SEGAN_CLAMP(y, 0, maxscroll.y);

		if (m_scroll.y != y)	
		{
			m_scroll.y = y;
			BurnBuffer();
		}

		m_scroll.x = x;

		m_onScrollChange(this);
	}

	PointI Label::GetScrollValue( void )
	{
		return m_scroll;
	}

	PointI Label::GetScrollMaxValue( void )
	{
		PointI maxscroll(0, 0);

		for (int i=0; i<m_lines.Count(); i++)
		{
			if (maxscroll.x < m_lines[i]->width) maxscroll.x = m_lines[i]->width;
		}

		maxscroll.x = maxscroll.x	  - int(m_Size.x);
		maxscroll.y = m_lines.Count() - int(m_Size.y / (float)m_font->GetDesc().lineHeight);

		if (maxscroll.x < 0) maxscroll.x = 0;
		if (maxscroll.y < 0) maxscroll.y = 0;

		return maxscroll;
	}

	//////////////////////////////////////////////////////////////////////////
	//	PROTECTED
	FORCEINLINE bool IsDigit( WCHAR c )
	{
		return ( c >= '0' && c <= '9' );
	}

	void Label::ReverseRTLText( str1024& in_str )
	{
		for (int i=0; i<in_str.Length(); i++)
		{
			if (in_str[i]>1000)
			{
				int farsiBegin = i;
				for (; i<in_str.Length(); i++)
				{
					if (in_str[i]<1000 &&	in_str[i]!='\n'	&&
						in_str[i]!=' ' &&	in_str[i]!='.'	&& 
						in_str[i]!=',' &&	in_str[i]!='\t' &&
						!IsDigit(in_str[i]) )
					{
						for (;i>0 && in_str[i]<1000; i--);
						i++;
						break;
					}
				}
				in_str.Revers(farsiBegin, i-1);

				for ( int j=farsiBegin; j<i; j++ )
				{
					if ( IsDigit(in_str[j]) )
					{
						int digitBegin = j;
						for (; j<i; j++)
						{
							if ( !IsDigit(in_str[j]) && in_str[j]!='.' )
							{
								for (;j>0 && !IsDigit(in_str[j]); j--);
								j++;
								break;
							}
						}
						in_str.Revers(digitBegin, j-1);
					}
				}

			}//if pline
		}//for i
	}

	void Label::CreateLines( void )
	{
		for (int i=0; i<m_lines.Count(); i++)
		{
			sx_delete(m_lines[i]);
		}
		m_lines.Clear();
		if ( !m_font || !m_text.Text() ) return;

		//////////////////////////////////////////////////////////////////////////
		//  separate words in lines
		for (int i=0, j=0; i<=m_text.Length(); i++ )
		{
			WCHAR ch = m_text[i];
			s_tmp[j] = ch;

			if ( ch == 0 && j > 0 )
			{
				GUITextLine* line = sx_new( GUITextLine );
				line->text = s_tmp;
				line->UpdateWidth( m_font, false );
				m_lines.PushBack(line);

				break; //	this is a end of text. so break out
			}
			else if ( j>(TMP_SIZE-2) || ( ch == '\n' && (m_Option & SX_GUI_PROPERTY_MULTILINE) ) )
			{
				s_tmp[j] = 0;

				GUITextLine* line = sx_new( GUITextLine );
				line->text = s_tmp;
				line->UpdateWidth( m_font, false );
				m_lines.PushBack(line);

				j=0;
			}
			else
			{
				j++;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//	apply word wrap to the lines
		if ( m_Option & SX_GUI_PROPERTY_WORDWRAP && m_Option & SX_GUI_PROPERTY_MULTILINE )
		{
			GUITextLine testline;

			for ( int i=0; i<m_lines.Count(); i++ )
			{
				GUITextLine* curline = m_lines[i];

				if ( m_Size.x > 0.1f && curline->width > m_Size.x )
				{
					int cur_j = 0, last_j = 0;
					for ( int j=0; j<=curline->text.Length(); j++ )
					{
						WCHAR ch = curline->text[j];
						s_tmp[cur_j] = ch;

						if ( ch==0 || ch==' ' || ch=='\t' || ch=='.' || ch==',' )
						{
							s_tmp[cur_j] = 0;

							testline.text = s_tmp;
							testline.UpdateWidth( m_font, false );
							if ( last_j && testline.width > m_Size.x && testline.width < curline->width )
							{
								//	insert new line after this line
								GUITextLine* newline = sx_new( GUITextLine );
								curline->text.CopyTo( newline->text, last_j, 9999999 );
								newline->UpdateWidth( m_font, false );
								m_lines.Insert( i+1, newline );

								//  change this line
								curline->text.Delete( last_j, 99999999 );
								curline->UpdateWidth( m_font, false );

								//	check next line ( new line inserted )
								j=9999999;
							}
							else
							{
								s_tmp[cur_j] = ch;
								last_j = cur_j;
								cur_j++;
							}

						}
						else cur_j++;
					}
				}
			}
		}
		

		//  apply auto size for label
		if ( m_Option & SX_GUI_PROPERTY_AUTOSIZE )
		{
			float2 maxSize(10,5);
			for (int i=0; i<m_lines.Count(); i++)
			{
				GUITextLine* line = m_lines[i];
				if ( maxSize.x < line->width ) maxSize.x = float(line->width);
				maxSize.y += float(m_font->GetDesc().lineHeight);
			}

			if ( sx_abs_f(m_Size.x - maxSize.x)>5 || sx_abs_f(m_Size.y - maxSize.y)>5 )
				SetSize(maxSize);
		}

#if 0
		WCHAR			tmp[TMP_SIZE];			//  this is a temp string ! :D
		int				itmp		= 0;		//	use play with characters in lines
		int				lineSize	= 0;		//  calculate line size
		PGUITextLine	pline		= NULL;		
		PGUIFontChar	pfnch		= NULL;

		//  separate words in lines
		for (int i=0; i<=m_Text.Length(); i++)
		{
			WCHAR ch = m_Text[i];

			if ( m_Text[i] == '`' && i+10<m_Text.Length() && m_Text[i+9] == '`' && m_Text[i+1] == '0' &&  String::LowerChar(m_Text[i+2]) == 'x' )
			{
				tmp[itmp++] = m_Text[i+0];
				tmp[itmp++] = m_Text[i+1];
				tmp[itmp++] = m_Text[i+2];
				tmp[itmp++] = m_Text[i+3];
				tmp[itmp++] = m_Text[i+4];
				tmp[itmp++] = m_Text[i+5];
				tmp[itmp++] = m_Text[i+6];
				tmp[itmp++] = m_Text[i+7];
				tmp[itmp++] = m_Text[i+8];
				tmp[itmp++] = m_Text[i+9];
				i += 10;
				ch = m_Text[i];
			}

			//  add character to the tmp string
			tmp[itmp] = ch;

			if (ch == 0)
			{
				tmp[itmp] = NULL;

				pline = sx_new( GUITextLine );
				pline->m_Text = tmp;
				pline->m_Width = lineSize;

				m_Lines.PushBack(pline);
				itmp = 0; lineSize = 0;
				break; //	this is a end of text. so break out
			}

			if (ch == '\n')
			{
				if (m_Option & SX_GUI_PROPERTY_MULTILINE)
				{
					tmp[itmp+1] = NULL;

					pline = sx_new( GUITextLine );
					pline->m_Text = tmp;
					pline->m_Width = lineSize;

					m_Lines.PushBack(pline);
					itmp = 0; lineSize = 0;
				}

				continue;
			}

			if ( ch == '\r' || !m_Font->GetChar(m_Text, i, pfnch) )	continue;
			lineSize += pfnch->xAdvance;

			if ( itmp>(TMP_SIZE-2) || (lineSize >= m_Size.x && m_Option & SX_GUI_PROPERTY_WORDWRAP && m_Option & SX_GUI_PROPERTY_MULTILINE) )
			{
				//  reverse to the previous character
				for (;i>0; i--, itmp--)
				{
					ch = m_Text[i];
					if (ch==' ' || ch=='\t' || ch=='.' || ch==',' || ch=='\r' || ch=='\n') break;
					m_Font->GetChar(m_Text, i, pfnch);
					lineSize -= pfnch->xAdvance;
				}

				//  now add new line in the string list
				tmp[itmp+1] = NULL;
				pline = sx_new( GUITextLine );
				pline->m_Text = tmp;
				pline->m_Width = lineSize;

				m_Lines.PushBack(pline);
				itmp = 0; lineSize = 0;
				continue;
			}

			itmp++;
		}

		//  apply auto size for label
		if ( m_Option & SX_GUI_PROPERTY_AUTOSIZE )
		{
			float2 maxSize(10,5);
			for (int i=0; i<m_Lines.Count(); i++)
			{
				pline = m_Lines[i];
				if ( maxSize.x < pline->m_Width ) maxSize.x = float(pline->m_Width);
				maxSize.y += float(m_Font->GetDesc().LineHeight);
			}

			if ( abs(m_Size.x - maxSize.x)>5 || abs(m_Size.y - maxSize.y)>5 )
				SetSize(maxSize);
		}
#endif

	}

	void Label::ReleaseBuffer( void )
	{
		m_numBuffer = 0;
		d3d::Resource3D::ReleaseVertexBuffer( m_vertexBuffer0 );
		d3d::Resource3D::ReleaseVertexBuffer( m_vertexBuffer1 );
		d3d::Resource3D::ReleaseIndexBuffer( m_indexBuffer );
	}

	void Label::CreateBuffer( int MaxCharacter )
	{
		if (m_numBuffer >= MaxCharacter) return;
		ReleaseBuffer();
		m_numBuffer= MaxCharacter;

		UINT NumVrtx	= m_numBuffer * 4;
		UINT NumFace	= m_numBuffer * 6;

		d3d::Resource3D::GetVertexBuffer(NumVrtx, SEGAN_SIZE_VERTEX_0, m_vertexBuffer0);
		d3d::Resource3D::GetVertexBuffer(NumVrtx, SEGAN_SIZE_VERTEX_1, m_vertexBuffer1);
		d3d::Resource3D::GetIndexBuffer(NumFace, m_indexBuffer);
	}

	bool Label::CanBurnBuffer( void )
	{
		return m_font && m_font->m_texture && !m_lines.IsEmpty();
	}

	void Label::BurnBuffer( void )
	{
		if (!CanBurnBuffer()) 
		{
			ReleaseBuffer();
			return;
		}

		GUIFontDesc fontDesc = m_font->GetDesc();
		TextureDesc	txurDesc;
		m_font->m_texture->GetDesc(txurDesc);
		if ( txurDesc.Width<2 || m_Size.y < fontDesc.lineHeight || m_Size.x < fontDesc.size )
		{
			ReleaseBuffer();
			return;
		}

		RectF rect		= GetRect();
		int lineTop		= m_scroll.y;
		int lineBottom	= lineTop + int( m_Size.y / (float)fontDesc.lineHeight );
		if (lineBottom > m_lines.Count()) lineBottom = m_lines.Count();

		//  ignore 'space' and 'return' characters from count calculation
		int charCount=0;
		for (int i=lineTop; i<lineBottom; i++)
		{
			PGUITextLine pline = m_lines[i];
			charCount += pline->GetNumCharToBuffer( m_font );
		}

		//  verify that the buffer capacity is enough to contain all words
		if (charCount<=0)
		{
			ReleaseBuffer();
			return;
		}
		CreateBuffer(charCount);
		m_numChar = charCount;

		D3DColor charColor = m_elements[1]->Color();
		float alphaFactor = ( m_Option & SX_GUI_PROPERTY_IGNOREBLEND ) ? Element::AlphaPercent() * 2 : Element::AlphaPercent();

		PD3DVertex0 v0;
		if ( m_vertexBuffer0 && SUCCEEDED ( m_vertexBuffer0->Lock(0, 0, (void**)&v0, 0) ) )
		{
			PD3DVertex1 v1;
			if ( SUCCEEDED ( m_vertexBuffer1->Lock(0, 0, (void**)&v1, 0) ) )
			{
				PDWORD indx;
				if ( SUCCEEDED ( m_indexBuffer->Lock(0, 0, (void**)&indx, 0) ) )
				{
					int j = 0;
					for (int iline=lineTop; iline<lineBottom; iline++)
					{
						GUITextLine line = *m_lines[iline];
						ReverseRTLText(line.text);
						line.UpdateWidth( m_font, true );

						//  setup buffer by list of characters
						float h = m_Size.y;
						float w = m_Size.x;
						float x = rect.x1;
						switch (m_align)
						{
						case GTA_CENTER:	x += (w - line.width) * 0.5f;	break;
						case GTA_RIGHT:		x += (w - line.width);			break;
						}
						x = (float)int(x);
						float y = rect.y1 - fontDesc.lineHeight * (iline - lineTop);
						float z = 0.0f;
						float u, v, u2, v2;
						PGUIFontChar ch = NULL;
						int curLine = 0;

						for (int i = 0; i<(int)line.text.Length(); i++)
						{
							//	extract color
							if ( line.text[i] == '`' && i+9<line.text.Length() && line.text[i+9] == '`' && line.text[i+1] == '0' &&  String::LowerChar(line.text[i+2]) == 'x' )
							{
								WCHAR strtmp[7] = {0,0,0,0,0,0,0,};
								strtmp[0] = ( line.text[i+3] );
								strtmp[1] = ( line.text[i+4] );
								strtmp[2] = ( line.text[i+5] );
								strtmp[3] = ( line.text[i+6] );
								strtmp[4] = ( line.text[i+7] );
								strtmp[5] = ( line.text[i+8] );

								DWORD ccode = 0;
								swscanf_s( strtmp , L"%x", &ccode);
								charColor =  ccode;

								line.text.Delete(i, 10);
							}

							if ( !m_font->GetChar(line.text, i, ch, true) ) continue;

							//  ignore space and return characters from buffer
							if ( ch->ID==' ' || ch->ID=='\t' || ch->ID=='\n' || ch->ID=='\r' )
							{
								x += ch->xAdvance;
								continue;
							}

							if ( i == 0 ) x -= ch->xOffset;

							u =		((float)ch->x+0.5f) / (float)txurDesc.Width;
							v =		((float)ch->y+0.5f) / (float)txurDesc.Width;
							u2 = u + (float)ch->width   / (float)txurDesc.Width;
							v2 = v + (float)ch->height  / (float)txurDesc.Width;

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

							charColor.a *= alphaFactor;
							v1[j*4+0].col0	= charColor;
							v1[j*4+1].col0	= charColor;
							v1[j*4+2].col0	= charColor;
							v1[j*4+3].col0	= charColor;

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

					m_indexBuffer->Unlock();
				}
				m_vertexBuffer1->Unlock();
			}
			m_vertexBuffer0->Unlock();
		}
	}

}}	//  namespace sx { namespace gui {