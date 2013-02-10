/********************************************************************
	created:	2012/04/15
	filename: 	Network.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of engine to manage network 
				in game
*********************************************************************/
#ifndef GUARD_Network_HEADER_FILE
#define GUARD_Network_HEADER_FILE

//! this is a network object which has an instance of a server and a client
class SEGAN_API Network
{
	SEGAN_IMPLEMENT_MEMORY_DLL(Network);
	SEGAN_IMPLEMENT_STERILE_CLASS(Network);

public:

	Network( void );
	~Network( void );

	//! initialize network system and create server/client instances if initialization success
	void Initialize( const word defaultPort );

	//! finalize network system and destroy server/client instances
	void Finalize( void );

	//! update should be called every frame
	void Update( float elpsTime );

	//! search servers in the net and fill out netInfoArray.
	void SearchServers( NetInfo* netInfoArray, const sint netInfoCount );

public:

	NetInfo		m_curInfo;		//  network information of the current machine
	Server*		m_server;		//  server of this network
	Client*		m_client;		//  client of this network

};

#endif	//	GUARD_Network_HEADER_FILE