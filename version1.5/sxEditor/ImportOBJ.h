/********************************************************************
	created:	2011/11/18
	filename: 	ImportOBJ.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a function to import OBJ file
*********************************************************************/
#ifndef GUARD_ImportOBJ_HEADER_FILE
#define GUARD_ImportOBJ_HEADER_FILE

#include "ImportModel.h"

/*! 
Import OBJ file to the project directory and return saved nodes
*/
void ImportOBJFile(const WCHAR* FileName, OUT sx::core::ArrayPNode& nodeList, ImportModelOptions* importOption);


/*! 
export all nodes in the node list to OBJ file
*/
void ExportOBJFile(const WCHAR* FileName, IN sx::core::ArrayPNode& nodeList);

#endif	//	GUARD_ImportOBJ_HEADER_FILE
