#ifndef _MULTITASK_TASK_H_
#define _MULTITASK_TASK_H_

#include "multitask/Thread.h"

#include <CU_Uncopyable.h>
#include <CU_RefCountObj.h>

#include <vector>

#include <stddef.h>

namespace mt
{

class Task : private cu::Uncopyable, public cu::RefCountObj
{
public:
	Task(unsigned int type);
	virtual ~Task() {}

	virtual void Run() = 0;
	
	unsigned int Type() const { return m_type; }

	void SetNext(Task* next) { m_next = next; }
	Task* GetNext() { return m_next; }

private:
	unsigned int m_type;

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

class Mutex;
class SafeTaskQueue : private cu::Uncopyable
{
public:
	SafeTaskQueue();
	~SafeTaskQueue();
	
 	void Push(Task* task);
 	Task* TryPop();

private:
	Mutex* m_mutex;

	Task *m_head, *m_tail;

}; // SafeTaskQueue

}

#endif // _MULTITASK_TASK_H_