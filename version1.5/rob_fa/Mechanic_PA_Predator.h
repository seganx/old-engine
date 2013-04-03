/********************************************************************
	created:	2012/02/28
	filename: 	Mechanic_PA_Predator.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of power attack which have
				an wild airplane to kill enemies
*********************************************************************/
#ifndef GUARD_Mechanic_PA_Predator_HEADER_FILE
#define GUARD_Mechanic_PA_Predator_HEADER_FILE

#include "ImportEngine.h"
#include "Mechanic.h"

class Entity;

namespace GM
{
	//! use this mechanic class to implement an predator power attack
	class Mechanic_PA_Predator: public Mechanic
	{

	public:

		Mechanic_PA_Predator(void);
		~Mechanic_PA_Predator(void);

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
		float		m_Distance;		//  distance of movement
		int			m_Index;		//  index of power attack used in gui alignment
		str512		m_Hint;			//  hint of power attack contain name and description

		Entity*		m_Predator;		//  entity in the scene

	public:

		sx::gui::PPanelEx		m_panelEx;
		sx::gui::PProgressBar	m_progBar;

	};


} // namespace GM

#endif	//	GUARD_Mechanic_PA_Predator_HEADER_FILE
