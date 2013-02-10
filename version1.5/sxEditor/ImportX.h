/********************************************************************
	created:	2010/09/08
	filename: 	ImportX.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple class to import x files
*********************************************************************/
#ifndef GUARD_ImportX_HEADER_FILE
#define GUARD_ImportX_HEADER_FILE


#include "ImportEngine.h"

typedef struct XVertex
{
	float3		pos;
	float3		nrm;
	float2		txc0;
	D3DCOLOR	col0;
	D3DCOLOR	col1;
	float3		tng;
	float2		txc1;
	float4		bw;
	BYTE		bi[4];

} *PXVertex;


typedef void* XMesh;

bool xmesh_LoadFile(OUT XMesh& mesh, const WCHAR* fileName);

bool xmesh_Filter(XMesh mesh);

bool xmesh_ComputeNormal(XMesh mesh);

bool xmesh_ComputeTangent(XMesh mesh);

bool ImportXToLibrary(const WCHAR* fileName, sx::core::PNode& node);

#endif	//	GUARD_ImportX_HEADER_FILE
