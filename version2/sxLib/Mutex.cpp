#include "Mutex.h"

#if defined(_WIN32)
#include <windows.h>
#endif

Mutex::Mutex()
{
#if defined(_WIN32)
	m_mutex = ::CreateMutex(NULL, FALSE, NULL);
#endif
}

Mutex::~Mutex()
{
#if defined(_WIN32)
	if (m_mutex)
		CloseHandle(m_mutex);
#endif
}

void Mutex::Lock()
{
#if defined(_WIN32)
	WaitForSingleObject(m_mutex, INFINITE);
#endif
}

void Mutex::Unlock()
{
#if defined(_WIN32)
	ReleaseMutex(m_mutex);
#endif
}

