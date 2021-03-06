#include "ImportFont.h"


bool isFirstWord(const WCHAR* str, const WCHAR* subStr)
{
	String		 Str = str;
	for (int i=0; i<(int)Str.Length(); i++)
	{
		if (Str[i]==' ' || Str[i]=='	')
		{
			String		 SubStr;
			Str.CopyTo(SubStr, 0, i);
			return (SubStr == subStr);
		}
	}
	return false;
}

class DrivedFont: public sx::gui::Font
{
public:
	~DrivedFont(void){
		sx::gui::Font::~Font();
	}

	void LoadFromFile( const WCHAR* FileName ){
		//  clear current font
		m_chars.Clear();

		//	texture file name
		String		 textureFile;

		//  load string file to the string class
		sx::cmn::StringList slist;
		slist.LoadFromFile(FileName);

		//  traverse in the string list
		for (int i=0; i<slist.Count(); i++)
		{
			//  extract font information
			String		 tmp;
			if (slist[i]->Length() > 5)
			{
				if ( isFirstWord(slist[i]->Text(), L"char") )
				{

					PGUIFontChar pCharInfo = sx_new( GUIFontChar );

					sx::cmn::String_GetValue(*slist[i], L"id", tmp);
					pCharInfo->ID = tmp.StrToInt(tmp);

					sx::cmn::String_GetValue(*slist[i], L"x", tmp);
					pCharInfo->x = tmp.StrToInt(tmp);

					sx::cmn::String_GetValue(*slist[i], L"y", tmp);
					pCharInfo->y = tmp.StrToInt(tmp);

					sx::cmn::String_GetValue(*slist[i], L"width", tmp);
					pCharInfo->width = tmp.StrToInt(tmp);

					sx::cmn::String_GetValue(*slist[i], L"height", tmp);
					pCharInfo->height = tmp.StrToInt(tmp);

					sx::cmn::String_GetValue(*slist[i], L"xoffset", tmp);
					pCharInfo->xOffset = tmp.StrToInt(tmp);

					sx::cmn::String_GetValue(*slist[i], L"yoffset", tmp);
					pCharInfo->yOffset = tmp.StrToInt(tmp);

					sx::cmn::String_GetValue(*slist[i], L"xadvance", tmp);
					pCharInfo->xAdvance = tmp.StrToInt(tmp);

					sx::cmn::String_GetValue(*slist[i], L"page", tmp);
					pCharInfo->page = tmp.StrToInt(tmp);

					m_chars.Insert(pCharInfo->ID, pCharInfo);
				} 
				else if ( isFirstWord(slist[i]->Text(), L"page") )
				{
					sx::cmn::String_GetValue(*slist[i], L"file", textureFile);
				} 
				else if ( isFirstWord(slist[i]->Text(), L"info") )
				{
					sx::cmn::String_GetValue(*slist[i], L"size", tmp);
					m_desc.size = tmp.StrToInt(tmp);

					sx::cmn::String_GetValue(*slist[i], L"outline", tmp);
					m_desc.outline = tmp.StrToInt(tmp);
				} 
				else if ( isFirstWord(slist[i]->Text(), L"common") )
				{
					sx::cmn::String_GetValue(*slist[i], L"lineHeight", tmp);
					m_desc.lineHeight = tmp.StrToInt(tmp);

					sx::cmn::String_GetValue(*slist[i], L"base", tmp);
					m_desc.base = tmp.StrToInt(tmp);
				} 
				else if ( isFirstWord(slist[i]->Text(), L"chars") )
				{
					sx::cmn::String_GetValue(*slist[i], L"count", tmp);
					m_desc.charCount = tmp.StrToInt(tmp);
				}
			}
		}

		//  import textures
		str1024 texfile = textureFile;
		texfile.ExcludeFileExtension();
		texfile << L".txr";
		sx::d3d::Texture::Manager::Create(m_texture, texfile);
		
		texfile = FileName;
		texfile.ExtractFilePath();
		texfile.MakePathStyle();
		texfile << textureFile;
		m_texture->LoadFromImageFile( texfile, D3DFMT_A8R8G8B8 );

		MemoryStream mem;
		m_texture->Save(mem);

		texfile = textureFile;
		texfile.ExcludeFileExtension();
		texfile << L".txr";
		sx::sys::FileManager::File_Save(texfile, SEGAN_PACKAGENAME_TEXTURE, mem);
	}

};

bool ImportFontToLibrary(const WCHAR* SrcFilePath)
{
	str1024 tmp = SrcFilePath;
	tmp.ExtractFileName();
	sx::gui::Font* font;
	sx::gui::Font::Manager::Create(font, tmp);
	((DrivedFont*)font)->LoadFromFile(SrcFilePath);

	MemoryStream mem;
	font->Save(mem);

	sx::gui::Font::Manager::Release(font);
	
	str1024 filename = SrcFilePath;
	filename.ExtractFileName();
	return sx::sys::FileManager::File_Save(filename, SEGAN_PACKAGENAME_COMMON, mem);
}
