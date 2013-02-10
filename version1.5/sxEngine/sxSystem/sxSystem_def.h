/********************************************************************
	created:	2010/09/27
	filename: 	sxSystem_Def.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain Perprocess defines for system header file
*********************************************************************/
#ifndef GUARD_sxSystem_def_HEADER_FILE
#define GUARD_sxSystem_def_HEADER_FILE

#include "../../sxLib/lib.h"
#include <Windows.h>

#define SEGAN_LOG_LEVEL1			//! log some important events happened in engine
#define SEGAN_LOG_LEVEL2			//! log more things
//#define SEGAN_LOG_LEVEL3			//! log events in deep inside in the engine

	/*
	Critical Section keep safe operations on multi threaded mode.
	*/
	class CriticalSection
	{
	public:
		CriticalSection(void){
			InitializeCriticalSection(&CS);
		}
		~CriticalSection(void){
			DeleteCriticalSection(&CS);
		}
		void Enter(void){
			EnterCriticalSection(&CS);
		}
		void Leave(void){
			LeaveCriticalSection(&CS);
		}
	private:
		CRITICAL_SECTION CS;
	};

	#define SEGAN_CLAMP(var, Min, Max)			{ if (var<Min) var=Min; if (var>Max) var=Max; }
	#define	SEGAN_BETWEEN(var, Min, Max)		(Min<=var && var<=Max)

#endif // GUARD_sxSystem_def_HEADER_FILE
