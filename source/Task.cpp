#include "multitask/Task.h"
#include "multitask/Thread.h"

namespace mt
{

/************************************************************************/
/* class Task                                                           */
/************************************************************************/

Task::Task(unsigned int type) 
	: m_type(type)
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
		m_head = m_tail = NULL;
	} else {
		m_head = m_head->GetNext();
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

		// insert to result
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

		t = m_working_queue.Front();
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