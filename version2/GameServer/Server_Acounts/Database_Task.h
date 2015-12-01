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

#include "../Net.h"

#define  SX_DB_RESULT_SIZE  2048

class DatabaseTask
{
public:
	DatabaseTask( const char* receivedMsg, const uint msgSize );

public:
	char	m_msg[SX_NET_BUFF_SIZE];	//! received message from connection
	char	m_res[SX_DB_RESULT_SIZE];	//! result of database
	word	m_ressize;					//! result size in byte
};


#endif // DATABASE_TASK_DEFINED