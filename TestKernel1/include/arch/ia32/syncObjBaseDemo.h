#pragma once
#include <types.h>
#include <arch/ia32/threads.h>
#include <arch/ia32/autoResetEvent.h>

// sync obj base structure {

typedef struct syncObjImpl_t {
	struct ObjMethods_t* _;
	sleeping_threads_queue_t threadsQueue;
} syncObjImpl_t;

typedef void(*syncObjMethod_t)(syncObjImpl_t* obj);

typedef struct syncObjMethods_t {
	syncObjMethod_t wait, release;
} syncObjMethods_t;

// } sync obj base structure

typedef struct mutexImpl_t {
	struct syncObjImpl_t base;
	// struct mutexMethods_t* _;
	// sleeping_threads_queue_t threadsQueue;

	int ownerThreadId;
	int enterCounter;
	slock_t lock;
	autoResetEvent_t releaseSignal;
} mutexImpl_t;

typedef void(*mutexMethod_f)(mutexImpl_t* sem);

typedef struct mutexMethods_t {
	mutexMethod_f wait, release;
} mutexMethods_t;

// --------------------------------

typedef union mutex_t {
	syncObjImpl_t base;
	mutexImpl_t me;
};

mutex_t newMutex();
