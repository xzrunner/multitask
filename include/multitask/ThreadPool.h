#ifndef _MULTITASK_THREAD_POOL_H_
#define _MULTITASK_THREAD_POOL_H_

#include "multitask/Task.h"

#include <CU_Uncopyable.h>

namespace mt
{

class ThreadPool : private cu::Uncopyable
{
public:
	ThreadPool();
	~ThreadPool();

	void Submit(Task* task) {
		m_work_queue.Push(task);
	}
	Task* Fetch() {
		return m_work_queue.TryPop();
	}

private:
	void InitThreads();

private:
	SafeTaskQueue m_work_queue;

	std::vector<Thread*> m_threads;

}; // ThreadPool

}

#endif // _MULTITASK_THREAD_POOL_H_