/********************************************************************
	created:	2011/11/12
	filename: 	Mechanic_TowerCreate.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain mechanic class for create towers
*********************************************************************/
#ifndef GUARD_Mechanic_TowerCreate_HEADER_FILE
#define GUARD_Mechanic_TowerCreate_HEADER_FILE

#include "ImportEngine.h"
#include "Mechanic.h"
#include "com_ShowRange.h"

class Entity;

namespace GM
{

	//! use this mechanic class to create towers
	class Mechanic_TowerCreate: public Mechanic
	{
	public:

		Mechanic_TowerCreate(void);
		~Mechanic_TowerCreate(void);

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

		//!  set the name of tower to place
		void SetData(const WCHAR* towerName, Entity* entTower);

	private:

		//! catch a tower to create
		void CatchTower( int index );

		//! release caught Tower
		void ReleaseCaughtTower( bool force = false );

		//! create tower
		void CreateTower(float3 pos);

		//! return true if the player can bye this tower
		bool CanBuyTower( int iindex );

		void ShowPanel( void );
		void HidePanel( void );

		sx::gui::PPanelEx	m_pnlCreate0;		//  background panel of tower creator
		sx::gui::PPanelEx	m_pnlCreate1;		//  background panel of tower creator
		sx::gui::PPanelEx	m_pnlArrow;			//	arrow to selected tower
		sx::gui::PPanelEx	m_btnCreate[5];		//  buttons to create towers
		str512				m_tower_locked;		//	the string to show that the tower is not unlocked
		
		float				m_tower_distance;	//	minimum distance of towers
		int					m_towerIndex;		//  index of tower type
		Entity*				m_Tower;			//  tower entity to place
		com_ShowRange		m_ShowRange;		//  use to show the range of tower
		sx::core::Mesh*		m_zone;				//	zone mesh
	};


} // namespace GM

#endif	//	GUARD_Mechanic_TowerEdit_HEADER_FILE
