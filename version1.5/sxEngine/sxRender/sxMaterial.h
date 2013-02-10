/********************************************************************
	created:	2010/11/11
	filename: 	sxMaterial.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of material. this class have
				some colors and textures depend on shader used
*********************************************************************/
#ifndef GUARD_sxMaterial_HEADER_FILE
#define GUARD_sxMaterial_HEADER_FILE

#include "sxRender_def.h"
#include "sxTexture.h"
#include "sxShader.h"
#include "../sxSystem/sxSystem.h"

#define MATERIAL_TEXTURE_COUNT	4	//  number of textures used in material
#define MATERIAL_FLOAT4_COUNT	4	//  number of vector used in material
#define MATERIAL_FLOAT_COUNT	8	//  number of float used in material

namespace sx { namespace d3d {

	class SEGAN_API Material
	{
		friend class Renderer;

		SEGAN_STERILE_CLASS(Material);

	public:
		Material();
		~Material();

		//! save current material to the stream
		void Save(Stream& stream);

		//! load current material from stream
		void Load(Stream& stream);

		//! set shader of the material
		void SetShader(const WCHAR* src);

		//! return the shader src of the material
		const WCHAR* GetShader(void);

		//! set texture to this material
		void SetTexture(const int index, const WCHAR* src);

		//! set a new texture to this material
		const WCHAR* GetTexture(const int index);

		//! set new option for this material
		void SetOption(DWORD SX_MATERIAL_ option);

		//! return options of this material
		DWORD GetOption(void);

		//! set a float value to this material
		void SetFloat(const int index, const float value);

		//! return a float value of this material. return 0.0 for invalid index
		float GetFloat(const int index);

		//! set vector values to this material
		void SetFloat4(const int index, const float4& value);

		//! return vector values of this material. return zero vector for invalid index
		float4 GetFloat4(const int index);

		//! validate textures
		void Validate(int LOD = 0);

		//! invalidate textures
		void Invalidate();

		//! apply current material to the device.
		void SetToDevice(DWORD SX_MATERIAL_ flag);

	private:
		DWORD				m_Option;		//  some option of material
		sx::d3d::PShader	m_Shader;		//  the shader object used in this material

		//  these resources and values will use in shader
		float				m_Float[MATERIAL_FLOAT_COUNT];		//  these values will use in shader
		sx::math::float4	m_Float4[MATERIAL_FLOAT4_COUNT];	//  values contain color, vector and etc will use in shader
		sx::d3d::PTexture	m_Texture[MATERIAL_TEXTURE_COUNT];	//  these textures used by this material		
		

		DWORD				reserved;	//  used in internal settings
	};
	typedef Material *PMaterial;

}} //  namespace sx { namespace d3d {

#endif	//	GUARD_sxMaterial_HEADER_FILE