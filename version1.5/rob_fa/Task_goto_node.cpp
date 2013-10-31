#include "Task_goto_node.h"
#include "Entity.h"
#include "PathFinder.h"
#include "Game.h"
#include "Player.h"
#include "GameGUI.h"
#include "GameConfig.h"

Task_goto_node::Task_goto_node( void )
: Task()
, m_stunValue(0)
, m_stunTime(0)
, m_animScale(0)
, m_startNode(0)
, m_endNode(0)
, m_targetNode(0)
, m_group(0)
, m_matIndex(-1)
{
	sx_callstack();

	m_Type = GTT_GOTO_NODE;
}

Task_goto_node::~Task_goto_node( void )
{
	sx_callstack();
}

void Task_goto_node::Initialize( void )
{
	sx_callstack();

	Task::Initialize();

	if ( !m_startNode || !m_endNode ) return;

	if ( m_endNode->GetPosition_world().Distance_sqr( m_owner->GetPosition() ) > 1.0f )
	{
		m_Path.Clear();

		if ( !PathFinder::FindPath_byNode( m_startNode, m_endNode, m_Path, m_group ) )
			m_Status = TS_FAILED;
	}
}

void Task_goto_node::Finalize( void )
{
	sx_callstack();

	m_Path.Clear();
	m_startNode = 0;
	m_endNode = 0;
	m_stunValue = 0;
	m_stunTime = 0;
	m_animScale = 1;
	m_targetNode = 0;
	m_group = 0;
	m_matIndex = 0;
}

void Task_goto_node::Update( float elpstime, DWORD& status )
{
	if ( m_Status == TS_FAILED || m_owner->m_health.icur<1 || !m_startNode || !m_endNode ) return;
	
	sx_callstack();

	float3 disPos = m_endNode->GetPosition_world();
	if ( !m_owner->m_move.moveSpeed || disPos.Distance_sqr( m_owner->GetPosition() ) < 1.0f )
	{
		status = TS_COMPLETED;
		m_Status = TS_COMPLETED;
		return;
	}

	//  we should have some checkups to verify that moving has no problem
	if ( false )
	{
		status = TS_FAILED;		//  rapid reply
		m_Status = TS_FAILED;	//  this will removed in next loop from sub states
		return;
	}

	//  just move entity to target position
	float3 curPos		= m_owner->GetPosition();
	float3 curDir		= m_owner->GetDirection();
	float moveSpeed		= ( m_owner->m_move.moveSpeed / 100.0f ) * m_owner->m_move.moveScale;

	//  check the stun value
	if ( m_stunTime > 0 )
	{
		 m_stunTime -= elpstime * 0.001f;

		 if ( m_stunValue > 0 )
			 moveSpeed /= ( m_stunValue + 1 );
	}
	else
	{
		m_stunValue = 1;

		if ( m_matIndex > -1 )
		{
			msg_Mesh mesh( 0, 0, L"body", m_matIndex );
			m_owner->m_node->MsgProc( MT_MESH, &mesh );
			m_matIndex = -1;

			if ( m_owner->m_move.animScale > 0 )
				m_owner->m_move.animScale = 1.0f;
		}
	}

	if ( !m_Path.IsEmpty() && m_targetNode<m_Path.Count() && m_targetNode >= 0 )
	{
		disPos = m_Path[m_targetNode]->GetPosition();
		if ( curPos.Distance_sqr( disPos ) < 2.0f )
			m_targetNode += m_stunValue < 0 ? -1 : +1;

		if ( m_owner == Entity::GetSelected()	&&	m_owner->m_partyCurrent == PARTY_ENEMY	&& 
			(m_targetNode > m_Path.Count() - 10	||	m_targetNode < 5) )
		{
			g_game->m_player->m_camera_MBL.Attach( NULL );
			g_game->m_player->m_camera_MBL.m_Activate = false;
		}
	}

	float3 movedir = disPos - curPos;
	movedir.Normalize( movedir );

	//	update position offset
	if ( m_owner->m_move.type == GMT_GROUND )
	{
		curDir += (movedir - curDir) * elpstime * 0.003f;
	}
	else
	{
		float eltime = elpstime * 0.003f;
		curDir.x += ( movedir.x - curDir.x ) * eltime;
		curDir.y = 0;
		curDir.z += ( movedir.z - curDir.z ) * eltime;
	}
	curPos += movedir * elpstime * 0.1f * moveSpeed;

	m_owner->SetPosition( curPos );
	m_owner->SetDirection( curDir );

	if ( m_owner->m_move.type == GMT_AIR )
		m_owner->m_posOffset.y += 0.0f - m_owner->m_posOffset.y * elpstime * 0.0007f;

	if ( m_owner->m_move.walk )
		m_owner->SetState(ES_WALK);

	if ( m_owner->m_node && m_animScale != m_owner->m_move.animScale )
	{
		m_animScale = m_owner->m_move.animScale;
		msg_Animator anim( 0, 0, 0, -1, -1, m_animScale );
		m_owner->m_node->MsgProc( MT_ANIMATOR, &anim );

		if ( Config::GetData()->display_Debug == 3 && m_matIndex < 0 && m_owner->m_move.animSpeed < 0.5f )
			sxLog::Log( L"WARNING : entity %s has %.2f animSpeed !", m_owner->m_typeName.Text(), m_owner->m_move.animSpeed );
	}

	//	compute traveling value
	{
		const float maxdistance = (float)m_Path.Count(); //m_startNode->GetPosition_world().Distance_sqr( m_endNode->GetPosition_world() );
		const float curdistance = (float)m_targetNode; //curPos.Distance_sqr( m_endNode->GetPosition_world() );
		m_owner->m_traveling = sx_clamp_f( 1.0f - (curdistance / maxdistance), 0.0f, 1.0f );
	}
}

FORCEINLINE void Task_goto_node::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(Task_goto_node::MsgProc(msg=%d), msg);

	switch (msg)
	{
	case GMT_I_DIED:
		if ( data == m_owner )
		{
			if ( m_matIndex > -1 )
			{
				msg_Mesh mesh( 0, 0, L"body", m_matIndex );
				m_owner->m_node->MsgProc( MT_MESH, &mesh );
				m_matIndex = -1;
			}

			m_owner->m_move.animScale = 1.0f;	
			msg_Animator anim( 0, 0, 0, -1, -1, 1.0f );
			m_owner->m_node->MsgProc( MT_ANIMATOR, &anim );
		}
		break;

	case GMT_GOTO:
		if( data )
		{
			Mission* mission = (Mission*)data;
			m_startNode = (sx::core::PNode)mission->data0;
			m_endNode	= (sx::core::PNode)mission->data1;
			m_group		= int( mission->pos.x );
		}
		break;
		
	case GMT_DAMAGE:
		if ( data )
		{
			if ( m_owner->m_health.icur < 1 && m_matIndex > -1 )
			{
				m_owner->m_move.animScale = 1.0f;	
				msg_Animator anim( 0, 0, 0, -1, -1, 1.0f );
				m_owner->m_node->MsgProc( MT_ANIMATOR, &anim );

				msg_Mesh mesh( 0, 0, L"body", m_matIndex );
				m_owner->m_node->MsgProc( MT_MESH, &mesh );
				m_matIndex = -1;

				break;
			}

			msgDamage* damage = (msgDamage*)data;
			if ( damage->stunTime < 0.5f )	break;		//	message is invalid
			if ( m_stunValue > 15 )						//	entity is in freeze mode
			{
				if ( damage->sender )
					damage->sender->MsgProc( GMT_LEAVE_ME, m_owner );
				break;
			}

			//	verify that entity should go back
			if ( m_stunValue >= 0 && damage->stunValue < 0 )
			{
				//	verify that this entity is not an attacker
				sx::core::PNode weapon = NULL;
				m_owner->m_node->GetChildByName( L"weapon", weapon );
				if ( !weapon )
				{
					m_stunValue = damage->stunValue;

					if ( damage->stunTime > m_stunTime )
						m_stunTime = damage->stunTime;

					if ( m_targetNode > 0 )
						m_targetNode--;

					if ( damage->sender )
						damage->sender->MsgProc( GMT_LEAVE_ME, m_owner );
				}
			}
			else	//	the entity should be slow down or freeze
			{
				//	verify that the entity should be freeze
				if ( damage->stunValue > 15 && m_matIndex < 0 && m_owner->m_node )
				{
					m_stunValue = damage->stunValue;
					m_stunTime = damage->stunTime;

					{	//	get current material index
						msg_Mesh mesh( 0, 0, L"body", -1 );
						m_owner->m_node->MsgProc( MT_MESH, &mesh );
						m_matIndex = mesh.matIndex > -1 ? mesh.matIndex : 0;
					}

					{	//	set freeze material
						msg_Mesh mesh( 0, 0, L"body", 1 );
						m_owner->m_node->MsgProc( MT_MESH, &mesh );
					}

					if ( m_owner->m_move.animScale > 0.9f )
						m_owner->m_move.animScale = 0.04f;

					if ( m_stunValue < 0 && damage->sender )
						damage->sender->MsgProc( GMT_LEAVE_ME, m_owner );
				}
				//	just slow down the entity
				else if ( m_stunValue >= 0 )
				{
					if ( damage->stunValue > m_stunValue )
						m_stunValue = damage->stunValue;

					if ( damage->stunTime > m_stunTime )
						m_stunTime = damage->stunTime;
				}
			}
		}
		break;
	}
}
