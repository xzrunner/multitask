#include "multitask/TickThread.h"
#include "multitask/CommonTask.h"
#include "multitask/ThreadPool.h"

namespace mt
{

static void* 
tick_thread_loop(void* arg)
{
	TickThread* tick = static_cast<TickThread*>(arg);
	while (tick->IsRunning())
	{
		tick->Run();
		Thread::Delay(5);
	}
	return NULL;
}

TickThread::TickThread(ThreadPool* pool)
	: m_pool(pool)
	, m_thread(NULL)
	, m_running(true)
{
	m_thread = new Thread(tick_thread_loop, this);
}

TickThread::~TickThread()
{
	{
		mt::Lock lock(m_mutex);
		m_running = false;
	}
	delete m_thread;
}

void TickThread::Run()
{
	mt::Lock lock(m_mutex);

	for (int i = 0, n = m_update_cb.size(); i < n; ++i) 
	{
		CommonTask* task = CommonTask::Create(m_update_cb[i].first, m_update_cb[i].second);
		m_pool->Run(task);	
	}
}

void TickThread::RegisterUpdateCB(void (*update)(void* arg), void* arg)
{
	mt::Lock lock(m_mutex);

	m_update_cb.push_back(std::make_pair(update, arg));
}

void TickThread::UnregisterUpdateCB(void (*update)(void* arg))
{
	mt::Lock lock(m_mutex);

	std::vector<std::pair<void (*)(void*), void*> >::iterator itr
		= m_update_cb.begin();
	for ( ; itr != m_update_cb.end(); ) {
		if (itr->first == update) {
			itr = m_update_cb.erase(itr);
		} else {
			++itr;
		}
	}
}

}