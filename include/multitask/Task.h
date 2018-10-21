#ifndef _MULTITASK_TASK_H_
#define _MULTITASK_TASK_H_

#include "multitask/Thread.h"

#include <cu/uncopyable.h>
#include <cu/CU_RefCountObj.h>

#include <vector>

#include <stddef.h>

namespace mt
{

class Task : private cu::Uncopyable, public cu::RefCountObj
{
public:
	Task();
	virtual ~Task() {}

	virtual void Run() = 0;

	void SetNext(Task* next) { m_next = next; }
	Task* GetNext() { return m_next; }

private:
	Task* m_next;

}; // Task

class TaskQueue : private cu::Uncopyable
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

class SafeTaskQueue : private cu::Uncopyable
{
public:
	SafeTaskQueue();

 	void Push(Task* task);
 	Task* TryPop();

	bool Empty();

private:
	std::mutex m_mutex;

	Task *m_head, *m_tail;

}; // SafeTaskQueue

}

#endif // _MULTITASK_TASK_H_