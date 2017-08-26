#include "multitask/CommonTask.h"

namespace mt
{

SafeTaskQueue CommonTask::m_freelist;

CommonTask::CommonTask()
	: mt::Task(TASK_TYPE)
	, m_run_cb(NULL)
	, m_run_ud(NULL)
{
}

void CommonTask::Run()
{
	if (m_run_cb) {
		m_run_cb(m_run_ud);
	}
	m_run_cb = NULL;
	m_run_ud = NULL;
	m_freelist.Push(this);
}

CommonTask* CommonTask::Create(void (*run_cb)(void* arg), void* arg)
{
	CommonTask* task = static_cast<CommonTask*>(m_freelist.TryPop());
	if (!task) {
		task = new CommonTask();
	} else {
		task->m_run_cb = run_cb;
		task->m_run_ud = arg;
	}
	return task;
}

}