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

//! here is the object to pair game and secret key
struct GameKeys
{
	uint	id;			//! id of the game
	char	key[16];	//! secret key of the game	
};

struct AuthenKeys
{
	uint	id;					//! id of the authentication key
	uint	time_out;			//! time out to delete object
	char	game_key[16];		//! secret key of the game
	char	recv_key[16];		//! received key from client
	char	gnrt_key[16];		//! generated key for send to client	
};

//! this object holds access code for clients
struct AccessCode
{
	char	access_code[16];	//! access code to the system
	uint	time_out;			//! time to left
};

//! authenticator class
class Authenticator
{
public:
	Authenticator();
	~Authenticator();

	//! return the secret key of the game. return null for invalid id
	const char* get_key_of_game(uint id);

	//! update should call every one second
	void Update( void );

public:
	uint				m_authen_timeout;	//! maximum time in seconds for authentication
	uint				m_access_timeout;	//! maximum time in seconds for access code validation

	GameKeys			m_game_keys[8];		//! array of game-key	

	Table<AuthenKeys*>	m_keys;				//! table of objects contain authentication keys 
	Mutex				m_mutex_keys;		//! used for multi-threading

	Table<AccessCode*>	m_codes;			//! table of objects contain access codes
	Mutex				m_mutex_sess;		//! used for multi-threading
};

extern Authenticator* g_authen;

#endif // DEFINED_authenticator

