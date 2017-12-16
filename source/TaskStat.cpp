#include "multitask/TaskStat.h"

#include <glp_loop.h>

namespace mt
{

CU_SINGLETON_DEFINITION(TaskStat)

TaskStat::TaskStat()
{
	m_time_min = std::numeric_limits<uint32_t>::max();
	m_time_max = std::numeric_limits<uint32_t>::min();
}

int TaskStat::RegisterTaskType(const CU_STR& desc)
{
	auto itr = m_types.find(desc);
	if (itr != m_types.end()) {
		return itr->second.type;
	} else {
		int type = m_types.size();
		m_types.insert(std::make_pair(desc, TaskType(type, desc)));
		return type;
	}
}

void TaskStat::Add(std::thread::id thread_id, int type, uint32_t begin, uint32_t end) 
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_tasks.push_back(Task(thread_id, type, begin, end));

	if (begin < m_time_min) {
		m_time_min = begin;
	}
	if (end > m_time_max) {
		m_time_max = end;
	}
}

void TaskStat::Clear() 
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_tasks.clear();

	m_time_min = std::numeric_limits<uint32_t>::max();
	m_time_max = std::numeric_limits<uint32_t>::min();
}

/************************************************************************/
/* class TaskStat::Checkpoint                                           */
/************************************************************************/

TaskStat::Checkpoint::Checkpoint(std::thread::id thread_id, int type)
	: m_thread_id(thread_id)
	, m_type(type)
{
	m_begin = glp_get_time();
}

TaskStat::Checkpoint::~Checkpoint()
{
	uint32_t end = glp_get_time();
	TaskStat::Instance()->Add(m_thread_id, m_type, m_begin, end);
}

}