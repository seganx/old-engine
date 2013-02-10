#include "sxFont.h"
#include "../sxCommon/sxCommon.h"

namespace sx { namespace gui {

	Font::Font( void ): m_RefCount(1), m_Texture(NULL)
	{
	}

	Font::~Font( void )
	{
		CleanUp();
	}

	GUIFontDesc& Font::GetDesc( void )
	{
		return m_FontDesc;
	}

	void Font::Save( Stream& stream )
	{
		//  at first write the font version
		int ver = 1;
		SEGAN_STREAM_WRITE(stream, ver);

		//  write font information
		SEGAN_STREAM_WRITE(stream, m_FontDesc);

		//  now write the texture name
		String txSrc;
		if (m_Texture) txSrc = m_Texture->GetSource();
		cmn::String_Save(txSrc, &stream);
		
		//  write the number of characters and finally all characters
		int n = m_Chars.Count();
		SEGAN_STREAM_WRITE(stream, n);

		if(n>0)
		{	
			//  write all of the character information to the stream
			for (GUICharacterMap::Iterator it = m_Chars.First(); !it.IsLast(); it++)
			{
				stream.Write(&(*it), sizeof(GUIFontChar));
			}
		}
	}

	void Font::Load( Stream& stream )
	{
		//  clear current font
		CleanUp();

		//  at first read the font version
		int ver = 0;
		SEGAN_STREAM_READ(stream, ver);

		if(ver == 1)
		{
			//  read font information
			SEGAN_STREAM_READ(stream, m_FontDesc);

			//  now read the texture name
			String txSrc;
			cmn::String_Load(txSrc, &stream);
			String txPath;

			//  check to see if the source font was full path then correct texture path
			if (String::IsFullPath(m_Src)) 
			{
				txPath = m_Src;
				txPath.ExtractFilePath();
				txPath.MakePathStyle();
			}
			
			txPath << txSrc;
			if (d3d::Texture::Manager::Get(m_Texture, txPath))
				m_Texture->Activate();

			//  read the number of characters and finally all characters
			int n = 0;
			SEGAN_STREAM_READ(stream, n);

			if(n>0)
			{	
				//  read all of the character information from the stream and insert them to the map
				for (int i=0; i<n; i++)
				{
					GUIFontChar* fchar = (GUIFontChar*)sx_mem_alloc( sizeof(GUIFontChar) );
					stream.Read( fchar, sizeof(GUIFontChar) );
					m_Chars.Insert(fchar->ID, fchar);
				}
			}
		}
	}

	void Font::SetSource( const WCHAR* src )
	{
		m_Src = src;

		PStream stream = NULL;
		if( sys::FileManager::File_Open(m_Src, SEGAN_PACKAGENAME_COMMON, stream) )
		{
			Load(*stream);
			sys::FileManager::File_Close(stream);
		}
	}

	const WCHAR* Font::GetSource( void )
	{
		return m_Src;
	}

	bool Font::GetChar( const DWORD charID, OUT PGUIFontChar& pchar )
	{
		return m_Chars.Find(charID, pchar);
	}

	bool Font::GetChar( const WCHAR* string, const int charIndex, OUT PGUIFontChar& pchar, bool reversed /*= false*/ )
	{
		sx_assert(string); sx_assert(charIndex>-1);
		if (string[charIndex]<1000) return m_Chars.Find(string[charIndex], pchar);


		WCHAR prvCh = 0, nxtCh = 0;
		if (reversed)
		{
			prvCh = string[charIndex+1];
			nxtCh = charIndex>0 ? string[charIndex-1] : 32;
		}
		else
		{
			prvCh = charIndex>0 ? string[charIndex-1] : 32;
			nxtCh = string[charIndex+1];
		}
		int	  prev	= !(prvCh < 1000 || prvCh == 1570 || prvCh == 1575 || prvCh == 1608 || prvCh == 1688 || (prvCh > 1582 && prvCh < 1587));
		int   next	= !(nxtCh < 1000);
		WORD  charID= 0;

		// traverse in Persian script
		switch (string[charIndex])
		{
		case 1574:	charID = 65164 + prev + next*2 - 3;	break;
		case 1575:	charID = prev ? 65166 : 65165;		break;
		case 1576:	charID = 65170 + prev + next*2 - 3;	break;
		case 1578:	charID = 65176 + prev + next*2 - 3;	break;
		case 1579:	charID = 65180 + prev + next*2 - 3;	break;
		case 1580:	charID = 65184 + prev + next*2 - 3;	break;
		case 1581:	charID = 65188 + prev + next*2 - 3;	break;
		case 1582:	charID = 65192 + prev + next*2 - 3;	break;
		case 1583:	charID = prev ? 65194 : 65193;		break;
		case 1584:	charID = prev ? 65196 : 65195;		break;
		case 1585:	charID = prev ? 65198 : 65197;		break;
		case 1586:	charID = prev ? 65200 : 65199;		break;
		case 1587:	charID = 65204 + prev + next*2 - 3;	break;
		case 1588:	charID = 65208 + prev + next*2 - 3;	break;
		case 1589:	charID = 65212 + prev + next*2 - 3;	break;
		case 1590:	charID = 65216 + prev + next*2 - 3;	break;
		case 1591:	charID = prev ? 65220 : 65219;		break;
		case 1592:	charID = prev ? 65224 : 65223;		break;
		case 1593:	charID = 65228 + prev + next*2 - 3;	break;
		case 1594:	charID = 65232 + prev + next*2 - 3;	break;
		case 1601:	charID = 65236 + prev + next*2 - 3;	break;
		case 1602:	charID = 65240 + prev + next*2 - 3;	break;
		case 1604:	charID = 65248 + prev + next*2 - 3;	break;
		case 1605:	charID = 65252 + prev + next*2 - 3;	break;
		case 1606:	charID = 65256 + prev + next*2 - 3;	break;
		case 1607:	charID = 65260 + prev + next*2 - 3;	break;
		case 1608:	charID = prev ? 65262 : 65261;		break;
		case 1662:	charID = 64345 + prev + next*2 - 3;	break;
		case 1670:	charID = 64381 + prev + next*2 - 3;	break;
		case 1688:	charID = prev ? 64395 : 64394;		break;
		case 1705:	charID = 64401 + prev + next*2 - 3;	break;
		case 1711:	charID = 64405 + prev + next*2 - 3;	break;
		case 1740:	
		case 1610:	if(prev && next) charID = 65268; else if(prev) charID = 65264; 
					else if(next)	 charID = 65267; else charID = 65263;	break;
		default:	charID = string[charIndex];
		}

		return m_Chars.Find(charID, pchar);
	}

	void Font::CleanUp( void )
	{
		for ( GUICharacterMap::Iterator it = m_Chars.First(); !it.IsLast(); it++ )
		{
			sx_delete( *it );
		}
		m_Chars.Clear();
		if(m_Texture)
		{
			m_Texture->Deactivate();
			d3d::Texture::Manager::Release(m_Texture);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//	MANAGER
	//////////////////////////////////////////////////////////////////////////
	typedef Map<UINT, sx::gui::PFont> sxMapFont;
	static 	sxMapFont FontMap;		//  hold the created font objects

	bool Font::Manager::Exist( OUT PFont& pFont, const WCHAR* src )
	{
		UINT key = sx::cmn::GetCRC32(src);
		return FontMap.Find(key, pFont);
	}

	bool Font::Manager::Get( OUT PFont& pFont, const WCHAR* src )
	{
		UINT key = sx::cmn::GetCRC32(src);

		if (FontMap.Find(key, pFont))
		{
			pFont->m_RefCount++;
		}
		else
		{
			if (Create(pFont, src))
			{
				FontMap.Insert(key, pFont);
			}
			else return false;
		}

		return true;
	}

	bool Font::Manager::Create( OUT PFont& pFont, const WCHAR* src )
	{
		pFont = sx_new( Font );
		if (pFont)
		{
			pFont->SetSource(src);
			return true;
		}
		else return false;
	}

	void Font::Manager::Release( PFont& pFont )
	{
		if (!pFont) return;
		PFont font = pFont;
		pFont = NULL;

		font->m_RefCount--;
		if (font->m_RefCount!=0) return;

		//  remove from font map
		PFont fn = NULL; UINT fID = sx::cmn::GetCRC32(font->m_Src);
		if (FontMap.Find(fID, fn) && fn==font)
			FontMap.Remove(fID);

		sx_delete_and_null(font);
	}

	void Font::Manager::ClearAll( void )
	{
		if (FontMap.IsEmpty()) return;

		String str = L"Releasing remains Fonts(s) :\r\n\t\t\t";

		for (sxMapFont::Iterator it = FontMap.First(); !it.IsLast(); it++)
		{
			PFont f = *it;
			str << (f->GetSource()!=NULL ? f->GetSource() : L" ??? no name ???") << L"\r\n\t\t\t";

			sx_delete_and_null(f);
		}
		FontMap.Clear();

		sxLog::Log(str);
	}

}}	//	namespace sx { namespace gui {