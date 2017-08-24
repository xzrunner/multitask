#ifndef _MULTITASK_TASK_POOL_H_
#define _MULTITASK_TASK_POOL_H_

#include "multitask/Task.h"

#include <CU_Singleton.h>

namespace mt
{

// no order
class CommonTask : public Task
{
public:
	CommonTask() 
		: Task(0, false)
		, m_update_cb(NULL)
		, m_arg(NULL) 
	{}

	virtual void Run() {
		if (m_update_cb) {
			m_update_cb(m_arg);
		}
	}

	void SetUpdateCB(void (*update_cb)(void*), void* arg) {
		m_update_cb = update_cb;
		m_arg = arg;
	}

private:
	void (*m_update_cb)(void*);
	void* m_arg;

}; // CommonTask

class TaskPool
{
public:
	Task* Fetch();
	void Return(Task*);

private:
	Mutex* m_mutex;

	TaskQueue m_freelist;

	SINGLETON_DECLARATION(TaskPool)

}; // TaskPool

}

#endif // _MULTITASK_TASK_POOL_H_