#ifndef _MULTITASK_THREAD_H_
#define _MULTITASK_THREAD_H_

#if defined(__MINGW32__) && !defined(_GLIBCXX_HAS_GTHREADS)
#include <mingw.thread.h>
#include <mutex>
#include <mingw.mutex.h>
#include <mingw.condition_variable.h>
#else
#include <mutex>
#include <thread>
#include <condition_variable>
#endif

#endif // _MULTITASK_THREAD_H_