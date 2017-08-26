#ifndef _MULTITASK_COMMON_TASK_H_
#define _MULTITASK_COMMON_TASK_H_

#include "multitask/Task.h"

namespace mt
{

class CommonTask : public mt::Task
{
public:
	virtual void Run();

	static CommonTask* Create(void (*run_cb)(void* arg), void* arg);

private:
	CommonTask();

private:
	void (*m_run_cb)(void*);
	void* m_run_ud;

private:
	static const unsigned int TASK_TYPE = 0;

	static SafeTaskQueue m_freelist;

}; // CommonTask

}

#endif // _MULTITASK_COMMON_TASK_H_