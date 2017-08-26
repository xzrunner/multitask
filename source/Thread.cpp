#include "multitask/Thread.h"

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

}