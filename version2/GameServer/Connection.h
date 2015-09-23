/********************************************************************
	created:	2012/04/11
	filename: 	Connection.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a basic part of network system

				NOTE: this module implemented to use in single thread.
				using in multi threaded system may cause to unpredicted
				behavior
				
*********************************************************************/
#ifndef GUARD_Connection_HEADER_FILE
#define GUARD_Connection_HEADER_FILE

#include "Net.h"


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

	//! set connection speed base in packet per second. pass 0 to disable speed control
	void SetSpeed( const int packPerSecond );

	//! set connection time out
	void SetTimeOut( const double timeOut );

	//! send data to the destination address
	bool Send( const void* buffer, const int size, const word option );

	//! pick received packet and return size of the packet. return 0 if no packet exist
	uint Received( void* destBuffer, const int destSize );

	//! update the connection to send critical messages and handle timeout
	void Update( class Socket* socket, const double elpsTime );

	//! append received messages to the received list
	void AppendReceivedMessage( class Socket* socket, const void* buffer, const int size );

public:
	bool				m_connected;		//! specified that the connection is established

	NetAddress			m_destination;		//! connection address
	Array<NetPackage*>	m_sendingPacks;		//!	list of packages for send
	Array<NetPackage*>	m_receivedPacks;	//!	list of received packages

	word				m_sentNumber;		//! used in reliability system. hold the number of message sent
	word				m_RecvNumber;		//! used in reliability system. hold the number of latest message has been received

	word				m_confirmNumber;	//! indicates that connection is waiting for received confirmation
	word				m_confirmChecksum;	//! indicates that connection is waiting for received confirmation

	double				m_delayTime;		//!	delay time counter
	double				m_delayTimeMax;		//! maximum delay time

	double				m_retryTime;		//!	retry time counter
	double				m_retryTimeMax;		//! maximum retry time

	double				m_outTime;			//!	time out counter
	double				m_outTimeMax;		//! maximum time out
};

#endif	//	GUARD_Connection_HEADER_FILE


