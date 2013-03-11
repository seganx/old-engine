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
#include "input/Input.h"
#include "d3d/d3dDevice.h"
#include "engine/draw_debugger.h"


//! structure of engine configuration
struct EngineConfig
{
	LoggerConfig*			logger;						//	pointer to logger configuration. this can be null to leave it to default
	dword					net_id;						//	net ID used to avoid net conflict
	Window*					window_main;				//	main rendering window. this can be null to create a new window a pointer to existing window
	WindowEventCallback		window_callback;			//	window call back to handle window events. this can be null to leave it to default
	dword					d3d_flag;					//	rendering device flags
	InputDevice*			input_device[4];			//	free slot to attach input devices


	EngineConfig()
		: logger(null)
		, net_id(0x2729)
		, window_main(null)
		, window_callback(null)
		, d3d_flag( SX_D3D_CREATE_GL | SX_D3D_VSYNC )
		//, input_device(0, 0, 0, 0)
	{

	}
};


//! main engine structure
class SEGAN_ENG_API Engine
{
public:
	class Logger*				m_logger;
	class Network*				m_network;
 
	class Window*				m_window;
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
