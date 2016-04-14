/********************************************************************
	created:	2016/4/14
	filename: 	Mutex.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contains a wrapped class of mutual exclusion 
				object which is useful for multi threaded systems
*********************************************************************/
#ifndef DEFINED_Mutex
#define DEFINED_Mutex

#include "Def.h"

//! use this class to simply create a mutex, lock and unlock it
class SEGAN_LIB_API Mutex
{
public:
	Mutex();
	~Mutex();

	//! lock this mutex object
	void Lock();

	//! unlock this mutex object
	void Unlock();

private:
	handle	m_mutex;	//! pointer to OS dependent mutex object
};

#endif // DEFINED_Mutex

