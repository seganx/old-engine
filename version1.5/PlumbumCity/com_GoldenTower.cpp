#include "com_GoldenTower.h"
#include "Entity.h"


com_GoldenTower::com_GoldenTower( void ): Component(), m_time(0)
{
	sx_callstack_push(com_GoldenTower::com_GoldenTower());
	m_name = L"Golden Tower";
}

com_GoldenTower::~com_GoldenTower( void )
{
	sx_callstack_push(com_GoldenTower::~com_GoldenTower());
	
}

void com_GoldenTower::Initialize( void )
{
	sx_callstack_push(com_GoldenTower::Initialize());
}

void com_GoldenTower::Finalize( void )
{
	sx_callstack_push(com_GoldenTower::Finalize());
}

void com_GoldenTower::Update( float elpsTime )
{
	if ( !m_owner || m_time <= 0 || m_owner->m_health.icur < 1 || !m_owner->m_node ) return;

	sx_callstack_push(com_GoldenTower::Update());

	m_owner->m_curAttack.physicalDamage		*= m_owner->m_curAttack.goldenValue;
	m_owner->m_curAttack.electricalDamage	*= m_owner->m_curAttack.goldenValue;

	m_time -= elpsTime * 0.001f;
	if ( m_time <= 0 )
	{
		msg_Mesh msgMat(0, 0, 0, 0);
		m_owner->m_mesh->MsgProc( MT_MESH, &msgMat );
	}
}

void com_GoldenTower::MsgProc( UINT msg, void* data )
{
	sx_callstack_push(com_GoldenTower::MsgProc(msg=%d), msg);

	switch ( msg )
	{
	case GMT_GOLDEN_TOWER:
		if ( data && m_owner->m_node )
		{
			m_time = *( (float*)data );

			msg_Mesh msgMat(0, 0, 0, 1);
			m_owner->m_mesh->MsgProc( MT_MESH, &msgMat );
		}
		break;

	case GMT_DAMAGE:
		if ( data && m_time > 0 )
		{
			msgDamage* damage = (msgDamage*)data;
			damage->physicalDamage = 0;
			damage->electricalDamage = 0;
		}
	}
}

Component* com_GoldenTower::Clone( void )
{
	sx_callstack_push(com_GoldenTower::Clone());

	com_GoldenTower* me = sx_new( com_GoldenTower );
	return me;
}
