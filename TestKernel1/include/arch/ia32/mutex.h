#pragma once
#include <types.h>
#include <arch/ia32/threads.h>
#include <arch/ia32/autoResetEvent.h>

typedef struct mutex_t {
	struct mutexMethods_t* _;
	int ownerThreadId;
	int enterCounter;
	slock_t lock;
	autoResetEvent_t releaseSignal;
	sleeping_threads_queue_t threadsQueue;
} mutex_t;

typedef void(*mutexMethod_f)(mutex_t* sem);

typedef struct mutexMethods_t {
	mutexMethod_f wait, release;
} mutexMethods_t;

mutex_t newMutex();
