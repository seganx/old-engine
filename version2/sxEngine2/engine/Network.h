/********************************************************************
	created:	2013/03/12
	filename: 	Network.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple network class to classify
				network functions and use in engine
*********************************************************************/
#ifndef GUARD_Network_HEADER_FILE
#define GUARD_Network_HEADER_FILE

#include "../../sxLib/Lib.h"


/*!
this is a simple concept class and is not designed yet.
just for test and debug !
*/
class SEGAN_ENG_API Network
{
	SEGAN_STERILE_CLASS(Network);

public:

	Network( void );
	~Network( void );

	void Initialize( void );

	void Finalize( void );

	void Update( float elpsTime );

	bool Send( const char* buffer, const int sizeinbyte, const bool critical = false );

public:

	class Client*	m_client;
	class Server*	m_server;

	float			m_delayTime;
	float			m_timeOut;
};

#endif	//	GUARD_Network_HEADER_FILE