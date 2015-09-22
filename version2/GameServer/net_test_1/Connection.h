/********************************************************************
	created:	2012/04/11
	filename: 	Connection.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a basic parts of network system

				NOTE: this module implemented to use in single thread.
				using in multi threaded system may cause to unpredicted
				behavior
				
*********************************************************************/
#ifndef GUARD_Connection_HEADER_FILE
#define GUARD_Connection_HEADER_FILE

#include "Net.h"

#define SX_NET_CON_BUFF_SIZE	1024

//! basic UPD connection class
class Connection
{
public:

	Connection( void );
	~Connection( void );
 
	//! prepare connection on the specified address
	void Open( const NetAddress& destination );

	//! clear all allocated memories and close the connection
	void Close( void );

	//! set connection speed base in packet per second
	void SetSpeed(const int packPerSecond);

	//! send data to the destination address
	bool Send( const void* buffer, const int size, const byte option );

	//! pick received packet and return size of the packet. return 0 if no packet exist
	uint Received( void* destBuffer, const int destSize );

	//! update the connection to send critical messages and handle timeout
	void Update( class Socket* socket, const float elpsTime );

	//! append received messages to the received list
	void AppendReceivedMessage( class Socket* socket, const void* buffer, const int size );

public:

	struct ConHead
	{
		byte netId;
		byte number;
		byte option;
		byte checksum;
	};

	struct ConPack
	{
		uint	 size;							//! size of all data contain header and user data
		char	 data[SX_NET_CON_BUFF_SIZE];	//! it also contains header
		ConHead* header;						//! no allocation needed. It just points to data
	};
	
	NetAddress			m_destination;		//! connection address
	Array<ConPack*>		m_sendingPacks;		//!	list of packages for send
	Array<ConPack*>		m_receivedPacks;	//!	list of received packages

	float				m_delayTime;		//!	delay time counter
	float				m_delayTimeMax;		//! maximum delay time

	byte				m_sendingId;		//! used in reliability system
	byte				m_receivedId;		//! used in reliability system

	byte				m_confirmId;		//! indicates that connection is waiting for received confirmation
	byte				m_confirmChecksum;	//! indicates that connection is waiting for received confirmation
};

#endif	//	GUARD_Net_HEADER_FILE


