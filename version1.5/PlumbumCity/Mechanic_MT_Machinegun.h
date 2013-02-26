/********************************************************************
	created:	2013/02/12
	filename: 	Mechanic_MT_Machinegun.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain mechanic for manual tower machinegun
*********************************************************************/
#ifndef GUARD_Mechanic_MT_Machinegun_HEADER_FILE
#define GUARD_Mechanic_MT_Machinegun_HEADER_FILE


#include "ImportEngine.h"
#include "Mechanic.h"
#include "GameTypes.h"

namespace GM
{
	//! use this mechanic class to implement an repair power attack
	class Mechanic_MT_Machinegun: public Mechanic
	{
	public:

		Mechanic_MT_Machinegun(void);
		~Mechanic_MT_Machinegun(void);

		//! initialize mechanic after game start
		void Initialize(void);

		//! finalize mechanic before game closed
		void Finalize(void);

		//! process player input
		void ProcessInput(bool& inputHandled, float elpsTime);

		//! update mechanic
		void Update(float elpsTime);

		//! process messages
		void MsgProc(UINT recieverID, UINT msg, void* data);

	public:

		//! enter to manual tower mode
		void EnterToManual(void);

		//! leave manual tower mode
		void LeaveManual(void);

		//! update the camera
		void UpdateCamera( float elpsTime );

		//! shoot a bullet and change index of pipe
		void ShootTheBullet( const prpAttack* attack, const bool special );

	public:

		prpAttack			m_attack;			//	attack property of tower
		sx::core::PNode		m_node;				//	the node in the scene
		sx::core::PNode		m_nodeCamera;		//	camera node
		sx::core::PNode		m_nodeWeapon;		//  weapon node
		sx::core::PNode		m_nodePipe[4];		//  pipes of weapon to shoot
		int					m_pipeIndex;		//  index of weapon pipeline
		int					m_shootCount;		//  number of shooting
		float				m_shootTime;		//  time of shooting
		float3				m_Rot;				//  direction of weapon
		float3				m_RotOffset;		//  offset of direction
		float3				m_RotMax;			//	maximum rotation value
		float				m_shakeMagnitude;
		float				m_forceFeedback;
		int					m_fire;				//  fire order for weapon
		int					m_magazineCap;		//	capacity of each magazine
		int					m_bullets;			//	number of bullets in weapon
		int					m_firedCount;
		float				m_reloadTime;		//	time of reload for weapon
		float				m_reload;
		bool				m_selected;			//	true of this tower has been selected

		sx::gui::ProgressBar*	m_reloadBar;
		sx::gui::Label*		m_lblMagazine;
		sx::gui::Label*		m_lblBullet;
	};


} // namespace GM


#endif	//	GUARD_Mechanic_MT_Machinegun_HEADER_FILE

