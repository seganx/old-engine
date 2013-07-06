#include <Windows.h>
#include "gameup_import.h"


typedef int ( __cdecl * gu_start )( void );
typedef int ( __cdecl * gu_stop )( void );
typedef int ( __cdecl * gu_get_lock_code )( const int index );
typedef int ( __cdecl * gu_begin_score )( void );
typedef int ( __cdecl * gu_add_score )( const int reason, const int mode );
typedef int ( __cdecl * gu_end_score )( void );
typedef int ( __cdecl * gu_set_info )( const int index, int val );
typedef int ( __cdecl * gu_get_info )( const int index );
typedef int ( __cdecl * gu_set_data_i )( const int index, int val );
typedef int ( __cdecl * gu_get_data_i )( const int index );
typedef int ( __cdecl * gu_set_data_f )( const int index, float val );
typedef float ( __cdecl * gu_get_data_f )( const int index );

int gameup_init( GameUp* gameup )
{
	gameup->dllmodul = LoadLibrary( L"GameUp.dll" );
	gameup->get_lock_code	= (gu_get_lock_code)GetProcAddress( (HMODULE)gameup->dllmodul, "gu_get_lock_code" );
	gameup->begin_score		= (gu_begin_score)	GetProcAddress( (HMODULE)gameup->dllmodul, "gu_begin_score" );
	gameup->add_score		= (gu_add_score)	GetProcAddress( (HMODULE)gameup->dllmodul, "gu_add_score" );
	gameup->end_score		= (gu_end_score)	GetProcAddress( (HMODULE)gameup->dllmodul, "gu_end_score" );
	gameup->set_info		= (gu_set_info)		GetProcAddress( (HMODULE)gameup->dllmodul, "gu_set_info" );
	gameup->get_info		= (gu_get_info)		GetProcAddress( (HMODULE)gameup->dllmodul, "gu_get_info" );
	gameup->set_data_i		= (gu_set_data_i)	GetProcAddress( (HMODULE)gameup->dllmodul, "gu_set_data_i" );
	gameup->get_data_i		= (gu_get_data_i)	GetProcAddress( (HMODULE)gameup->dllmodul, "gu_get_data_i" );
	gameup->set_data_f		= (gu_set_data_f)	GetProcAddress( (HMODULE)gameup->dllmodul, "gu_set_data_f" );
	gameup->get_data_f		= (gu_get_data_f)	GetProcAddress( (HMODULE)gameup->dllmodul, "gu_get_data_f" );

	gu_start start = (gu_start)GetProcAddress( (HMODULE)gameup->dllmodul, "gu_start" );
	return start();
}

void gameup_finit( GameUp* gameup )
{
	gu_stop stop = (gu_stop)GetProcAddress( (HMODULE)gameup->dllmodul, "gu_stop" );
	stop();
	FreeLibrary( (HMODULE)gameup->dllmodul );
}

