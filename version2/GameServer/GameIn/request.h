/********************************************************************
	created:	2016/4/15
	filename: 	request.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain the main request object which will
	be throw through all plugins.

	The RO has function and params comes from client so
	plugins can understand if they could handle the request
	or not. finally the plugins can call the callback to
	sent result through connection.
	*********************************************************************/
#ifndef DEFINED_request
#define DEFINED_request

typedef int(*request_callback)(void* connection, const void* buffer, int size);

struct Request
{
	bool			handled = false;	//! indicated that the request has been handled
	char*			userid = 0;			//! user id specified in authentication process and point to the player id
	int				version = 0;		//! version of the request specified by the client
	char*			func = 0;			//! function name. can be null.
	char*			params = 0;			//! function parameter. can be null.
	int				paramsize = 0;		//! size of the parameter in byte.

	//	helper parameters
	class Player*		player = 0;		//! player object indicated that which player called the request. can be null.
	void*				connection = 0;	//! pointer to the connection.
	request_callback	send = 0;		//! callback function to send data to client. pass connection to the callback function as first parameter.
};

#endif // DEFINED_request

