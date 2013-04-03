#include "com_HealthModifier.h"
#include "Entity.h"


com_HealthModifier::com_HealthModifier( void ): Component()
	,	m_value(0)
	,	m_count(0)
	,	m_coolTime(0)
	,	m_time(0)
{
	sx_callstack();

	m_name = L"HealthModifier";
	m_tag = MAKEFOURCC('C','M','H','M');
}

com_HealthModifier::~com_HealthModifier( void )
{
	sx_callstack();	
}

void com_HealthModifier::Initialize( void )
{
	sx_callstack();
}

void com_HealthModifier::Finalize( void )
{
	sx_callstack();
}

void com_HealthModifier::Update( float elpsTime )
{
	if ( !m_owner || m_owner->m_health.icur < 1 ) return;

	sx_callstack();

	if ( m_count < 1 || m_coolTime < EPSILON || !m_value )
	{
		m_deleteMe = true;
		return;
	}

	m_time += elpsTime * 0.001f;
	if ( m_time > m_coolTime )
	{
		m_count--;
		m_time = 0;

		int icur = m_owner->m_health.icur;
		m_owner->m_health.SetCur( icur + m_value );
		m_owner->MsgProc( GMT_DAMAGE, NULL );

		sx::core::PNode damageNode = NULL;
		if ( m_nodeName.Text() && m_owner->m_node->GetChildByName( m_nodeName, damageNode ) )
		{
			msg_Particle msgPar( SX_PARTICLE_SPRAY );
			damageNode->MsgProc( MT_PARTICLE, &msgPar );
		}		
	}
}

void com_HealthModifier::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(com_HealthModifier::MsgProc(msg=%d), msg);
}

Component* com_HealthModifier::Clone( void )
{
	sx_callstack();

	com_HealthModifier* me = sx_new( com_HealthModifier );
	
	me->m_name		= m_name;
	me->m_tag		= m_tag;
	me->m_nodeName	= m_nodeName;
	me->m_value		= m_value;
	me->m_count		= m_count;
	me->m_coolTime	= m_coolTime;

	return me;
}
