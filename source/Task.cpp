#include "multitask/Task.h"

namespace mt
{

/************************************************************************/
/* class Task                                                           */
/************************************************************************/

Task::Task()
	: m_next(nullptr)
{
}

/************************************************************************/
/* class TaskQueue                                                      */
/************************************************************************/

TaskQueue::TaskQueue()
	: m_head(nullptr)
	, m_tail(nullptr)
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
		m_head->SetNext(nullptr);
		m_head = m_tail = nullptr;
	} else {
		Task* next = m_head->GetNext();
		m_head->SetNext(nullptr);
		m_head = next;
	}
	task->RemoveReference();
}

/************************************************************************/
/* class SafeTaskQueue                                                  */
/************************************************************************/

SafeTaskQueue::SafeTaskQueue()
	: m_head(nullptr)
	, m_tail(nullptr)
{
}

void SafeTaskQueue::Push(Task* task)
{
	if (!task) {
		return;
	}

	std::lock_guard<std::mutex> lock(m_mutex);

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
	std::lock_guard<std::mutex> lock(m_mutex);

	if (!m_head) {
		return nullptr;
	}

	Task* ret = m_head;
	if (m_head == m_tail) {
		m_head->SetNext(nullptr);
		m_head = m_tail = nullptr;
	} else {
		Task* next = m_head->GetNext();
		m_head->SetNext(nullptr);
		m_head = next;
	}
	ret->RemoveReference();
	return ret;
}

bool SafeTaskQueue::Empty()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	return m_head == nullptr;
}

}