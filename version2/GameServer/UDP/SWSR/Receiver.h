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

//! Receiver class for selective repeat protocol
class Receiver
{
public:

	Receiver( void );
	~Receiver( void );
 
	//! return true if message has been received
	bool IsDublicated(NetHeader* nh);

public:
	word	m_acks[SX_NET_RUDP_BUFFSIZE];	//! window buffer
};

#endif	//	RUDP_RECEIVER_DEFINED


