/********************************************************************
	created:	2010/10/01
	filename: 	sxEditor.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the header file of editor of seganx
*********************************************************************/
#ifndef GUARD_sxEditor_HEADER_FILE
#define GUARD_sxEditor_HEADER_FILE

#include "resource.h"

#include "ImportEngine.h"

#define NET_ACTIVATE	1
#define NET_DELAY_TIME	60
#define NET_TIMEOUT		60000

//////////////////////////////////////////////////////////////////////////
//  SOME GLOBAL VARIABLES
extern Client* g_client;

//////////////////////////////////////////////////////////////////////////
//  some global functions
void Loop_Start(void);
void Loop_Stop(int value);

#endif	//	GUARD_sxEditor_HEADER_FILE