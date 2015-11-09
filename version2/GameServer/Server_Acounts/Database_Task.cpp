#include "Database_Task.h"

DatabaseTask::DatabaseTask( const char* receivedMsg, const uint msgSize )
{
	sx_mem_set( m_msg, 0, SX_NET_BUFF_SIZE );
	sx_mem_copy( m_msg, receivedMsg, msgSize );
}

