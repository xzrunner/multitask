#ifndef _MULTITASK_TASK_H_
#define _MULTITASK_TASK_H_

#include "multitask/Thread.h"

#include <CU_Uncopyable.h>

#include <vector>

#include <stddef.h>

namespace mt
{

class Task : private cu::Uncopyable
{
public:
	Task(unsigned int type);
	virtual ~Task() {}

	virtual void Run()  = 0;
	
	unsigned int Type() const { return m_type; }

	void SetNext(Task* next) { m_next = next; }
	Task* GetNext() { return m_next; }

private:
	unsigned int m_type;

	Task* m_next;

}; // Task

class TaskQueue
{
public:
	TaskQueue();

	bool Empty() const { return !m_head; }
	
	Task* Front() { return m_head; }

	void Push(Task* task);

	void Pop();

private:
	Task *m_head, *m_tail;

}; // Task

class Mutex;
class Thread;
class TaskThread : private cu::Uncopyable
{
public:
	TaskThread();
	virtual ~TaskThread();

	void AddTask(Task* task);

	void Update();

	bool EmptyNoLock() { return m_working_queue.Empty(); }

	unsigned int GetDelay() const { return m_delay_ms; }

	void Lock() { m_mutex->Lock(); }
	void Unlock() { m_mutex->Unlock(); }

	Task* GetResult(unsigned int type);

	void ClearWorking();
	void ClearResult();

private:
	void ClearQueueNoLock(TaskQueue& queue);

private:
	static const unsigned int DEFAULT_DELAY = 5;

private:
	Mutex* m_mutex;
	Thread* m_thread;

	TaskQueue m_working_queue;

	// todo hash
	std::vector<std::pair<unsigned int, TaskQueue> > m_result_queue;

	unsigned int m_delay_ms;

}; // TaskThread

}

#endif // _MULTITASK_TASK_H_