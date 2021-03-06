#ifndef _MULTITASK_THREAD_POOL_H_
#define _MULTITASK_THREAD_POOL_H_

#include "multitask/Thread.h"

#include <cu/uncopyable.h>

#include <deque>
#include <vector>
#include <atomic>

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

	int QueryThreadIdx(std::thread::id id) const;

private:
	bool IsFull() const;

private:
	std::mutex m_mutex;

	std::condition_variable m_not_empty;
	std::condition_variable m_not_full;

	std::deque<Task*> m_queue;
	size_t m_max_queue_size;

	std::vector<std::thread> m_threads;

	std::atomic<bool> m_running;

}; // ThreadPool

}

#endif // _MULTITASK_THREAD_POOL_H_