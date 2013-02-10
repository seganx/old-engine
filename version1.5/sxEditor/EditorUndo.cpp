#include "EditorUndo.h"

UndoManager::UndoManager( UndoCallBack callback ): m_callBack(callback)
{
}

UndoManager::~UndoManager( void )
{
	Clear();
}

void UndoManager::Clear( void )
{
	while ( m_Actions.Count() )
	{
		UndoAction* action = m_Actions.Top();

		switch (action->type)
		{
		case UAT_DELETE:
			{
				for (int i=0; i<action->nodes.Count(); i++)
				{
					sx_delete( action->nodes[i] );
				}
			}
			break;
		}

		sx_delete( action );
		m_Actions.Pop();
	}
}

void UndoManager::AddAction( UndoActionType actionType, const sx::core::PNode* nodeArray, const int numNode )
{
	UndoAction* action = sx_new( UndoAction );
	m_Actions.Push( action );

	action->type = actionType;
	switch (actionType)
	{
	case UAT_CREATE:
		{
			for (int i=0; i<numNode; i++)
			{
				action->nodes.PushBack( nodeArray[i] );
				action->parents.PushBack( nodeArray[i]->GetParent() );
			}
		}
		break;

	case UAT_DELETE:
		{
			for (int i=0; i<numNode; i++)
			{
				sx::core::PNode node = nodeArray[i];
				action->nodes.PushBack( node );
				action->parents.PushBack( nodeArray[i]->GetParent() );

				//  additional operation
				msg_SoundStop msgSound(false);
				node->MsgProc( MT_SOUND_STOP, &msgSound );
			}
		}
		break;

	case UAT_MOVE:
		{
			for (int i=0; i<numNode; i++)
			{
				action->nodes.PushBack( nodeArray[i] );
				action->positions.PushBack( nodeArray[i]->GetPosition_local() );
			}
		}
		break;
	}
}

void UndoManager::Undo( void )
{
	if ( m_Actions.IsEmpty() ) return;

	UndoAction* action = m_Actions.Top();
	m_Actions.Pop();

	switch (action->type)
	{
	case UAT_CREATE:
	case UAT_DELETE:
		{
			if ( m_callBack )
				m_callBack( action );
		}
		break;

	case UAT_MOVE:
		{
			for (int i=0; i<action->nodes.Count(); i++)
			{
				action->nodes[i]->SetPosition( action->positions[i] );
			}
		}
		break;
	}

	sx_delete( action );
}

void UndoManager::DeleteNode( const sx::core::PNode* nodeArray, const int numNode )
{
	AddAction( UAT_DELETE, nodeArray, numNode );

	for (int i=0; i<numNode; i++)
		sx::core::Scene::RemoveNode( nodeArray[i] );
}