/********************************************************************
	created:	2012/03/11
	filename: 	Mechanic_PA_Trap.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of power attack which create
				hot zone to damage to enemies
*********************************************************************/
#ifndef GUARD_Mechanic_PA_Trap_HEADER_FILE
#define GUARD_Mechanic_PA_Trap_HEADER_FILE


#include "ImportEngine.h"
#include "Mechanic.h"
#include "GameTypes.h"

class Entity;

namespace GM
{

	//! use this mechanic class to trap
	class Mechanic_PA_Trap: public Mechanic
	{
	public:

		Mechanic_PA_Trap(void);
		~Mechanic_PA_Trap(void);

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

		//! create trap
		void CreateTrap(void);

		//! clear all traps from the scene
		void ClearTraps(void);

	public:

		int					m_Cost;
		float				m_Time;
		float				m_coolTime;
		int					m_index;		//  index of power attack used in gui alignment
		str512				m_Hint;			//  hint of power attack contain name and description

		prpAttack			m_attack;
		sx::core::PNode		m_node;		//  node to show trap
		float3				m_pos;		//	position of trap


		Array<struct Trap*> m_traps;	//	array of traps


	public:

		sx::gui::PPanelEx		m_panelEx;
		sx::gui::PProgressBar	m_progBar;
	};


} // namespace GM

#endif	//	GUARD_Mechanic_PA_Trap_HEADER_FILE

