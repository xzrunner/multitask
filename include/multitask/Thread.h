#ifndef _MULTITASK_THREAD_H_
#define _MULTITASK_THREAD_H_

#include <CU_Uncopyable.h>

#include <pthread.h>

namespace mt
{

class Mutex : private cu::Uncopyable
{
public:
	Mutex() {
		pthread_mutex_init(&m_mutex, NULL);
	}
	~Mutex() {
		pthread_mutex_destroy(&m_mutex);
	}

	void Lock() {
		pthread_mutex_lock(&m_mutex);
	}
	void Unlock() {
		pthread_mutex_unlock(&m_mutex);
	}

	pthread_mutex_t* GetPthreadMutex() { return &m_mutex; }

private:
	pthread_mutex_t m_mutex;

}; // Mutex

class Lock : private cu::Uncopyable
{
public:
	Lock(Mutex& mutex) : m_mutex(mutex) {
		pthread_mutex_lock(m_mutex.GetPthreadMutex());
	}
	~Lock() {
		pthread_mutex_unlock(m_mutex.GetPthreadMutex());
	}

private:
	Mutex& m_mutex;

}; // Lock

class Condition : private cu::Uncopyable
{
public:
	Condition(Mutex& mutex) : m_mutex(mutex) {
		pthread_cond_init(&m_cond, NULL);
	}
	~Condition() {
		pthread_cond_destroy(&m_cond);
	}

	void Wait() {
	 	pthread_cond_wait(&m_cond, m_mutex.GetPthreadMutex());
	}

	void Notify() {
		pthread_cond_signal(&m_cond);
	}

	void NotifyAll() {
		pthread_cond_broadcast(&m_cond);
	}

private:
	Mutex& m_mutex;	

	pthread_cond_t m_cond;

}; // Condition

class Thread : private cu::Uncopyable
{
public:
	Thread(void* (*main)(void*), void* arg);
	~Thread();

	static void Delay(unsigned int ms);

private:
	pthread_t m_thread;

}; // Thread

}

#endif // _MULTITASK_THREAD_H_