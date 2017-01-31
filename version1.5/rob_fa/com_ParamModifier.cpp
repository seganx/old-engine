#include "com_ParamModifier.h"
#include "Entity.h"



com_ParamModifier::com_ParamModifier( void ): Component()
	,	m_coolTime(0)
	,	m_time(0)
	,	m_radius(0)
	,	m_moveScale(0)
	,	m_animIndex(-1)
	,	m_animTime(0)
	,	m_modifyTime(0)
	,	m_speed(1)
	,	m_offsetY(0)
	,	m_physicalArmor(0)
	,	m_electricalArmor(0)
	,	m_health(0)
	,	m_moveType(GMT_NULL)
	,	m_mainMoveType(GMT_NULL)
	,	m_emitter(true)
{
	sx_callstack();

	m_name = L"ParamModifier";
	m_tag = MAKEFOURCC('C','M','P','M');
}

com_ParamModifier::~com_ParamModifier( void )
{
	sx_callstack();	
}

void com_ParamModifier::Initialize( void )
{
	sx_callstack_param(com_ParamModifier::Initialize(m_emitter=%d), byte(m_emitter));

	if ( m_emitter )
	{
		m_time = - sx::cmn::Random( m_coolTime );
	}
	else
	{
		m_time = sx::cmn::Random(1.5f) - sx::cmn::Random(1.5f);
		m_mainMoveType = m_owner->m_move.type;

		if ( m_health != 0 )
		{
			int icur = m_owner->m_health.icur;
			m_owner->m_health.SetCur( icur + m_health );
			m_owner->MsgProc( GMT_DAMAGE, NULL );
		}
	}
}

void com_ParamModifier::Finalize( void )
{
	sx_callstack();
}

void com_ParamModifier::Update( float elpsTime )
{
	sx_callstack();

	using namespace sx::core;
	if ( !m_owner || m_owner->m_health.icur < 1 || !m_owner->m_node ) return;

#if 0
	if ( m_owner->m_health.icur < 1 )
	{
		//	stop spray magic particle and stop magic sound
		PNode modifierNode = NULL;
		if ( m_owner->m_node->GetChildByName( L"_onspell", modifierNode ) )
		{
			//	invisible mesh
			msg_Mesh msgMsh( SX_MESH_INVISIBLE );
			modifierNode->MsgProc( MT_MESH, &msgMsh );

			//	stop spray particles
			msg_Particle msgPar( 0, SX_PARTICLE_SPRAY );
			modifierNode->MsgProc( MT_PARTICLE, &msgPar );

			//	stop sounds
			msg_SoundStop msgSnd( false );
			modifierNode->MsgProc( MT_SOUND_STOP, &msgSnd );
		}

		return;
	}
#endif

	if ( m_emitter )
	{
		if ( m_health < 0 )
			m_owner->m_curAttack.physicalDamage -= (float)m_health;

		// let the animator play modifier animation
		if ( m_animTime > 0 )
		{
			m_animTime -= elpsTime * 0.001f;

			if ( m_animTime <= 0 )
			{
				m_owner->m_move.moveScale = 1.0f;
				m_owner->m_move.walk = true;

				//	stop spray magic particle and stop magic sound
				PNode modifierNode = NULL;
				if ( m_owner->m_node->GetChildByName( L"_onspell", modifierNode ) )
				{
					//	invisible mesh
					msg_Mesh msgMsh( SX_MESH_INVISIBLE );
					modifierNode->MsgProc( MT_MESH, &msgMsh );

					//	stop spray particles
					msg_Particle msgPar( 0, SX_PARTICLE_SPRAY );
					modifierNode->MsgProc( MT_PARTICLE, &msgPar );

					//	stop sounds
					msg_SoundStop msgSnd( false );
					modifierNode->MsgProc( MT_SOUND_STOP, &msgSnd );
				}
			}
			else
			{
				m_owner->m_move.moveScale = m_moveScale;
				m_owner->m_move.walk = false;
			}

			return;
		}

		//	traverse the cool time to spell the magic word :D
		m_time += elpsTime * 0.001f;
		if ( m_time > m_coolTime )
		{
			//	ignore spelling while stunned
			if ( m_owner->m_move.animScale < 0.1f )
			{
				m_time = sx::cmn::Random(5.0f) - sx::cmn::Random(5.0f);
				return;
			}

			static ArrayPNode_inline nodes(512); nodes.Clear();
			Scene::GetNodesByArea( m_owner->GetPosition(), m_radius, nodes, NMT_ALL, m_health < 0 ? m_owner->m_partyEnemies : m_owner->m_partyCurrent );
			if ( nodes.Count() > 0 )
			{
				m_time = sx::cmn::Random(5.0f) - sx::cmn::Random(5.0f);
			}
			else
			{
				m_time = m_coolTime - 1.0f;
				return;
			}

			//	play spelling animation
			if ( m_animIndex >= 0 )
			{
				m_owner->SetState( ES_SPELLING );
				m_owner->m_move.moveScale = m_moveScale;
				m_owner->m_move.walk = false;

				msg_Animator_Count msgAnim;
				m_owner->m_node->MsgProc( MT_ANIMATOR_COUNT, &msgAnim );
				if ( msgAnim.numAnimator )
				{
					Animator* animator = (Animator*)msgAnim.animators[0];
					animator->SetAnimationByIndex( m_animIndex );
					m_animTime = animator->GetMaxTime();
				}
			}

			//	spray magic particle and play magic sound
			PNode modifierNode = NULL;
			if ( m_owner->m_node->GetChildByName( L"_onspell", modifierNode ) )
			{
				//	show mesh
				msg_Mesh msgMsh( 0, SX_MESH_INVISIBLE );
				modifierNode->MsgProc( MT_MESH, &msgMsh );

				//	spray particles
				msg_Particle msgPar( SX_PARTICLE_SPRAY );
				modifierNode->MsgProc( MT_PARTICLE, &msgPar );

				//	play sounds
				msg_SoundPlay msgSnd( false );
				modifierNode->MsgProc( MT_SOUND_PLAY, &msgSnd );
			}

			//	send modifier component to others
			if ( m_health < 0 )
			{
				for ( int i=0; i<nodes.Count(); i++ )
				{
					PNode node = nodes[i];
					Entity* entity = (Entity*)node->GetUserData();
					if ( !entity ) continue;

					float distance = m_owner->GetDistance_edge( entity );
					if ( distance > m_radius ) continue;

					msgDamage msg( -(float)m_health, 0, 0, 0, 0, 0, NULL );
					entity->MsgProc( GMT_DAMAGE, &msg );
				}
			}
			else
			{
				for ( int i=0; i<nodes.Count(); i++ )
				{
					PNode node = nodes[i];
					Entity* entity = (Entity*)node->GetUserData();
					if ( !entity || entity == m_owner ) continue;

					float distance = m_owner->GetDistance_edge( entity );
					if ( distance > m_radius ) continue;

					PNode modifierNode = NULL;
					if ( entity->m_node->GetChildByName( m_nodeName, modifierNode ) )
					{
						msg_Particle msgPar( SX_PARTICLE_SPRAY );
						modifierNode->MsgProc( MT_PARTICLE, &msgPar );

						msg_SoundPlay msgSnd( false );
						modifierNode->MsgProc( MT_SOUND_PLAY, &msgSnd );

						com_ParamModifier* com = (com_ParamModifier*)Clone();
						com->m_emitter = false;
						entity->Attach( com );
					}
				}
			}

			
		}
	}
	else	//	if ( m_emitter )
	{
		m_owner->m_move.moveScale = m_speed;
		if ( m_owner->m_move.animScale > 0.5f )
			m_owner->m_move.animScale = m_speed;
		m_owner->m_curAttack.physicalArmor += m_physicalArmor;
		m_owner->m_curAttack.electricalArmor += m_electricalArmor;

		if ( sx_abs_f(m_offsetY) > 0.1f )
		{
			m_owner->m_posOffset.y += ( m_offsetY - m_owner->m_posOffset.y ) * elpsTime * 0.0015f;
		}

		if ( m_moveType != GMT_NULL )
			m_owner->m_move.type = m_moveType;

		m_time += elpsTime * 0.001f;
		if ( m_time > m_modifyTime )
		{
			PNode modifierNode = NULL;
			if ( m_owner->m_node->GetChildByName( m_nodeName, modifierNode ) )
			{
				msg_Particle msgPar( 0, SX_PARTICLE_SPRAY );
				modifierNode->MsgProc( MT_PARTICLE, &msgPar );
			}

			m_owner->m_move.moveScale = 1.0f;
			if ( m_owner->m_move.animScale > 0.5f )
				m_owner->m_move.animScale = 1.0f;

			if ( m_mainMoveType )
				m_owner->m_move.type = m_mainMoveType;

			m_deleteMe = true;
			return;
		}
	}
}

void com_ParamModifier::MsgProc( UINT msg, void* data )
{
	sx_callstack_param(com_ParamModifier::MsgProc(msg=%d), msg);

	switch ( msg )
	{
	case GMT_DAMAGE:
		if ( data )
		{
			if ( m_animTime > 0 )
			{
				//
			}
		}
		break;

	case GMT_I_DIED:
		if ( data == m_owner )
		{
			//	stop spray magic particle and stop magic sound
			sx::core::PNode modifierNode = NULL;
			if ( m_owner->m_node->GetChildByName( L"_onspell", modifierNode ) )
			{
				//	invisible mesh
				msg_Mesh msgMsh( SX_MESH_INVISIBLE );
				modifierNode->MsgProc( MT_MESH, &msgMsh );

				//	stop spray particles
				msg_Particle msgPar( 0, SX_PARTICLE_SPRAY );
				modifierNode->MsgProc( MT_PARTICLE, &msgPar );

				//	stop sounds
				msg_SoundStop msgSnd( false );
				modifierNode->MsgProc( MT_SOUND_STOP, &msgSnd );
			}
		}
	}

}

Component* com_ParamModifier::Clone( void )
{
	sx_callstack();

	com_ParamModifier* me	= sx_new( com_ParamModifier );
	me->m_name				= m_name;
	me->m_tag				= m_tag;
	me->m_nodeName			= m_nodeName;
	me->m_coolTime			= m_coolTime;
	me->m_radius			= m_radius;
	me->m_moveScale			= m_moveScale;
	me->m_animIndex			= m_animIndex;
	me->m_modifyTime		= m_modifyTime;
	me->m_speed				= m_speed;
	me->m_physicalArmor		= m_physicalArmor;
	me->m_electricalArmor	= m_electricalArmor;
	me->m_health			= m_health;
	me->m_moveType			= m_moveType;
	me->m_offsetY			= m_offsetY;
	me->m_emitter			= m_emitter;

	return me;
}

