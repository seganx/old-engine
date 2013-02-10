#include "com_Turbo.h"
#include "Entity.h"


com_Turbo::com_Turbo( void ): Component()
	,	m_node(0)
	,	m_speed(0)
	,	m_speedTime(0)
	,	m_turboCount(1)
	,	m_time(-1)
{
	sx_callstack_push(com_Turbo::com_Turbo());

	m_name = L"Turbo";
	m_tag = MAKEFOURCC('C','M','T','B');
}

com_Turbo::~com_Turbo( void )
{
	sx_callstack_push(com_Turbo::~com_Turbo());	
}

void com_Turbo::Initialize( void )
{
	sx_callstack_push(com_Turbo::Initialize());
	if ( m_owner->m_node )
		m_owner->m_node->GetChildByName( m_nodeName, m_node );
	m_time = -1.0f;
}

void com_Turbo::Finalize( void )
{
	sx_callstack_push(com_Turbo::Finalize());
}

void com_Turbo::Update( float elpsTime )
{
	sx_callstack_push(com_Turbo::Update());

	if ( !m_owner || m_owner->m_health.icur < 1 || !m_owner->m_node ) return;

	if ( m_time > 0 )
	{
		m_owner->m_move.moveScale = m_speed;

		m_time -= elpsTime * 0.001f;
		if ( m_time <= 0 )
		{
			m_owner->m_move.moveScale = 1.0f;

			if ( m_node )
			{
				msg_Particle msgPar( 0, SX_PARTICLE_SPRAY );
				m_node->MsgProc( MT_PARTICLE, &msgPar );

				msg_SoundStop msgSnd( false );
				m_node->MsgProc( MT_SOUND_STOP, &msgSnd );
			}

			if ( m_turboCount < 1 )
				m_deleteMe = true;
		}
	}
}

void com_Turbo::MsgProc( UINT msg, void* data )
{
	sx_callstack_push(com_Turbo::MsgProc(msg=%d), msg);

	if ( !m_owner || m_owner->m_health.icur < 1 || !m_owner->m_node ) return;

	switch ( msg )
	{
	case GMT_DAMAGE:
		if ( data )
		{
			msgDamage* damage = (msgDamage*)data;

			if ( damage->stunValue < 15 && m_time < 0 && m_turboCount > 0 )
			{
				m_time = m_speedTime;
				m_turboCount--;

				if ( m_node )
				{
					msg_Particle msgPar( SX_PARTICLE_SPRAY );
					m_node->MsgProc( MT_PARTICLE, &msgPar );

					msg_SoundPlay msgSnd( false );
					m_node->MsgProc( MT_SOUND_PLAY, &msgSnd );
				}

				if ( damage->sender )
					damage->sender->MsgProc( GMT_LEAVE_ME, m_owner );

				//	ignore stunner damages
				damage->stunTime = 0;
				damage->stunValue = 0;
			}
		}
		break;
	}
}

Component* com_Turbo::Clone( void )
{
	sx_callstack_push(com_Turbo::Clone());

	com_Turbo* me		= sx_new( com_Turbo );
	me->m_name			= m_name;
	me->m_tag			= m_tag;
	me->m_nodeName		= m_nodeName;
	me->m_speed			= m_speed;
	me->m_speedTime		= m_speedTime;
	me->m_turboCount	= m_turboCount;

	return me;
}
