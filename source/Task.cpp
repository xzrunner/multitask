#include "multitask/Task.h"
#include "multitask/Thread.h"

namespace mt
{

/************************************************************************/
/* class Task                                                           */
/************************************************************************/

Task::Task() 
	: m_next(NULL) 
{
}

/************************************************************************/
/* class TaskQueue                                                      */
/************************************************************************/

TaskQueue::TaskQueue() 
	: m_head(NULL)
	, m_tail(NULL) 
{
}

void TaskQueue::Push(Task* task)
{
	if (!task) {
		return;
	}

	task->AddReference();
	if (m_tail) {
		m_tail->SetNext(task);
		m_tail = task;
	} else {
		m_head = m_tail = task;
	}
}

void TaskQueue::Pop()
{
	if (!m_head) {
		return;
	}

	Task* task = m_head;
	if (m_head == m_tail) {
		m_head->SetNext(NULL);
		m_head = m_tail = NULL;
	} else {
		Task* next = m_head->GetNext();
		m_head->SetNext(NULL);
		m_head = next;
	}
	task->RemoveReference();
}

/************************************************************************/
/* class SafeTaskQueue                                                  */
/************************************************************************/

SafeTaskQueue::SafeTaskQueue()
	: m_head(NULL)
	, m_tail(NULL)
{
}

void SafeTaskQueue::Push(Task* task)
{
	if (!task) {
		return;
	}

	mt::Lock lock(m_mutex);

	task->AddReference();
	if (m_tail) {
		m_tail->SetNext(task);
		m_tail = task;
	} else {
		m_head = m_tail = task;
	} 
}

Task* SafeTaskQueue::TryPop()
{
	mt::Lock lock(m_mutex);

	if (!m_head) {
		return NULL;
	}

	Task* ret = m_head;
	if (m_head == m_tail) {
		m_head->SetNext(NULL);
		m_head = m_tail = NULL;
	} else {
		Task* next = m_head->GetNext();
		m_head->SetNext(NULL);
		m_head = next;
	}
	ret->RemoveReference();
	return ret;
}

}