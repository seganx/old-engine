/********************************************************************
	created:	2012/05/20
	filename: 	Mechanic_PA_GoldenTowers.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a power attack to make towers powerful
*********************************************************************/
#ifndef GUARD_Mechanic_PA_GoldenTowers_HEADER_FILE
#define GUARD_Mechanic_PA_GoldenTowers_HEADER_FILE


#include "ImportEngine.h"
#include "Mechanic.h"

class Entity;

namespace GM
{
	//! use this mechanic class to implement an repair power attack
	class Mechanic_PA_GoldenTowers: public Mechanic
	{

	public:

		Mechanic_PA_GoldenTowers(void);
		~Mechanic_PA_GoldenTowers(void);

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

		int			m_cost;
		float		m_time;
		float		m_coolTime;
		float		m_goldenTime;
		int			m_index;		//  index of power attack used in gui alignment

	public:

		sx::gui::PPanelEx		m_panelEx;
		sx::gui::PProgressBar	m_progBar;

	};


} // namespace GM

#endif	//	GUARD_Mechanic_PA_GoldenTowers_HEADER_FILE

