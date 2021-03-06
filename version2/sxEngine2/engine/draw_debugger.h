/********************************************************************
	created:	2013/03/09
	filename: 	draw_debugger.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some simple functions to draw
				visual debugger
*********************************************************************/
#ifndef GUARD_draw_debugger_HEADER_FILE
#define GUARD_draw_debugger_HEADER_FILE

#include "../../sxLib/Lib.h"

class AABox;

//! draw a simple line
SEGAN_ENG_API void sx_debug_draw_line( const float3& v1, const float3& v2, const Color& color );

//! draw a grid, snapped to integer space but always in front of camera
SEGAN_ENG_API void sx_debug_draw_grid( const uint size, const Color& color );

//! draw a compass by specified size
SEGAN_ENG_API void sx_debug_draw_compass( void );

//! visualize a simple circle
SEGAN_ENG_API void sx_debug_draw_circle( const float3& center, const float radius, const Color& color );

//! visualize a wired box
SEGAN_ENG_API void sx_debug_draw_box( const AABox& box, const Color& color );

//! visualize a wired box
SEGAN_ENG_API void sx_debug_draw_box( const OBBox& box, const Color& color );

//! visualize a simple sphere
SEGAN_ENG_API void sx_debug_draw_sphere( const Sphere& sphere, const Color& color, const uint stacks = 17, const uint slices = 20 );

//! visualize an element of GUI
SEGAN_ENG_API void sx_debug_draw_gui_element( const class uiElement* elem );

#endif	//	GUARD_draw_debugger_HEADER_FILE