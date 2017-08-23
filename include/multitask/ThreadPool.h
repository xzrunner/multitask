#ifndef _MULTITASK_THREAD_POOL_H_
#define _MULTITASK_THREAD_POOL_H_

#include <CU_Singleton.h>

namespace mt
{

class Task;
class TaskThread;

class ThreadPool
{
public:
	static const int THREAD_NUM = 4;

public:
	void AddTask(Task* task);

	void Lock();
	void Unlock();
	void GetResult(unsigned int type, Task* tasks[THREAD_NUM]);

private:
	TaskThread* m_threads[THREAD_NUM];

	int m_last_idx;

	SINGLETON_DECLARATION(ThreadPool)

}; // ThreadPool

}

#endif // _MULTITASK_THREAD_POOL_H_