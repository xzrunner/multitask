#include "multitask/ThreadPool.h"
#include "multitask/Task.h"

namespace mt
{

SINGLETON_DEFINITION(ThreadPool)

ThreadPool::ThreadPool()
	: m_last_idx(-1)
{
	for (int i = 0; i < THREAD_NUM; ++i) {
		m_threads[i] = new TaskThread();
	}
}

ThreadPool::~ThreadPool()
{
	for (int i = 0; i < THREAD_NUM; ++i) {
		delete m_threads[i];
	}
}

void ThreadPool::AddTask(Task* task)
{
	// find a free thread
	for (int i = 0; i < THREAD_NUM; ++i) {
		if (m_threads[i]->EmptyNoLock()) {
			m_threads[i]->AddTask(task);
			m_last_idx = i;
			return;
		}
	}

	// use next thread
	int curr = (m_last_idx + 1) % THREAD_NUM;
	m_threads[curr]->AddTask(task);
	m_last_idx = curr;
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

}