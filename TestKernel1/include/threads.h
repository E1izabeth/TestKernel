#include <types.h>
#include <cpu/tables.h>

#ifndef THREADS_H
#define THREADS_H

#define NUM_THREADS 2
typedef struct
{
	//stack
	int num;
	int eip;
	bool processed;
}thread_t;

void init_threads(thread_t threads[NUM_THREADS]);
int change_thread(thread_t threads[NUM_THREADS], registers_t regs);
#endif