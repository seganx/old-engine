/********************************************************************
created:	2010/11/06
filename: 	sxShader3D.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain class of Shader3D. this class compile
			needed shader for Device3D, contain shader variables and etc.
*********************************************************************/
#ifndef GUARD_sxShader3D_HEADER_FILE
#define GUARD_sxShader3D_HEADER_FILE

#include "sxRender_def.h"
#include "sxTypes3D.h"
#include "../sxSystem/sxSystem.h"

namespace sx { namespace d3d {

	//! a shader class that represent Direct3D PS/VS shader resources
	class SEGAN_API Shader3D
	{
		SEGAN_STERILE_CLASS(Shader3D);

	public:
		Shader3D(void);
		~Shader3D(void);

		//! return true if shader has been compiled and is ready to use
		bool Exist(void);

		//! compile shader code in *.FX format
		bool CompileShader(const WCHAR* code, SQ_ ShaderQuality quality, const WCHAR* userDebugDesc = NULL);

		//! return the description of the shader
		bool GetDesc(D3DShaderDesc& desc);

		//! return the handle of the render technique by index
		D3DShaderHandle GetRenderTechnique(int index);

		//! return the handle of the render technique by name
		D3DShaderHandle GetRenderTechnique(const char* name);

		//! set the current active shader technique
		void SetRenderTechnique(D3DShaderHandle rTech);

		//! return the handle of the parameter by index
		D3DShaderHandle GetParameter(int index);

		//! return the handle of the parameter by name
		D3DShaderHandle GetParameter(const char* name);

		//! return the index and handle of the parameter by semantic. return -1 if semantic not found.
		int GetParameter(const char* semantic, PD3DShaderHandle paramHandle);

		//! set parameter value to the shader
		void SetValue(D3DShaderHandle hParam, const void* pData, UINT numBytes);

		//! get parameter value from the shader. return false on fail
		bool GetValue(D3DShaderHandle hParam, void* pData, UINT numBytes);

		//! set an array of matrices
		void SetMatrixArray(D3DShaderHandle hParam, const PMatrix pMatrix, UINT Count);

		//! return the description of the parameter
		bool GetParameterDesc(int index, OUT D3DShaderParamDesc& desc);

		//! return the handle of annotation of the parameter if the parameter has annotation
		D3DShaderHandle GetAnnotation(int paramIndex, OUT D3DShaderParamDesc& anno, int annoIndex = 0);

		//! active current technique and set specified VS/PS to the device
		void SetToDevice(void);

	private:
		PD3DXEffect			m_Effect;		//  use to compile the shader
		D3DShaderHandle		m_curTech;		//  use to notify render tech changes
	};
	typedef Shader3D *PShader3D;

}}  //  namespace sx { namespace d3d {


#endif // GUARD_sxShader3D_HEADER_FILE