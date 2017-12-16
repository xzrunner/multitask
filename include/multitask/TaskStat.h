#ifndef _MULTITASK_TASK_STAT_H_
#define _MULTITASK_TASK_STAT_H_

#include "multitask/Thread.h"

#include <cu/cu_macro.h>
#include <cu/cu_stl.h>

#include <thread>

namespace mt
{

class TaskStat
{
public:
	struct Task
	{
		Task(std::thread::id thread_id, int type, uint32_t begin, uint32_t end)
			: thread_id(thread_id), type(type), begin(begin), end(end) {}

		std::thread::id thread_id;
		int type;
		uint32_t begin, end;
	};

public:
	int RegisterTaskType(const CU_STR& desc);

	void Add(std::thread::id thread_id, int type, uint32_t begin, uint32_t end);

	void Clear();

	const CU_VEC<Task>& GetAllTasks() const { return m_tasks; }

	uint32_t GetTimeMin() const { return m_time_min; }
	uint32_t GetTimeMax() const { return m_time_max; }

public:
	class Checkpoint
	{
	public:
		Checkpoint(std::thread::id thread_id, int type);
		~Checkpoint();

	private:
		std::thread::id m_thread_id;
		int m_type;

		uint32_t m_begin;

	}; // Checkpoint
	
private:
	struct TaskType
	{
		TaskType(int type, const CU_STR& desc)
			: type(type), desc(desc) {}

		int type;
		CU_STR desc;
	};

private:
	std::mutex m_mutex;

	CU_VEC<Task> m_tasks;

	CU_MAP<CU_STR, TaskType> m_types;

	uint32_t m_time_min, m_time_max;

	CU_SINGLETON_DECLARATION(TaskStat);

}; // TaskStat

}

#endif // _MULTITASK_TASK_STAT_H_