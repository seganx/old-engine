/********************************************************************
	created:	2013/06/15
	filename: 	Database_Thread.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple thread class to handle 
				database query
*********************************************************************/
#ifndef DATABASE_THREAD_DEFINED
#define DATABASE_THREAD_DEFINED

#include "../Net.h"

enum DataBaseThreadStatus
{
	DBTS_NONE = 0,		//! thread is not initialized
	DBTS_READY,			//! thread has a query and it's ready to process
	DBTS_WAITING,		//! thread is waiting for SQL answer
	DBTS_RECEIVED,		//! query was responded and received data is available
	DBTS_JOBSDONE,		//! thread just finished his job and ready to destroyed
};

class DatabaseThread
{
public:
	DatabaseThread( void );
	~DatabaseThread( void );

	//! initialize object and create a thread
	void Init( const uint connectionId, const double timeout, const struct DatabaseConfig* databaseConfig );

	//! finalize thread and close it's handle
	void Finit( void );

	//! add a new task to thread
	void Add( class DatabaseTask* task );

	//! peek received results and return the result length in byte. return 0 if no result exist
	uint PeekResult( char* dest, const uint destsize );

	//! update thread
	void Update( void );

public:
	uint							m_id;				//! id of the thread
	double							m_deadtime;			//! time of the last received message
	double							m_maxdeadtime;		//! thread will be destroyed after this delay time when task queue became empty
	void*							m_thread;			//! handle of the thread
	const struct DatabaseConfig*	m_config;			//! configuration of SQL database 
	DataBaseThreadStatus			m_status;			//! status of the thread
	class DatabaseTask*				m_current;			//! current task is processing
	Queue<class DatabaseTask*>		m_queue;			//! queue of database query
};

#endif	//	DATABASE_THREAD_DEFINED
