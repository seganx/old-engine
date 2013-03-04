/********************************************************************
	created:	2013/02/12
	filename: 	Mechanic_MT_Sniper.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain mechanic for manual tower sniper
*********************************************************************/
#ifndef GUARD_Mechanic_MT_Sniper_HEADER_FILE
#define GUARD_Mechanic_MT_Sniper_HEADER_FILE


#include "ImportEngine.h"
#include "Mechanic.h"
#include "GameTypes.h"

class Entity;
struct prpAttack;

namespace GM
{
	//! use this mechanic class to implement ...
	class Mechanic_MT_Sniper: public Mechanic
	{
	public:

		Mechanic_MT_Sniper(void);
		~Mechanic_MT_Sniper(void);

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
		void ShootTheBullet(void);

	public:
		prpAttack			m_attack;			//	attack property of tower
		sx::core::PNode		m_node;				//	the node in the scene
		sx::core::PNode		m_nodeCamera;		//	camera node
		sx::core::PNode		m_nodeWeapon;		//  weapon node
		sx::core::PNode		m_nodePipe;			//  pipe of weapon to shoot
		int					m_shootCount;		//  number of shooting
		float				m_shootTime;		//  time of shooting
		float3				m_rot;				//  direction of weapon
		float3				m_rotOffset;		//  offset of direction
		float3				m_rotMax;			//	maximum rotation value
		float				m_fov_min;
		float				m_fov_max;
		float				m_fov;
		float				m_forceFeedback;
		float				m_cameraSpeed;		//	speed of camera movement
		float				m_cameraBreath;		//	movement caused by breathing
		int					m_fire;				//  fire order for weapon
		bool				m_selected;			//	true of this tower has been selected
		int					m_energyPerBullet;


		sx::gui::Label*		m_bulletIndicator;
	};


} // namespace GM


#endif	//	GUARD_Mechanic_MT_Sniper_HEADER_FILE

