/********************************************************************
	created:	2015/11/07
	filename: 	Thread_Manager.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple thread manager
*********************************************************************/
#ifndef THREAD_MANAGER_DEFINED
#define THREAD_MANAGER_DEFINED

#include "../Net.h"

class DatabaseThread;

class ThreadManager
{
public:
	ThreadManager( void );

	//! add a new thread with specific id
	DatabaseThread* Add( const uint id, const struct DatabaseConfig* databaseConfig );

	//! remove the thread and delete the object
	void Remove( DatabaseThread* dt );

	//! add a task to manager and return the thread object. it will create a new thread object if needed. return null on failed
	DatabaseThread* AddTask( const uint threadId, const struct DatabaseConfig* databaseConfig, const char* msg, const uint msgsize );

	//! update thread's time for dead connection detection
	void Update( void );

public:
	Array<DatabaseThread*>		m_threads;		//! list of threads used for iteration
	Map<uint, DatabaseThread*>	m_threadsMap;	//! map of threads to find a connection so fast
};


#endif // THREAD_MANAGER_DEFINED