/********************************************************************
	created:	2011/11/21
	filename: 	Mechanic_EnemyWave.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a class to control enemy waves
*********************************************************************/
#ifndef GUARD_Mechanic_EnemyWave_HEADER_FILE
#define GUARD_Mechanic_EnemyWave_HEADER_FILE

#include "ImportEngine.h"
#include "Mechanic.h"

class Entity;

namespace GM
{

#define WAVE_ENEMY_MAXTYPE	32

	struct EnemyWave{
		struct SubWave{
			Entity*				type;
			int					count;
			float				timeStep;
			float				startTime;
			float				elapsTime;
			sx::core::PNode		startNode;
			sx::core::PNode		baseNode;

			int					matIndex;
			int					addGold;
			int					addHealth;
			float				addPhysicalDamage;
			float				addElectricalDamage;
			float				addPhysicalArmor;
			float				addElectricalArmor;
			float				addFireRate;
			float				addMoveSpeed;
			float				addAnimSpeed;
			float				addExperience;

			WCHAR				infoTitle[128];
			WCHAR				infoDesc[512];
			WCHAR				infoImage[64];
			int					infoShowNow;
		};

		SubWave					subWave[WAVE_ENEMY_MAXTYPE];

		WCHAR					name[64];
		WCHAR					tipsStartNode[64];
		WCHAR					tipsStart[512];
		WCHAR					tipsEnd[512];
		WCHAR					tipsStartIcon[64];
		WCHAR					tipsEndIcon[64];
		
		int						addGold;
		int						addHealth;
		float					addPhysicalDamage;
		float					addElectricalDamage;
		float					addPhysicalArmor;
		float					addElectricalArmor;
		float					addFireRate;
		float					addMoveSpeed;
		float					addAnimSpeed;
		float					addExperience;
		float					nextWaveTime;
		float					goldPerSecond;	//	how many gold per seconds
		int						enemyCounts;
		sx::core::Node*			baseNode;
	};
	typedef Array<EnemyWave*>	arrayPEnemyWave;


	//! use this mechanic class to control enemy waves
	class Mechanic_EnemyWaves: public Mechanic
	{

	public:

		Mechanic_EnemyWaves(void);
		~Mechanic_EnemyWaves(void);

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

		//! clear waves
		void ClearWaves(void);

		//! load waves
		void LoadWaves(void);

		//! update music
		void UpdateMusic( float elpsTime );

	private:
		void SetNextWaveImage(void);
		void StartWave(void);
		void OnClick( sx::gui::PControl sender );

		arrayPEnemyWave			m_wavesSrc;			//  source of enemy wave list
		int						m_waveIndex;		//  index of enemy wave
		float					m_waveTime;
		int						m_enemyCount;		//  number of enemies in the scene

		sx::gui::PPanelEx		m_back;				//	gui to show wave info
		sx::gui::PLabel			m_label;			//	label to show number of waves
		sx::gui::PLabel			m_labelGold;		//	label to show the amount of catching golds
		sx::gui::PPanelEx		m_nextWave;			//	gui to show next wave info
		sx::gui::PProgressBar	m_startProgr;		//	progress bar to show the time
	
		sx::gui::PPanelEx		m_mapBack;			//	background for mini map

		sx::core::Node*			m_musicNode;			//	music node of the waves
		sx::core::Sound*		m_musicCurrSound;		//	current sound object
		sx::core::Sound*		m_musicLastSound;		//	last sound object
		int						m_musicLastIndex;		//	last music index
	};


} // namespace GM

#endif	//	GUARD_Mechanic_EnemyWave_HEADER_FILE

