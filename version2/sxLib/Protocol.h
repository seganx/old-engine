/********************************************************************
	created:	2016/4/19
	filename: 	Protocol.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contains a simple protocol class to transfer
				data as an encrypt and or compressed object
*********************************************************************/
#ifndef DEFINED_Protocol
#define DEFINED_Protocol

#include "Def.h"

#define  SX_PROTOCOL_COMPRESS	0x01000000		//	data compressed
#define  SX_PROTOCOL_ENCRYPT	0x02000000		//	data encrypted

//! protocol class used to transferring data 
class SEGAN_LIB_API Protocol
{
public:
	struct Header
	{
		uint	hhash;	//! checksum of protocol header to prevent process invalid data
		uint	id;		//! id of the protocol used to prevent duplication
		uint	flag;	//! flag of data format indicate that data is compressed or encrypted
		uint	key;	//!	hash key used in hash functions
		uint	dhash;	//! checksum of data in the protocol
		uint	size;	//! actual size of the whole uncompressed data in the protocol
	};

	struct Data
	{
		char	type[8];	//! type of data
		uint	size;		//! size of data
		char*	data;		//! pointer to the main data. DO NOT change that
	};

public:
	Protocol(void) : m_data(0), m_size(0), m_pos(0) {}
	~Protocol(void);

	//! add text to the protocol data
	void add_text(const char* text);

	//! add additional data to the protocol
	void add_data(const char* type, const uint size, const void* data);

	//! pack a protocol and prepare it to send through net
	void pack(const dword flag, const uint id, const uint key);

	//! unpack a protocol and prepare it to parse data
	void unpack(const char* data, const uint size);

	//! return the header of the protocol message. return zero id if the message is not valid
	const Header get_header(void);

	//! return data block of the protocol. return null if protocol and/or index is not valid
	const Data get_data(const uint index = 0);

public:

	char*	m_data;
	uint	m_size;
	uint	m_pos;
};

#endif // DEFINED_Protocol

