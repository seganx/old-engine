/********************************************************************
	created:	2015/11/05
	filename: 	Datababe_Task.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple task structure
*********************************************************************/
#ifndef DATABASE_TASK_DEFINED
#define DATABASE_TASK_DEFINED

#include "Database_Result.h"

class DatabaseTask
{
public:
	DatabaseTask( const char* receivedMsg, const uint msgSize );

public:
	char			m_msg[SX_NET_BUFF_SIZE];	//! received message from connection
	DatabaseResult	m_result;					//! result of database
};


#endif // DATABASE_TASK_DEFINED