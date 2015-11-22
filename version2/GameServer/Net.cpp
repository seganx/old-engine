#if defined(_WIN32)

#include "Net.h"


#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")


//////////////////////////////////////////////////////////////////////////
// use as initialized network structure
class NetInternal 
{
public:
	wchar					name[32];
	NetAddress				address;
};
static NetInternal* s_netInternal = NULL;


//////////////////////////////////////////////////////////////////////////
//	network functions
//////////////////////////////////////////////////////////////////////////
bool sx_net_initialize( void )
{
	sx_callstack();

	if ( s_netInternal  )
	{
		sx_print(L"Warning: calling sx_net_initialize() failed due to network system was initialized!");
		return 0;
	}
	bool netInitialized = true;

	//  initialize windows socket
	WSADATA wsaData;
	if( ::WSAStartup( MAKEWORD( 2, 2 ), &wsaData ) )
	{
		sx_print(L"Error: Network initialization on Windows failed! error code : %s !", sx_net_error_string(WSAGetLastError()));
		netInitialized = false;
	}

	//  check initialized version
	bool incorrectVersion = LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2;
	if ( netInitialized && incorrectVersion )
	{
		sx_print(L"Error: Network initialization on Windows failed! Invalid version detected!");
		netInitialized = false;
	}

	// Get local host name
	char hostName[128] = {0};
	if( netInitialized && ::gethostname( hostName, sizeof(hostName) ) )
	{
		sx_print(L"Error: function ::gethostname() failed with error code : %s !", sx_net_error_string(WSAGetLastError()));
		netInitialized = false;
	}

	// Get local IP address
	hostent* pHost = ::gethostbyname( hostName );
	if( netInitialized && !pHost )
	{
		sx_print(L"Error: function ::gethostbyname() failed with error code : %s !", sx_net_error_string(WSAGetLastError()));
		netInitialized = false;
	}
	

	if ( netInitialized )
	{
		//  initialize internal net object
		s_netInternal = sx_new( NetInternal );
		sx_str_copy( s_netInternal->name, 32, hostName );
		s_netInternal->address.ip_bytes[0] = pbyte(pHost->h_addr_list[0])[0];
		s_netInternal->address.ip_bytes[1] = pbyte(pHost->h_addr_list[0])[1];
		s_netInternal->address.ip_bytes[2] = pbyte(pHost->h_addr_list[0])[2];
		s_netInternal->address.ip_bytes[3] = pbyte(pHost->h_addr_list[0])[3];
		s_netInternal->address.port  = 0;

		sx_print(L"Network system initialized successfully on Windows.");
		sx_print(L"	Name: %s", s_netInternal->name);
		sx_print(L"	IP: %d.%d.%d.%d",
			s_netInternal->address.ip_bytes[0], s_netInternal->address.ip_bytes[1], s_netInternal->address.ip_bytes[2], s_netInternal->address.ip_bytes[3] );
	}
	else
	{
		sx_print(L"The network system is now disabled.");
		WSACleanup();
	}

	return netInitialized;
}

void sx_net_finalize( void )
{
	sx_callstack();

	if ( s_netInternal )
	{
		sx_safe_delete_and_null( s_netInternal );

		WSACleanup();
		sx_print(L"Network system Finalized.");
	}
	else
	{
		sx_print(L"Warning: calling sx_net_finalize() failed due to network system was finalized or was not initialized!");
	}
}


//////////////////////////////////////////////////////////////////////////
//	additional functions
//////////////////////////////////////////////////////////////////////////
wchar* sx_net_error_string( const sint code )
{
	switch( code ) {
	case WSAEINTR:				return L"WSAEINTR";
	case WSAEBADF:				return L"WSAEBADF";
	case WSAEACCES: 			return L"WSAEACCES";
	case WSAEDISCON: 			return L"WSAEDISCON";
	case WSAEFAULT: 			return L"WSAEFAULT";
	case WSAEINVAL: 			return L"WSAEINVAL";
	case WSAEMFILE: 			return L"WSAEMFILE";
	case WSAEWOULDBLOCK: 		return L"WSAEWOULDBLOCK";
	case WSAEINPROGRESS: 		return L"WSAEINPROGRESS";
	case WSAEALREADY: 			return L"WSAEALREADY";
	case WSAENOTSOCK: 			return L"WSAENOTSOCK";
	case WSAEDESTADDRREQ: 		return L"WSAEDESTADDRREQ";
	case WSAEMSGSIZE: 			return L"WSAEMSGSIZE";
	case WSAEPROTOTYPE: 		return L"WSAEPROTOTYPE";
	case WSAENOPROTOOPT: 		return L"WSAENOPROTOOPT";
	case WSAEPROTONOSUPPORT: 	return L"WSAEPROTONOSUPPORT";
	case WSAESOCKTNOSUPPORT: 	return L"WSAESOCKTNOSUPPORT";
	case WSAEOPNOTSUPP: 		return L"WSAEOPNOTSUPP";
	case WSAEPFNOSUPPORT: 		return L"WSAEPFNOSUPPORT";
	case WSAEAFNOSUPPORT: 		return L"WSAEAFNOSUPPORT";
	case WSAEADDRINUSE: 		return L"WSAEADDRINUSE";
	case WSAEADDRNOTAVAIL: 		return L"WSAEADDRNOTAVAIL";
	case WSAENETDOWN: 			return L"WSAENETDOWN";
	case WSAENETUNREACH: 		return L"WSAENETUNREACH";
	case WSAENETRESET: 			return L"WSAENETRESET";
	case WSAECONNABORTED:		return L"WSWSAECONNABORTEDAEINTR";
	case WSAECONNRESET: 		return L"WSAECONNRESET";
	case WSAENOBUFS: 			return L"WSAENOBUFS";
	case WSAEISCONN: 			return L"WSAEISCONN";
	case WSAENOTCONN: 			return L"WSAENOTCONN";
	case WSAESHUTDOWN: 			return L"WSAESHUTDOWN";
	case WSAETOOMANYREFS: 		return L"WSAETOOMANYREFS";
	case WSAETIMEDOUT: 			return L"WSAETIMEDOUT";
	case WSAECONNREFUSED: 		return L"WSAECONNREFUSED";
	case WSAELOOP: 				return L"WSAELOOP";
	case WSAENAMETOOLONG: 		return L"WSAENAMETOOLONG";
	case WSAEHOSTDOWN: 			return L"WSAEHOSTDOWN";
	case WSASYSNOTREADY: 		return L"WSASYSNOTREADY";
	case WSAVERNOTSUPPORTED: 	return L"WSAVERNOTSUPPORTED";
	case WSANOTINITIALISED: 	return L"WSANOTINITIALISED";
	case WSAHOST_NOT_FOUND: 	return L"WSAHOST_NOT_FOUND";
	case WSATRY_AGAIN: 			return L"WSATRY_AGAIN";
	case WSANO_RECOVERY: 		return L"WSANO_RECOVERY";
	case WSANO_DATA: 			return L"WSANO_DATA";
	default: 					return L"UNKNOWN";
	}
}

SEGAN_LIB_INLINE word sx_net_compute_checksum(const void* buffer, const uint size)
{
	const byte* buf = (const byte*)buffer;
	word sum1 = 0;
	word sum2 = 0;
	for ( uint index = 0; index < size; ++index )
	{
	   sum1 = (sum1 + buf[index]) % 255;
	   sum2 = (sum2 + sum1) % 255;
	}
	return (sum2 << 8) | sum1;
}

bool sx_net_verify_packet(const void* buffer, const uint size)
{
	//	validate message size
	if ( sx_between_i( size, sizeof(NetHeader), SX_NET_BUFF_SIZE ) == false )
		return false;

	NetHeader* ch = (NetHeader*)buffer;

	//	validate net id
	if ( ch->netId != SX_NET_ID )
		return false;

	//	validate data checksum
	if ( size > sizeof(NetHeader) )
	{
		const byte* buf = (const byte*)buffer + sizeof(NetHeader);
		if ( ch->checksum != sx_net_compute_checksum( buf, size - sizeof(NetHeader)) )
			return false;
	}
	
	// new we can suppose that the package is valid
	return true;
}

double sx_net_get_timer(void)
{
	static LARGE_INTEGER qFerquency;
	static LARGE_INTEGER qCounter;
	static BOOL	useHighRes = QueryPerformanceFrequency(&qFerquency);

	if (useHighRes && QueryPerformanceCounter(&qCounter))
	{
		return (((double)qCounter.LowPart / (double)qFerquency.LowPart));
	}
	else
	{
		return 0;
	}
}

dword sx_net_get_time(void)
{
	return GetTickCount();
}


#endif
