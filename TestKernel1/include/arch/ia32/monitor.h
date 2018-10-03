#pragma once
#include <types.h>
#include <arch/ia32/threads.h>
#include <arch/ia32/manualResetEvent.h>


typedef struct monitor_t {
	struct monitorMethods_t* _;
	bool isPulseHandled;
	bool isMultiPulse;
	int enterCounter;
	int ownerThreadId;
	manualResetEvent_t releaseSignal;
	manualResetEvent_t pulseSignal;
	slock_t lock;
} monitor_t;

typedef void(*monitorMethod_f)(monitor_t* mon);

typedef struct monitorMethods_t {
	monitorMethod_f capture, release, wait, notify, notifyAll;
} monitorMethods_t;

monitor_t newMonitor();
void monitorExample();