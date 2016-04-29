#include "authenticator.h"

extern Authenticator* g_authen = null;

Authenticator::Authenticator()
{
	m_authen_timeout = 3;
	m_access_timeout = 3600;

	m_game_keys[0].id = 31;
	sx_mem_copy(m_game_keys[0].key, "qwer1234asdfzxcv", 16);

	m_keys.set_size(25000);
	m_codes.set_size(50000);
}

Authenticator::~Authenticator()
{
	for (uint i = 0; i < m_keys.m_size; ++i )
		sx_mem_free_and_null( m_keys.m_slots[i] );

	for (uint i = 0; i < m_codes.m_size; ++i)
		sx_mem_free_and_null(m_codes.m_slots[i]);
}

const char* Authenticator::get_key_of_game(uint id)
{
	for ( int i = 0; i < 8; ++i )
		if ( m_game_keys[i].id == id )
			return m_game_keys[i].key;
	return null;
}

void Authenticator::Update(void)
{
	m_mutex_keys.lock();

	for (uint i = 0; i < m_keys.m_size; ++i )
	{
		AuthenKeys* ak = m_keys.m_slots[i];
		if ( ak )
		{
			ak->time_out--;

			printf("id:%u key:%.*s time:%u\n", ak->id, 16, ak->gnrt_key, ak->time_out);

			if ( ak->time_out < 1 )
			{
				m_keys.remove_index( ak->id );
				sx_mem_free( ak );
			}
		}
	}

	m_mutex_keys.unlock();
}

