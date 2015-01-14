#include "GameStrings.h"
#include "GameTypes.h"

GameStrings::GameStrings()
{
	
}

GameStrings::~GameStrings()
{
	Clear();
}

void GameStrings::Clear( void )
{
#if USE_UPDATE_1
	for ( Map<uint, GameString*>::Iterator it = m_texts.First(); !it.IsLast(); it++ )
	{
		GameString* tmp = *it;
		sx_delete(tmp);
	}
	m_texts.Clear();
#endif
}

void GameStrings::Load( const wchar* stringsfile )
{
	if ( !stringsfile || !sx::sys::FileExist(stringsfile) )
		return;

	m_list.Clear();
	{
		sx::sys::FileStream myFile;
		if ( !myFile.Open( stringsfile, FM_OPEN_READ | FM_SHARE_READ) ) return;
		int fsize = myFile.Size();

		//	check for encryption
		char encrypt[2] = {0};
		myFile.Read( &encrypt, 2 );
		bool encrypted = ( encrypt[0] == 'e' && encrypt[1] == 'n' );
		if ( encrypted )
			fsize -= 2;
		else
#if USE_ENCRPT_STR
			return;
#else
			myFile.SetPos(0);
#endif

#if USE_UPDATE_1
		wchar* buffer = (wchar*)sx_mem_alloc( fsize + 2 );
		myFile.Read( buffer, fsize );
		buffer[fsize/2] = 0;
		if ( encrypted )
			sx_decrypt( buffer, buffer, fsize, 12345 );

		sx::cmn::StringList strList;
		strList.LoadFromString( buffer );
		sx_mem_free( buffer );

		for ( int i = 0; i < strList.Count(); ++i )
		{
			sx::cmn::StringToker toker;
			toker.SetString( strList[i]->Text() );
			if ( toker.Next() == L"id" )
			{
				toker.Next(); // ignore ':'
				GameString* gstr = sx_new( GameString );
				gstr->id = sx_str_to_uint( toker.Next().Text() );

				while ( true )
				{
					toker.Next(); // ignore ','
					str32 field = toker.Next();
					if ( !field.Length() ) break;
					toker.Next(); // ignore ':'

					if ( field == L"font" )
					{
						swprintf_s( gstr->font, 42, L"%s.fnt" );
						//sx_str_copy( gstr->font, 32, toker.Next().Text() );
					}
					else if ( field == L"align" )
					{
						if ( toker.LookatNext() == L"left" )
							gstr->align = GTA_LEFT;
						else if ( toker.LookatNext() == L"center" )
							gstr->align = GTA_CENTER;
						else if ( toker.LookatNext() == L"right" )
							gstr->align = GTA_RIGHT;
						toker.Next();
					}
					else if ( field == L"x" )
					{
						gstr->x = toker.Next().ToFloat();
					}
					else if ( field == L"y" )
					{
						gstr->y = toker.Next().ToFloat();
					}
					else if ( field == L"text" )
					{
						str512 tmp = toker.GetCurChar();
						tmp.Replace( L"\\n", L"\n" );
						sx_str_copy( gstr->text, 512, tmp.Text() );
						break;
					}
				}// while ( true )

				if ( gstr->text[0] == 0 )
					sx_str_copy( gstr->text, 512, strList[++i]->Text() );
				if ( m_texts.Insert( gstr->id, gstr ) == false )
				{
					sxLog::Log( L" ERROR : Duplicated string id has been found in %s - id : %u ", stringsfile, gstr->id );
					sx_delete_and_null(gstr);
				}
			}
		}		
#else
		wchar* buffer = (wchar*)sx_mem_alloc( fsize + 2 );
		myFile.Read( buffer, fsize );
		buffer[fsize/2] = 0;
		if ( encrypted )
			sx_decrypt( buffer, buffer, fsize, 12345 );
		m_list.LoadFromString( (wchar*)buffer );
		sx_mem_free( buffer );

		for ( int i=0; i<m_list.Count(); ++i )
		{
			m_list[i]->Replace( L"\\n", L"\n" );
		}
#endif
	}
}

#if USE_UPDATE_1
GameStrings* GameStrings::Get( const uint id )
{
	GameString* res = null;
	if ( m_texts.Find( id, res ) )
	{
		return res;
	}
	else
	{
		static GameString tmp;
		sx_str_copy( tmp.text, 48, L"Text not found!" );
		return tmp;
	}
}
#else
const wchar* GameStrings::Get( const int index )
{
	String* res = m_list.At(index);
	if ( res )
		return res->Text();
	else
	{
		memset( m_tmp, 0, sizeof(m_tmp) );
		swprintf_s( m_tmp, 32, L" %d string not found", index );
		return m_tmp;
	}
}
#endif
