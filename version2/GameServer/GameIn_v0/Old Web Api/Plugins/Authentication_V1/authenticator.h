/********************************************************************
	created:	2016/4/23
	filename: 	authenticator.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		Here is some useful structures and functions used in
				authentication system
*********************************************************************/
#ifndef DEFINED_authenticator
#define DEFINED_authenticator

#include "../../imports.h"

#define diffie_hellman_l	32
#define diffie_hellman_g	7
#define diffie_hellman_p	23

#define session_checksum	238746

//! this object holds access key for clients
struct AuthenSession
{
	uint	session_id;			//! id of the session
	uint	session_id_chk;		//! checksum of the session id string sent to client
	uint	access_key;			//! access key used in cryptography
	uint	time_out;			//! time to left
};

//! authenticator class
class Authenticator
{
public:
	Authenticator();
	~Authenticator();

	//! update should call every one second
	void update( void );

	//! print all keys to screen
	void print_keys( void );

public:
	uint					m_authen_timeout;	//! maximum time in seconds for authentication
	uint					m_access_timeout;	//! maximum time in seconds for access code validation

	Table<AuthenSession*>	m_sessions;			//! table of sessions contain authentication keys
	Mutex					m_mutex;			//! used for multi-threading
};

extern Authenticator* g_authen;

#endif // DEFINED_authenticator

