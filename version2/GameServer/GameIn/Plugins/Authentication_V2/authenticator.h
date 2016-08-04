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

#define session_cryptokey	23874665
#define session_secret		"salam sajad"

struct AuthenConfig 
{
	uint time_out;
};

//! this object holds access key for clients
struct AuthenSession
{
	uint	access_key;			//! access key used in cryptography
	uint	birth_time;			//! the time that session created
	uint	time_out;			//! time to left
};


//! load authentication configuration from specified file
void authen_load_config(const char* fileName);

//! handle authentication request
int authen_handle_request(struct Request* req, const uint user_data);

//! handle access permissions to the server
int authen_handle_access(Request* req);


#endif // DEFINED_authenticator

