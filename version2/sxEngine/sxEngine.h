/********************************************************************
	created:	2010/09/27
	filename: 	sxEngine.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain any header files and declaration for
				using of SeganX engine functions and engine classes
*********************************************************************/
#ifndef GUARD_sxEngine_HEADER_FILE
#define GUARD_sxEngine_HEADER_FILE

#include "../sxLib/Lib.h"

#include "System/System.h"
#include "Math/Math.h"
#include "d3d/d3dDevice.h"


//! structure of engine configuration
struct EngineConfig
{
	LoggerConfig*			logger;
	dword					net_id;
	bool					math_force_use_generic;
	WindowEventCallback		window_event_callback;
	dword					d3d_flag;

	EngineConfig()
		: logger(null)
		, net_id(0x27272727)
		, math_force_use_generic(false)
		, window_event_callback(null)
		, d3d_flag(0)
	{

	}
};


//! main engine structure
class SEGAN_ENG_API Engine
{
public:
	class Logger*				m_logger;
	class Network*				m_network;
 
	class d3dDevice*			m_device3D;
	class uiDevice*				m_deviceUI;
	class phsDevice*			m_devicePhysics;
	class sndDevice*			m_deviceSound;
	class scrDevice*			m_deviceScript;
	class aiDevice*				m_deviceAI;

	class Input*				m_input;
	class FileManager*			m_file;
	class Renderer*				m_renderer;
	class SceneManager*			m_scene;
	class GuiManager*			m_gui;
	class PhysicsManager*		m_physics;
	class SoundManager*			m_sound;

};
extern Engine* g_engine;

//////////////////////////////////////////////////////////////////////////
//	this functions will call by the other functions. no need to call this functions from application
//////////////////////////////////////////////////////////////////////////

//! create main engine instance, initialize main systems and return pointer to the singleton object
SEGAN_ENG_API Engine* engine_get_singleton( EngineConfig* config = null );

//! initialize subsystems and verify objects. return true if engine has initialized 
SEGAN_ENG_API bool engine_initialize( void );

//! finalize all system and subsystem of engine
SEGAN_ENG_API void engine_finalize( void );

//!	start engine and call the main game loop
SEGAN_ENG_API void engine_start( ApplicationMainLoop mainloop );

#endif	//	GUARD_sxEngine_HEADER_FILE
