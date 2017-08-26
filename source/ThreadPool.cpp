#include "multitask/ThreadPool.h"
#include "multitask/Thread.h"
#include "multitask/Task.h"

#include <assert.h>

namespace mt
{

ThreadPool::ThreadPool()
	: m_mutex()
	, m_not_empty(m_mutex)
	, m_not_full(m_mutex)
	, m_max_queue_size(0)
	, m_running(false)
{
}

ThreadPool::~ThreadPool()
{
	if (m_running) {
		Stop();
	}
}

void ThreadPool::Run(Task* task)
{
	if (m_threads.empty()) 
	{
		task->Run();
	} 
	else 
	{
		mt::Lock lock(m_mutex);

		while (IsFull()) {
			m_not_full.Wait();
		}
		assert(!IsFull());

		task->AddReference();
		m_queue.push_back(task);

		m_not_empty.Notify();
	}
}

Task* ThreadPool::Take()
{
	mt::Lock lock(m_mutex);

	while (m_queue.empty() && m_running) {
		m_not_empty.Wait();
	}
	
	Task* task = NULL;
	if (!m_queue.empty()) 
	{
		task = m_queue.front();
		m_queue.pop_front();

		task->RemoveReference();
		if (m_max_queue_size > 0) {
			m_not_full.Notify();
		}
	}
	return task;
}

static void*
thread_loop(void* arg)
{
	ThreadPool* pool = static_cast<ThreadPool*>(arg);
	while (pool->IsRunning())
	{
		Task* task = pool->Take();
		if (task) {
			task->Run();
		}
	}
	return NULL;
}

void ThreadPool::Start(int num_threads)
{
	assert(m_threads.empty());
	m_running = true;
	m_threads.reserve(num_threads);
	for (int i = 0; i < num_threads; ++i) {
		m_threads.push_back(new Thread(thread_loop, this));
	}
}

void ThreadPool::Stop()
{
	{
		mt::Lock lock(m_mutex);
		m_running = false;
		m_not_empty.NotifyAll();
	}
	for (int i = 0, n = m_threads.size(); i < n; ++i) {
		delete m_threads[i];
	}
}

size_t ThreadPool::QueueSize()
{
	mt::Lock lock(m_mutex);
	return m_queue.size();
}

bool ThreadPool::IsFull() const
{	
	return m_max_queue_size > 0 && m_queue.size() >= m_max_queue_size;
}

}