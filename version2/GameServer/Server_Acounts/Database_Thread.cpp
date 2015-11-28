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

					db.Command( dt->m_current->m_result, "INSERT INTO players(deviceId, nickname) VALUES('%s', '%s')", deviceID, nickName );
					
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
	, m_time(0)
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
	m_time = timeout;
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
}

void DatabaseThread::Add( DatabaseTask* task )
{
	m_queue.push(task);
}

