#include <types.h>
#include <arch/ia32/cpu/tables.h>

#ifndef THREADS_H
#define THREADS_H

#define DEFAULT_STACK_SIZE 4096
#define MAX_TOTAL_THREADS 200

typedef struct
{
	registers_t regs;
}thread_t;

void threading_start();

void change_thread(registers_t* regs);
int create_thread(void* start_function, void* stPointer);

#endif