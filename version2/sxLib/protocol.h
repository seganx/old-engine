/********************************************************************
	created:	2013/09/03
	filename: 	protocol.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain header functions of the protocol
*********************************************************************/
#ifndef GUARD_protocol_HEADER_FILE
#define GUARD_protocol_HEADER_FILE


#define  SX_PROTOCOL_API _declspec( dllexport )
#define  SX_PROTOCOL_COMPRESS	0x01000000		//	data compressed
#define  SX_PROTOCOL_ENCRYPT	0x02000000		//	data encrypted

#ifndef uint
typedef unsigned __int8		u8,			byte;
typedef unsigned __int16	u16,		word;
typedef __int32				i32,		sint;
typedef unsigned __int32	u32,		uint, 		dword;
typedef __int64				i64,		int64;
typedef unsigned __int64	u64,		uint64;
typedef void				*pvoid, 	*handle;
typedef wchar_t				wchar;
typedef int					hresult;
typedef byte				*pbyte;
#endif

struct ProtocolHeader
{
	uint	hhash;	//! checksum of protocol header to prevent process invalid data
	uint	id;		//! id of the protocol used to prevent duplication
	uint	flag;	//! flag of data format indicate that data is compressed or encrypted
	uint	key;	//!	hash key used in hash functions
	uint	dhash;	//! checksum of data in the protocol
	uint	size;	//! actual size of the whole uncompressed data in the protocol
};

struct ProtocolData
{
	char	type[8];	//! type of data
	uint	size;		//! size of data
	char*	data;		//! pointer to the main data. DO NOT changing that
};


//! create a hash number from given data
SX_PROTOCOL_API uint sx_hash( const void* data, const uint size, const uint key = 1363 );

//! encrypt src data to the dest using key value
SX_PROTOCOL_API void sx_encrypt( void* dest, const void* src, const uint size, const uint key = 1363 );

//! decrypt src data to the dest using key value
SX_PROTOCOL_API void sx_decrypt( void* dest, const void* src, const uint size, const uint key = 1363 );


//! protocol class used to transferring data 
class SX_PROTOCOL_API Protocol
{
public:
	Protocol( void ): m_data(0), m_size(0), m_pos(0) {}
	~Protocol( void );

	//! add text to the protocol data
	void add_text( const char* text );

	//! add additional data to the protocol
	void add_data( const char* type, const uint size, const void* data );

	//! pack a protocol and prepare it to send through net
	void pack( const dword flag, const uint id, const uint key );

	//! unpack a protocol and prepare it to parse data
	void unpack( const char* data, const uint size );

	//! return the header of the protocol message. return zero id if the message is not valid
	const ProtocolHeader get_header( void );

	//! return data block of the protocol. return null if protocol and/or index is not valid
	const ProtocolData get_data( const uint index = 0 );

public:

	char*	m_data;
	uint	m_size;
	uint	m_pos;
};


#endif	//	GUARD_protocol_HEADER_FILE
