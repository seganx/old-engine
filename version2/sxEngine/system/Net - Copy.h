/********************************************************************
	created:	2012/04/11
	filename: 	Net.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a some basic parts of network system
				these parts are include some functions to initialize &
				finalize and two classes for server & client

				NOTE: this modul implemented to use in single thread.
				using in multi threaded system may cause to unpredicted
				behavior
				
*********************************************************************/
#ifndef GUARD_Net_HEADER_FILE
#define GUARD_Net_HEADER_FILE

#include "../../sxLib/Lib.h"

#define LOG_NETWORK_INFO		1
#define LOG_NETWORK_WARNING		1
#define LOG_NETWORK_ERROR		1


enum NetError
{
	NER_UNAVAILABLE = 20,		//	network is not available
	NER_UNINITIALIZED,			//	server/client is not initialized
	NER_UNCONNECTED,			//  client is not connected to server
	NER_INVALID_BUFFER,			//  buffer parameters are invalid
	NER_INVALID_CLIENT,			//	client socket is not available
};


//! describe state of net object
enum NetState
{
	STOPPED = 0,
	STARTED,
	CONNECTED,
	DISCONNECTED,
	RUNNING,
	LISTENING,
	CONNECTING,
};


//! describe an network address
struct NetAddress
{
	byte	ip[4];
	word	port;
};

//!	callback functions
typedef void (*CB_Server)(class Server* server, class Connection* client, const char* buffer, const uint size);
typedef void (*CB_Client)(class Client* client, const char* buffer, const uint size);



//! basic UPD socket class
class SEGAN_ENG_API Socket
{
public:

	Socket( void );
	~Socket( void );
 
	//! open a UPD socket and bind it to the specified port
	bool Open( const word port );

	//! close opened socket
	void Close( void );

	//! send data to the destination address
	bool Send( const NetAddress& destination, const char* buffer, const int size );

	//! pick up data on the port and fill out address of sender
	sint Receive( const char* buffer, const int size, NetAddress* OUT sender );

public:
	uint	m_socket;
};


//! use Connection class to create connection between sockets
class SEGAN_ENG_API Connection
{
	SEGAN_STERILE_CLASS(Connection);

public:

	Connection( void );
	~Connection( void );

	//! start the connection on the opened socket. NOTE: this will not open the socket. just initialize the connection.
	bool Start( Socket* psocket );

	//! stop the connection. NOTE: this will not close the socket. just disconnect and finalized the connection
	void Stop( void );

	//! set the connection to listen mode to wait for the other connection
	bool Listen( void );

	//! going to try to connect to the destination host
	void Connect( const NetAddress& destination );

	//! disconnect the connection
	void Disconnect( void );

	//! update the connection. this function should be called in application loop
	void Update( struct NetMessage* buffer, const float elpsTime, const float delayTime, const float timeOut );

	//! send a message through the connection
	bool Send( const char* buffer, const int size );

public:

	typedef void (*CB_Connection)(Connection* connection, const char* buffer, const uint size);

	String								m_name;				//	name of connection. this name usually is the destination name
	Socket*								m_socket;			//	socket used in connection
	NetAddress							m_destination;		//  address of behind the line
	NetState							m_state;			//  state of connection
	uint								m_sntAck;			//	number of packet sent
	uint								m_recAck;			//	number of packet received
	float								m_timeout;			//	if receive time became grater that time out, connection is lost
	float								m_sendTime;			//	send time to check connection
	Array<struct NetMessage*>			m_msgList;			//	queue of messages to handle unreliable connections
	Queue<struct NetMessage*>			m_sendQueue;		//	list of messages that are going to send
	Array<struct NetMessage*>			m_sentList;			//	list of messages that has been sent
	CB_Connection						m_callBack;			//	will call when message received
	void*								m_userData;			//	user data

};


//! use Server class to create a game server and manage connections between them
class SEGAN_ENG_API Server
{
	SEGAN_STERILE_CLASS(Server);

public:

	Server( void );
	~Server( void );

	bool Start( const word port, const uint maxClients, CB_Server callback );
	void Stop( void );

	//! listen to incoming messages and accept clients if any empty slot exist
	void Listen( const word clientPort );

	//! run the game. no more incoming clients will accepted and any empty slot will be closed
	void Run( void );

	//! update the server
	void Update( const float elpsTime, const float delayTime, const float timeOut );

	//! send message to all clients
	bool Send( const char* buffer, const int size );

public:

	String					m_name;				//	name of server
	Socket					m_socket;			//	used to create socket
	NetAddress				m_broadCastAddress;	//	broadcast address used to rise server flag
	NetState				m_state;			//	state of the server
	Array<Connection*>		m_clients;			//	list of clients that already connected
	CB_Server				m_callback;			//	will call when message received
	uint					m_numSend;			//	number of packet sent
	float					m_flagTime;			//	time period of rising flag
};


//! use Client class to find a game and connect to the founded server
class SEGAN_ENG_API Client
{
	SEGAN_STERILE_CLASS(Client);

public:

	Client( void );
	~Client( void );

	//! start the client on specified port. callback function will be called when any message has been received
	bool Start( const word port, CB_Client callback );

	//! stop the client and close the port
	void Stop( void );

	//! search for servers on the specified port and update list of servers if servers has been found
	void Listen( const word serverPort );

	//! connect to the destination server
	void Connect( NetAddress& destination );

	//! disconnect from the server
	void Disconnect( void );

	//! update client. this function most be call on application loop
	void Update( const float elpsTime, const float delayTime, const float timeOut );

	//! send a message to the connected server
	bool Send( const char* buffer, const int size );

public:

	String			m_name;				//	name of the client
	Socket			m_socket;			//	used to create socket
	Connection		m_connection;		//	connection
	CB_Client		m_callback;			//	will call when message received

	struct ServerInfo
	{
		wchar		name[32];
		NetAddress	address;
	};
	Array<ServerInfo>	m_servers;			//	array of servers

};


//! initialize network system
SEGAN_ENG_API hresult sx_net_initialize( const dword netID );

//! finalize network system
SEGAN_ENG_API void sx_net_finalize( void );


#endif	//	GUARD_Net_HEADER_FILE


