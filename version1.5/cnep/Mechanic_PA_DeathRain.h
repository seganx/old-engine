/********************************************************************
	created:	2012/03/12
	filename: 	Mechanic_PA_DeathRain.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of power attack which rain
				bombs in a specified local on enemies head
*********************************************************************/
#ifndef GUARD_Mechanic_PA_BombRain_HEADER_FILE
#define GUARD_Mechanic_PA_BombRain_HEADER_FILE

#include "ImportEngine.h"
#include "Mechanic.h"
#include "GameTypes.h"

class Entity;
class Projectile;

namespace GM
{

	//! use this mechanic class to create towers
	class Mechanic_PA_DeathRain: public Mechanic
	{
	public:

		Mechanic_PA_DeathRain(void);
		~Mechanic_PA_DeathRain(void);

		//! initialize mechanic
		void Initialize(void);

		//! finalize mechanic
		void Finalize(void);

		//! process player input
		void ProcessInput(bool& inputHandled, float elpsTime);

		//! update mechanic
		void Update(float elpsTime);

		//! process messages
		void MsgProc(UINT recieverID, UINT msg, void* data);

		//! gui clicked
		void OnGUIClick(sx::gui::PControl Sender);

		//! create hot zone
		void CreateBombZone(const float3& pos);

	public:

		int			m_Cost;
		float		m_Time;
		float		m_coolTime;
		int			m_Index;		//  index of power attack used in gui alignment
		str512		m_Hint;			//  hint of power attack contain name and description
		prpAttack	m_Attack;		//  attack value 
		
		sx::core::PNode	m_hotNode;			//  node to show hot zone

	public:

		sx::gui::PPanelEx		m_panelEx;
		sx::gui::PProgressBar	m_progBar;
	};


} // namespace GM

#endif	//	GUARD_Mechanic_PA_BombRain_HEADER_FILE
