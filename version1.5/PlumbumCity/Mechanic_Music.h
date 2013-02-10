/********************************************************************
	created:	2012/09/23
	filename: 	Mechanic_Music.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple mechanic to play music in game
*********************************************************************/
#ifndef GUARD_Mechanic_Music_HEADER_FILE
#define GUARD_Mechanic_Music_HEADER_FILE

#include "ImportEngine.h"
#include "Mechanic.h"

class Entity;

namespace GM
{
	//! use this mechanic class to implement an repair power attack
	class Mechanic_Music: public Mechanic
	{

	public:

		Mechanic_Music(void);
		~Mechanic_Music(void);

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

	public:

		float				m_time;
		sx::core::Node*		m_node;


	};


} // namespace GM

#endif	//	GUARD_Mechanic_Music_HEADER_FILE