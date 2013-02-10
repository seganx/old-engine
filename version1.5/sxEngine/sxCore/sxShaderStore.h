/********************************************************************
	created:	2011/04/03
	filename: 	sxShaderStore.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some necessary shader codes used in engine
*********************************************************************/
#ifndef GUARD_sxShaderStore_HEADER_FILE
#define GUARD_sxShaderStore_HEADER_FILE

#include "sxCore_def.h"

namespace sx { namespace core {

	//! this class contain some shader sources which used in engine
	class SEGAN_API ShaderStore
	{
	public: 
		static WCHAR* GetShadowFlusher(void);

		static const WCHAR* GetPost_Present( const WCHAR* psCode );

		static WCHAR* GetPost_ShadeScene(void);
	};

}} // namespace sx { namespace core {

#endif	//	GUARD_sxShaderStore_HEADER_FILE