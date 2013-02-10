/********************************************************************
	created:	2010/10/01
	filename: 	sxUI3D.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some function to draw UI and some
				view port tools like Line, Circle, Rectangle, Compass, 
				SelectBox, Grid and etc.
*********************************************************************/
#ifndef GUARD_sxUI3D_HEADER_FILE
#define GUARD_sxUI3D_HEADER_FILE

#include "sxTypes3D.h"

namespace sx { namespace d3d
{

class SEGAN_API UI3D
{
public:
	//! prepare device to render for debug mode
	static void ReadyToDebug(const D3DColor color);

	//!	draw a line between v1 and v2
	static void DrawLine(const Vector& v1, const Vector& v2);

	//! draw a simple quad. useful for posts and screen space effects
	static void DrawQuad(void);

	//! draw a filled rectangle with fixed UV to display a texture
	static void DrawRectangle(const Vector& v1, const Vector& v2, const Vector& v3, const Vector& v4);

	//! draw a wired box formed by by four lines
	static void DrawWiredRectangle(const Vector& v1, const Vector& v2, const Vector& v3, const Vector& v4);

	//! draw a 3d box. the application is responsible to set world matrix and/or render state wired or not
	static void DrawAABox(const math::AABox& box, const D3DColor color);

	//! draw a 3d box. the application is responsible to set world matrix and/or render state wired or not
	static void DrawOBBox(const math::OBBox& box, const D3DColor color);

	//! draw a wired box formed by six quad without any triangles
	static void DrawWiredAABox(const math::AABox& box, const D3DColor color);

	//! draw a wired box formed by six quad without any triangles
	static void DrawWiredOBBox(const math::OBBox& box, const D3DColor color);

	//! draw a clipped lined box to show selected object
	static void DrawSelectAABox(const math::AABox& box, const D3DColor color);

	//! draw a clipped lined box to show selected object
	static void DrawSelectOBBox(const math::OBBox& box, const D3DColor color);

	//!	draw a circle in by specified radius
	static void DrawCircle(const Matrix& matWorld, const float radius, const D3DColor color);

	//! draw a 3d sphere. the application is responsible to set render state wired or not
	static void DrawSphere(const Sphere& sphere, const D3DColor color);

	//! draw a simple sphere formed by three crossed circles
	static void DrawWiredSphere(const Sphere& sphere, const D3DColor color);

	//! draw a simple cone
	static void DrawCone(const D3DColor color);

	//! draw a compass by specified size
	static void DrawCompass(void);

	//! draw a grid, snapped to integer space but always in front of camera
	static void DrawGrid(int size, const D3DColor& color);

	//! return true if ray intersect with rectangle
	static bool IntersectRect(Rect3D& rc, Matrix& mat, Ray& ray, float3& OUT hitPoint, bool twoSide = false);

	//! return true if ray intersect with cone
	static bool IntersectCone(Matrix& mat, Ray& ray, float3& OUT hitPoint);
};

} } // namespace

#endif // GUARD_sxUI3D_HEADER_FILE