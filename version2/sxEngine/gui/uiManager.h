/********************************************************************
	created:	2013/03/15
	filename: 	uiManager.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contains a simple GUI manager for ordering 
				and drawing controls and managing events of them
*********************************************************************/
#ifndef GUARD_uiManager_HEADER_FILE
#define GUARD_uiManager_HEADER_FILE

#include "../Engine_def.h"
#include "uiDevice.h"

//! forward declarations
class uiControl;
class uiContext;
class d3dVertexBuffer;

/*! this class draw controls on the screen and manage events */
class SEGAN_ENG_API uiManager
{
	SEGAN_STERILE_CLASS(uiManager);

public:

	uiManager( void );
	~uiManager( void );

	//! add new control to manager
	void add( const uiControl* control );

	//! remove a control from manager. but not delete that
	void remove( const uiControl* control );

	//! clear and delete all GUI in the manager
	void clear( void );

	//! update all controls depend on view port width and view port height
	void update( float elpsTime, const float vpwidth, const float vpheight );

	//! handle process input
	//void ProcessInput( struct InputReport* inputReport );

	//! draw GUIs in screen space coordinate
	void draw( const dword flag );

	//! create and return a GUI by given type
	uiControl* create_contorl( const char* type );

	//! copy the src element to the dest element in the given index position
	void copy( uiContext* dest, uint& index, const uiContext* src );

	//! prepare for batching elements. pass count 0 to compute number of batch automatically
	void begin_batch( const uint count );

	//! add an element to the batch and return false if can't batch the element with another
	bool add_batch( const uiContext* elem );

	//! return number of vertices need to batch all added elements
	uint get_batch_vcount( void );

	//! end patch and insert them to dest element. the last data in dest will be lost
	void end_batch( uiContext* dest );

public:

	Array<uiContext*>	m_batches;		//!	us in batch system

	matrix				m_view;			//	view matrix used for controls
	matrix				m_proj;			//	protection matrix used for controls
	Array<uiControl*>	m_controls;		//	array of all controls
	Array<uiContext*>	m_elements;		//	array of elements
	uiContext*			m_drawable;		//	final drawable element
};

#endif	//	GUARD_uiManager_HEADER_FILE