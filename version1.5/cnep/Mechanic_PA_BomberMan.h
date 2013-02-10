/********************************************************************
	created:	2012/02/27
	filename: 	Mechanic_PA_BomberMan.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class of power attack which have
				an airplane to rain bomb on enemies
*********************************************************************/
#ifndef GUARD_Mechanic_PA_Bomber_HEADER_FILE
#define GUARD_Mechanic_PA_Bomber_HEADER_FILE

#include "ImportEngine.h"
#include "Mechanic.h"

class Entity;

namespace GM
{
	//! use this mechanic class to implement an bomber power attack
	class Mechanic_PA_BomberMan: public Mechanic
	{

	public:

		Mechanic_PA_BomberMan(void);
		~Mechanic_PA_BomberMan(void);

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

		int				m_Cost;
		float			m_Time;
		float			m_coolTime;
		int				m_Index;		//  index of power attack used in gui alignment
		str512			m_Hint;			//  hint of power attack contain name and description

		Entity*				m_Bomber;		//  entity in the scene
		sx::core::PNode		m_startNode;	//  start movement
		sx::core::PNode		m_endNode;		//  end of movement
	public:

		sx::gui::PPanelEx		m_panelEx;
		sx::gui::PProgressBar	m_progBar;

	};


} // namespace GM

#endif	//	GUARD_Mechanic_PA_Bomber_HEADER_FILE
