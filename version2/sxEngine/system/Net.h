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



//!	callback functions
typedef void (*CB_Connection)(class Connection* connection, const byte* buffer, const uint size);
typedef void (*CB_Server)(class Server* server, class Connection* client, const byte* buffer, const uint size);
typedef void (*CB_Client)(class Client* client, const byte* buffer, const uint size);


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
	bool Send( const NetAddress& destination, const void* buffer, const int size );

	//! pick up data on the port and fill out address of sender
	sint Receive( const void* buffer, const int size, NetAddress* OUT sender );

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
	bool Send( const void* buffer, const int sizeinbyte, const bool critical = false );

public:

	String								m_name;				//	name of connection. this name usually is the destination name
	Socket*								m_socket;			//	socket used in connection
	NetAddress							m_destination;		//  address of behind the line
	NetState							m_state;			//  state of connection
	uint								m_sntAck;			//	number of packet sent
	uint								m_recAck;			//	number of packet received
	float								m_timeout;			//	if receive time became grater that time out, connection is lost
	float								m_sendTime;			//	send time to check connection
	Array<struct NetMessage*>			m_unreliable;		//	list of messages to handle unreliable connections
	Array<struct NetMessage*>			m_sending;			//	list of messages that are going to send
	Array<struct NetMessage*>			m_sent;				//	list of messages that has been sent
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

	bool Start( const word port, const word clientPort, const uint maxClients, CB_Server callback );
	void Stop( void );

	//! listen to incoming messages and accept clients if any empty slot exist
	void Listen( void );

	//! run the game. no more incoming clients will accepted and any empty slot will be closed
	void Run( void );

	//! update the server
	void Update( const float elpsTime, const float delayTime, const float timeOut );

	//! send message to all clients
	bool Send( const char* buffer, const int sizeinbyte, const bool critical = false );

	/*! return the maximum time needed to send data queued
	the returned value can be used as delay time in application side
	to avoid data accumulation on the network.
	also the application can use Server::CanSend() to determinde when
	update the networked objects instead of using this */
	float GetMaxUpdateTime( void );

	/*! return true if the network system is ready to send new messages.
	the application can call Send() in anywhere but calling Send() function
	in the if ( Server::CanSend( elpstime ) ) { } block will help the traffic
	balancer to avoid data accumulation on the network.
	also the application can use Server::GetMaxUpdateTime() to compute delay
	time and take control over the traffic balancer instead of using this. */
	bool CanSend( const float elpsTime );

public:

	String					m_name;				//	name of server
	NetState				m_state;			//	state of the server
	Socket					m_socket;			//	used to create socket
	CB_Server				m_callback;			//	will call when message received
	uint					m_numSend;			//	number of packet sent
	float					m_flagTime;			//	time period of rising flag
	word					m_clientPort;		//	broadcast address used to rise server flag
	Array<Connection*>		m_clients;			//	list of clients that already connected
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
	void Listen( void );

	//! connect to the destination server
	void Connect( NetAddress& destination );

	//! disconnect from the server
	void Disconnect( void );

	//! update client. this function most be call on application loop
	void Update( const float elpsTime, const float delayTime, const float timeOut );

	//! send a message to the connected server
	bool Send( const char* buffer, const int sizeinbyte, const bool critical = false );

	/*! return the maximum time needed to send data queued
	the returned value can be used as delay time in application side
	to avoid data accumulation on the network.
	also the application can use Client::CanSend() to determined when
	update the networked objects instead of using this */
	float GetMaxUpdateTime( void );

	/*! return true if the network system is ready to send new messages.
	the application can call Send() in anywhere but calling Send() function
	in the if ( Client::CanSend( elpstime ) ) { } block will help the traffic
	balancer to avoid data accumulation on the network.
	also the application can use Client::GetMaxUpdateTime() to compute delay
	time and take control over the traffic balancer instead of using this.
	*/
	bool CanSend( const float elpsTime );

public:

	String			m_name;				//	name of the client
	Socket			m_socket;			//	used to create socket
	Connection		m_connection;		//	connection
	CB_Client		m_callback;			//	will call when message received

	struct ServerInfo
	{
		wchar		name[32];
		NetAddress	address;
		float		age;
	};
	Array<ServerInfo>	m_servers;			//	array of servers

};


//! initialize network system
SEGAN_ENG_API bool sx_net_initialize( const dword netID );

//! finalize network system
SEGAN_ENG_API void sx_net_finalize( void );


#endif	//	GUARD_Net_HEADER_FILE


