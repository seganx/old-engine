#include "Database_Task.h"

DatabaseTask::DatabaseTask( const char* receivedMsg, const uint msgSize ): m_ressize(0)
{
	sx_mem_set( m_res, 0, SX_DB_RESULT_SIZE );
	sx_mem_set( m_msg, 0, SX_NET_BUFF_SIZE );
	sx_mem_copy( m_msg, receivedMsg, msgSize );
}
