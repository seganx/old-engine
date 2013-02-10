/********************************************************************
	created:	2011/11/09
	filename: 	Mechanic.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain main classes and functions to		
				implement game mechanics
*********************************************************************/
#ifndef GUARD_Mechanic_HEADER_FILE
#define GUARD_Mechanic_HEADER_FILE

#include "ImportEngine.h"

namespace GM
{

	//! use this abstract class as grandpa of other game mechanics
	class Mechanic
	{
	public:

		Mechanic(void);
		virtual ~Mechanic(void);

		//! initialize mechanic will called after level loaded
		virtual void Initialize(void) = 0;

		//! finalize mechanic will called before level cleared
		virtual void Finalize(void) = 0;

		//! process player input
		virtual void ProcessInput(bool& inputHandled, float elpsTime) = 0;

		//! update mechanic
		virtual void Update(float elpsTime) = 0;

		//! process messages
		virtual void MsgProc(UINT recieverID, UINT msg, void* data);

		//! return true if game is paused or is in menu level
		bool NotInGame( void );

		//! return true if this mechanic is focused
		bool IsFocused(void);

		//! pass true to set this mechanic as focused mechanic
		void SetFocused(bool focused);

		//! return focused mechanic object. return null if no mechanic focused
		static Mechanic* GetFocused(void);

	public:
		sx::core::Node*	m_soundNode;
	};
	typedef Mechanic *PMechanic;
	typedef Array<PMechanic> arrayPMechanic;

} // namespace GM

#endif	//	GUARD_Mechanic_HEADER_FILE