#include "sxSettings.h"
#include "sxRenderer.h"


//////////////////////////////////////////////////////////////////////////
//	SOME INTERNAL VAIABLES
static float2		sunlightPosition( 0.4f, 0.0f );

namespace sx { namespace core {

	//////////////////////////////////////////////////////////////////////////
	//	OPTION
	//////////////////////////////////////////////////////////////////////////	
	Settings::Option::Option( void ): value(0)
	{
	}

	FORCEINLINE bool Settings::Option::HasPermission( void )
	{
#if _DEBUG
		int by_system	= (value & OPT_BY_SYSTEM);
		int by_artist	= (value & OPT_BY_ARTIST);
		int by_user		= (value & OPT_BY_USER);
		return by_system && by_artist && by_user;
#else
		return (value & OPT_BY_SYSTEM) && (value & OPT_BY_ARTIST) && (value & OPT_BY_USER);
#endif
	}

	FORCEINLINE void Settings::Option::AddPermission( OptionPermissionType OPT_ who )
	{
		SEGAN_SET_ADD(value, who);
	}

	FORCEINLINE void Settings::Option::RemPermission( OptionPermissionType OPT_ who )
	{
		SEGAN_SET_REM(value, who);
	}

	FORCEINLINE int Settings::Option::GetLevel( void )
	{
		return SEGAN_1TH_BYTEOF(value);
	}

	FORCEINLINE void Settings::Option::SetLevel( int level )
	{
		SEGAN_1TH_BYTEOF(value) = (BYTE)level;
	}


	//////////////////////////////////////////////////////////////////////////
	//	SETTINGS
	//////////////////////////////////////////////////////////////////////////
	void Settings::Initialize( void )
	{
		// options of the current texture level
		GetOption_TextureLevel()->SetLevel(0);
		//d3d::Texture::Manager::SetLOD(9);

		//  options of the shadow
		GetOption_Shadow()->AddPermission(OPT_BY_SYSTEM);
		GetOption_Shadow()->AddPermission(OPT_BY_ARTIST);
		GetOption_Shadow()->AddPermission(OPT_BY_USER);
		GetOption_Shadow()->SetLevel(2);

		//  options of the reflection
		GetOption_Reflection()->AddPermission(OPT_BY_SYSTEM);
		GetOption_Reflection()->AddPermission(OPT_BY_ARTIST);
		GetOption_Reflection()->AddPermission(OPT_BY_USER);
		GetOption_Reflection()->SetLevel(0);

		//  sun light properties
		SetSunLightPosition(0.4f, 0.2f);
	}

	FORCEINLINE Settings::POption Settings::GetOption_TextureLevel( void )
	{
		static Option op;
		return &op;
	}

	FORCEINLINE Settings::POption Settings::GetOption_Shadow( void )
	{
		static Option op;
		return &op;
	}

	FORCEINLINE Settings::POption Settings::GetOption_Reflection( void )
	{
		static Option op;
		return &op;
	}

	float2 Settings::GetSunLightPosition( void )
	{
		return sunlightPosition;
	}

	void Settings::SetSunLightPosition( float theta, float phi )
	{
		sx_callstack();

		sunlightPosition.Set(theta, phi);

		Renderer::GetSunLight()->_11 = - cosf(phi) * sinf(theta);
		Renderer::GetSunLight()->_12 = - cosf(theta);
		Renderer::GetSunLight()->_13 = - sinf(phi) * sinf(theta);


		if ( 0 )
		{
			float4 light;
			float f_0_1 = 0.5f + sinf(theta) * 0.5f;
			float f_1_0 = 1.0f - f_0_1;
			float f_1_08 = 1.0f - f_0_1*0.8f;
			float f_0_1_0 = cosf(theta);

			//  set diffuse color
			const float4 dawn_Diffuse(		0.69f, 0.73f, 0.97f, 1.0f	);
			const float4 sunset_Diffuse(	1.00f, 0.90f, 0.70f, 1.0f	);

			light.Lerp( dawn_Diffuse, sunset_Diffuse, f_0_1 );
			light.x *= sqrt(f_0_1_0) * 3.0f;
			light.y *= sqrt(f_0_1_0) * 3.0f;
			light.z *= sqrt(f_0_1_0) * 3.0f;
			SunLightColor().Set(light.x, light.y, light.z, light.w);

			//  set ambient color
			const float4 down_Ambient(		0.36f, 0.38f, 0.52f, 1.0f	);
			const float4 sunset_Ambient(	0.53f, 0.47f, 0.30f, 1.0f	);

			light.Lerp( down_Ambient, sunset_Ambient, f_0_1 );
			light.x *= f_1_08 * 0.5f + f_0_1_0;
			light.y *= f_1_08 * 0.5f + f_0_1_0;
			light.z *= f_1_08 * 0.5f + f_0_1_0;
			AmbientColor().Set(light.x, light.y, light.z, light.w);
		}
	}

	FORCEINLINE float4& Settings::SunLightColor( void )
	{
		return Renderer::SunLightColor();
	}

	FORCEINLINE float4& Settings::AmbientColor( void )
	{
		return Renderer::AmbientColor();
	}

}} // namespace sx { namespace core {

