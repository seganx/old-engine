/********************************************************************
	created:	2010/11/17
	filename: 	EditorTypes.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some enumeration types and other things
*********************************************************************/
#ifndef GUARD_EditorTypes_HEADER_FILE
#define GUARD_EditorTypes_HEADER_FILE

#define EDITOR_PAUSE					-1000		//  setting this value to Render_Stop() will cause to pause editor
#define EDITOR_SLEEP					0			//  setting this value to Render_Stop() will cause to sleep editor

#define EDITOR_MOUSESTATE_SELECT		0x00000001	//  mouse state in editor is select
#define EDITOR_MOUSESTATE_MOVE			0x00000002	//  mouse state in editor is move
#define EDITOR_MOUSESTATE_ROTATE		0x00000004	//  mouse state in editor is rotate
#define EDITOR_MOUSEAXIS_X				0x00000008	//  mouse axis contain x direction
#define EDITOR_MOUSEAXIS_Y				0x00000010	//  mouse axis contain y direction
#define EDITOR_MOUSEAXIS_Z				0x00000020	//  mouse axis contain z direction


enum EditorWorkingSpace
{
	EWS_NULL,
	EWS_SETTINGS,
	EWS_OBJECT,
	EWS_SCENE,
	EWS_HUD,
	EWS_GAME
};
#define EWS_

enum EditorCameraMode
{
	ECM_SPHERICAL,
	ECM_RTS,
	ECM_FLY,
	ECM_WALK
};
#define ECM_

enum UndoActionType
{
	UAT_CREATE,
	UAT_DELETE,
	UAT_MOVE,
};
#define UAT_

#endif	//	GUARD_EditorTypes_HEADER_FILE