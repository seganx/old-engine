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
	//	data from client
	const char*		uri;				//! URL-decoded URI
	char			data[1024];			//! data from the client
	int				size = 0;			//! size of data in byte

	//	helper parameters
	bool				handled;		//! indicated that the request has been handled
	class Player*		player;			//! player object indicated that which player called the request. can be null.
	void*				connection;		//! pointer to the connection.
	request_callback	send;			//! callback function to send data to client. pass connection to the callback function as first parameter.
};

#endif // DEFINED_request

