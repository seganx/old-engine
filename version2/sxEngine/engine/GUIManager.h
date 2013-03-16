/********************************************************************
	created:	2013/03/15
	filename: 	GUIManager.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contains a simple GUI manager for ordering 
				and drawing controls and managing events of them
*********************************************************************/
#ifndef GUARD_GUIManager_HEADER_FILE
#define GUARD_GUIManager_HEADER_FILE

#include "../../sxLib/Lib.h"


//! forward declarations
class uiControl;

/*! this class draw controls on the screen and manage events */
class SEGAN_ENG_API GUIManager
{
	SEGAN_STERILE_CLASS(GUIManager);

public:

	GUIManager( void );
	~GUIManager( void );

	//! add new control to manager
	void Add( const uiControl* control );

	//! remove a control from manager. but not delete that
	void Remove( const uiControl* control );

	//! delete a control and make null the reference
	void Delete( uiControl*& control );

	//! clear and delete all GUI in the manager
	void Clear( void );

	//! update all controls
	void Update( float elpsTime );

	//! handle process input
	void ProcessInput( void );

	//! draw GUIs in screen space coordinate which started from top and left
	void Draw_topleft( const dword flag );

public:

	Array<uiControl*>	m_gui;		//	array of all controls

};

#endif	//	GUARD_GUIManager_HEADER_FILE