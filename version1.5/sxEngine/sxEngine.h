/********************************************************************
	created:	2010/09/27
	filename: 	sxEngine.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain any header files and declaration for
				useing of seganx engine functions and engine classes
*********************************************************************/
#ifndef GUARD_sxEngine_HEADER_FILE
#define GUARD_sxEngine_HEADER_FILE

#include "../sxLib/Lib.h"
#include "sxCommon/sxCommon.h"
#include "sxSystem/sxSystem.h"
#include "sxRender/sxRender.h"
#include "sxInput/sxInput.h"
#include "sxGUI/sxGUI.h"
#include "sxCore/sxCore.h"
#include "sxSound/sxSound.h"


void SEGAN_API sx_engine_init();
void SEGAN_API sx_engine_finit();


#endif	//	GUARD_sxEngine_HEADER_FILE
