#include "Scripter.h"


UINT _GetKey(int sectionID, const WCHAR* str)
{
	static str1024 tmp;
	tmp.Format( L"%d_%s_%d", sectionID, str, sectionID );
	return sx::cmn::GetCRC32( tmp );
}

Scripter::Scripter( void ): m_SectionCount(0)
{

}

Scripter::~Scripter( void )
{
	Clear();
}

void Scripter::Clear( void )
{
	m_mapValue.Clear();
	m_mapString.Clear();
	m_SectionCount = 0;
}

void Scripter::Load( const WCHAR* scriptFile )
{
	if ( !scriptFile ) return;

	sx::cmn::StringList strList;
	strList.LoadFromFile( scriptFile );

	bool beginBlock = false;
	sx::cmn::StringToker toker;
	for (int i=0; i<strList.Count(); i++)
	{
		toker.SetString( strList[i] );
		
		if ( beginBlock )
		{
			str512 paramName = toker.Next().Text();
			if ( paramName == L"}" )
			{
				beginBlock = false;
				continue;
			}

			toker.Next();	//  ignore = + *
			str512 paramValue = toker.Next().Text();

			if ( paramName.Length()>0 && paramValue.Length()>0 )
			{
				if ( iswdigit(paramValue[0]) || paramValue[0]=='-' )
				{
					float v = paramValue.StrToFloat( paramValue );
					m_mapValue.Insert( _GetKey(m_SectionCount-1, paramName), v );
				}
				else
				{
					m_mapString.Insert( _GetKey(m_SectionCount-1, paramName), paramValue.Text() );
				}
			}
		}
		else
		{
			str512 token = toker.Next().Text();
			if ( token == L"{" )
			{
				beginBlock = true;
				continue;
			}

			//  new section found
			else if ( token == L"object" )
			{
				str512 secType = toker.Next().Text();
				toker.Next();	//  ignore ( { [
				str512 secName = toker.Next().Text();

				if ( secType.Length() && secName.Length() )
				{
					m_SectionCount++;
					m_mapString.Insert( _GetKey(m_SectionCount-1, L"Type"), secType.Text() );
					m_mapString.Insert( _GetKey(m_SectionCount-1, L"Name"), secName.Text() );
				}				
				continue;
			}
		}
	}
}

int Scripter::GetObjectCount( void )
{
	return m_SectionCount;
}

bool Scripter::GetFloat( const int sectionID, const WCHAR* name, float& value )
{
	if ( !name ) return false;
	UINT id = _GetKey(sectionID, name);

	return m_mapValue.Find(id, value);
}

bool Scripter::GetInteger( const int sectionID, const WCHAR* name, int& value )
{
	if ( !name ) return false;
	UINT id = _GetKey(sectionID, name);

	float v = 0;
	if ( m_mapValue.Find(id, v) )
	{
		value = int( v + 0.2f );
		return true;
	}
	else
		return false;
}

bool Scripter::GetString( const int sectionID, const WCHAR* name, str512& value )
{
	if ( !name ) return false;
	UINT id = _GetKey(sectionID, name);

	return m_mapString.Find(id, value);
}

