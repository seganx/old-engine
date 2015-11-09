/********************************************************************
	created:	2015/11/05
	filename: 	Database_Result.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple database result structure
*********************************************************************/
#ifndef GUARD_Database_Result_HEADER_FILE
#define GUARD_Database_Result_HEADER_FILE

#include "../Net.h"


//////////////////////////////////////////////////////////////////////////
//	database result
//	contain the result of database power by simple splitter to access fields
//////////////////////////////////////////////////////////////////////////
class DatabaseResult
{
public:
	DatabaseResult( void );
	~DatabaseResult( void );

	void add( const char* val );
	void curr( wchar* dest, const uint destcount );
	const wchar* curr( void );
	bool next( void );

	void Print();
public:
	wchar**		m_data;
	uint		m_count;
	uint		m_index;
};


#endif	//	GUARD_Database_Result_HEADER_FILE
