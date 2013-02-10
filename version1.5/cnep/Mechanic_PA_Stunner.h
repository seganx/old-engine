/********************************************************************
	created:	2012/02/03
	filename: 	Mechanic_PA_Stunner.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain  a power attack class.
				this power attack stun all enemies in the the scene
*********************************************************************/
#ifndef GUARD_Mechanic_PA_Stunner_HEADER_FILE
#define GUARD_Mechanic_PA_Stunner_HEADER_FILE


#include "ImportEngine.h"
#include "Mechanic.h"

class Entity;

namespace GM
{
	//! use this mechanic class to implement an stunner power attack
	class Mechanic_PA_Stunner: public Mechanic
	{

	public:

		Mechanic_PA_Stunner(void);
		~Mechanic_PA_Stunner(void);

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
		float		m_stunTime;
		float		m_stunValue;

		int			m_index;		//  index of power attack used in gui alignment

		str512		m_Hint;			//  hint of power attack contain name and description

	public:

		sx::gui::PPanelEx		m_panelEx;
		sx::gui::PProgressBar	m_progBar;

	};


} // namespace GM

#endif	//	GUARD_Mechanic_PA_Stunner_HEADER_FILE
