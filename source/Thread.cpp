#include "multitask/Thread.h"

#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

namespace mt
{

/************************************************************************/
/* class Thread                                                         */
/************************************************************************/

Thread::Thread(void* (*main)(void*), void* arg)
{
	pthread_create(&m_thread, NULL, main, arg);
}

Thread::~Thread()
{
	pthread_join(m_thread, NULL);
}

void Thread::Delay(unsigned int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(1000 * ms);
#endif
}

}