#include "multitask/ThreadPool.h"
#include "multitask/Task.h"

namespace mt
{

/************************************************************************/
/* class DaemonThread                                                   */
/************************************************************************/

static void* 
daemon_thread_loop(void* arg)
{
	DaemonThread* daemon = static_cast<DaemonThread*>(arg);
	while (true)
	{
		daemon->Update();
		Thread::Delay(5);
	}
}

DaemonThread::DaemonThread(ThreadPool* pool)
	: m_pool(pool)
{
	m_mutex  = new Mutex();
	m_thread = new Thread(daemon_thread_loop, this);
}

DaemonThread::~DaemonThread()
{
	delete m_mutex;
	delete m_thread;
}

void DaemonThread::Update()
{
	mt::Lock lock(m_mutex);

	for (int i = 0, n = m_update_cb.size(); i < n; ++i) {
		m_update_cb[i].first(m_update_cb[i].second);
	}

	if (m_tasks.Empty()) {
		return;
	}

	Task* task = m_tasks.Front();
	if (m_pool->AddTaskOnlyFree(task)) {
		m_tasks.Pop();
	}
}

void DaemonThread::RegisterUpdateCB(void (*update)(void* arg), void* arg)
{
	mt::Lock lock(m_mutex);

	m_update_cb.push_back(std::make_pair(update, arg));
}

void DaemonThread::UnregisterUpdateCB(void (*update)(void* arg))
{
	mt::Lock lock(m_mutex);

	std::vector<std::pair<void (*)(void*), void*> >::iterator itr
		= m_update_cb.begin();
	for ( ; itr != m_update_cb.end(); ) {
		if (itr->first == update) {
			itr = m_update_cb.erase(itr);
		} else {
			++itr;
		}
	}
}

void DaemonThread::AddTask(Task* task)
{
	mt::Lock lock(m_mutex);

	m_tasks.Push(task);
}

/************************************************************************/
/* class ThreadPool                                                     */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <windows.h>
#elif __MACOSX
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

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

//////////////////////////////////////////////////////////////////////////

SINGLETON_DEFINITION(ThreadPool)

ThreadPool::ThreadPool()
{
	m_daemon = new DaemonThread(this);
	for (int i = 0; i < THREAD_NUM; ++i) {
		m_threads[i] = new TaskThread();
	}
}

ThreadPool::~ThreadPool()
{
	delete m_daemon;
	for (int i = 0; i < THREAD_NUM; ++i) {
		delete m_threads[i];
	}
}

void ThreadPool::AddTask(Task* task)
{
	if (!AddTaskOnlyFree(task)) {
		m_daemon->AddTask(task);
	}
}

void ThreadPool::Flush()
{
	for (int i = 0; i < THREAD_NUM; ++i) {
		m_threads[i]->Flush();
	}
}

void ThreadPool::Lock()
{
	for (int i = 0; i < THREAD_NUM; ++i) {
		m_threads[i]->Lock();
	}
}

void ThreadPool::Unlock()
{
	for (int i = 0; i < THREAD_NUM; ++i) {
		m_threads[i]->Unlock();
	}
}

void ThreadPool::GetResult(unsigned int type, Task* tasks[THREAD_NUM])
{
	for (int i = 0; i < THREAD_NUM; ++i) {
		tasks[i] = m_threads[i]->GetResult(type);
	}
}

bool ThreadPool::AddTaskOnlyFree(Task* task)
{
	for (int i = 0; i < THREAD_NUM; ++i) {
		if (m_threads[i]->EmptyNoLock()) {
			m_threads[i]->AddTask(task);
			return true;
		}
	}
	return false;
}

}