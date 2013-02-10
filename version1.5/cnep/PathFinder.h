/********************************************************************
	created:	2011/07/14
	filename: 	PathFinder.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain have some helper function to find path
*********************************************************************/
#ifndef GUARD_PathFinder_HEADER_FILE
#define GUARD_PathFinder_HEADER_FILE

#include "ImportEngine.h"

class PathFinder
{
public:

	// ! return nearest path node to the specified position
	static sx::core::PPathNode GetPathNode_nearest(const float3& pos, const float threshold = 10.0f);

	//! return a path node randomly in specified position and threshold
	static sx::core::PPathNode GetPathNode_random(const float3 pos, const float threshold = 20.0f);

	//! find and fill out path stack between to position. return false if no path found
	static bool FindPath_byPos(const float3 fromPos, const float fromThreshold, const float3 toPos, const float toThreshold, sx::core::ArrayPPathNode& path, UINT group = 0);

	//! find and fill out path stack between to scene node. return false if no path found
	static bool FindPath_byNode(sx::core::PNode start, sx::core::PNode goal, sx::core::ArrayPPathNode& path, UINT group = 0);

};

#endif	//	GUARD_PathFinder_HEADER_FILE