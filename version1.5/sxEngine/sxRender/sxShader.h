/********************************************************************
created:	2010/11/09
filename: 	sxShader.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain Shader interface that use internal
			Shader3D class and manage some internal variable parameters
			and simple internal manager. The shader abstract class 
			can't be initialized or create directly. Use PShader type 
			and Shader::Manager to create or release usable shaders.
*********************************************************************/
#ifndef GUARD_sxShader_HEADER_FILE
#define GUARD_sxShader_HEADER_FILE

#include "sxShader3D.h"

namespace sx { namespace d3d {

	// forward declaration
	typedef class Shader *PShader;
	typedef class ShaderParameter *PShaderParameter;
	typedef class Texture *PTexture;

	/*!
	Shader class represents Direct3D PS/VS shader resources
	Shader abstract class can't be initialized or create directly. 
	Use PShader type and Shader::Manager to create or release usable shader.
	*/
	class SEGAN_API Shader
	{
	public:
		//! return the ID of this resource
		virtual DWORD GetID(void) = 0;

		//! set source file of the shader in *.FX format. this call will update ID by CRC32 algorithm
		virtual void SetSource(const WCHAR* srcAddress) = 0;

		//! return the source of the shader file
		virtual const WCHAR* GetSource(void) = 0;

		//! compile shader code by specified quality
		virtual bool Compile(SQ_ ShaderQuality quality) = 0;

		//! get number of parameters used in this shader
		virtual int GetParamCount(void) = 0;

		//! get parameter by index. return null for invalid index
		virtual const PShaderParameter GetParam(int index) = 0;

		//! set VS/PS to the device. return false if setting shader failed.
		virtual void SetToDevice(DWORD SX_SHADER_ flag) = 0;


		//////////////////////////////////////////////////////////////////////////
		//  INTERNAL MANAGER
		//////////////////////////////////////////////////////////////////////////
		class SEGAN_API Manager
		{
		public:

			/*! 
			return true if shader of the 'src' is exist in the manager and throw out founded one.
			NOTE: this function DO NOT increase the internal reference counter of the object. so releasing
			the object after the work finished may cause to destroy the object.
			*/
			static bool Exist(OUT PShader& pShdr, const WCHAR* src);

			/*!
			search for shader of the src. if not exist in manager, create and return new one.
			NOTE: this function increase the internal reference counter of the object. so release
			the object after the work finished.
			*/
			static bool Get(OUT PShader& pShdr, const WCHAR* src);

			//! create a new shader independent of the others.
			static bool Create(OUT PShader& pShdr, const WCHAR* src);

			//! decrease the internal reference counter and destroy the shader if reference counter became zero.
			static void Release(PShader& pShdr);

			//! clear all shader in the manager
			static void ClearAll(void);

			//! set shader quality for all shaders
			static void SetShaderQuality(ShaderQuality quality);

			//! return current shader quality of shaders
			static ShaderQuality GetShaderQuality(void);

		};

	};

	//! shader parameter hold shader handle and set them to the shader
	class SEGAN_API ShaderParameter
	{
	public:
		//! return description of the parameter
		virtual const PShaderAnnotation GetDesc(void) = 0;

		//! set float value to the shader parameter
		virtual void SetFloat(float f) = 0;

		//! set vector to the shader parameter
		virtual void SetFloat4(float4& f) = 0;

		//! set float/float4 to the shader parameter
		void SetValue(float* f);
	};


}}  //  namespace sx { namespace d3d {


#endif // GUARD_sxShader_HEADER_FILE