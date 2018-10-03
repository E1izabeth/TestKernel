#pragma once
#include <types.h>
#include <arch/ia32/threads.h>

typedef struct autoResetEvent_t {
	struct autoResetEventMethods_t* _;
	slock_t signal;
} autoResetEvent_t;

typedef void(*autoResetEventMethod_f)(autoResetEvent_t* ev);


typedef struct autoResetEventMethods_t {
	autoResetEventMethod_f set, reset, waitOne;
} autoResetEventMethods_t;

autoResetEvent_t newAutoResetEvent(bool signal);

void autoResetEventExample();