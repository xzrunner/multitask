#ifndef _MULTITASK_THREAD_POOL_H_
#define _MULTITASK_THREAD_POOL_H_

#include "multitask/Task.h"

#include <CU_Singleton.h>
#include <CU_Uncopyable.h>

namespace mt
{

class Task;
class TaskThread;
class Thread;
class ThreadPool;

class DaemonThread : private cu::Uncopyable
{
public:
	DaemonThread(ThreadPool* pool);
	~DaemonThread();

	void Update();
	void RegisterUpdateCB(void (*update)(void* arg), void* arg);
	void UnregisterUpdateCB(void (*update)(void* arg));

	void AddTask(Task* task);

private:
	ThreadPool* m_pool;

	Mutex*  m_mutex;
	Thread* m_thread;

	TaskQueue m_tasks;

	std::vector<std::pair<void (*)(void*), void*> > m_update_cb;

}; // DaemonThread

class ThreadPool
{
public:
	static const int THREAD_NUM = 4;

public:
	void AddTask(Task* task);

	void Lock();
	void Unlock();
	void GetResult(unsigned int type, Task* tasks[THREAD_NUM]);

	void RegisterUpdateCB(void (*update)(void* arg), void* arg) {
		m_daemon->RegisterUpdateCB(update, arg);
	}
	void UnregisterUpdateCB(void (*update)(void* arg)) {
		m_daemon->UnregisterUpdateCB(update);
	}

private:
	bool AddTaskOnlyFree(Task* task);

private:
	DaemonThread* m_daemon;

	TaskThread* m_threads[THREAD_NUM];

	friend class DaemonThread;

	SINGLETON_DECLARATION(ThreadPool)

}; // ThreadPool

}

#endif // _MULTITASK_THREAD_POOL_H_