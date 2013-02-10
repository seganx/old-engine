#include "PathFinder.h"

using namespace sx::core;


PPathNode	s_lastStart[32];
PPathNode	s_lastEnd[32];
int s_inti(void)
{
	ZeroMemory( &s_lastStart, sizeof(s_lastStart) );
	ZeroMemory( &s_lastEnd, sizeof(s_lastEnd) );
	return 0;
}
int init = s_inti();

int cmpPathNode( const PNode& n1, const PNode& n2 )
{
	UINT t1 = n1->GetUserTag();
	UINT t2 = n2->GetUserTag();
	return ( t1 > t2 ? 1 : ( t1 < t2 ? -1 : 0 ) );
}

sx::core::PPathNode PathFinder::GetPathNode_nearest( const float3& pos, const float threshold )
{
	sx_callstack_push(PathFinder::GetPathNode_nearest());

	static ArrayPNode_inline nodes(512); nodes.Clear();
	Scene::GetNodesByArea(pos, threshold, nodes, NMT_PATHNODE);

	//  find the nearest node
	PNode nodeFounded = NULL;
	float dis = FLT_MAX;
	for (int i=0; i<nodes.Count(); i++)
	{
		PNode pNode = nodes[i];
		float d = pos.Distance_sqr( pNode->GetPosition_world() );
		if ( d < dis )
		{
			dis = d;
			nodeFounded = pNode;				
		}
	}

	//  if node founded then extract path node
	PPathNode pathNode = NULL;
	if ( nodeFounded )
	{
		for (int i=0; i<nodeFounded->GetMemberCount(); i++)
		{
			if ( nodeFounded->GetMemberByIndex(i)->GetType() == NMT_PATHNODE )
			{
				pathNode = (PPathNode)nodeFounded->GetMemberByIndex(i);
				break;
			}
		}
	}

	return pathNode;
}

sx::core::PPathNode PathFinder::GetPathNode_random( const float3 pos, const float threshold )
{
	sx_callstack_push(PathFinder::GetPathNode_random());

	static ArrayPNode_inline nodes(512); nodes.Clear();
	Scene::GetNodesByArea(pos, threshold, nodes, NMT_PATHNODE);

	int index = sx::cmn::Random( (int)nodes.Count() );
	PNode nodeFounded = nodes[index];

	//  if node founded then extract path node
	PPathNode pathNode = NULL;
	for (int i=0; i<nodeFounded->GetMemberCount(); i++)
	{
		if ( nodeFounded->GetMemberByIndex(i)->GetType() == NMT_PATHNODE )
		{
			pathNode = (PPathNode)nodeFounded->GetMemberByIndex(i);
			break;
		}
	}

	return pathNode;
}

bool PathFinder::FindPath_byPos( const float3 fromPos, const float fromThreshold, const float3 toPos, const float toThreshold, sx::core::ArrayPPathNode& path, UINT group /*= 0*/ )
{
	sx_callstack_push(PathFinder::FindPath_byPos());

	//  find start node
	ArrayPNode_inline startNodes(512);
	Scene::GetNodesByArea(fromPos, fromThreshold, startNodes, NMT_PATHNODE);

	//  find end node
	ArrayPNode_inline endNodes(512);
	Scene::GetNodesByArea(toPos, toThreshold, endNodes, NMT_PATHNODE);

	//  make them randomize
	for ( int i=0; i<startNodes.Count(); i++ )
		startNodes[i]->SetUserTag( sx::cmn::Random(10000) );
	for ( int i=0; i<endNodes.Count(); i++ )
		endNodes[i]->SetUserTag( sx::cmn::Random(10000) );
	startNodes.Sort(&cmpPathNode);
	endNodes.Sort(&cmpPathNode);


	for (int i=0; i<startNodes.Count(); i++)
	{
		PPathNode start = (PPathNode)startNodes[i]->GetMemberByIndex(0);

		if ( start == s_lastStart[group] && ( i < startNodes.Count() - 1 ) )
			continue;
		s_lastStart[group] = start;

		for ( int j=0; j<endNodes.Count(); j++ )
		{
			PPathNode end = (PPathNode)endNodes[j]->GetMemberByIndex(0);

			if ( end == s_lastEnd[group] && ( j < endNodes.Count() - 1 ) )
				continue;
			s_lastEnd[group] = end;

			if ( PathNode::FindPath( start, end, path ) )
				return true;
		}
	}

	return false;
}

bool PathFinder::FindPath_byNode( sx::core::PNode start, sx::core::PNode goal, sx::core::ArrayPPathNode& path, UINT group /*= 0*/ )
{
	if ( !start || !goal ) return false;

	sx_callstack_push(PathFinder::FindPath_byNode());

	bool res = FindPath_byPos(
		start->GetPosition_world(), start->GetSphere_local().r, 
		goal->GetPosition_world(), goal->GetSphere_local().r,
		path, group );

	if ( !res )
		res = FindPath_byPos(
		start->GetPosition_world(), start->GetSphere_local().r, 
		goal->GetPosition_world(), goal->GetSphere_local().r,
		path, group );

	return res;
}
