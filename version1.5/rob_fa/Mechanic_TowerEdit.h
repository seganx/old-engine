/********************************************************************
	created:	2011/11/21
	filename: 	Mechanic_TowerEdit.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a clas to edit towers in the game
*********************************************************************/
#ifndef GUARD_Mechanic_TowerEdit_HEADER_FILE
#define GUARD_Mechanic_TowerEdit_HEADER_FILE

#include "ImportEngine.h"
#include "Mechanic.h"

class Entity;
class GameGuid;

namespace GM
{

	//! use this mechanic class to edit towers
	class Mechanic_TowerEdit: public Mechanic
	{
	public:

		Mechanic_TowerEdit(void);
		~Mechanic_TowerEdit(void);

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

	private:

		void OnButtonClick(sx::gui::PControl Sender);

		sx::gui::PPanelEx		m_back;				//  first background
		sx::gui::PPanelEx		m_pnlEditor1;		//  second background

		sx::gui::PPanelEx		m_pnlUpdate[4];		//	update panel
		sx::gui::PPanelEx		m_pnlLamp[3];		//	update panel

		sx::gui::PPanelEx		m_pnlSell;			//	sell panel
		sx::gui::PPanelEx		m_pnlButton[3];		//  buttons used in editor [ repair/ upgrade / sell ]

		sx::gui::Label*			m_lblHealth;
		sx::gui::Label*			m_lblXP;
		sx::gui::Label*			m_lblDamage;
		sx::gui::Label*			m_lblFireRate;

		GameGuid*				m_guide;

		UINT					m_mode;				//  current editor mode
	};


} // namespace GM

#endif	//	GUARD_Mechanic_TowerEdit_HEADER_FILE
