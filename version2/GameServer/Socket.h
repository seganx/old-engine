/********************************************************************
	created:	2012/04/11
	filename: 	Socket.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a basic part of network system

				NOTE: this module implemented to use in single thread.
				using in multi threaded system may cause to unpredicted
				behavior
				
*********************************************************************/
#ifndef GUARD_Socket_HEADER_FILE
#define GUARD_Socket_HEADER_FILE

#include "Net.h"

//! basic UPD socket class
class Socket
{
public:

	Socket( void );
	~Socket( void );
 
	//! open a UPD socket and bind it to the specified port
	bool Open( const word port );

	//! close opened socket
	void Close( void );

	//! send data to the destination address
	bool Send( const NetAddress& destination, const void* buffer, const int size );

	//! pick up data on the port and fill out address of sender
	sint Receive( void* buffer, const int size, NetAddress* OUT sender );

public:
	uint	m_socket;
};

#endif	//	GUARD_Socket_HEADER_FILE


