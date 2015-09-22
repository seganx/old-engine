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

#include "../../sxLib/Lib.h"


#define SX_NET_ID				31
#define SX_NET_BROADCAST		0
#define SX_NET_OPTN_SAFESEND	0x01	//! used in message header and indicates that the message should be resend till other sides confirms
#define SX_NET_OPTN_CONFIRMED	0x02	//! used in message header and indicates that the message is confirmation by the other sides

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

//! initialize network system
bool sx_net_initialize( void );

//! finalize network system
void sx_net_finalize( void );

//!	additional functions
wchar* sx_net_error_string( const sint code );

//! simple function to compute checksum
byte sx_net_compute_checksum( const void* buffer, const uint size );

#endif	//	GUARD_Net_HEADER_FILE


