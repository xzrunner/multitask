#ifndef _MULTITASK_THREAD_POOL_H_
#define _MULTITASK_THREAD_POOL_H_

#include "multitask/Thread.h"

#include <CU_Uncopyable.h>

#include <deque>
#include <vector>

namespace mt
{

class Task;

class ThreadPool : private cu::Uncopyable
{
public:
	ThreadPool();
	~ThreadPool();

	bool IsRunning() const { return m_running; }

	void Run(Task* task);
	Task* Take();

	void Start(int num_threads);
	void Stop();

	void SetMaxQueueSize(int max_size) { m_max_queue_size = max_size; }
	size_t QueueSize();

private:
	bool IsFull() const;

private:
	mt::Mutex m_mutex;

	mt::Condition m_not_empty;
	mt::Condition m_not_full;

	std::deque<Task*> m_queue;
	size_t m_max_queue_size;

	std::vector<Thread*> m_threads;

	bool m_running;

}; // ThreadPool

}

#endif // _MULTITASK_THREAD_POOL_H_