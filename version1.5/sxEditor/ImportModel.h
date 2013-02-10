/********************************************************************
	created:	2012/02/06
	filename: 	ImportModel.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some enumerations and structures used
				to import models to enine
*********************************************************************/
#ifndef GUARD_ImportModel_HEADER_FILE
#define GUARD_ImportModel_HEADER_FILE

#include "ImportEngine.h"

#define IMPORT_MODEL_ONE_NODE		0x00000001		//  pack all meshes into one node
#define IMPORT_MODEL_TEXTURES		0x00000002		//  convert and overwrite all textures
#define IMPORT_MODEL_RENORMAL		0x00000004		//  recompute normals
#define IMPORT_MODEL_CENTERPV		0x00000008		//  move pivot to center of object

struct ImportModelOptions
{
	DWORD			flag;
	DWORD			meshOption;
	float			precisionLOD1;
	float			precisionLOD2;
	str512			shaderName;
	str512			prefix;

	ImportModelOptions()
		: flag( 0 )
		, meshOption( SX_MESH_CASTSHADOW | SX_MESH_RECIEVESHADOW )
		, precisionLOD1( 0.0f )
		, precisionLOD2( 0.0f )
	{
	}
};


#endif	//	GUARD_ImportModel_HEADER_FILE