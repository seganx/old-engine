/********************************************************************
	created:	2016/4/15
	filename: 	gamein.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the main class of the application.
				The main class creates and contains other classes.

				We will use this class as a global class to have access
				through all sub systems
*********************************************************************/
#ifndef DEFINED_gamein
#define DEFINED_gamein

#include "imports.h"

//! the class of GameIn server
class GameIn
{
public:
	GameIn();
	~GameIn();

	//! start the main loop of the server
	void start(const wchar* configFile, struct mg_callbacks* mgcallbacks);

public:
	struct mg_context*	m_mongoose;		//! mongoose main context
	class PluginMan*	m_plugins;		//! plugin manager of the server

};

#endif // DEFINED_gamein

