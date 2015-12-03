#include "Thread_Manager.h"
#include "Database.h"
#include "Database_Thread.h"
#include "Database_Task.h"
#include "../Connection.h"

ThreadManager::ThreadManager(void): m_threads(16) { }

DatabaseThread* ThreadManager::Add(const uint id, const struct DatabaseConfig* databaseConfig)
{
	//	add a new database thread
	DatabaseThread* dbThread = sx_new DatabaseThread;
	dbThread->Init(id, databaseConfig->threadTimeout * 0.001, databaseConfig);

	m_threads.push_back(dbThread);
	m_threadsMap.insert(id, dbThread);

	sx_print_a("Info: Thread [%u] has been created!", dbThread->m_id);

	return dbThread;
}

void ThreadManager::Remove(DatabaseThread* dt)
{
	sx_print_a("Info: Thread [%u] has been removed!", dt->m_id);

	m_threads.remove(dt);
	m_threadsMap.remove(dt->m_id);

	dt->Finit();
	sx_delete dt;
}

DatabaseThread* ThreadManager::AddTask(const uint threadId, const struct DatabaseConfig* databaseConfig, const char* msg, const uint msgsize)
{
	DatabaseTask* dbTask = sx_new DatabaseTask(msg, msgsize);

	DatabaseThread* dbThread = null;
	if (m_threadsMap.find(threadId, dbThread) == false)
		dbThread = Add(threadId, databaseConfig);

	dbThread->Add(dbTask);

	return dbThread;
}

void ThreadManager::Update(void)
{
	for (sint i = 0; i < m_threads.m_count; ++i)
	{
		DatabaseThread* dbThread = m_threads[i];

		dbThread->Update();

		if (dbThread->m_status.dead)
		{
			Remove(dbThread);
			--i;
		}
	}
}
