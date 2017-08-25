#include "multitask/Task.h"
#include "multitask/Thread.h"
#include "multitask/TaskPool.h"

namespace mt
{

/************************************************************************/
/* class Task                                                           */
/************************************************************************/

Task::Task(unsigned int type, bool need_result) 
	: m_type(type)
	, m_need_result(need_result)
	, m_next(NULL) 
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

	if (m_head == m_tail) {
		m_head->SetNext(NULL);
		m_head = m_tail = NULL;
	} else {
		Task* next = m_head->GetNext();
		m_head->SetNext(NULL);
		m_head = next;
	}
}

void TaskQueue::Flush()
{
	if (!m_head) {
		return;
	}

	Task* prev = NULL;
	Task* curr = m_head;
	Task* next = curr->GetNext();
	while (curr) 
	{
		if (curr->Finish()) 
		{
			if (prev) {
				prev->SetNext(next);
			}
			if (m_head == curr) {
				m_head = next;
			}
			if (m_tail == curr) {
				m_tail = prev;
			}
			delete curr;
		} 
		else 
		{
			prev = curr;
		}
		curr = next;
		next = curr ? curr->GetNext() : NULL;
	}
}

/************************************************************************/
/* class TaskThread                                                     */
/************************************************************************/

static void* 
thread_loop(void* arg)
{
	TaskThread* thread = static_cast<TaskThread*>(arg);
	while (true)
	{
		thread->Update();
		Thread::Delay(thread->GetDelay());
	}
}

TaskThread::TaskThread()
	: m_delay_ms(DEFAULT_DELAY)
{
	m_mutex = new Mutex();
	m_thread = new Thread(thread_loop, this);
}

TaskThread::~TaskThread()
{
	ClearWorking();
	ClearResult();

	delete m_mutex;
	delete m_thread;
}

void TaskThread::AddTask(Task* task)
{
	mt::Lock lock(m_mutex);

	m_working_queue.Push(task);
}

void TaskThread::Update()
{
	mt::Lock lock(m_mutex);

	Task* t = m_working_queue.Front();
	while (t) 
	{
		t->Run();

		m_working_queue.Pop();

		if (t->NeedResult())
		{
			int find = false;
			for (int i = 0, n = m_result_queue.size(); i < n; ++i) {
				if (m_result_queue[i].first == t->Type()) {
					m_result_queue[i].second.Push(t);
					find = true;
					break;
				}
			}
			if (!find) {
				TaskQueue q;
				q.Push(t);
				m_result_queue.push_back(std::make_pair(t->Type(), q));
			}
		}
		else
		{
			TaskPool::Instance()->Return(t);
		}

		t = m_working_queue.Front();
	}
}

void TaskThread::Flush()
{
	mt::Lock lock(m_mutex);

	for (int i = 0, n = m_result_queue.size(); i < n; ++i) {
		m_result_queue[i].second.Flush();
	}
}

Task* TaskThread::GetResult(unsigned int type)
{
	for (int i = 0, n = m_result_queue.size(); i < n; ++i) {
		if (m_result_queue[i].first == type) {
			return m_result_queue[i].second.Front();
		}
	}
	return NULL;
}

void TaskThread::ClearWorking()
{
	mt::Lock lock(m_mutex);

	ClearQueueNoLock(m_working_queue);
}

void TaskThread::ClearResult()
{
	mt::Lock lock(m_mutex);

	for (int i = 0, n = m_result_queue.size(); i < n; ++i) {
		ClearQueueNoLock(m_result_queue[i].second);
	}
	m_result_queue.clear();
}

void TaskThread::ClearQueueNoLock(TaskQueue& queue)
{
	Task* t = queue.Front();
	while (t) {
		delete t;
		queue.Pop();
	}
}

}