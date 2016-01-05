#include "Receiver.h"


//////////////////////////////////////////////////////////////////////////
//	HELPER FUNCTIONS
//////////////////////////////////////////////////////////////////////////
void clear_all_packets(Array<NetPacket*>& packets)
{
	sx_callstack();
	for (int i = 0; i < packets.m_count; ++i)
		sx_mem_free(packets[0]);
	packets.clear();
}


//////////////////////////////////////////////////////////////////////////
//	SENDER 
//////////////////////////////////////////////////////////////////////////

Receiver::Receiver(void)
{

}

Receiver::~Receiver(void)
{

}

void Receiver::Init(const NetAddress& destination, Socket* ackSocket)
{

}

void Receiver::Received(NetPacket* packet)
{

}

void Receiver::Update(void)
{

}
