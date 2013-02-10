/********************************************************************
created:	2011/02/24
filename: 	ImportEngine.h
Author:		Sajad Beigjani
eMail:		sajad.b@gmail.com
Site:		www.SeganX.com
Desc:		This file contain the header files of SeganX engine.

			How to run the engine :
				use sx_engine_create() to create a singleton instance of the engine.

				before initialize the engine you can change any subsystem to override
				functions and classes.
				
				use sx_engine_initialize() and sx_engine_finalize() to initialize 
				and finalize engine. after that you can access to the instance
				of the engine by g_engine declared in these header files.
				
				for example: 
				g_engine->m_renderer->Update( eplsTime );
*********************************************************************/
#ifndef GUARD_ImportEngine_HEADER_FILE
#define GUARD_ImportEngine_HEADER_FILE


#include "../sxEngine/sxEngine.h"

//! create main engine instance, initialize main systems and return pointer to the singleton object
Engine* sx_engine_get_singleton( EngineConfig* config = null );

//! initialize subsystems and verify objects. return true if engine has started and false if starting failed
bool sx_engine_initialize( void );

//! finalize all system and subsystem of engine
void sx_engine_finalize( void );

//!	start engine and call the main game loop
void sx_engine_start( ApplicationMainLoop mainloop );


#if defined(_WIN32)
#define _WIN32_WINNT	0x0500		// use new features of the windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN			// exclude APIs such as Cryptography, DDE, RPC, Shell, and Windows Sockets.
#endif
#include <Windows.h>
#endif 


#endif	//	GUARD_ImportEngine_HEADER_FILE