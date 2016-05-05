#include "authenticator.h"

extern Authenticator* g_authen = null;

Authenticator::Authenticator()
{
	m_authen_timeout = 30;
	m_access_timeout = 3600;

	m_sessions.set_size(200000);
}

Authenticator::~Authenticator()
{
	for (uint i = 0; i < m_sessions.m_size; ++i )
		sx_mem_free_and_null( m_sessions.m_slots[i] );
}

void Authenticator::update(void)
{
	m_mutex.lock();

	for (uint i = 0; i < m_sessions.m_size; ++i )
	{
		AuthenSession* as = m_sessions.m_slots[i];
		if ( as )
		{
			as->time_out--;

			//printf("id:%u key:%.*s time:%u\n", ak->id, 16, ak->gnrt_key, ak->time_out);

			if ( as->time_out < 1 )
			{
				m_sessions.remove_index( as->session_id );
				sx_mem_free( as );
			}
		}
	}

	m_mutex.unlock();
}

void Authenticator::print_keys(void)
{
	m_mutex.lock();
	for (uint i = 0; i < m_sessions.m_size; ++i)
		if (m_sessions.m_slots[i])
		{
			AuthenSession* as = m_sessions.m_slots[i];
			printf("id:%2u key:%10u time:%u\n", as->session_id, as->access_key, as->time_out);
		}
	m_mutex.unlock();
}

