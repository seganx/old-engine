#include "Receiver.h"

Receiver::Receiver(void)
{
	sx_mem_set(m_acks, 0, sizeof(m_acks));
}

Receiver::~Receiver(void)
{

}

bool Receiver::IsDublicated(NetHeader* nh)
{
	// check to see if message is duplicated or not
	bool duplicated = false;
	for (int i = 0; i < SX_NET_RUDP_BUFFSIZE; ++i)
		if (m_acks[i] == nh->number)
			duplicated = true;
	return duplicated;
}
