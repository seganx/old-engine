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
		void EnterToManual( const Entity* tower );

		//! leave manual tower mode
		void LeaveManual(void);

		//! shoot a bullet and change index of pipe
		void ShootTheBullet( const prpAttack* pAttack, const float3& dir );

	public:

		Entity*				m_tower;			//	selected tower
		sx::core::PNode		m_nodeWeapon;		//  weapon node
		sx::core::PNode		m_nodePipe;			//  pipe of weapon to shoot

		int					m_shootCount;		//  number of shooting
		float				m_shootTime;		//  time of shooting
		float3				m_dir;				//  direction of weapon
		float3				m_dirOffset;		//  offset of direction

		int					m_fire;				//  fire order for weapon

		int					m_magazineCap;		//	capacity of each magazine
		int					m_bullets;			//	number of bullets in weapon
		float				m_reloadTime;		//	time of reload for weapon

	};


} // namespace GM


#endif	//	GUARD_Mechanic_MT_Sniper_HEADER_FILE

