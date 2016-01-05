/********************************************************************
	created:	2012/04/11
	filename: 	Net.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some basic parts of network system
				these parts are include some functions to initialize &
				finalize				
*********************************************************************/
#ifndef GUARD_Net_HEADER_FILE
#define GUARD_Net_HEADER_FILE

#include "../sxLib/Lib.h"

#define SX_NET_ID				31
#define SX_NET_BUFF_SIZE		256		//! maximum size of a packet in bytes
#define SX_NET_OPTN_SAFESEND	0x01	//! used in message header and indicates that the message should be resend till other sides confirms
#define SX_NET_OPTN_CONFIRMED	0x02	//! used in message header and indicates that the message is confirmation by the other sides
#define SX_NET_OPTN_RESEND		0x04	//! used in message header and indicates that the receiver request to resend a lost packet
#define SX_NET_RUDP_WINSIZE		4		//! window size of selective repeat protocol


//! report network statistics
struct NetStats
{
	uint		start_time;				//! the time when server started
	uint		curr_time;				//! current time counter

	uint		cycle_per_sec;			//! number of server cycles per second
	uint		max_cycle_per_sec;		//! maximum number of server cycles per second
	uint		min_cycle_per_sec;		//! minimum number of server cycles per second

	uint		total_recv_packs;		//! total packets received from start time
	uint		recv_packs_per_sec;		//! number of packets received in one second

	uint		total_recv_bytes;		//! total bytes received from start time
	uint		recv_bytes_per_sec;		//! number of bytes received in one second
	
	uint		total_connections;		//!	total number of connections established on the server
	uint		curr_connections;		//!	number of connections established on the server right now
	uint		max_connections;		//!	maximum number of connections established on the server
	
	uint		socket_queued_packs;	//! number of packets queued on the socket port
	uint		retry_count;			//!	number of resend packets to the clients

	uint		total_invalid_packs;	//! number of invalid packets received
	uint		invalid_packs_per_sec;	//! number of received invalid packets per second

	//////////////////////////////////////////////////////////////////////////
	uint		helper_timer;			//! helper parameter to compute one second events
	uint		helper_cps;				//! helper parameter to compute cycle per second
	uint		helper_rpps;			//! helper parameter to compute received packets per second
	uint		helper_rbps;			//! helper parameter to compute received bytes per second
	uint		helper_ipps;			//! helper parameter to compute received invalid packets per second
};

//! describe an network address
struct NetAddress
{
	union
	{
		uint	ip;
		byte	ip_bytes[4];
	};
	word	port;

	NetAddress( void ): ip(0), port(0) {}
	NetAddress( const uint ip_addr, const word port_number ): ip(ip_addr), port(port_number) {}
	NetAddress( const byte ip1, const byte ip2, const byte ip3, const byte ip4, const word port_number ): port(port_number)
	{
		ip = sx_fourcc( ip1, ip2, ip3, ip4 );
	}
};

//! describe header of packet
struct NetHeader
{
	word	netId;			//!	net id helps to avoid servers conflicts
	word	option;			//! message option contains message flag
	word	number;			//! message number
	word	checksum;		//! checksum of the content data
};

//! describe a packet content
struct NetPacket
{
	NetHeader*	header;						//! no allocation needed. It just points to data
	uint		size;						//! size of all data contain header and user data
	char		data[SX_NET_BUFF_SIZE];		//! it also contains header
};

//! describe net configuration
class NetConfig
{
public:
	NetConfig(void) : m_id(31), m_recv_port(31000), m_send_port(31001), m_ack_port(31002), m_packs_per_sec(10), m_retry_time(5000), m_retry_timeout(10000) {}

public:
	word	m_id;					//!	net id helps to avoid servers conflicts
	uint	m_recv_port;			//! server port for receiving packets
	uint	m_send_port;			//! server port for sending packets
	uint	m_ack_port;				//! acknowledge port for send/receive ACK
	uint	m_packs_per_sec;		//!	number of sent packets per second. 0 means unlimited packets sent
	uint	m_retry_time;			//! resend time in seconds for critical packets that should be confirmed by client
	uint	m_retry_timeout;		//! time out in seconds for stop resending packets and close the connection
};
extern NetConfig * g_net;			//! global network configuration variable

//! initialize network system
bool sx_net_initialize( void );

//! finalize network system
void sx_net_finalize( void );

//!	additional functions
wchar* sx_net_error_string( const sint code );

//! simple function to compute checksum
word sx_net_compute_checksum( const void* buffer, const uint size );

//! return true if the packet is a valid message
bool sx_net_verify_packet( const void* buffer, const uint size );


#endif	//	GUARD_Net_HEADER_FILE


