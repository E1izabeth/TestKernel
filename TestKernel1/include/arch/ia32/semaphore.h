#pragma once
#include <types.h>
#include <arch/ia32/threads.h>
#include <arch/ia32/autoResetEvent.h>

typedef struct semaphore_t {
	struct semaphoreMethods_t* _;
	int max;
	int min;
	int counter;
	sleeping_threads_queue_t threadsQueue;
	autoResetEvent_t guard;
	slock_t spinlock;
} semaphore_t;

typedef void(*semaphoreMethod_f)(semaphore_t* sem);

typedef struct semaphoreMethods_t {
	semaphoreMethod_f wait, release;
} semaphoreMethods_t;

semaphore_t newSemaphore(int max, int min);
