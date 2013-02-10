#include "EditorSettings.h"

int2 EditorSettings::GetInitWindowSize( void )
{
	return int2( 800, 600 );
}

UINT EditorSettings::GetSleepTime( void )
{
	return 3000000;
}

bool EditorSettings::GetGridVisible( void )
{
	return true;
}

int EditorSettings::GetGridSize( void )
{
	return 10;
}

D3DColor EditorSettings::GetGridColor( void )
{
	return 0xFF222222;
}
