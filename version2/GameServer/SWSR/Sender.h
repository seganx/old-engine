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

//! Sender class for selective repeat sliding window protocol
class Sender
{
public:

	Sender(void);
	~Sender(void);

	//! try to put the packet in window. return false if window is full/busy
	bool Add(NetPacket* np);

	//! start to sending all packets in window
	void Send(void);

	//! update window to handle timeouts
	void Update(const NetAddress& address, Socket* sendSocket);

	//! handle received ACK number
	void Delivered(const NetHeader* header);

public:
	bool		m_busy;								//! indicates that window is sending content
	NetPacket*	m_packets[SX_NET_RUDP_WINSIZE];		//! window buffer
	double		m_timeout[SX_NET_RUDP_WINSIZE];		//! timeout for resend packets
};

#endif	//	RUDP_SENDER_DEFINED


