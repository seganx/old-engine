/********************************************************************
	created:	2010/11/17
	filename: 	sxInput_def.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Perprocess defines for input header file
*********************************************************************/
#ifndef GUARD_sxInput_def_HEADER_FILE
#define GUARD_sxInput_def_HEADER_FILE

#include "../../sxLib/lib.h"

#ifndef INITGUID
#define INITGUID
#endif

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment (lib, "dinput8.lib")

#endif	//	GUARD_sxInput_def_HEADER_FILE