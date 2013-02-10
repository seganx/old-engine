#include "EditorUI.h"


//  hold a reference of created textures to avoid releasing them on invalidation that calls by editor objects
static Array<sx::d3d::PTexture> s_poolTX;

void AddTextureToPool(const WCHAR* txName)
{
	sx::d3d::PTexture def = NULL;
	sx::d3d::Texture::Manager::Exist( def, txName );

	if ( s_poolTX.IndexOf( def ) < 0 )
	{
		sx::d3d::Texture::Manager::Get( def, txName );
		def->Activate(0);
		s_poolTX.PushBack( def );
	}
}

void EditorUI::Initialize( void )
{

}

void EditorUI::Finalize( void )
{
	for (int i=0; i<s_poolTX.Count(); i++)
	{
		s_poolTX[i]->Deactivate();
		sx::d3d::Texture::Manager::Release( s_poolTX[i] );
	}
}

const WCHAR* EditorUI::GetTexture( const WCHAR* name )
{
	static String str;
	str = sx::sys::GetAppFolder();
	str.MakePathStyle();
	str << L"Media\\" << name << L".txr";

	if ( sx::sys::FileExist(str) )
		AddTextureToPool(str);
	else
	{
		str = sx::sys::GetAppFolder();
		str.MakePathStyle();
		str << L"Media\\seganx.txr";
	}

	return str;
}

const WCHAR* EditorUI::GetHardwareTexture( void )
{
	SystemInfo sinfo;
	sx::sys::GetSystemInfo(sinfo);
	if (sinfo.LidPresent)
		return  GetTexture(L"Laptop");
	else
		return  GetTexture(L"PC");
}

const WCHAR* EditorUI::GetDefaultFont( int size )
{
	static String tmp;
	tmp = sx::sys::GetAppFolder();
	tmp.MakePathStyle();

	switch (size)
	{
	case 60: tmp << L"Media/Font_Arial_60.fnt"; break;
	case 40: tmp << L"Media/Font_Arial_40.fnt"; break;
	case 30: tmp << L"Media/Font_Arial_30.fnt"; break;
	case 25: tmp << L"Media/Font_Arial_25.fnt"; break;
	case 12: tmp << L"Media/Font_Arial_12.fnt"; break;
	case 11: tmp << L"Media/Font_Arial_11.fnt"; break;
	case 10: tmp << L"Media/Font_Arial_10.fnt"; break;
	case 9: tmp << L"Media/Font_Arial_09.fnt"; break;
	case 8: tmp << L"Media/Font_Arial_08.fnt"; break;

	default: tmp << L"Media/Font_Arial_08.fnt"; break;
	}

	return tmp;
}

const D3DColor EditorUI::GetDefaultFontColor( void )
{
	return D3DColor(1.0f, 1.0f, 1.0f, 0.9f);
}

const WCHAR* EditorUI::GetFormTexture( const WCHAR* name )
{
	static String str;
	str = L"Form";
	str << name;
	return GetTexture(str);
}

const D3DColor EditorUI::GetFormColor( void )
{
	return D3DColor(0.0f, 0.0f, 0.0f, 1.0f);
}

const WCHAR* EditorUI::GetFormCaptionFont( void )
{
	static String tmp;
	tmp = sx::sys::GetAppFolder();
	tmp.MakePathStyle();
	tmp << L"Media\\Font_Form_Caption.fnt";
	return tmp;
}

const D3DColor EditorUI::GetFormCaptionFontColor( void )
{
	return D3DColor(1.0f, 0.9f, 0.6f, 1.0f);
}

const WCHAR* EditorUI::GetScrollTexture( int index )
{
	static String tmp;
	tmp = sx::sys::GetAppFolder();
	tmp.MakePathStyle();
	tmp << L"Media/Scroll" << index << L".txr";
	AddTextureToPool(tmp);
	return tmp;
}

const D3DColor EditorUI::GetListBackColor( void )
{
	return D3DColor(0.0f, 0.0f, 0.0f, 0.4f);
}

const WCHAR* EditorUI::GetListSelectedTexture( void )
{
	return GetTexture(L"Selected");
}

const WCHAR* EditorUI::GetDiskDriveTexture( DWORD type )
{
	switch (type)
	{
	case DRIVE_FIXED:		return GetTexture(L"Drive_HD");
	case DRIVE_CDROM:		return GetTexture(L"Drive_CD");
	case DRIVE_REMOVABLE:	return GetTexture(L"Drive_RM");
	default:				return GetTexture(L"Drive_UN");
	}
}

const WCHAR* EditorUI::GetFileExtTexture( const WCHAR* type )
{
	static String tmp;
	tmp = sx::sys::GetAppFolder();
	tmp.MakePathStyle();

	if (!type)
	{
		tmp << L"Media/FileFormat_other.txr";	
	}
	else if (wcscmp(type, L"<DIR>")==0)
	{
		tmp << L"Media/FileFormat_folder.txr";
	}
	else if (
		wcscmp(type, L"mp3")==0 ||	wcscmp(type, L"wma")==0 ||
		wcscmp(type, L"aac")==0 ||	wcscmp(type, L"ogg")==0 ||
		wcscmp(type, L"wav")==0 ||	wcscmp(type, L"flac")==0 )
	{
		tmp << L"Media/FileFormat_music.txr";
	}
	else if (
		wcscmp(type, L"bmp")==0 ||
		wcscmp(type, L"jpg")==0 ||
		wcscmp(type, L"tga")==0 ||
		wcscmp(type, L"dds")==0 ||
		wcscmp(type, L"png")==0 ||
		wcscmp(type, L"gif")==0)
	{
		tmp << L"Media/FileFormat_picture.txr";
	}
	else if (
		wcscmp(type, L"avi")==0 ||
		wcscmp(type, L"div")==0 ||
		wcscmp(type, L"mpg")==0 ||
		wcscmp(type, L"mp4")==0 ||
		wcscmp(type, L"flv")==0	||
		wcscmp(type, L"mkv")==0	||
		wcscmp(type, L"wmv")==0 ||
		wcscmp(type, L"mpeg")==0)
	{
		tmp << L"Media/FileFormat_movie.txr";
	}
	else if (
		wcscmp(type, L"txt")==0 ||
		wcscmp(type, L"rtf")==0 ||
		wcscmp(type, L"doc")==0)
	{
		tmp << L"Media/FileFormat_text.txr";
	}
	else
		tmp << L"Media/FileFormat_other.txr";
	
	AddTextureToPool(tmp);
	return tmp;
}

const WCHAR* EditorUI::GetButtonTexture( const WCHAR* name, int index )
{
	static String tmp;
	tmp = sx::sys::GetAppFolder();
	tmp.MakePathStyle();
	tmp << L"Media/Button" << name << index << L".txr";
	AddTextureToPool(tmp);
	return tmp;
}

const WCHAR* EditorUI::GetBackgroundTexture( void )
{
	return GetTexture(L"Background");
}

const WCHAR* EditorUI::GetCheckBoxTexture( int index )
{
	static String tmp;
	tmp = sx::sys::GetAppFolder();
	tmp.MakePathStyle();
	tmp << L"Media/CheckBox" << index << L".txr";
	AddTextureToPool(tmp);
	return tmp;
}

const WCHAR* EditorUI::GetEditTexture( void )
{
	return GetTexture(L"EditBack");
}

const WCHAR* EditorUI::GetMemberTexture( NodeMemberType type )
{
	switch (type)
	{
	case NMT_UNKNOWN:		return GetTexture(L"nmt_Node");
	case NMT_MESH:			return GetTexture(L"nmt_Mesh");
	case NMT_ANIMATOR:		return GetTexture(L"nmt_Anim");
	case NMT_GUI:			return GetTexture(L"nmt_GUI");
	case NMT_PARTICLE:		return GetTexture(L"nmt_Particle");
	case NMT_SOUND:			return GetTexture(L"nmt_Sound");
	default:				return GetTexture(L"nmt_null");
	}
}

sx::gui::PLabel EditorUI::CreateLabel( sx::gui::PControl parent, const float width, const int fontSize, const WCHAR* caption /*= NULL*/ )
{
	//  create control
	sx::gui::PLabel lb = sx_new( sx::gui::Label );
	lb->SetParent(parent);
	lb->SetFont( EditorUI::GetDefaultFont(fontSize) );
	lb->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	lb->GetElement(1)->Color() = EditorUI::GetDefaultFontColor();
	lb->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
	lb->SetSize( float2(width, (float)fontSize+10.0f) );
	lb->SetText( caption );

	return lb;
}

sx::gui::PButton EditorUI::CreateButton( sx::gui::PControl parent, const float width, const WCHAR* caption )
{
	//  create control
	sx::gui::PButton bt = sx_new( sx::gui::Button );
	bt->GetElement(0)->Color() = D3DColor(1.0f, 1.0f, 1.0f, 1.0f);
	bt->GetElement(1)->Color() = D3DColor(1.0f, 0.5f, 0.5f, 1.0f);
	bt->GetElement(2)->Color() = D3DColor(0.5f, 0.5f, 0.5f, 1.0f);

	str1024 str;
	if ( SEGAN_BETWEEN(width, 0, 70) )			str = L"Default64";
	else if ( SEGAN_BETWEEN(width, 71, 140) )	str = L"Default128";
	else if ( SEGAN_BETWEEN(width, 141, 2600) )	str = L"Default256";

	bt->GetElement(0)->SetTextureSrc( EditorUI::GetButtonTexture(str, 0) );
	bt->GetElement(1)->SetTextureSrc( EditorUI::GetButtonTexture(str, 0) );
	bt->GetElement(2)->SetTextureSrc( EditorUI::GetButtonTexture(str, 0) );

	bt->SetSize( float2(width, 28.0f) );

	sx::gui::PLabel lb = sx_new( sx::gui::Label );
	lb->SetParent(bt);
	lb->SetAlign(GTA_CENTER);
	lb->SetFont( EditorUI::GetDefaultFont(9) );
	lb->GetElement(0)->Color() = D3DColor(0.0f, 0.0f, 0.0f, 0.0f);
	lb->GetElement(1)->Color() = EditorUI::GetDefaultFontColor();
	lb->RemProperty( SX_GUI_PROPERTY_ACTIVATE );
	lb->SetSize( float2(width, 18.0f) );
	lb->SetText( caption );

	bt->SetParent( parent );

	return bt;
}

sx::gui::PButton EditorUI::CreateButtonEx( sx::gui::PControl parent, const float width, const float height, const WCHAR* texture)
{
	//  create control
	sx::gui::PButton bt = sx_new( sx::gui::Button );

	bt->GetElement(0)->Color() = D3DColor(1.0f, 1.0f, 1.0f, 1.0f);
	bt->GetElement(1)->Color() = D3DColor(1.0f, 0.5f, 0.5f, 1.0f);
	bt->GetElement(2)->Color() = D3DColor(0.5f, 0.5f, 0.5f, 1.0f);
	
	bt->GetElement(0)->SetTextureSrc( texture );
	bt->GetElement(1)->SetTextureSrc( texture );
	bt->GetElement(2)->SetTextureSrc( texture );

	bt->SetSize( float2(width, width) );

	bt->SetParent( parent );

	return bt;
}

sx::gui::PTrackBar EditorUI::CreateTrackbar( sx::gui::PControl parent, const float width, const float fMin, const float fMax, const float angle /*= 0*/ )
{
	//  create control
	sx::gui::PTrackBar gui = sx_new( sx::gui::TrackBar );
	gui->SetParent(parent);
	gui->GetElement(0)->SetTextureSrc( EditorUI::GetScrollTexture(0) );
	gui->GetElement(1)->SetTextureSrc( EditorUI::GetScrollTexture(1) );
	gui->SetSize( float2(width, 18.0f) );
	gui->Rotation().z = angle;
	gui->SetMax( fMax );
	gui->SetMin( fMin );

	return gui;
}

sx::gui::PTextEdit EditorUI::CreateEditBox( sx::gui::PControl parent, const float left, const float width, const WCHAR* text /*= NULL*/ )
{
	//  create control
	sx::gui::PTextEdit gui = sx_new( sx::gui::TextEdit );
	gui->SetParent(parent);
	gui->SetSize( float2(width, 18.0f) );
	gui->SetFont( EditorUI::GetDefaultFont(8) );
	gui->GetElement(0)->SetTextureSrc( GetEditTexture() );
	gui->GetElement(0)->Color() = D3DColor(0.7f, 0.7f, 0.7f, 0.7f);
	gui->GetElement(1)->Color() = EditorUI::GetDefaultFontColor();
	gui->SetText(text);
	gui->Position().x = left;
//	gui->SetAlign(GTA_CENTER);

	return gui;
}

sx::gui::PTextEdit EditorUI::CreateLabeldEditBox( sx::gui::PControl parent, float left, float textWidth, float labelWidth, const WCHAR* caption /*= NULL*/, const WCHAR* text /*= NULL*/ )
{
	//  create control
	sx::gui::PTextEdit gui = CreateEditBox(parent, left + labelWidth + textWidth/2, textWidth, text);
	sx::gui::PLabel lbl = CreateLabel( gui, labelWidth, 8, caption );
	lbl->Position().x = -(labelWidth + textWidth) * 0.5f;

	return gui;
}

sx::gui::PCheckBox EditorUI::CreateCheckBox( sx::gui::PControl parent, const float left, const float width, const WCHAR* caption /*= NULL*/ )
{
	sx::gui::PCheckBox ch = sx_new( sx::gui::CheckBox );
	ch->SetParent(parent);
	ch->SetSize( float2(16.0f, 16.0f) );
	ch->GetElement(0)->SetTextureSrc( GetCheckBoxTexture(0) );
	ch->GetElement(0)->Color() = D3DColor( 1.0f, 0.6f, 0.6f, 1.0f );
	ch->GetElement(1)->SetTextureSrc( GetCheckBoxTexture(1) );
	ch->GetElement(1)->Color() = D3DColor( 0.0f, 0.0f, 0.0f, 1.0f );
	ch->Position().x = left;

	sx::gui::PLabel lbl = CreateLabel( ch, width, 8, caption );
	lbl->Position().x = 10 + width/2;

	return ch;
}

