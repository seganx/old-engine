#include "Sender.h"


Sender::Sender(void) : m_busy(false)
{
	sx_mem_set(m_packets, 0, sizeof(m_packets));
	sx_mem_set(m_timeout, 0, sizeof(m_timeout));
}

Sender::~Sender(void)
{

}

bool Sender::Add(NetPacket* np)
{
	if (m_busy) return;

	// search through window for free slot
	for (int i = 0; i < SX_NET_RUDP_WINSIZE; ++i)
	{
		if (m_packets[i]) continue;
		m_packets[i] = np;
		m_timeout[i] = g_net->m_retry_time;
		return true;
	}
	return false;
}

void Sender::Send(void)
{
	for (int i = 0; i < SX_NET_RUDP_WINSIZE; ++i)
	{
		if (m_packets[i])
		{
			m_busy = true;
			break;
		}
	}
}

void Sender::Update(const NetAddress& address, Socket* sendSocket)
{
	sx_callstack();
	if (!m_busy) return;

	//	check timeout to resend packets
	for (int i = 0; i < SX_NET_RUDP_WINSIZE; ++i)
	{
		if (m_packets[i] == null) continue;

		m_timeout[i] += g_timer->m_elpsTime;
		if (m_timeout[i] < g_net->m_retry_time) continue;
		m_timeout[i] = 0;	//	reset slot timer

		//	send packet to network
		NetPacket* np = m_packets[i];
		sendSocket->Send(address, np->data, np->size);
		sx_print_a("Info: Sent [NO: %d, OP: %d, IP:%d.%d.%d.%d:%d]", np->header->number, np->header->option, address.ip_bytes[0], address.ip_bytes[1], address.ip_bytes[2], address.ip_bytes[3], address.port);
	}
}

void Sender::Delivered(const NetHeader* header)
{
	sx_callstack();

	//	search through packet to find the goal one
	for (int i = 0; i < SX_NET_RUDP_WINSIZE; ++i)
	{
		if (m_packets[i] == null) continue;

		NetHeader* nh = m_packets[i]->header;
		if (header->checksum == nh->checksum && header->number == nh->number)
		{
			if (header->option & SX_NET_OPTN_DELIVERED) //	if ACK has been received we just delete the packet from window
				sx_mem_free_and_null(m_packets[i]);
			break; // scape the loop
		}
	}

	// check to see if all packets has been delivered
	bool busy = false;
	for (int i = 0; i < SX_NET_RUDP_WINSIZE; ++i)
		if (m_packets[i])
			busy = true;
	m_busy = busy;
}
