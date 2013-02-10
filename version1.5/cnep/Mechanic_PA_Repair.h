/********************************************************************
	created:	2012/02/22
	filename: 	Mechanic_PA_Repair.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple power attack class which
				repair all towers in the game
*********************************************************************/
#ifndef GUARD_Mechanic_PA_Repair_HEADER_FILE
#define GUARD_Mechanic_PA_Repair_HEADER_FILE


#include "ImportEngine.h"
#include "Mechanic.h"

class Entity;

namespace GM
{
	//! use this mechanic class to implement an repair power attack
	class Mechanic_PA_Repair: public Mechanic
	{

	public:

		Mechanic_PA_Repair(void);
		~Mechanic_PA_Repair(void);

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

		//! gui clicked
		void OnGUIClick(sx::gui::PControl Sender);

	public:

		int			m_Cost;
		float		m_Time;
		float		m_coolTime;

		int			m_index;		//  index of power attack used in gui alignment

		str512		m_Hint;			//  hint of power attack contain name and description

	public:

		sx::gui::PPanelEx		m_panelEx;
		sx::gui::PProgressBar	m_progBar;

	};


} // namespace GM

#endif	//	GUARD_Mechanic_PA_Repair_HEADER_FILE
