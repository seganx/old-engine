#include "sxFont.h"
#include "../sxCommon/sxCommon.h"

namespace sx { namespace gui {

	Font::Font( void ): m_refCount(1), m_texture(NULL)
	{
	}

	Font::~Font( void )
	{
		CleanUp();
	}

	GUIFontDesc& Font::GetDesc( void )
	{
		return m_desc;
	}

	void Font::Save( Stream& stream )
	{
		//  at first write the font version
		int ver = 1;
		SEGAN_STREAM_WRITE(stream, ver);

		//  write font information
		SEGAN_STREAM_WRITE(stream, m_desc);

		//  now write the texture name
		String txSrc;
		if (m_texture) txSrc = m_texture->GetSource();
		cmn::String_Save(txSrc, &stream);
		
		//  write the number of characters and finally all characters
		int n = m_chars.Count();
		SEGAN_STREAM_WRITE(stream, n);

		if(n>0)
		{	
			//  write all of the character information to the stream
			for (GUICharacterMap::Iterator it = m_chars.First(); !it.IsLast(); it++)
			{
				GUIFontChar *ch = *it;
				stream.Write( ch, sizeof(GUIFontChar) );
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
			SEGAN_STREAM_READ(stream, m_desc);

			//  now read the texture name
			String txSrc;
			cmn::String_Load(txSrc, &stream);
			String txPath;

			//  check to see if the source font was full path then correct texture path
			if (String::IsFullPath(m_src)) 
			{
				txPath = m_src;
				txPath.ExtractFilePath();
				txPath.MakePathStyle();
			}
			
			txPath << txSrc;
			if (d3d::Texture::Manager::Get(m_texture, txPath))
				m_texture->Activate();

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
					m_chars.Insert(fchar->ID, fchar);
				}
			}
		}
	}

	void Font::SetSource( const WCHAR* src )
	{
		m_src = src;

		PStream stream = NULL;
		if( sys::FileManager::File_Open(m_src, SEGAN_PACKAGENAME_COMMON, stream) )
		{
			Load(*stream);
			sys::FileManager::File_Close(stream);
		}
	}

	const WCHAR* Font::GetSource( void )
	{
		return m_src;
	}

	bool Font::GetChar( const DWORD charID, OUT PGUIFontChar& pchar )
	{
		return m_chars.Find(charID, pchar);
	}

	bool Font::GetChar( const WCHAR* string, const int charIndex, OUT PGUIFontChar& pchar, bool reversed /*= false*/ )
	{
		sx_assert(string); sx_assert(charIndex>-1);
		if (string[charIndex]<1000) return m_chars.Find(string[charIndex], pchar);


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

		return m_chars.Find(charID, pchar);
	}

	void Font::CleanUp( void )
	{
		for ( GUICharacterMap::Iterator it = m_chars.First(); !it.IsLast(); it++ )
		{
			GUIFontChar *ch = *it;
			sx_delete( ch );
		}
		m_chars.Clear();
		if(m_texture)
		{
			m_texture->Deactivate();
			d3d::Texture::Manager::Release(m_texture);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//	MANAGER
	//////////////////////////////////////////////////////////////////////////
	typedef Map<UINT, sx::gui::Font*> sxMapFont;
	static 	sxMapFont s_fontMap;		//  hold the created font objects

	bool Font::Manager::Exist( OUT Font*& pFont, const WCHAR* src )
	{
		UINT key = sx::cmn::GetCRC32(src);
		return s_fontMap.Find(key, pFont);
	}

	bool Font::Manager::Get( OUT Font*& pFont, const WCHAR* src )
	{
		UINT key = sx::cmn::GetCRC32(src);

		if (s_fontMap.Find(key, pFont))
		{
			pFont->m_refCount++;
		}
		else
		{
			if (Create(pFont, src))
			{
				s_fontMap.Insert(key, pFont);
			}
			else return false;
		}

		return true;
	}

	bool Font::Manager::Create( OUT Font*& pFont, const WCHAR* src )
	{
		pFont = sx_new( Font );
		if (pFont)
		{
			pFont->SetSource(src);
			return true;
		}
		else return false;
	}

	void Font::Manager::Release( Font*& pFont )
	{
		if (!pFont) return;
		Font* font = pFont;
		pFont = NULL;

		font->m_refCount--;
		if (font->m_refCount!=0) return;

		//  remove from font map
		Font* fn = NULL; UINT fID = sx::cmn::GetCRC32(font->m_src);
		if (s_fontMap.Find(fID, fn) && fn==font)
			s_fontMap.Remove(fID);

		sx_delete_and_null(font);
	}

	void Font::Manager::ClearAll( void )
	{
		if (s_fontMap.IsEmpty()) return;
		sx_callstack();

		for (sxMapFont::Iterator it = s_fontMap.First(); !it.IsLast(); it++)
		{
			Font* f = *it;
			sx_delete_and_null(f);
		}
		s_fontMap.Clear();
	}

}}	//	namespace sx { namespace gui {