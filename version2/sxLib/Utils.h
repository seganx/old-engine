/********************************************************************
	created:	2013/09/03
	filename: 	Utils.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain some functions and classes depend on
				other tools in library
*********************************************************************/
#ifndef GUARD_Utils_HEADER_FILE
#define GUARD_Utils_HEADER_FILE

#include "Array.h"
#include "String.h"


#define  SX_PROTOCOL_COMPRESS	0x01000000		//	data compressed
#define  SX_PROTOCOL_ENCRYPT	0x02000000		//	data encrypted

//!	load a text file and append that to the string
SEGAN_LIB_API bool sx_load_string( String& dest, const wchar* filename );

//!	load a text file and parse the lines. return false if operation failed
SEGAN_LIB_API bool sx_load_string_list( Array<String*>& dest, const wchar* filename );


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
		char*	data;		//! pointer to the main data. DO NOT changing that
	};

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
	const Header get_header( void );

	//! return data block of the protocol. return null if protocol and/or index is not valid
	const Data get_data( const uint index = 0 );

public:

	char*	m_data;
	uint	m_size;
	uint	m_pos;
};


#endif	//	GUARD_Utils_HEADER_FILE
