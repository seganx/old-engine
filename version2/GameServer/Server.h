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

//! general callback function for connections. return true to notify the action performed
typedef bool (*CBServer) (Connection* connection);


//! basic UPD server class
class Server
{
public:

	Server(void);
	~Server(void);

	//! initialize the server on the specified port
	void Initialize( const NetConfig* config = null, CBServer add = null, CBServer remove = null );

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

	//! update statistics
	void UpdateStatistics( void );

public:
	NetConfig*				m_config;			//! hold server configuration
	NetStats				m_stats;			//!	hold server statistics data
	Socket*					m_sendSocket;		//! UDP socket for send packets
	Socket*					m_recvSocket;		//! UDP socket to receive packets
	Array<Connection*>		m_connections;		//! list of connections used for iteration
	Map<uint, Connection*>	m_connectionMap;	//! map of connections to find a connection so fast

	CBServer				m_connectionAdd;	//! called when a new connection added to server.
	CBServer				m_connectionRem;	//! called when a connection is going to be deleted. if callback function returned false, server will not remove that
};

#endif	//	GUARD_Server_HEADER_FILE