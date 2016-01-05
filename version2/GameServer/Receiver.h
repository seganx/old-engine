/********************************************************************
	created:	2016/01/05
	filename: 	Receiver.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Receiver class of Selective Repeat
				protocol for reliable UDP system.
				
*********************************************************************/
#ifndef RUDP_RECEIVER_DEFINED
#define RUDP_RECEIVER_DEFINED

#include "Socket.h"

//! selective repeat receiver for RUPD
class Receiver
{
public:

	Receiver( void );
	~Receiver( void );
 
	//! initialize with specified address and socket for sending ACK
	void Init( const NetAddress& destination, Socket* ackSocket );

	//! handle received packets
	void Received( NetPacket* packet );
	
	//! update the sender to handle timers and sent ACKs
	void Update( void );

public:
	NetAddress			m_address;		//! destination address
	Socket*				m_socket;		//! sending ACK socket
	Array<NetPacket*>	m_packets;		//!	list of buffered packets
};

#endif	//	RUDP_RECEIVER_DEFINED


