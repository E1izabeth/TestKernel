#include <types.h>
#include <arch/ia32/cpu/tables.h>

#ifndef THREADS_H
#define THREADS_H

#define DEFAULT_STACK_SIZE 4096
#define MAX_TOTAL_THREADS 200

typedef struct
{
	// registers_t regs;

	byte* stackPtr;
	int id;

} thread_t;

void threading_start();
void change_thread(registers_t** regs);
int create_thread(void* start_function, void* stPointer, int stackSize);
thread_t* get_thread();

typedef struct
{
	int state;
} slock_t;

slock_t slockInit();
void slockCapture(slock_t *lock);
void slockRelease(slock_t *lock);

#endif