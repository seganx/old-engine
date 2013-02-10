#include "sxShaderStore.h"

namespace sx { namespace core {

	const WCHAR* ShaderStore::GetPost_Present( const WCHAR* psCode )
	{
		static String result;
		result = 
			L"matrix matWorld = {1, 0, 0, 0,\n\
			0, 1, 0, 0,\n\
			0, 0, 0, 0,\n\
			0, 0, 0, 1};\n\
			shared float fTime;\n\
		void sceneVS ( 	in   float4 vPos : POSITION,\n\
			in   float2 vTex : TEXCOORD0,\n\
			out  float4 oPos : POSITION,\n\
			out  float2 oTex : TEXCOORD0)\n\
		{\n\
			// Output the position\n\
			oPos = mul(	vPos, matWorld );\n\
			oTex = vTex;\n\
		}\n\
		sampler samp0 : register(s0) = sampler_state{AddressU=Clamp;AddressV=Clamp;MinFilter=Linear;MagFilter=Linear;MipFilter=Linear;};\n\
		sampler samp1 : register(s1);\n\
		float4 scenePS ( float2 tex0 : TEXCOORD0 ) : COLOR0\n\
		{\n";

		result << psCode << L"\n\
		}\n\
		technique rtech_main\n\
		{\n\
			pass P0\n\
			{\n\
				VertexShader = compile vs_2_0 sceneVS();\n\
				PixelShader  = compile ps_2_0 scenePS();\n\
			}\n\
		}\n";

		return result.Text();
	}

	WCHAR* ShaderStore::GetPost_ShadeScene( void )
	{
		return 
		L"shared matrix matLight;\n\
		matrix matWorld = {	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,	0, 0, 0, 1};\n\
		void vs_main( 	in   float4 vPos : POSITION,\n\
			in   float2 vTex : TEXCOORD0,\n\
			out  float4 oPos : POSITION,\n\
			out  float2 oTex : TEXCOORD0){\n\
			oPos = mul(	vPos, matWorld );\n\
			oTex = vTex;}\n\
		sampler2D sampZdepth : register(s0) = sampler_state{AddressU=Clamp;AddressV=Clamp;MinFilter=Point;MagFilter=Point;MipFilter=Point;};\n\
		sampler2D sampNormal : register(s1) = sampler_state{AddressU=Clamp;AddressV=Clamp;MinFilter=Point;MagFilter=Point;MipFilter=Point;};\n\
		void ps_main(	in float2 tex0	: TEXCOORD0,\n\
				out float4 col0 : COLOR0){\n\
				float4 z = tex2D( sampZdepth, tex0 );\n\
				half3 n3 = ( tex2D( sampNormal, tex0 ).xyz - 0.5f ) * 2.0f;\n\
				float d = max( 0, dot( matLight[0].xyz, n3 ) );\n\
				col0 = float4( z.rgb * (matLight[1].rgb * d + matLight[2].rgb), 1.0f );\n}\n\
		technique rtech_main\n{\n pass P0 {\n\
			VertexShader = compile vs_2_0 vs_main();\n\
			PixelShader  = compile ps_2_0 ps_main();\n\
		}}\n";
	}

	WCHAR* ShaderStore::GetShadowFlusher( void )
	{
		return 
			L"matrix matWorld = {1, 0, 0, 0,\n\
			 0, 1, 0, 0,\n\
			 0, 0, 0, 0,\n\
			 0, 0, 0, 1};\n\
			 void sceneVS ( 	in   float4 vPos : POSITION,\n\
			 in   float2 vTex : TEXCOORD0,\n\
			 out  float4 oPos : POSITION,\n\
			 out  float2 oTex : TEXCOORD0)\n\
			 {\n\
			 // Output the position\n\
			 oPos = mul( vPos, matWorld );\n\
			 oTex = vTex;\n\
			 }\n\
			 sampler samp0 : register(s0);\n\
			 float4 scenePS ( float2 Tex : TEXCOORD0 ) : COLOR0\n\
			 {\n\
			 return tex2D( samp0, Tex ) ;\n\
			 }\n\
			 technique rtech_main\n\
			 {\n\
			 pass P0\n\
			 {\n\
			 VertexShader = compile vs_2_0 sceneVS();\n\
			 PixelShader  = compile ps_2_0 scenePS();\n\
			 }\n\
			 }\n";
	}

}} // namespace sx { namespace core {