#ifndef _MULTITASK_TICK_THREAD_H_
#define _MULTITASK_TICK_THREAD_H_

#include <CU_Uncopyable.h>

#include <vector>
#include <mutex>

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

	void RegisterUpdateCB(void (*update)(void* arg), void* arg);
	void UnregisterUpdateCB(void (*update)(void* arg));

private:
	ThreadPool* m_pool;

	std::mutex m_mutex;
	std::thread* m_thread;

	std::vector<std::pair<void (*)(void*), void*> > m_update_cb;

	bool m_running;

}; // TickThread

}

#endif // _MULTITASK_TICK_THREAD_H_