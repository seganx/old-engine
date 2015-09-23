/********************************************************************
	created:	2015/09/23
	filename: 	Server.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a basic part of network system

				NOTE: this module implemented to use in single thread.
				using in multi threaded system may cause to unpredicted
				behavior
*********************************************************************/
#ifndef GUARD_Server_HEADER_FILE
#define GUARD_Server_HEADER_FILE

#include "Net.h"

class Socket;
class Connection;

//! basic UPD server class
class Server
{
public:

	Server(void);
	~Server(void);

	//! initialize the server on the specified port
	void Initialize( const word port );

	//! finalize the server and clear all allocated memory
	void Finalize( void );

	//! peek data from port and manage connections
	void Update( const double elpsTime );

private:

	//! create a new connection
	Connection* AddConnection( const NetAddress& address );

	//! destroy a connection
	void RemoveConnection( const int index );

	//! read data from port and put them on connections
	void PeekReceivedMessages( void );

public:
	Socket*					m_socket;			//! main UDP socket
	Array<Connection*>		m_connections;		//! list of connections used for iteration
	Map<uint, Connection*>	m_connectionMap;	//! map of connections to find a connection so fast
};

#endif	//	GUARD_Server_HEADER_FILE