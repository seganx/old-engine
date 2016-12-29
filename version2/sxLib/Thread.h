/********************************************************************
	created:	2016/4/14
	filename: 	Thread.h
	author:		Sajad Beigjani
	email:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contains a wrapped class of multi threading 
				objects include mutual exclusion object, condition, etc
				which are useful for multi threaded systems
*********************************************************************/
#ifndef DEFINED_Thread
#define DEFINED_Thread

#if __cplusplus
#include "Def.h"
#endif

//! use this class to simply create a mutex, lock and unlock it
class SEGAN_LIB_API Mutex
{
public:
	Mutex();
	~Mutex();

	//! lock this mutex object
	int lock();

	//! unlock this mutex object
	int unlock();

private:
	handle	m_mutex;	//! pointer to OS dependent mutex object
};

//! use this class to simply create a condition object
class SEGAN_LIB_API Condition
{
public:
	Condition();
	~Condition();

	int Wait(Mutex * mutex);
	int Signal();
	int Broadcast();

private:
	handle	m_signal;
	handle	m_broadcast;
};



#endif // DEFINED_Thread

