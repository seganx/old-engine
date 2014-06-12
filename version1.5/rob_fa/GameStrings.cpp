#include "GameStrings.h"

GameStrings::GameStrings()
{
	
}

GameStrings::~GameStrings()
{

}

void GameStrings::Load( const wchar* stringsfile )
{
	if ( !stringsfile || !sx::sys::FileExist(stringsfile) )
		return;

	{
		sx::sys::FileStream MyFile;
		if ( !MyFile.Open( stringsfile, FM_OPEN_READ | FM_SHARE_READ) ) return;
		int fsize = MyFile.Size();

		//	check for encryption
		char encrypt[2] = {0};
		MyFile.Read( &encrypt, 2 );
		bool encrypted = ( encrypt[0] == 'e' && encrypt[1] == 'n' );
		if ( encrypted )
			fsize -= 2;
		else
			MyFile.SetPos(0);

		wchar* buffer = (wchar*)sx_mem_alloc( fsize + 2 );
		MyFile.Read( buffer, fsize );
		buffer[fsize/2] = 0;
		if ( encrypted )
			sx_decrypt( buffer, buffer, fsize, 12345 );
		m_strlist.LoadFromString( (wchar*)buffer );
		sx_mem_free( buffer );
	}

	for ( int i=0; i<m_strlist.Count(); ++i )
	{
		m_strlist[i]->Replace( L"\\n", L"\n" );
	}
}

const wchar* GameStrings::Get( const int index )
{
	String* res = m_strlist.At(index);
	if ( res )
		return res->Text();
	else
	{
		memset( m_tmp, 0, sizeof(m_tmp) );
		swprintf_s( m_tmp, 32, L" %d string not found", index );
		return m_tmp;
	}
}
