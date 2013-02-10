/********************************************************************
	created:	2011/05/01
	filename: 	ImportTexture.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some classes and function to import 
				textures to the engine
*********************************************************************/
#ifndef GUARD_ImportTexture_HEADER_FILE
#define GUARD_ImportTexture_HEADER_FILE

#include "ImportEngine.h"

//! convert specified texture and import that to texture library
void ImportTexture(const WCHAR* fileName);

//! convert specified texture to given destination filename. if destination file name was null convent in place
void ConvertTexture(const WCHAR* srcFile, const WCHAR* destFile);

#endif	//	GUARD_ImportTexture_HEADER_FILE