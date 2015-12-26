#include <windows.h>
#include "Database_Thread.h"
#include "Database_Task.h"
#include "Database.h"


DWORD WINAPI ThreadProc(__in  LPVOID lpParameter)
{
	DatabaseThread* dt = (DatabaseThread*)lpParameter;

	Database db;
	if ( db.initalize( dt->m_config ) == false )
		dt->m_status.jobsdone = true;

	while ( !dt->m_status.jobsdone )
	{
		if ( dt->m_status.ready )
		{
			dt->m_status.ready = false;
			if (dt->m_current)
			{
				dt->m_status.waiting = true;
#if 1
				char* s = dt->m_current->m_msg;

				char deviceID[64] = { 0 };
				for (int i = 0; i < 64 && *s != '#'; ++i)
					deviceID[i] = *s++;

				s++;

				char nickName[64] = { 0 };
				for (int i = 0; i < 64 && *s != 0; ++i)
					nickName[i] = *s++;
#else
				char deviceID[64] = { 0 };
				char nickName[64] = { 0 };

				const char* tmp = sx_raw_read(deviceID, 64, dt->m_current->m_msg);
				tmp = sx_raw_read(nickName, 64, tmp);
#endif

				dt->m_current->m_ressize = db.FormatCommand(dt->m_current->m_res, SX_DB_RESULT_SIZE, "INSERT INTO players(deviceId, nickname) VALUES('%s', '%s')", deviceID, nickName);
				//dt->m_current->m_ressize = db.FormatCommand(dt->m_current->m_res, SX_DB_RESULT_SIZE, "SELECT COUNT(*) FROM players;");
				
				dt->m_status.waiting = false;
				dt->m_status.received = true;
			}
			else dt->m_status.idle = true;
		}

		Sleep(1);
	}
	
	db.Finalize();
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//	DATABASE THREAD OBJECT
//////////////////////////////////////////////////////////////////////////
DatabaseThread::DatabaseThread(void)
	: m_current(null)
	, m_id(0)
	, m_thread(null)
	, m_config(null)
	, m_deadtime(0)
	, m_maxdeadtime(5)
{
}

DatabaseThread::~DatabaseThread(void)
{
	if (m_thread)
		Finit();
}

void DatabaseThread::Init( const uint connectionId, const double timeout, const struct DatabaseConfig* databaseConfig )
{
	m_id = connectionId;
	m_deadtime = m_maxdeadtime = timeout;
	m_config = databaseConfig;

	m_thread = CreateThread(0, 0, &ThreadProc, this, 0, 0);
	if ( m_thread == null )
		sx_print(L"Error: Creating thread failed! OS Error: %u", GetLastError());
}

void DatabaseThread::Finit(void)
{
	if ( m_thread )
	{
		CloseHandle(m_thread);
		m_thread = null;
	}

	sx_safe_delete_and_null(m_current);

	DatabaseTask* task = null;
	while (m_queue.pop(task))
		sx_safe_delete_and_null(task);
}

void DatabaseThread::Add( DatabaseTask* task )
{
	m_queue.push(task);
}

uint DatabaseThread::PeekResult(char* dest, const uint destsize)
{
	uint res = 0;
	if (m_status.received)
	{
		m_status.received = false;
		if (m_current)
		{
			if ( m_current->m_ressize > 0 && m_current->m_ressize <= destsize )
			{
				sx_mem_copy( dest, m_current->m_res, m_current->m_ressize );
				res = m_current->m_ressize;
			}
			sx_delete_and_null( m_current );
		}
		m_status.idle = true;
	}
	return res;
}

void DatabaseThread::Update(void)
{
	if (m_status.idle)
	{
		if (m_current == null)
		{
			if (m_queue.pop(m_current))
			{
				m_deadtime = m_maxdeadtime;
				m_status.idle = false;
				m_status.ready = true;
			}
			else
			{
				m_deadtime -= g_timer->m_elpsTime;
				if (m_deadtime < 0)
				{
					m_status.idle = false;
					m_status.jobsdone = true;
				}
			}
		}
	}
	else if (m_status.jobsdone)
	{
		DWORD exitCode = 0;
		if ( GetExitCodeThread(m_thread, &exitCode) )
		{
			if ( exitCode != STILL_ACTIVE )
				m_status.dead = true;
		}
	}
}

