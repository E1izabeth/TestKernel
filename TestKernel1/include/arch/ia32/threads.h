#ifndef THREADS_H
#define THREADS_H

#include <arch/ia32/cpu/tables.h>
#include <arch/ia32/cpu/cpu.h>
#include <arch/ia32/display.h>
#include <mem.h>
#include <types.h>

#define DEFAULT_STACK_SIZE 4096
#define MAX_TOTAL_THREADS 200

enum threadState
{
	THREADSTATE_WAITSLEEPJOIN,
	THREADSTATE_RUNNING,
	THREADSTATE_UNSTARTED,
	THREADSTATE_STOPPED,
	THREADSTATE_ABORTED
}threadState;

typedef struct
{
	// registers_t regs;

	byte* stackPtr;
	int id;
	bool isUnderSheduling;
	enum threadState state;
	void* (*startFunc)(void*);

} thread_t;

void init_main_thread();
void threading_start();
void change_thread(registers_t** regs);
int create_thread(void* (*start_function)(void*), void* stPointer, int stackSize);
thread_t* get_thread();
int get_current_thread_id();
void sleep(int mills);

typedef struct
{
	int state;
} slock_t;

slock_t slockInit();
void slockCapture(slock_t *lock);
void slockRelease(slock_t *lock);
void slockSet(slock_t *lock);


// void passive_wait_cond(bool(*cond)(void*), void* args);
// void active_wait_cond(bool(*cond)(void*), void* args);

typedef struct sleeping_thread_entry_t {
	struct sleeping_thread_entry_t* next;
	int threadId;
} sleeping_thread_entry_t;

typedef struct sleeping_threads_queue_t {
	sleeping_thread_entry_t *head, *tail;
	slock_t lock;
	int count;
} sleeping_threads_queue_t;

sleeping_threads_queue_t init_threads_queue();
void wake_queued_thread(sleeping_threads_queue_t* q);
void wake_queued_threads(sleeping_threads_queue_t* q);
void queue_waiting_thread(sleeping_threads_queue_t* q);

#endif