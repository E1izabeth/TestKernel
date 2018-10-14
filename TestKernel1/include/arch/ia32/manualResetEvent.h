#pragma once
#include <types.h>
#include <arch/ia32/threads.h>
#include <arch/ia32/display.h>
#include <mem.h>


typedef struct manualResetEvent_t {
	struct manualResetEventMethods_t* _;
	slock_t signal;
	slock_t lock;
	bool isSet;
	sleeping_threads_queue_t threadsQueue;
} manualResetEvent_t;

typedef void(*manualResetEventMethod_f)(manualResetEvent_t* ev);

typedef struct manualResetEventMethods_t {
	manualResetEventMethod_f set, reset, wait;
} manualResetEventMethods_t;

manualResetEvent_t newManualResetEvent(bool signal);
void manualResetEventExample();