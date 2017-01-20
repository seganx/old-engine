#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/types.h> 
#include <unistd.h>
#include <pthread.h>
#endif

#include "Thread.h"

//////////////////////////////////////////////////////////////////////////
//	MUTEXT IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////
Mutex::Mutex()
{
#if defined(_WIN32)
	m_mutex = ::CreateMutex(NULL, FALSE, NULL);
#endif
}

Mutex::~Mutex()
{
#if defined(_WIN32)
	if (m_mutex) CloseHandle(m_mutex);
#endif
}

int Mutex::lock()
{
#if defined(_WIN32)
	return WaitForSingleObject(m_mutex, INFINITE) == WAIT_OBJECT_0 ? 0 : -1;
#endif
}

int Mutex::unlock()
{
#if defined(_WIN32)
	return ReleaseMutex(m_mutex) == 0 ? -1 : 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
//	CONDITION IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////
Condition::Condition()
{
#if defined(_WIN32)
	m_signal = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_broadcast = CreateEvent(NULL, TRUE, FALSE, NULL);
#endif
}

Condition::~Condition()
{
#if defined(_WIN32)
	if (m_signal) CloseHandle(m_signal);
	if (m_broadcast) CloseHandle(m_broadcast);
#endif
}

int Condition::Wait( Mutex * mutex )
{
#if defined(_WIN32)
	HANDLE handles[] = {m_signal, m_broadcast};
	mutex->unlock();
	WaitForMultipleObjects(2, handles, FALSE, INFINITE);
	return mutex->lock();
#endif
}

int Condition::Signal()
{
#if defined(_WIN32)
	return SetEvent(m_signal) == 0 ? -1 : 0;
#endif
}

int Condition::Broadcast()
{
#if defined(_WIN32)
	// Implementation with PulseEvent() has race condition, see
	// http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
	return PulseEvent(m_broadcast) == 0 ? -1 : 0;
#endif
}

SEGAN_LIB_API uint sx_process_currentId()
{
#if defined(_WIN32)
	return SEGAN_LIB_API uint();
#else
	return (uint)getpid();
#endif
}

SEGAN_LIB_API uint sx_thread_currentId()
{
#if defined(_WIN32)
	return SEGAN_LIB_API uint();
#else
	return (uint)pthread_self();		 
#endif
}
