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

#include "../Engine_def.h"
#include "uiDevice.h"

//! forward declarations
class uiControl;
class uiElement;
class d3dVertexBuffer;

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

	//! update all controls depend on view port width and view port height
	void Update( float elpsTime, const float vpwidth, const float vpheight );

	//! handle process input
	//void ProcessInput( struct InputReport* inputReport );

	//! draw GUIs in screen space coordinate
	void Draw( const dword flag );

	//! create and return a GUI by given type
	uiControl* CreateContorl( const uiType type );

	//! copy the src element to the dest element in the given index position
	void Copy( uiElement* dest, uint& index, const uiElement* src );

	//! prepare for batching elements. pass count 0 to compute number of batch automatically
	void BeginBatch( const uint count );

	//! add an element to the batch and return false if can't batch the element with another
	bool AddBatch( const uiElement* elem );

	//! return number of vertices need to batch all added elements
	uint GetBatchVertexCount( void );

	//! end patch and insert them to dest element. the last data in dest will be lost
	void EndBatch( uiElement* dest );

public:

	Array<uiElement*>	m_batches;		//!	us in batch system

	matrix				m_view;			//	view matrix used for controls
	matrix				m_proj;			//	protection matrix used for controls
	Array<uiControl*>	m_controls;		//	array of all controls
	Array<uiElement*>	m_elements;		//	array of elements
	uiElement*			m_drawable;		//	final drawable element
};

#endif	//	GUARD_GUIManager_HEADER_FILE