#include "GameConfig.h"




int ConfigSearch( sx::cmn::StringList& configList, const WCHAR* name )
{
	for (int i=0; i<configList.Count(); i++)
	{
		int index = configList.At(i)->Find( L"=" );
		if ( index > -1 )
		{
			String strname;
			configList.At(i)->CopyTo(strname, 0, index);
			strname.Trim();

			if ( strname == name )
				return i;
		}
	}

	return -1;
}


//////////////////////////////////////////////////////////////////////////
//  some local variable
static Config::GameConfig	s_configData;
Config::GameConfig::GameConfig( void )
: device_CreationFlag(SX_D3D_VSYNC)
, display_Size( 800,600 )
, display_Debug( 0 )
, game_speed( 1 )
, shaderLevel( 0 )
, shadowLevel( 0 )
, musicVolume( 1 )
, soundVolume( 1 )
, mouseSpeed( 1 )
{
	
}

void Config::LoadConfig( void )
{
	//  load settings
	String confgFile = sx::sys::GetAppFolder();
	confgFile << L"/game.config";
	
	sx::cmn::StringList configList;
	configList.LoadFromFile(confgFile);

	for (int i=0; i<configList.Count(); i++)
	{
		int index = configList.At(i)->Find( L"=" );
		if ( index > -1 )
		{
			String name, value;
			configList.At(i)->CopyTo(name, 0, index);
			configList.At(i)->CopyTo(value, index+1, 255);
			name.Trim();
			value.Trim();

			if ( name == L"shader_level" )
			{
				s_configData.shaderLevel = value.StrToInt(value);
				switch ( s_configData.shaderLevel )
				{
				case 0: sx::d3d::Shader::Manager::SetShaderQuality(SQ_HIGH);	break;
				case 1: sx::d3d::Shader::Manager::SetShaderQuality(SQ_MEDIUM);	break;
				case 2: sx::d3d::Shader::Manager::SetShaderQuality(SQ_LOW);		break;
				}
			}
			else if ( name == L"shadow_level" )
			{
				s_configData.shadowLevel = value.StrToInt(value);
				switch ( s_configData.shadowLevel )
				{
				case 0: case 1: case 2:
					sx::core::Settings::GetOption_Shadow()->SetLevel( s_configData.shadowLevel );
					sx::core::Settings::GetOption_Shadow()->AddPermission( OPT_BY_USER );
					break;
				default:
					sx::core::Settings::GetOption_Shadow()->RemPermission( OPT_BY_USER );
					break;
				}
			}
			else if ( name == L"reflection_level" )
			{
				s_configData.reflectionLevel = value.StrToInt(value);
				switch ( s_configData.reflectionLevel )
				{
				case 0: case 1: case 2: case 3:
					sx::core::Settings::GetOption_Reflection()->SetLevel( s_configData.reflectionLevel );
					sx::core::Settings::GetOption_Reflection()->AddPermission( OPT_BY_USER );
					break;
				default:
					sx::core::Settings::GetOption_Reflection()->RemPermission( OPT_BY_USER );
					break;
				}
			}
			else if ( name == L"music_volume" )
			{
				sx::snd::Device::GetVolume( s_configData.musicVolume, s_configData.soundVolume );
				s_configData.musicVolume = value.StrToInt(value) * 0.01f;
				sx::snd::Device::SetVolume( s_configData.musicVolume, s_configData.soundVolume );
			}
			else if ( name == L"sound_volume" )
			{
				sx::snd::Device::GetVolume( s_configData.musicVolume, s_configData.soundVolume );
				s_configData.soundVolume = value.StrToInt(value) * 0.01f;
				sx::snd::Device::SetVolume( s_configData.musicVolume, s_configData.soundVolume );
			}
			else if ( name == L"mouse_speed" )
			{
				s_configData.mouseSpeed = value.StrToInt(value) * 0.01f;
				sx::io::Input::SendSignal( 0, IST_SET_SPEED, &s_configData.mouseSpeed );
			}
			else if ( name == L"texture_level" )
			{
				int l = value.StrToInt(value);
				sx::d3d::Texture::Manager::SetLOD(l);
			}

			else if ( name == L"fullscreen" )
			{
				int l = value.StrToInt(value);
				if (l)
				{
					s_configData.device_CreationFlag |= SX_D3D_FULLSCREEN;
				}
			}

			else if ( name == L"vsync" )
			{
				int l = value.StrToInt(value);
				if (l)
					s_configData.device_CreationFlag |= SX_D3D_VSYNC;
				else
					s_configData.device_CreationFlag &= ~SX_D3D_VSYNC;
			}

			else if ( name == L"screen_size" )
			{
				int l = value.StrToInt(value);
				s_configData.display_Size.x = sx::sys::GetDesktopWidth() * l / 100;
				s_configData.display_Size.y = sx::sys::GetDesktopHeight() * l / 100;
			}

			else if ( name == L"debug_draw" )
			{
				s_configData.display_Debug = value.StrToInt(value);
			}

			else if ( name == L"mesh_LOD_range" )
			{
				int l = value.StrToInt(value);
				sx::core::Mesh::Manager::LODRange() = l * 0.01f;
			}

			else if ( name == L"mesh_minimumVolume" )
			{
				int l = value.StrToInt(value);
				sx::core::Mesh::Manager::MinimumVolume() = l * 0.01f;
			}

			else if ( name == L"particle_minimumVolume" )
			{
				int l = value.StrToInt(value);
				sx::core::Particle::Manager::MinimumVolume() = l * 0.01f;
			}
		}		
	}

	if ( ConfigSearch( configList, L"mesh_LOD_range" ) < 0 )
		sx::core::Mesh::Manager::LODRange() = 300 * 0.01f;

	if ( ConfigSearch( configList, L"mesh_minimumVolume") < 0 )
		sx::core::Mesh::Manager::MinimumVolume() = 0.01f;

	if ( ConfigSearch( configList, L"particle_minimumVolume") < 0 )
		sx::core::Particle::Manager::MinimumVolume() = 5 * 0.01f;

	//	force to release game
	if ( s_configData.device_CreationFlag & SX_D3D_FULLSCREEN )
	{
		Config::GetData()->display_Size.x = sx::sys::GetDesktopWidth();
		Config::GetData()->display_Size.y = sx::sys::GetDesktopHeight();
	}
	else
	{
		Config::GetData()->display_Size.x = 1024; 
		Config::GetData()->display_Size.y = 1024 * sx::sys::GetDesktopHeight() / sx::sys::GetDesktopWidth();
	}
}

void Config::SaveConfig( void )
{
	//  save settings
	String confgFile = sx::sys::GetAppFolder();
	confgFile << L"/game.config";

	sx::cmn::StringList configList;
	configList.LoadFromFile( confgFile );

	int i = -1;
	str512 tmpstr;

	tmpstr.Format( L"shader_level=%d				// shader quality 		0=High			1=Midd		2=Low" , s_configData.shaderLevel );
	i = ConfigSearch( configList, L"shader_level" );
	if ( i >= 0 )
		configList.At(i)->SetText( tmpstr );
	else
		configList.PushBack( tmpstr );

	tmpstr.Format( L"shadow_level=%d	 			// level of shadow size 0=2048 			1=1024	 	2=512	 	3=OFF" , s_configData.shadowLevel );
	i = ConfigSearch( configList, L"shadow_level" );
	if ( i >= 0 )
		configList.At(i)->SetText( tmpstr );
	else
		configList.PushBack( tmpstr );

	tmpstr.Format( L"reflection_level=%d	 		// level of reflection 	0=Very High		1=High		2=Midd		3=Low		4=OFF" , s_configData.reflectionLevel );
	i = ConfigSearch( configList, L"reflection_level" );
	if ( i >= 0 )
		configList.At(i)->SetText( tmpstr );
	else
		configList.PushBack( tmpstr );

	tmpstr.Format( L"music_volume=%d" , int(s_configData.musicVolume * 100 + 0.5f) );
	i = ConfigSearch( configList, L"music_volume" );
	if ( i >= 0 )
		configList.At(i)->SetText( tmpstr );
	else
		configList.PushBack( tmpstr );

	tmpstr.Format( L"sound_volume=%d" , int(s_configData.soundVolume * 100 + 0.5f) );
	i = ConfigSearch( configList, L"sound_volume" );
	if ( i >= 0 )
		configList.At(i)->SetText( tmpstr );
	else
		configList.PushBack( tmpstr );

	tmpstr.Format( L"mouse_speed=%d" , int(s_configData.mouseSpeed * 100 + 0.5f) );
	i = ConfigSearch( configList, L"mouse_speed" );
	if ( i >= 0 )
		configList.At(i)->SetText( tmpstr );
	else
		configList.PushBack( tmpstr );

	int l = ( s_configData.device_CreationFlag & SX_D3D_FULLSCREEN ) ? 1 : 0;
	tmpstr.Format( L"fullscreen=%d" , l );
	i = ConfigSearch( configList, L"fullscreen" );
	if ( i >= 0 )
		configList.At(i)->SetText( tmpstr );
	else
		configList.PushBack( tmpstr );


	if ( ConfigSearch( configList, L"mesh_LOD_range" ) < 0 )
		configList.PushBack( L"mesh_LOD_range=300 			//  lod range to compute LOD depend on camera distance" );

	if ( ConfigSearch( configList, L"mesh_minimumVolume") < 0 )
		configList.PushBack( L"mesh_minimumVolume=1		//  minimum volume of mesh to display" );

	if ( ConfigSearch( configList, L"particle_minimumVolume") < 0 )
		configList.PushBack( L"particle_minimumVolume=5" );


	configList.SaveToFile( confgFile );

}

Config::PGameConfig Config::GetData( void )
{
	return &s_configData;
}




#if 0
bool Config::Search( const WCHAR* name, int& value )
{
	str512	strName = name;
	if ( name == L"shader_level" )
	{
		value = s_configData.shaderLevel;
		return true;
	}
	else if ( name == L"shadow_level" )
	{
		value = s_configData.shadowLevel;
		return true;
	}
	else if ( name == L"reflection_level" )
	{
		s_configData.reflectionLevel = value.StrToInt(value);
		value = s_configData.reflectionLevel;
		return true;
	}
	else if ( name == L"music_volume" )
	{
		value = s_configData.musicVolume;
		return true;
	}
	else if ( name == L"sound_volume" )
	{
		value = s_configData.soundVolume;
		return true;
	}
	else if ( name == L"mouse_speed" )
	{
		value = s_configData.mouseSpeed;
		return true;
	}
	else if ( name == L"texture_level" )
	{
		value = (int)sx::d3d::Texture::Manager::GetLOD();
		return true;		
	}

	else if ( name == L"fullscreen" )
	{
		value = s_configData.device_CreationFlag & SX_D3D_FULLSCREEN;
		return true;
	}

	else if ( name == L"vsync" )
	{
		value = s_configData.device_CreationFlag & SX_D3D_VSYNC;
		return true;
	}

	// 	else if ( name == L"screen_size" )
	// 	{
	// 		value = s_configData.shadowLevel;
	// 		return true;
	// 		int l = value.StrToInt(value);
	// 		s_configData.display_Size.x = sx::sys::GetDesktopWidth() * l / 100;
	// 		s_configData.display_Size.y = sx::sys::GetDesktopHeight() * l / 100;
	// 	}

	else if ( name == L"debug_draw" )
	{
		value = s_configData.display_Debug;
		return true;
	}

	else if ( name == L"mesh_LOD_range" )
	{
		//value = sx::core::Mesh::Manager::LODRange();
		return true;
	}

	else if ( name == L"mesh_minimumVolume" )
	{
		//value = sx::core::Mesh::Manager::MinimumVolume();
		return true;
	}

	else if ( name == L"particle_minimumVolume" )
	{
		//value = sx::core::Particle::Manager::MinimumVolume();
		return true;
	}
}
#endif