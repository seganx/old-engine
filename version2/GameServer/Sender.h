/********************************************************************
	created:	2016/01/05
	filename: 	Sender.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Sender class of Selective Repeat
				protocol for reliable UDP system.
				
*********************************************************************/
#ifndef RUDP_SENDER_DEFINED
#define RUDP_SENDER_DEFINED

#include "Socket.h"

//! selective repeat sender for RUPD
class Sender
{
public:

	Sender( void );
	~Sender( void );
 
	//! initialize with specified sockets and address
	void Init( const NetAddress& destination, Socket* sendSocket );

	//! add packet to the sending packet
	void Add( NetPacket* packet );

	//! handle received ACK number
	void ReceivedAck( const NetHeader* header );
	
	//! update the sender to handle timers and sent packet
	void Update( void );

private:

	//! try to put the packet in window. return false if window is full
	bool PutInWindow( NetPacket* np );

public:
	NetAddress			m_address;						//! destination address
	Socket*				m_socket;						//! sending socket
	Array<NetPacket*>	m_packets;						//!	list of packets for send
	NetPacket*			m_window[SX_NET_RUDP_WINSIZE];	//! window buffer
	double				m_times[SX_NET_RUDP_WINSIZE];	//! timeout for resend packets
};

#endif	//	RUDP_SENDER_DEFINED


