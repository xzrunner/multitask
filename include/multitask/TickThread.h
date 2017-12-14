#ifndef _MULTITASK_TICK_THREAD_H_
#define _MULTITASK_TICK_THREAD_H_

#include <cu/uncopyable.h>

#include <vector>
#if defined(__MINGW32__) && !defined(_GLIBCXX_HAS_GTHREADS)
#include <mutex>
#include <mingw.mutex.h>
#include <mingw.thread.h>
#else
#include <mutex>
#include <thread>
#endif

namespace mt
{

class ThreadPool;

class TickThread : private cu::Uncopyable
{
public:
	TickThread(ThreadPool* pool);
	~TickThread();

	bool IsRunning() const { return m_running; }

	void Run();
	void Stop();

	void RegisterUpdateCB(void (*update)(void* arg), void* arg);
	void UnregisterUpdateCB(void (*update)(void* arg));

private:
	ThreadPool* m_pool;

	std::mutex m_mutex;
	std::thread m_thread;

	std::vector<std::pair<void (*)(void*), void*> > m_update_cb;

	bool m_running;

}; // TickThread

}

#endif // _MULTITASK_TICK_THREAD_H_