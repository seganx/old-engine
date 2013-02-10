/********************************************************************
created:	2011/05/06
filename: 	sxShaderPool.cpp
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain an static class contain functions to
			access shader pool used in rendering system
*********************************************************************/
#ifndef GUARD_sxShaderPool_HEADER_FILE
#define GUARD_sxShaderPool_HEADER_FILE

#include "sxRender_def.h"

namespace sx { namespace d3d {

	//! shader pool actually is a pool of shared variables in shaders
	class SEGAN_API ShaderPool
	{
	public:

		//! initialize the shader pool. this will call by engine automatically
		static void Initialize(void);

		//! finalize shader pool and release resources. this will call by engine automatically
		static void Finalize(void);

		//! update shader pool means update some conventional matrices, time and etc...
		static void Update(float elpsTime);

		//! set light properties. pass NULL to any parameters which no need to update
		static void SetLight(const PMatrix sunLight = 0, const PMatrix matLight = 0);

		//! set world matrices in shader pool
		static void SetWorldMatrices(PMatrix arrayMatrix, UINT count);
	};

} }	//	namespace sx { namespace core {

#endif	//	GUARD_sxShaderPool_HEADER_FILE