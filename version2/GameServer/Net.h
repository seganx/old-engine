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
#define SX_NET_BROADCAST		0
#define SX_NET_BUFF_SIZE		256		//! maximum size of a packet in bytes
#define SX_NET_OPTN_SAFESEND	0x01	//! used in message header and indicates that the message should be resend till other sides confirms
#define SX_NET_OPTN_CONFIRMED	0x02	//! used in message header and indicates that the message is confirmation by the other sides


//! describe net configuration
struct NetConfig
{
	word		id;						//!	net id helps to avoid servers conflicts
	word		recv_port;				//! server port for receiving packets
	word		send_port;				//! server port for sending packets
	word		packs_per_sec;			//!	number of sent packets per second. 0 means unlimited packets sent
	word		retry_time;				//! resend time for critical packets that should be confirmed by client
	word		retry_timeout;			//! time out for stop resending packets and close the connection

	NetConfig( void ): id(31), recv_port(31000), send_port(32000), packs_per_sec(10), retry_time(1), retry_timeout(10) {}
};

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
	word	number;			//! message number
	word	option;			//! message option contains message flag
	word	checksum;		//! checksum of the content data
};

//! describe a packet content
struct NetPacket
{
	uint		size;						//! size of all data contain header and user data
	char		data[SX_NET_BUFF_SIZE];		//! it also contains header
	NetHeader*	header;						//! no allocation needed. It just points to data
};


//! initialize network system
bool sx_net_initialize( void );

//! finalize network system
void sx_net_finalize( void );

//!	additional functions
wchar* sx_net_error_string( const sint code );

//! simple function to compute checksum
word sx_net_compute_checksum( const void* buffer, const uint size );

//! return true if the packet is a valid message
bool sx_net_verify_packet( const void* buffer, const uint size, const word lastNumber );

//! return system timer with high frequency
double sx_net_get_timer( void );

//! return the current system time
dword sx_net_get_time( void );


#endif	//	GUARD_Net_HEADER_FILE


