#include "multitask/TaskPool.h"

namespace mt
{

SINGLETON_DEFINITION(TaskPool)

TaskPool::TaskPool()
{
	m_mutex = new Mutex();
}

TaskPool::~TaskPool()
{
	delete m_mutex;
}

Task* TaskPool::Fetch()
{
	mt::Lock lock(m_mutex);

	Task* ret = m_freelist.Front();
	if (!ret) {
		ret = new CommonTask();
	}
	return ret;
}

void TaskPool::Return(Task* task)
{
	mt::Lock lock(m_mutex);

	m_freelist.Push(task);
}

}