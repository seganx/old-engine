/********************************************************************
	created:	2010/10/26
	filename: 	sxSimplifier3D.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some functions for work with geometries
*********************************************************************/
#ifndef GUARD_sxGeometry3DTool_HEADER_FILE
#define GUARD_sxGeometry3DTool_HEADER_FILE

#include "sxGeometry3D.h"

namespace sx { namespace d3d 
{

	class SEGAN_API GeometryTool
	{
	public:

		//! weld geometry vertices
		static void WeldVertices(IN_OUT PGeometry3D pInGeometry, bool weldPartial = true);

		//! compute normals
		static void ComputeNormals(IN_OUT PGeometry3D pInGeometry);

		//! compute tangents
		static void ComputeTangents(IN_OUT PGeometry3D pInGeometry);

		//! Optimize by removing back faces and vertex cache reordering
		static void Optimize(IN_OUT PGeometry3D pInGeometry);

		//! weld vertices and optimize geometry and compute normals and tangents
		static void Weld_Optimize_ComputeNT(IN_OUT PGeometry3D pInGeometry, bool weldPartial = true);

		//! refill out geometry by simplified main geometry
		static void Simplify(IN PGeometry3D pInGeometry, OUT PGeometry3D pOutGeometry, UINT MinFace, const WCHAR* useLog);

	};

} } // namespace sx { namespace d3d


#endif	//	GUARD_sxGeometry3DTool_HEADER_FILE
