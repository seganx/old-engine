#include "Sender.h"


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
Sender::Sender(void) : m_socket(null), m_packets(10)
{
	sx_mem_set(m_window, 0, sizeof(m_window));
	sx_mem_set(m_times, 0, sizeof(m_times));
}

Sender::~Sender(void)
{
	clear_all_packets(m_packets);
}

void Sender::Init(const NetAddress& destination, Socket* sendSocket)
{
	m_address = destination;
	m_socket = sendSocket;
}

bool Sender::PutInWindow(NetPacket* np)
{
	// search through window for free slot
	for (int i = 0; i < SX_NET_RUDP_WINSIZE; ++i)
	{
		if (m_window[i]) continue;

		//  pop packet from the list and push it to window
		m_window[i] = np;

		//	force slot to send packet immediately
		m_times[i] = g_net->m_retry_time;

		return true;
	}
	return false;
}

void Sender::Add(NetPacket* packet)
{
	sx_callstack();

	//	try to put the packet in window. buffer the packet if window is full
	if (!PutInWindow(packet))
		m_packets.push_back(packet);
}

void Sender::ReceivedAck(const NetHeader* header)
{
	sx_callstack();
	for (int i = 0; i < SX_NET_RUDP_WINSIZE; ++i)
	{
		// remove the ACK message from window
		if (m_window[i])
		{
			NetHeader* nh = m_window[i]->header;
			if (header->checksum == nh->checksum && header->number == nh->number)
			{
				if (header->option & SX_NET_OPTN_CONFIRMED) //	if ACK has been received we just delete the packet from window
				{
					sx_mem_free_and_null(m_window[i]);
				}
				else if (header->option & SX_NET_OPTN_RESEND) // the receiver request to resend packet
				{
					NetPacket* np = m_window[i];
					m_socket->Send(m_address, np->data, np->size);
					sx_print_a("Info: Sent [NO: %d, OP: %d, IP:%d.%d.%d.%d:%d]", np->header->number, np->header->option, m_address.ip_bytes[0], m_address.ip_bytes[1], m_address.ip_bytes[2], m_address.ip_bytes[3], m_address.port);
				}
			}
		}

		// replace window slot with new packet
		if (m_packets.m_count && m_window[i] == null)
		{
			m_window[i] = m_packets[0];
			m_packets.remove_index(0);

			//	force slot to send packet immediately
			m_times[i] = g_net->m_retry_time;
		}
	}
}

void Sender::Update(void)
{
	//	check timeout to resend packets
	for (int i = 0; i < SX_NET_RUDP_WINSIZE; ++i)
	{
		if (!m_window[i]) continue;

		m_times[i] += g_timer->m_elpsTime;
		if (m_times[i] < g_net->m_retry_time) continue;

		//	reset slot timer
		m_times[i] = 0;

		//	send packet to network
		NetPacket* np = m_window[i];
		m_socket->Send(m_address, np->data, np->size);
		sx_print_a("Info: Sent [NO: %d, OP: %d, IP:%d.%d.%d.%d:%d]", np->header->number, np->header->option, m_address.ip_bytes[0], m_address.ip_bytes[1], m_address.ip_bytes[2], m_address.ip_bytes[3], m_address.port);
	}
}
