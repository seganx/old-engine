#include <windows.h>
#include "Database_Thread.h"
#include "Database_Task.h"
#include "Database.h"


DWORD WINAPI ThreadProc(__in  LPVOID lpParameter)
{
	DatabaseThread* dt = (DatabaseThread*)lpParameter;

	Database db;
	if ( db.initalize( dt->m_config ) == false )
		goto _exit;

	while ( dt->m_status != DBTS_JOBSDONE )
	{
		Sleep(1);

		switch ( dt->m_status )
		{
			case DBTS_NONE: continue;
			case DBTS_READY:
				if (dt->m_current)
				{
					dt->m_status = DBTS_WAITING;
					
					char* s = dt->m_current->m_msg;

					char deviceID[64] = { 0 };
					for ( int i = 0; i< 64 && *s != '#'; ++i )
							deviceID[i] = *s++;

					s++;

					char nickName[64] = { 0 };
					for (int i = 0; i < 64 && *s != 0; ++i)
						nickName[i] = *s++;

					dt->m_current->m_ressize = db.FormatCommand(dt->m_current->m_res, SX_DB_RESULT_SIZE, "INSERT INTO players(deviceId, nickname) VALUES('%s', '%s')", deviceID, nickName);
					//dt->m_current->m_ressize = db.FormatCommand(dt->m_current->m_res, SX_DB_RESULT_SIZE, "SELECT COUNT(*) FROM players;");
					
					dt->m_status = DBTS_RECEIVED;
				}
		}
	}

	_exit:
	dt->m_status = DBTS_JOBSDONE;
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//	DATABASE THREAD OBJECT
//////////////////////////////////////////////////////////////////////////
DatabaseThread::DatabaseThread(void)
	: m_status(DBTS_NONE)
	, m_current(null)
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
	switch (m_status)
	{
		case DBTS_RECEIVED:
			if (m_current)
			{
				if ( m_current->m_ressize > 0 && m_current->m_ressize <= destsize )
				{
					sx_mem_copy( dest, m_current->m_res, m_current->m_ressize );
					res = m_current->m_ressize;
				}
				sx_delete_and_null( m_current );
			}
			m_status = DBTS_NONE;
			break;
	}
	return res;
}

void DatabaseThread::Update(void)
{
	switch (m_status)
	{
		case DBTS_NONE:
			if (m_current == null)
			{
				if (m_queue.pop(m_current))
				{
					m_deadtime = m_maxdeadtime;
					m_status = DBTS_READY;
				}
				else
				{
					m_deadtime -= g_timer->m_elpsTime;
					if (m_deadtime < 0)
						m_status = DBTS_JOBSDONE;
				}
			}
			break;
	}
}

