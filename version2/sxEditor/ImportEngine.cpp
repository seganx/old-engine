#include "ImportEngine.h"


extern Engine* g_engine = 0;

Engine* sx_engine_get_singleton( EngineConfig* config /*= null */ )
{
	g_engine = engine_get_singleton( config );
	return g_engine;
}

bool sx_engine_initialize( void )
{
	return engine_initialize();
}

void sx_engine_finalize( void )
{
	engine_finalize();
}

void sx_engine_start( ApplicationMainLoop mainloop )
{
	engine_start( mainloop );
}

