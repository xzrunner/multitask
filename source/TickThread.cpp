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
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
	return nullptr;
}

TickThread::TickThread(ThreadPool* pool)
	: m_pool(pool)
	, m_thread(tick_thread_loop, this)
	, m_running(true)
{
}

TickThread::~TickThread()
{
	if (m_running) {
		Stop();
	}
}

void TickThread::Run()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	for (int i = 0, n = m_update_cb.size(); i < n; ++i) 
	{
		CommonTask* task = CommonTask::Create(m_update_cb[i].first, m_update_cb[i].second);
		m_pool->Run(task);	
	}
}

void TickThread::RegisterUpdateCB(void (*update)(void* arg), void* arg)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_update_cb.push_back(std::make_pair(update, arg));
}

void TickThread::UnregisterUpdateCB(void (*update)(void* arg))
{
	std::lock_guard<std::mutex> lock(m_mutex);

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

void TickThread::Stop()
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_running = false;
	}
	m_thread.join();
}

}