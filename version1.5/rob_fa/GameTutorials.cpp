#include "GameTutorials.h"
#include "GameTypes.h"
#include "Scripter.h"


GameTutorials::GameTutorials()
{
	
}

GameTutorials::~GameTutorials()
{
	Clear();
}

void GameTutorials::Clear( void )
{
	for (int i=0; i<m_tutor.Count(); ++i)
	{
		sx::gui::Panel* image = m_tutor[i].image;
		if (image)
		{
			image->SetParent(null);
			sx_delete(image);
		}
	}

	m_tutor.Clear();
}

void GameTutorials::Load( const wchar* stringsfile )
{
	if ( !stringsfile ) return;
	
	str1024 path = sx::sys::FileManager::Project_GetDir();
	path << stringsfile;

	if ( !sx::sys::FileExist(path) )
		return;

	Scripter script;
	script.Load( path );

	for (int i=0; i<script.GetObjectCount(); i++)
	{
		str512 tmpStr;
		if ( script.GetString(i, L"Type", tmpStr) )
		{
			if ( tmpStr == L"Tutorial" )
			{
				GameTutorial tmp;
				ZeroMemory(&tmp, sizeof(GameTutorial));

				if ( !script.GetString( i, L"Name", tmpStr ) ) continue;
				sx_str_copy(tmp.name, 32, tmpStr);

				if ( !script.GetString( i, L"image", tmpStr ) ) continue;
				tmp.image = sx_new(sx::gui::Panel);
				tmp.image->SetSize( float2(1024, 1024) );
				tmp.image->Position().y = 64.0f;
				tmp.image->GetElement(0)->SetTextureSrc(tmpStr);
				if ( tmp.image->GetElement(0)->GetTexture() )
					tmp.image->GetElement(0)->GetTexture()->Activate();

				for (int k=0; k<16; ++k)
				{
					str64 textName;
					textName.Format(L"text_%d", k);
					if ( !script.GetUint(i, textName, tmp.texts[k]) ) break;
				}

				script.GetInt( i, L"level", tmp.level );

				//	add new tutorial to the list
				m_tutor.PushBack(tmp);
			}
		}
	}
}

GameTutorial* GameTutorials::GetByName( const wchar* name )
{
	static GameTutorial tmp;
	ZeroMemory(&tmp, sizeof(GameTutorial));
	if ( !name ) return &tmp;

	for ( int i=0; i<m_tutor.Count(); ++i )
	{
		if ( wcscmp( name, m_tutor[i].name ) != 0 )
			return &m_tutor[i];
	}

	return &tmp;
}
