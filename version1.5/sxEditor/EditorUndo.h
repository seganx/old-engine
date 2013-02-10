/********************************************************************
	created:	2012/01/05
	filename: 	EditorUndo.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class to control undo / redo
*********************************************************************/
#ifndef GUARD_EditorUndo_HEADER_FILE
#define GUARD_EditorUndo_HEADER_FILE

#include "ImportEngine.h"
#include "EditorTypes.h"


class UndoManager
{
public:
	struct UndoAction
	{
		UndoActionType			type;
		sx::core::ArrayPNode	parents;
		sx::core::ArrayPNode	nodes;
		Array<float3>			positions;
	};
	typedef void (*UndoCallBack)(UndoAction* uAction);

public:
	UndoManager(UndoCallBack callback);
	~UndoManager(void);

	//! clear all actions
	void Clear(void);

	//! add new action to action list
	void AddAction(UndoActionType actionType, const sx::core::PNode* nodeArray, const int numNode);
	
	//! undo. this may call the callback function
	void Undo(void);

	//! delete a node from scene
	void DeleteNode(const sx::core::PNode* nodeArray, const int numNode);

private:

	Stack<UndoAction*>		m_Actions;
	UndoCallBack			m_callBack;
};

#endif	//	GUARD_EditorUndo_HEADER_FILE