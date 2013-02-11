#include "Component.h"
#include "Entity.h"


Component::Component( void )
	: m_owner(NULL)
	, m_tag(0)
	, m_deleteMe(false)
{
	sx_callstack();
}

Component::~Component( void )
{
	sx_callstack();

	if ( m_owner )
	{
		m_owner->m_components.Remove( this );
		m_owner = NULL;
	}
}

void Component::SetOwner( Entity* owner )
{
	sx_callstack();

	if ( m_owner == owner ) return;

	if ( m_owner )
		m_owner->m_components.Remove( this );

	m_owner = owner;

	if ( m_owner && m_owner->m_components.IndexOf( this ) < 0 )
		m_owner->m_components.PushBack( this );
}

Entity* Component::GetOwner( void )
{
	return m_owner;
}

void Component::Initialize( void )
{
}

void Component::Finalize( void )
{
}

void Component::Update( float elpsTime )
{

}

void Component::MsgProc( UINT msg, void* data )
{

}
