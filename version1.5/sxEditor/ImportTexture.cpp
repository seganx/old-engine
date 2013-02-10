#include "ImportTexture.h"

D3DFormat SelectFormat(const WCHAR* src)
{
	D3DXIMAGE_INFO info;
	ZeroMemory(&info, sizeof(D3DXIMAGE_INFO));
	D3DXGetImageInfoFromFile(src, &info);

	switch ( info.ImageFileFormat )
	{
	case D3DXIFF_TGA:
		{
			if ( info.Format == D3DFMT_A8R8G8B8 )
				return D3DFMT_A8R8G8B8;
			else
				return D3DFMT_DXT1;
		}
		break;

	default:
		{
			if ( info.Format == D3DFMT_A8R8G8B8 )
				return D3DFMT_DXT5;
			else
				return D3DFMT_DXT1;
		}
	}
}

void ImportTexture( const WCHAR* srcFile )
{
	if ( !sx::sys::FileExist(srcFile) )
	{
		sxLog::Log(L"File '%s' could not found!", srcFile);
		return;
	}

	sx::d3d::PTexture texture;
	sx::d3d::Texture::Manager::Create(texture, NULL);

	if ( texture->LoadFromImageFile(srcFile, SelectFormat(srcFile) ) )
	{
		str1024 texfile = srcFile;
		texfile.ExtractFileName();
		texfile.ExcludeFileExtension();
		texfile << L".txr";

		MemoryStream mem;
		texture->Save(mem);

		if ( !sx::sys::FileManager::File_Save(texfile, SEGAN_PACKAGENAME_TEXTURE, mem) )
			sxLog::Log(L"Can't save texture to '%s'", texfile);
	}
	else sxLog::Log(L"Can't open texture file '%s'", srcFile);

	sx::d3d::Texture::Manager::Release(texture);

}

void ConvertTexture( const WCHAR* srcFile, const WCHAR* destFile )
{
	if ( !sx::sys::FileExist(srcFile) )
	{
		sxLog::Log(L"File '%s' could not found!", srcFile);
		return;
	}

	sx::d3d::PTexture texture;
	sx::d3d::Texture::Manager::Create(texture, NULL);

	//  just converting format ?
	str1024 filexten = srcFile; filexten.ExtractFileExtension();
	if ( filexten == L"txr" )
	{
		sx::sys::FileStream file;
		if ( file.Open(srcFile, FM_OPEN_READ | FM_SHARE_READ) )
		{
			texture->Load(file, false);
			file.Close();
		}

		str1024 texfile = srcFile;
		texfile.ExcludeFileExtension();
		texfile << L".dds";
		D3DXSaveTextureToFile(texfile.Text(), D3DXIFF_DDS, texture->GetD3DTexture(), NULL);
	}
	else if ( texture->LoadFromImageFile(srcFile, SelectFormat(srcFile) ) )
	{
		str1024 texfile;

		if (destFile)
			texfile = destFile;
		else
		{
			texfile = srcFile;
			texfile.ExcludeFileExtension();
			texfile << L".txr";
		}
		
		sx::sys::FileStream file;
		if ( file.Open(texfile, FM_CREATE) )
		{
			texture->Save(file);
			file.Close();
		}
		else sxLog::Log(L"Can't save texture to '%s'", texfile);
	}
	else sxLog::Log(L"Can't open texture file '%s'", srcFile);

	sx::d3d::Texture::Manager::Release(texture);
}