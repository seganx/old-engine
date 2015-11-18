#include "Thread_Manager.h"
#include "Database_Thread.h"
#include "Database_Task.h"
#include "../Connection.h"

ThreadManager::ThreadManager(const uint timeout)
	: m_timeout(timeout)
	, m_threads(16)
{

}

DatabaseThread* ThreadManager::Add(const uint id, const struct DatabaseConfig* databaseConfig)
{
	//	add a new database thread
	DatabaseThread* dbThread = sx_new DatabaseThread;
	dbThread->Init( id, m_timeout, databaseConfig );

	m_threads.push_back( dbThread );
	m_threadsMap.insert( id, dbThread );

	sx_print_a("Thread [%u] has been created!\n", dbThread->m_id);

	return dbThread;
}

void ThreadManager::Remove(DatabaseThread* dt)
{
	m_threads.remove( dt );
	m_threadsMap.remove( dt->m_id );

	dt->Finit();
	sx_delete(dt);
}

DatabaseThread* ThreadManager::AddTask(const uint threadId, const struct DatabaseConfig* databaseConfig, const char* msg, const uint msgsize)
{
	DatabaseTask* dbTask = sx_new DatabaseTask(msg, msgsize);

	DatabaseThread* dbThread = null;
	if ( m_threadsMap.find( threadId, dbThread ) == false )
		dbThread = Add( threadId, databaseConfig );

	dbThread->Add(dbTask);

	return dbThread;
}

void ThreadManager::Update(const uint threadId, class Connection* connection)
{
	DatabaseThread* dbThread = null;
	if ( m_threadsMap.find( threadId, dbThread ) == false )
		return;
	
	switch ( dbThread->m_status )
	{
		case DBTS_NONE:
			if ( dbThread->m_current == null )
			{
				if ( dbThread->m_queue.pop( dbThread->m_current ) )
				{
					dbThread->m_time = m_timeout;
					dbThread->m_status = DBTS_READY;
				}					
			}
			break;

		//case DBTS_WAITING: sx_print_a("\nSQL Waiting ...\n"); break;

		case DBTS_RECEIVED:
			//sx_print_a("SQL Received !\n"); 
			if ( dbThread->m_current )
			{
				if ( dbThread->m_current->m_result.m_count > 0 )
				{
					dbThread->m_current->m_result.Print();

					//	send the database result to through the connection
					//const wchar* res = dbThread->m_current->m_result.curr();
					//connection->Send(res, sx_str_len(res) * 2 + 2, SX_NET_OPTN_SAFESEND);
				}
				sx_delete_and_null(dbThread->m_current);
			}
			dbThread->m_status = DBTS_NONE;
			break;
	}	
}

void ThreadManager::CheckThreadsTime(const double elpsTime)
{
	for ( sint i = 0; i < m_threads.m_count; ++i )
	{
		DatabaseThread* dbThread = m_threads[i];

		switch ( dbThread->m_status )
		{
			case DBTS_NONE:
				if (dbThread->m_current == null)
				{
					dbThread->m_time -= elpsTime;
					if (dbThread->m_time < 0)
					{
						dbThread->m_time = 5;
						dbThread->m_status = DBTS_JOBSDONE;
					}
				}
				break;

			case DBTS_JOBSDONE:	//	remove the database thread
				dbThread->m_time -= elpsTime;
				if (dbThread->m_time < 0)
				{
					sx_print_a("Thread [%u] has been removed!\n", dbThread->m_id);
					Remove(dbThread);
				}
				break;
		}		
	}
}
