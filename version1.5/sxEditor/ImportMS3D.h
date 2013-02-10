/********************************************************************
	created:	2011/04/24
	filename: 	ImportMS3D.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some functions to import ms3d file

				NOTE: for import ms3d animations separately, system use
				a text file which contain animation names and key frames.
				this file named same as main file with '.txt' extension.
				formation of the file is simple : 
				[start frame]-[end frame] [name of animation]

				sample:
				0-100		idle
				101-150		walk
*********************************************************************/
#ifndef GUARD_ImportMS3D_HEADER_FILE
#define GUARD_ImportMS3D_HEADER_FILE

#include "ImportEngine.h"

#define IMPORT_MS3D_MODEL		0x0000001	//  import ms3d model to project
#define IMPORT_MS3D_ANIMATION	0x0000002	//  import ms3d animation

/*! 
Import MS3D file to the project directory and return saved node

NOTE: for import ms3d animations separately, system use
a text file which contain animation names and key frames.
this file named same as main file with '.txt' extension.
formation of the file in simple : 
[start frame]-[end frame] [name of animation]

sample:
0-100		idle
101-150		walk
*/
void ImportMS3DFile(const WCHAR* FileName, const UINT flag, OUT sx::core::PNode& node);

#endif	//	GUARD_ImportMS3D_HEADER_FILE
