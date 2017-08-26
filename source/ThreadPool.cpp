#include "multitask/ThreadPool.h"
#include "multitask/Thread.h"

// get_num_cores()
#ifdef _WIN32
#include <windows.h>
#elif __MACOSX
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

namespace mt
{

ThreadPool::ThreadPool()
{
	InitThreads();
}

ThreadPool::~ThreadPool()
{
	for (int i = 0, n = m_threads.size(); i < n; ++i) {
		delete m_threads[i];
	}
}

static inline int 
get_num_cores() 
{
#ifdef _WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	return sysinfo.dwNumberOfProcessors;
#elif __MACOSX
	int nm[2];
	size_t len = 4;
	uint32_t count;

	nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
	sysctl(nm, 2, &count, &len, NULL, 0);

	if(count < 1) {
		nm[1] = HW_NCPU;
		sysctl(nm, 2, &count, &len, NULL, 0);
		if(count < 1) { count = 1; }
	}
	return count;
#else
	return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

static void*
thread_loop(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (true)
	{
		Task* task = pool->Fetch();
		if (task) {
//			printf("++++++++++++++++ thread run %d\n", pthread_self());
			task->Run();
		} else {
			Thread::Delay(5);
		}
	}
}

void ThreadPool::InitThreads()
{
	int num_cores = get_num_cores();
	for (int i = 0; i < num_cores; ++i) {
		m_threads.push_back(new Thread(thread_loop, this));		
	}
}

}